#include "hhbus_protocol.h"

CHHBusMsgWriter::CHHBusMsgWriter(CHHBusMsg *msg, uint8_t *buffer, uint32_t buffer_len) :
    m_msg(msg), m_buffer(buffer), m_buffer_len(buffer_len), m_pos(0), m_valid(true) {
    if (m_msg) {
        m_msg->body = NULL;
    } else {
        m_valid = false;
    }
}

CHHBusMsgWriter::~CHHBusMsgWriter() {
}

// 写入消息头
CHHBusMsgWriter &CHHBusMsgWriter::writeHeader() {
    if (!m_valid || m_buffer_len < 2) {
        m_valid = false;
        return *this;
    }

    // 写入帧头
    m_buffer[m_pos++] = 0xAA;
    m_buffer[m_pos++] = 0x55;

    return *this;
}

// 写入code和sessionid的异或值
CHHBusMsgWriter &CHHBusMsgWriter::writeCodeXorSessionId() {
    if (!m_valid || m_pos + 2 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    uint16_t xor_val = m_msg->code ^ m_msg->sessionid;
    m_buffer[m_pos++] = (xor_val >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = xor_val & 0xFF;        // 低8位

    return *this;
}

// 写入code
CHHBusMsgWriter &CHHBusMsgWriter::writeCode() {
    if (!m_valid || m_pos + 2 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_buffer[m_pos++] = (m_msg->code >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = m_msg->code & 0xFF;        // 低8位

    return *this;
}

// 写入sessionid
CHHBusMsgWriter &CHHBusMsgWriter::writeSessionId() {
    if (!m_valid || m_pos + 2 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_buffer[m_pos++] = (m_msg->sessionid >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = m_msg->sessionid & 0xFF;        // 低8位

    return *this;
}

// 写入源地址
CHHBusMsgWriter &CHHBusMsgWriter::writeSrcAddr() {
    if (!m_valid || m_pos + 1 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_buffer[m_pos++] = m_msg->src_addr;
    return *this;
}

// 写入目的地址
CHHBusMsgWriter &CHHBusMsgWriter::writeDstAddr() {
    if (!m_valid || m_pos + 1 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_buffer[m_pos++] = m_msg->dst_addr;
    return *this;
}

// 写入消息体长度
CHHBusMsgWriter &CHHBusMsgWriter::writeBodyLen() {
    if (!m_valid || m_pos + 2 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_buffer[m_pos++] = (m_msg->bodylen >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = m_msg->bodylen & 0xFF;        // 低8位

    return *this;
}

// 写入消息体
CHHBusMsgWriter &CHHBusMsgWriter::writeBody() {
    if (!m_valid || !m_msg || m_msg->bodylen == 0 || !m_msg->body) {
        return *this;
    }

    if (m_pos + m_msg->bodylen > m_buffer_len) {
        m_valid = false;
        return *this;
    }

    memcpy(&m_buffer[m_pos], m_msg->body, m_msg->bodylen);
    m_pos += m_msg->bodylen;

    return *this;
}

// 计算并写入CRC16
CHHBusMsgWriter &CHHBusMsgWriter::writeCrc16() {
    if (!m_valid || m_pos + 2 > m_buffer_len) {
        m_valid = false;
        return *this;
    }

    // 计算当前已写入数据的CRC16(不包括CRC字段本身)
    m_msg->crc16 = crc16_calculate(m_buffer, m_pos);

    m_buffer[m_pos++] = (m_msg->crc16 >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = m_msg->crc16 & 0xFF;        // 低8位

    return *this;
}

// 完整编码过程
CHHBusMsgWriter &CHHBusMsgWriter::encode() {
    return writeHeader()
        .writeCodeXorSessionId()
        .writeCode()
        .writeSessionId()
        .writeSrcAddr()
        .writeDstAddr()
        .writeBodyLen()
        .writeBody()
        .writeCrc16();
}

// 获取编码后的数据长度
uint32_t CHHBusMsgWriter::getLength() const {
    return m_valid ? m_pos : 0;
}

// 检查是否有效
bool CHHBusMsgWriter::isValid() const {
    return m_valid;
}