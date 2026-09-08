#pragma once

namespace Uart_Task {
    // Входная функция задачи FreeRTOS.
    // parameter — указатель на App_Context::Queues.
    // Переданный контекст должен существовать всё время работы задачи.
    void Run(void* parameter);
}  // namespace Uart_Task
