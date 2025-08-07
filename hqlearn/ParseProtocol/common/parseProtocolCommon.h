#ifndef PARSE_PROTOCOL_COMMON_H
#define PARSE_PROTOCOL_COMMON_H
#include <stdint.h>
#include <string.h>

#define PARSE_PROTOCOL_XMODEM_NUM (0x1021) // XMODEM多项式
#define PARSE_PROTOCOL_MESSAGE_LEN (14)
// 消息结构定义
typedef struct
{
    uint16_t code;      // 2B 消息码
    uint16_t sessionid; // 2B 会话ID
    uint8_t src_addr;   // 1B 源地址
    uint8_t dst_addr;   // 1B 目的地址
    uint16_t bodylen;   // 2B 消息体长度
    uint8_t *body;      // 消息体数据
    uint16_t crc16;     // 2B CRC16校验
} CHHBusMsg;

void CHHBusMsg_Destroy(CHHBusMsg *msg);

// CRC16计算函数
uint16_t crc16_calculate(const uint8_t *data, uint32_t length);

#endif