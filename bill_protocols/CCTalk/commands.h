#ifndef COMMANDS_CCTALK_H
#define COMMANDS_CCTALK_H

enum class validator_commands_CCTALK : unsigned char {
    ACK = 0x00,
    NAK = 0x05, // 0xFF by CCNET
    POLL = 0xFE, // must return ACK
    GET_STATUS = 0xF8, // HEADER = 248
    RESET = 0x01, // HEADER = 1

    BUSY = 0x06,
    NUM = 0xF2,
};

#endif // COMMANDS_TALK_H
