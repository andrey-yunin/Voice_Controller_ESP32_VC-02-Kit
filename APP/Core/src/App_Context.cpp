#include "../inc/App_Context.h"
#include "../inc/App_Config.h"
#include "../inc/Command_Types.h"

namespace App_Context {
    bool CreateQueues(Queues& queues) {

        // Вызывается до старта задач, когда все дескрипторы ещё nullptr.
        queues.q_input = xQueueCreate(
            App_Config::Queues::INPUT_LENGTH, sizeof(InputCommand));

        queues.q_audio = xQueueCreate(
            App_Config::Queues::AUDIO_LENGTH, sizeof(DeviceCommand));

        queues.q_tv = xQueueCreate(
            App_Config::Queues::TV_LENGTH, sizeof(DeviceCommand));

        queues.q_ir_tx = xQueueCreate(
            App_Config::Queues::IR_TX_LENGTH, sizeof(IrJob));

        queues.q_diag = xQueueCreate(
            App_Config::Queues::DIAG_LENGTH, sizeof(DiagnosticEvent));

        // Успех только тогда, когда созданы все пять очередей.
        if (queues.q_input != nullptr &&
            queues.q_audio != nullptr &&
            queues.q_tv != nullptr &&
            queues.q_ir_tx != nullptr &&
            queues.q_diag != nullptr) {
            return true;
        }

        // При частичной ошибке освобождаем успешно созданные очереди.
        if (queues.q_input != nullptr) {
            vQueueDelete(queues.q_input);
        }

        if (queues.q_audio != nullptr) {
            vQueueDelete(queues.q_audio);
        }

        if (queues.q_tv != nullptr) {
            vQueueDelete(queues.q_tv);
        }

        if (queues.q_ir_tx != nullptr) {
            vQueueDelete(queues.q_ir_tx);
        }

        if (queues.q_diag != nullptr) {
            vQueueDelete(queues.q_diag);
        }

        // После удаления очередей убираем указатели на освобождённую память.
        queues = Queues{};
        return false;
    }
}  // namespace App_Context
