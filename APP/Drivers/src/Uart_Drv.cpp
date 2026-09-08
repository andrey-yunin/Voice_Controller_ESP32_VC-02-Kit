#include "../inc/Uart_Drv.h"

#include <Arduino.h>

namespace Uart_Drv {
    void Init(std::uint32_t baudRate) {
        // На первом этапе используем UART0 через USB–UART-мост платы.
        Serial.begin(baudRate, SERIAL_8N1);
    }

    bool ReadByte(std::uint8_t& value) {
        // read() возвращает int: 0–255 — байт, -1 — данных нет.
        const int received = Serial.read();

        if (received < 0) {
            return false;
        }

        value = static_cast<std::uint8_t>(received);
        return true;
    }
}  // namespace Uart_Drv
