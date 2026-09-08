#pragma once

#include <cstdint>

namespace App_Config {
    namespace Queues {
        // Максимальное ожидание свободного места в q_input, миллисекунды.
        constexpr std::uint32_t INPUT_SEND_TIMEOUT_MS = 30;

        constexpr std::uint32_t INPUT_LENGTH = 8;
        constexpr std::uint32_t AUDIO_LENGTH = 8;
        constexpr std::uint32_t TV_LENGTH = 8;
        constexpr std::uint32_t IR_TX_LENGTH = 8;
        constexpr std::uint32_t DIAG_LENGTH = 32;
    }
}
