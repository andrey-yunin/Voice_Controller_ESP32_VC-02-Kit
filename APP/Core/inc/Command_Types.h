#pragma once

#include <cstdint>

// Проверенная входная команда для передачи из Uart_Task в Command_Task.
struct InputCommand {
    // Назначается Uart_Task после успешной проверки кадра.
    std::uint32_t requestId;

    // Номер команды, извлечённый парсером из UART-кадра.
    std::uint8_t commandId;
};

// Логическая команда от Command_Task к задаче устройства через q_audio или q_tv.
// Профиль устройства преобразует её действие в конкретное задание IrJob.
struct DeviceCommand {

    // Вложенный тип получателя; пример значения: DeviceCommand::Target::AUDIO.
    enum class Target : std::uint8_t {
        AUDIO,
        TV
    };

    // Вложенный тип действия; эти значения ещё не являются ИК-кодами.
    enum class Action : std::uint8_t {
        POWER  // Переключение питания, а не подтверждённое включение.
    };

    // Сохраняется из исходной InputCommand.
    std::uint32_t requestId;

    // Получатель, определённый Command_Task при маршрутизации.
    Target target;

    // Требуемое действие устройства.
    Action action;
};


// Задание для Ir_Task, передаваемое через q_ir_tx.
// На текущем этапе используется протокол NEC.
struct IrJob {
    // Сохраняется из исходной команды для диагностики.
    std::uint32_t requestId;

    // Адрес устройства для передачи по NEC.
    std::uint16_t address;

    // ИК-код команды, выбранный профилем устройства.
    std::uint8_t command;

    // Количество дополнительных повторов; 0 — без повторов.
    std::uint8_t repeats;
};

// Событие от рабочей задачи к Diagnostic_Task через q_diag.
struct DiagnosticEvent {
    // Результат конкретного этапа обработки.
    enum class Code : std::uint8_t {
        FRAME_REJECTED,     // Парсер отклонил кадр.
        INPUT_QUEUED,       // Команда помещена в q_input.
        INPUT_QUEUE_FULL,  // В q_input не осталось свободного места.
        TEST_PING_HANDLED,  // Command_Task обработала TEST_PING.
        COMMAND_REJECTED   // Номер команды не поддерживается.
    };

    // Номер запроса; 0, если запрос ещё не назначен.
    std::uint32_t requestId;

    // Номер команды из проверенного кадра.
    // Для FRAME_REJECTED заполняется нулём и не интерпретируется.
    std::uint8_t commandId;

    // Определяет смысл события и использование остальных полей.
    Code code;
};
