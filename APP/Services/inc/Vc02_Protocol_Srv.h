#pragma once

#include <cstdint>

namespace Vc02_Protocol_Srv {

    // Результат обработки очередного байта.
    enum class ParseResult : std::uint8_t {
        Waiting,   // Полного кадра пока нет.
        Accepted,  // Кадр собран и прошёл проверку.
        Rejected   // Кадр собран, но не прошёл проверку.
    };

    // Принимает один байт и продолжает сборку кадра.
    // nowMs — время приёма в миллисекундах для контроля пауз.
    // commandId изменяется только при результате Accepted.
    // Вызывать из одной задачи — Uart_Task.
    ParseResult ProcessByte(std::uint8_t byte,
                            std::uint32_t nowMs,
                            std::uint8_t& commandId);
}  // namespace Vc02_Protocol_Srv
