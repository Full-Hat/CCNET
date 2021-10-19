#include "protocol_package.h"

package::package() {}

package::package(byte cmd, vec_bytes data) : m_cmd{cmd}, m_data{data} {
}

void package::set_cmd(byte cmd)
{
    this->m_cmd = cmd;
}

byte package::get_cmd()
{
    return this->m_cmd;
}

vec_bytes package::get_data()
{
    return this->m_data;
}

void package::set_data(vec_bytes data)
{
    this->m_data = data;
}

int package::get_length() {
    return this->m_data.size() + 6;
}

vec_bytes package::getCRC16(vec_bytes buf_data, int size_data) {
    unsigned int tmpCRC, CRC;
    unsigned char j;
    CRC = 0;
    for(int i=0; i < size_data; i++) {
        tmpCRC = CRC ^ buf_data[i];
        for(j=0; j < 8; j++) {
            if(tmpCRC & 0x0001) {
                tmpCRC >>= 1; tmpCRC ^= POLYNOMIAL;
            }
            else {
                tmpCRC >>= 1;
            }
        }
        CRC = tmpCRC;
    }

    vec_bytes v_CRC;
    v_CRC.push_back((unsigned char)CRC & 0xFF);
    v_CRC.push_back((unsigned char)((CRC >> 8) & 0xFF));

    return v_CRC;
}
