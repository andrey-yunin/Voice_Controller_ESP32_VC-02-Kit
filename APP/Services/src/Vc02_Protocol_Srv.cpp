#include "../inc/Vc02_Protocol_Srv.h"

namespace Vc02_Protocol_Srv {

    // Внутренние данные доступны только в этом .cpp.
    namespace {
        // Максимальная пауза между байтами кадра в миллисекундах.
        constexpr std::uint32_t BYTE_TIMEOUT_MS = 100;

        // Байты собираем постепенно, между вызовами ProcessByte().
        std::uint8_t frame[5] = {};

        // Количество уже накопленных байт: 0 означает ожидание старта.
        std::uint8_t receivedCount = 0;

        // Время последнего байта незавершённого кадра.
        std::uint32_t lastByteMs = 0;
    }

    ParseResult ProcessByte(std::uint8_t byte,
                            std::uint32_t nowMs,
                            std::uint8_t& commandId) {
        // Сбрасываем незавершённый кадр после долгой паузы.
        // Текущий байт затем рассматриваем как возможное начало нового.
        if (receivedCount > 0 &&
            static_cast<std::uint32_t>(nowMs - lastByteMs) >= BYTE_TIMEOUT_MS) {
            receivedCount = 0;
        }

        // До начала кадра пропускаем всё, кроме стартового байта.
        if (receivedCount == 0 && byte != 0x5A) {
            return ParseResult::Waiting;
        }

        // Сохраняем очередной байт и время его обработки.
        frame[receivedCount] = byte;
        ++receivedCount;
        lastByteMs = nowMs;

        if (receivedCount < sizeof(frame)) {
            return ParseResult::Waiting;
        }

        // Получены все пять байт: проверяем резерв и XOR.
        const std::uint8_t checksum =
            frame[0] ^ frame[1] ^ frame[2] ^ frame[3];

        if (frame[2] == 0x00 &&
            frame[3] == 0x00 &&
            frame[4] == checksum) {
            commandId = frame[1];
            receivedCount = 0;
            return ParseResult::Accepted;
        }

        // При ошибке ищем следующий возможный старт внутри
        // накопленных данных, чтобы сохранить начало нового кадра.
        receivedCount = 0;

        for (std::uint8_t start = 1; start < sizeof(frame); ++start) {
            if (frame[start] == 0x5A) {
                for (std::uint8_t i = start; i < sizeof(frame); ++i) {
                    frame[receivedCount] = frame[i];
                    ++receivedCount;
                }
                break;
            }
        }
        return ParseResult::Rejected;
    }
}  // namespace Vc02_Protocol_Srv
