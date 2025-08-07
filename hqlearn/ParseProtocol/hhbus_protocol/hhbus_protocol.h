#ifndef HHBUS_PROTOCOL_H
#define HHBUS_PROTOCOL_H

#include <stdint.h>
#include <string.h>

#include "parseProtocolCommon.h"

class CHHBusMsgWriter {
private:
    CHHBusMsg *m_msg;
    uint8_t *m_buffer;
    uint32_t m_buffer_len;
    uint32_t m_pos;
    bool m_valid;

public:
    CHHBusMsgWriter(CHHBusMsg *msg, uint8_t *buffer, uint32_t buffer_len);
    ~CHHBusMsgWriter();

public:
    // 写入消息头
    CHHBusMsgWriter &writeHeader();

    // 写入code和sessionid的异或值
    CHHBusMsgWriter &writeCodeXorSessionId();

    // 写入code
    CHHBusMsgWriter &writeCode();

    // 写入sessionid
    CHHBusMsgWriter &writeSessionId();

    // 写入源地址
    CHHBusMsgWriter &writeSrcAddr();

    // 写入目的地址
    CHHBusMsgWriter &writeDstAddr();

    // 写入消息体长度
    CHHBusMsgWriter &writeBodyLen();

    // 写入消息体
    CHHBusMsgWriter &writeBody();

    // 计算并写入CRC16
    CHHBusMsgWriter &writeCrc16();

    // 完整编码过程
    CHHBusMsgWriter &encode();

    // 获取编码后的数据长度
    uint32_t getLength() const;

    // 检查是否有效
    bool isValid() const;
};

#endif