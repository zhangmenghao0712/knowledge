#include <stdio.h>
#include "hhbus_protocol.h"

// 编码示例
void codingExample() {
    CHHBusMsg message;
    uint32_t buffer_len = 20;
    uint8_t buffer[] = {1, 2, 3, 4, 5, 6, 7, 8, 10, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14};
    message.code = 0x12;
    message.sessionid = 0x1A;
    message.src_addr = 0xAB;
    message.dst_addr = 0xBA;
    message.body = new uint8_t[buffer_len];
    memcpy(message.body, buffer, buffer_len);
    message.bodylen = buffer_len;
    CHHBusMsgWriter msg_writer(message);
    msg_writer.encode();
    msg_writer.showMessage();
}

// 解码示例
void enCodingExample() {
    CHHBusMsg message;
    uint32_t buffer_len = 0;
    uint8_t buffer[] = {170, 85, 0, 8, 0, 18, 0, 26, 171, 186, 0, 20, 1, 2, 3, 4, 5, 6, 7, 8, 10, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 174, 229};
    buffer_len = sizeof(buffer);
    CHHBusMsgReader message_read(&message, buffer, buffer_len);
    message_read.decode().showCHHBusMsg();
    // message_read.showCHHBusMsg();
    CHHBusMsg_Destroy(&message);
}

int main() {
    codingExample();
    enCodingExample();
    return 0;
}