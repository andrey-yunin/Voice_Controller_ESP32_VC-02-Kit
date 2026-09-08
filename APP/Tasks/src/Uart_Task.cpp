#include "../inc/Uart_Task.h"

#include "../../Core/inc/App_Config.h"
#include "../../Core/inc/App_Context.h"
#include "../../Core/inc/Command_Types.h"
#include "../../Drivers/inc/Uart_Drv.h"
#include "../../Services/inc/Vc02_Protocol_Srv.h"

#include <Arduino.h>
#include <freertos/task.h>

namespace Uart_Task {
    namespace {
        // Ограничиваем непрерывное чтение, чтобы давать работать другим задачам.
        constexpr std::uint32_t READ_BATCH_SIZE = 64;
    }

    void Run(void* parameter) {
        // Контекст и очереди должны быть подготовлены до запуска задачи.
        const auto* queues = static_cast<App_Context::Queues*>(parameter);

        // Ноль зарезервирован для событий без назначенного запроса.
        std::uint32_t requestId = 0;

        const TickType_t sendTimeout =
            pdMS_TO_TICKS(App_Config::Queues::INPUT_SEND_TIMEOUT_MS);

        for (;;) {
            for (std::uint32_t i = 0; i < READ_BATCH_SIZE; ++i) {
                std::uint8_t byte = 0;

                if (!Uart_Drv::ReadByte(byte)) {
                    break;
                }

                std::uint8_t commandId = 0;
                const auto result = Vc02_Protocol_Srv::ProcessByte(
                    byte, millis(), commandId);

                if (result == Vc02_Protocol_Srv::ParseResult::Waiting) {
                    continue;
                }

                DiagnosticEvent event{};

                if (result == Vc02_Protocol_Srv::ParseResult::Rejected) {
                    // Для повреждённого кадра идентификаторы остаются нулевыми.
                    event.code = DiagnosticEvent::Code::FRAME_REJECTED;
                } else {
                    // Waiting обработан выше, Rejected — предыдущей веткой.
                    // Здесь result == Accepted: парсер выдал проверенный commandId.
                    // Назначаем номер только проверенному кадру.
                    ++requestId;

                    // При переполнении счётчика пропускаем зарезервированный ноль.
                    if (requestId == 0) {
                        ++requestId;
                    }

                    // Формируем элемент q_input для передачи в Command_Task.
                    // InputCommand объявлена в Core/inc/Command_Types.h.
                    // command — локальная переменная этой структуры; {} обнуляет поля.
                    // Передаём номер запроса и команды, а не исходные пять байт UART.
                    InputCommand command{};
                    command.requestId = requestId;
                    command.commandId = commandId;

                    // Связываем диагностическое событие с той же командой.
                    event.requestId = requestId;
                    event.commandId = commandId;

                    // Передаём адрес заполненной command в xQueueSend().
                    // При успехе q_input хранит собственную копию структуры,
                    // поэтому локальная command после отправки больше не нужна.
                    const BaseType_t sent = xQueueSend(
                        queues->q_input, &command, sendTimeout);

                    if (sent == pdPASS) {
                        event.code = DiagnosticEvent::Code::INPUT_QUEUED;
                    } else {
                        event.code = DiagnosticEvent::Code::INPUT_QUEUE_FULL;
                    }
                }

                // Диагностика не задерживает приём команд.
                // При заполненной q_diag это событие отбрасывается.
                (void)xQueueSend(queues->q_diag, &event, 0);
            }

            // Блокируемся на один тик после порции чтения или опустошения UART.
            vTaskDelay(1);
        }
    }

}  // namespace Uart_Task
