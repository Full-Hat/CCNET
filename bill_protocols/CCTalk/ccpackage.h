#ifndef CCPACKAGE_TALK_H
#define CCPACKAGE_TALK_H
#define POLYNOMIAL 0x08408

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "commands.h"

#include "../protocol_package.h"
#include "commands.h"

typedef std::vector<unsigned char> vec_bytes;
typedef unsigned char byte;

/*
 * STRUCT OF CCNET PACK
 *
 * 1) Destination address           host_addr=1 & dest_addr=2
 * 2) Length of data part
 * 3) CRC 16 bit - left part
 * 4) Header (command)
 * 5) [Data]
 * 6) CRC 16 bit - right part
 * */

class CCTalkPackage : public package
{
    // Message transmission start code [02H], fixed
    // Бит синхронизации (фиксированный)
    const byte m_dest_addr = 0x02;

    // Peripheral Addresses - CashCode
    // Перифирийный адресс оборудования для CashCode (фіксований)
    const byte m_adr = 0x01;
 
public:   
    // Methods
    // Возвращает массив байтов пакета
    virtual vec_bytes get_bytes() override;  

    vec_bytes create_response(validator_commands_CCTALK type);
};

#endif // CCPACKAGE_TALK_H
