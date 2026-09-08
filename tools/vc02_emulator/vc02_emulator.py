#!/usr/bin/env python3
"""Отправка одного бинарного командного кадра VC-02 через UART."""

import argparse
import sys


def command_id_value(value):
    """Принимает десятичный номер или шестнадцатеричный с префиксом 0x."""
    try:
        number = int(value, 16 if value.lower().startswith("0x") else 10)
    except ValueError as error:
        raise argparse.ArgumentTypeError("Ожидается число, например 1 или 0x01") from error
    if not 0 <= number <= 255:
        raise argparse.ArgumentTypeError("commandId должен быть от 0 до 255")
    return number


def positive_integer(value):
    """Скорость UART должна быть положительным целым числом."""
    try:
        number = int(value)
    except ValueError as error:
        raise argparse.ArgumentTypeError("Ожидается целое число") from error
    if number <= 0:
        raise argparse.ArgumentTypeError("Значение должно быть больше нуля")
    return number


def build_frame(command_id):
    """Формирует START, COMMAND, два резервных байта и XOR."""
    payload = bytes((0x5A, command_id, 0x00, 0x00))
    checksum = 0
    for byte in payload:
        checksum ^= byte
    return payload + bytes((checksum,))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command_id", type=command_id_value, help="Номер команды: 1 или 0x01")
    parser.add_argument("--port", help="Последовательный порт, например /dev/ttyUSB0")
    parser.add_argument("--baud", type=positive_integer, default=115200,
                        help="Скорость UART (по умолчанию 115200, формат 8N1)")
    parser.add_argument("--dry-run", action="store_true",
                        help="Показать кадр без открытия порта и отправки")
    args = parser.parse_args()
    if not args.dry_run and not args.port:
        parser.error("Для отправки укажите --port; для просмотра используйте --dry-run")

    frame = build_frame(args.command_id)
    print("Кадр:", frame.hex(" ").upper())
    if args.dry_run:
        return 0

    # pyserial нужен только для реальной отправки; просмотр работает без него.
    try:
        import serial
    except ImportError:
        print("Установите зависимость: tools/.venv/bin/python -m pip install pyserial",
              file=sys.stderr)
        return 1

    try:
        # Линии управления не используются для намеренного сброса ESP32.
        with serial.Serial(port=None, baudrate=args.baud, bytesize=serial.EIGHTBITS,
                           parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                           write_timeout=2, xonxoff=False, rtscts=False,
                           dsrdtr=False) as uart:
            uart.dtr = False
            uart.rts = False
            uart.port = args.port
            uart.open()
            # Отправляем пять байт, без ASCII-кодирования и без CR/LF.
            written = uart.write(frame)
            if written != len(frame):
                raise OSError(f"Отправлено только {written} из {len(frame)} байт")
            uart.flush()
    except (serial.SerialException, OSError, ValueError) as error:
        print(f"Ошибка UART: {error}", file=sys.stderr)
        return 1

    print(f"Отправлено {written} байт в {args.port}, {args.baud} 8N1")
    return 0


if __name__ == "__main__":
    sys.exit(main())
