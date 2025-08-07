#include "parseProtocolCommon.h"

// CRC16计算实现 (使用常用的XMODEM多项式)
uint16_t crc16_calculate(const uint8_t *data, uint32_t length) {
    if (data == NULL || length == 0) {
        return 0;
    }

    uint16_t crc = 0;
    const uint16_t polynomial = 0x1021; // XMODEM多项式

    for (uint32_t i = 0; i < length; ++i) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;

        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
            crc &= 0xFFFF; // 保持16位
        }
    }

    return crc;
}