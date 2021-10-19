#include "ccpackage.h"

vec_bytes CCNetPackage::get_bytes()
{
    vec_bytes buffer;

    // Байт 1: Флаг синхронизации
    buffer.push_back(this->m_sync);

    // Байт 2: адрес устройства
    buffer.push_back(this->m_adr);

    // Байт 3: длина пакета
    // рассчитаем длину пакета
    int length = this->get_length();

    // Если длина пакета вместе с байтами SYNC, ADR, LNG, CRC, CMD  больше 250
    if(length > 250) {
        // то делаем байт длины равный 0, а действительная длина сообщения будет в DATA
        buffer.push_back(0x00);
    } else {
        // DEC 2 HEX conversion
        buffer.push_back((unsigned char)length & 0xFF);
    }

    // Байт 4: Команда
    buffer.push_back(this->m_cmd);

    // Байт 5-N: Данные
    if(length > 0)
        for(unsigned char& c : this->m_data)
            buffer.push_back(c);

    // Последний 2 Байта это CRC
    vec_bytes CRC = this->getCRC16(buffer, buffer.size());
    buffer.insert(std::end(buffer), std::begin(CRC), std::end(CRC));

    return buffer;
}

vec_bytes CCNetPackage::create_response(validator_commands_CCNET type)
{
    // Буффер пакета (без 2-х байт CRC). Первые четыре байта это SYNC, ADR, LNG, CMD
    vec_bytes buffer;

    // Байт 1: Флаг синхронизации
    buffer.push_back(this->m_sync);

    // Байт 2: адрес устройства
    buffer.push_back(this->m_adr);

    // Байт 3: длина пакета, всегда 6
    buffer.push_back(0x06);

    // Байт 4: Данные ответа
    if(type == validator_commands_CCNET::ACK)
        buffer.push_back(0x00);
    else if(type == validator_commands_CCNET::NAK)
        buffer.push_back(0xFF);

    vec_bytes CRC = this->getCRC16(buffer, buffer.size());
    buffer.insert(std::end(buffer), std::begin(CRC), std::end(CRC));

    return buffer;
}
