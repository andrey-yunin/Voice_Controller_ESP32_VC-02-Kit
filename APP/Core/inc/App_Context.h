#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace App_Context {
    // Дескрипторы очередей приложения.
    struct Queues {
        QueueHandle_t q_input = nullptr;
        QueueHandle_t q_audio = nullptr;
        QueueHandle_t q_tv = nullptr;
        QueueHandle_t q_ir_tx = nullptr;
        QueueHandle_t q_diag = nullptr;
    };

    // Вызывается один раз при запуске, до старта рабочих задач.
    // На входе все дескрипторы должны быть nullptr.
    // true — созданы все пять очередей.
    // При ошибке освобождает созданные очереди и возвращает false.
    bool CreateQueues(Queues& queues);
}  // namespace App_Context
