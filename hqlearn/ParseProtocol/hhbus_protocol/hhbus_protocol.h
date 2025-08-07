#ifndef HHBUS_PROTOCOL_H
#define HHBUS_PROTOCOL_H

#include <stdint.h>
#include <string.h>

#include "parseProtocolCommon.h"

class CHHBusMsgWriter {
private:
    CHHBusMsg m_msg;
    unsigned char *m_buffer;
    uint32_t m_pos;

public:
    CHHBusMsgWriter(CHHBusMsg msg);
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

    bool showMessage();
    // // 检查是否有效
    // bool isValid() const;
};

// 解码辅助类(读取)
class CHHBusMsgReader {
private:
    CHHBusMsg *m_msg;      // 传入CHHBusMsg
    uint8_t *m_buffer;     // 传入消息
    uint32_t m_buffer_len; // 传入消息长度
    uint32_t m_pos;        // 数组索引
    bool m_valid;          // 传入消息是否有效

public:
    // 构造函数
    CHHBusMsgReader(CHHBusMsg *msg, uint8_t *buffer, uint32_t buffer_len);

    // 析构函数
    ~CHHBusMsgReader();

    // 读取并验证消息头
    CHHBusMsgReader &readHeader();
    // 读取code和sessionid的异或值(仅读取，不存储)
    CHHBusMsgReader &readCodeXorSessionId();

    // 读取code
    CHHBusMsgReader &readCode();

    // 读取sessionid
    CHHBusMsgReader &readSessionId();

    // 读取源地址
    CHHBusMsgReader &readSrcAddr();

    // 读取目的地址
    CHHBusMsgReader &readDstAddr();

    // 读取消息体长度
    CHHBusMsgReader &readBodyLen();

    // 读取消息体
    CHHBusMsgReader &readBody();

    // 读取并验证CRC16
    CHHBusMsgReader &readCrc16();

    // 完整解码过程
    CHHBusMsgReader &decode();

    // 检查是否有效
    bool isValid() const;

    // 打印结构体数据
    void showCHHBusMsg();
};

#endif