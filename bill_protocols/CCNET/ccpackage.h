#ifndef CCPACKAGE_H
#define CCPACKAGE_H
#define POLYNOMIAL 0x08408

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "commands.h"

#include "../protocol_package.h"

typedef std::vector<unsigned char> vec_bytes;
typedef unsigned char byte;

/*
 * STRUCT OF CCNET PACK
 *
 * 1)
 * */

class CCNetPackage : public package
{
    // Message transmission start code [02H], fixed
    // Бит синхронизации (фиксированный)
    const byte m_sync = 0x02;

    // Peripheral Addresses - CashCode
    // Перифирийный адресс оборудования для CashCode (фіксований)
    const byte m_adr = 0x03;
 
public:   
    // Methods
    // Возвращает массив байтов пакета
    virtual vec_bytes get_bytes() override;  

    vec_bytes create_response(validator_commands_CCNET type);
};

#endif // CCPACKAGE_H
