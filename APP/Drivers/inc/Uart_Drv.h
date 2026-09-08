#pragma once

#include <cstdint>

namespace Uart_Drv {
    // Вызывается один раз до запуска задач, использующих UART.
    void Init(std::uint32_t baudRate = 115200);

    // Читает один байт без ожидания новых данных.
    // При успехе возвращает true и записывает байт в value.
    // Если данных нет, возвращает false и сохраняет value.
    bool ReadByte(std::uint8_t& value);
}  // namespace Uart_Drv
