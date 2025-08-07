#include <cstdio>
#include "hhbus_protocol.h"

CHHBusMsgWriter::CHHBusMsgWriter(CHHBusMsg msg) :
    m_msg(msg), m_pos(0) {
    m_buffer = new unsigned char[m_msg.bodylen + PARSE_PROTOCOL_MESSAGE_LEN];
}

CHHBusMsgWriter::~CHHBusMsgWriter() {
    if (m_buffer != NULL) {
        delete[] m_buffer;
        m_buffer = NULL;
    }
}

// 写入消息头
CHHBusMsgWriter &CHHBusMsgWriter::writeHeader() {
    // 写入帧头
    m_buffer[m_pos++] = 0xAA;
    m_buffer[m_pos++] = 0x55;

    return *this;
}

// 写入code和sessionid的异或值
CHHBusMsgWriter &CHHBusMsgWriter::writeCodeXorSessionId() {
    uint16_t xor_val = m_msg.code ^ m_msg.sessionid;
    m_buffer[m_pos++] = (xor_val >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = xor_val & 0xFF;        // 低8位

    return *this;
}

// 写入code
CHHBusMsgWriter &CHHBusMsgWriter::writeCode() {
    m_buffer[m_pos++] = (m_msg.code >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = m_msg.code & 0xFF;        // 低8位

    return *this;
}

// 写入sessionid
CHHBusMsgWriter &CHHBusMsgWriter::writeSessionId() {
    m_buffer[m_pos++] = (m_msg.sessionid >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = m_msg.sessionid & 0xFF;        // 低8位

    return *this;
}

// 写入源地址
CHHBusMsgWriter &CHHBusMsgWriter::writeSrcAddr() {
    m_buffer[m_pos++] = m_msg.src_addr;
    return *this;
}

// 写入目的地址
CHHBusMsgWriter &CHHBusMsgWriter::writeDstAddr() {
    m_buffer[m_pos++] = m_msg.dst_addr;
    return *this;
}

// 写入消息体长度
CHHBusMsgWriter &CHHBusMsgWriter::writeBodyLen() {
    m_buffer[m_pos++] = (m_msg.bodylen >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = m_msg.bodylen & 0xFF;        // 低8位

    return *this;
}

// 写入消息体
CHHBusMsgWriter &CHHBusMsgWriter::writeBody() {
    memcpy(&m_buffer[m_pos], m_msg.body, m_msg.bodylen);
    m_pos += m_msg.bodylen;

    return *this;
}

// 计算并写入CRC16
CHHBusMsgWriter &CHHBusMsgWriter::writeCrc16() {
    // 计算当前已写入数据的CRC16(不包括CRC字段本身)
    m_msg.crc16 = crc16_calculate(m_buffer, m_pos);

    m_buffer[m_pos++] = (m_msg.crc16 >> 8) & 0xFF; // 高8位
    m_buffer[m_pos++] = m_msg.crc16 & 0xFF;        // 低8位

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

uint32_t CHHBusMsgWriter::getLength() const {
    return m_pos;
}

bool CHHBusMsgWriter::showMessage() {
    printf("message: ");
    for (uint32_t i = 0; i < m_pos; i++) {
        printf("%u ", m_buffer[i]);
    }
    printf("\r\n");
    return true;
}

// 构造函数
CHHBusMsgReader::CHHBusMsgReader(CHHBusMsg *msg, uint8_t *buffer, uint32_t buffer_len) :
    m_msg(msg), m_buffer_len(buffer_len), m_pos(0) {
    m_buffer = new uint8_t[m_buffer_len];
    memcpy(m_buffer, buffer, m_buffer_len);
}
// 析构函数
CHHBusMsgReader::~CHHBusMsgReader() {
    if (NULL != m_buffer) {
        delete[] m_buffer;
        m_buffer = NULL;
    }
}
// 读取并验证消息头
CHHBusMsgReader &CHHBusMsgReader::readHeader() {
    if (!m_valid || m_buffer_len < 2) {
        m_valid = false;
        return *this;
    }

    if (m_buffer[0] != 0xAA || m_buffer[1] != 0x55) {
        m_valid = false;
        return *this;
    }

    m_pos += 2;
    return *this;
}

// 读取code和sessionid的异或值(仅读取，不存储)
CHHBusMsgReader &CHHBusMsgReader::readCodeXorSessionId() {
    // 这里只是读取，实际验证在后面进行
    if (!m_valid || m_pos + 2 > m_buffer_len) {
        m_valid = false;
        return *this;
    }

    m_pos += 2;
    return *this;
}

// 读取code
CHHBusMsgReader &CHHBusMsgReader::readCode() {
    if (!m_valid || m_pos + 2 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_msg->code = (static_cast<uint16_t>(m_buffer[m_pos]) << 8) | m_buffer[m_pos + 1];
    m_pos += 2;

    return *this;
}

// 读取sessionid
CHHBusMsgReader &CHHBusMsgReader::readSessionId() {
    if (!m_valid || m_pos + 2 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_msg->sessionid = (static_cast<uint16_t>(m_buffer[m_pos]) << 8) | m_buffer[m_pos + 1];
    m_pos += 2;

    // 验证code和sessionid的异或值
    uint16_t xor_val = (static_cast<uint16_t>(m_buffer[2]) << 8) | m_buffer[3];
    if ((m_msg->code ^ m_msg->sessionid) != xor_val) {
        m_valid = false;
    }

    return *this;
}

// 读取源地址
CHHBusMsgReader &CHHBusMsgReader::readSrcAddr() {
    if (!m_valid || m_pos + 1 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_msg->src_addr = m_buffer[m_pos++];
    return *this;
}

// 读取目的地址
CHHBusMsgReader &CHHBusMsgReader::readDstAddr() {
    if (!m_valid || m_pos + 1 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_msg->dst_addr = m_buffer[m_pos++];
    return *this;
}

// 读取消息体长度
CHHBusMsgReader &CHHBusMsgReader::readBodyLen() {
    if (!m_valid || m_pos + 2 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_msg->bodylen = (static_cast<uint16_t>(m_buffer[m_pos]) << 8) | m_buffer[m_pos + 1];
    m_pos += 2;

    // 检查消息体长度是否合理
    if (m_pos + m_msg->bodylen + 2 > m_buffer_len) { // +2是为了CRC16
        m_valid = false;
    }

    return *this;
}

// 读取消息体
CHHBusMsgReader &CHHBusMsgReader::readBody() {
    if (!m_valid || !m_msg || m_msg->bodylen == 0) {
        return *this;
    }

    if (m_pos + m_msg->bodylen > m_buffer_len) {
        m_valid = false;
        return *this;
    }
    m_msg->body = new uint8_t[m_msg->bodylen];
    memcpy(m_msg->body, &m_buffer[m_pos], m_msg->bodylen);
    m_pos += m_msg->bodylen;

    return *this;
}

// 读取并验证CRC16
CHHBusMsgReader &CHHBusMsgReader::readCrc16() {
    if (!m_valid || m_pos + 2 > m_buffer_len || !m_msg) {
        m_valid = false;
        return *this;
    }

    m_msg->crc16 = (static_cast<uint16_t>(m_buffer[m_pos]) << 8) | m_buffer[m_pos + 1];
    m_pos += 2;

    // 验证CRC16
    uint16_t calculated_crc = crc16_calculate(m_buffer, m_pos - 2); // 减去CRC字段本身
    if (m_msg->crc16 != calculated_crc) {
        m_valid = false;
    }

    return *this;
}

// 完整解码过程
// 会为CHHBusMsg.body申请内存，需要外部释放
CHHBusMsgReader &CHHBusMsgReader::decode() {
    return readHeader()
        .readCodeXorSessionId()
        .readCode()
        .readSessionId()
        .readSrcAddr()
        .readDstAddr()
        .readBodyLen()
        .readBody()
        .readCrc16();
}

// 检查是否有效
bool CHHBusMsgReader::isValid() const {
    return m_valid;
}

void CHHBusMsgReader::showCHHBusMsg() {
    if (m_valid) {
        printf("CHHBusMsgReader: \r\n");
        printf("code: %x\r\n", m_msg->code);
        printf("sessionid: %x\r\n", m_msg->sessionid);
        printf("src_addr: %x\r\n", m_msg->src_addr);
        printf("dst_addr: %x\r\n", m_msg->dst_addr);
        printf("bodylen: %x\r\n", m_msg->bodylen);
        printf("body: ");
        for (int i = 0; i < m_msg->bodylen; i++) {
            printf("%x ", m_msg->body[i]);
        }
        printf("\r\n");
        printf("crc16: %u\r\n", m_msg->crc16);
    } else {
        printf("m_buffer is unlawful!\r\n");
    }
}