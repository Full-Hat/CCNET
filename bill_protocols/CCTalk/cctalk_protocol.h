#ifndef CCTALK_CODE_PROTOCOL_H
#define CCTALK_CODE_PROTOCOL_H

#include <string>
#include <vector>
#include <iomanip>
#include <boost/thread.hpp>

#include <boost/chrono.hpp>

#include "commands.h"
#include "../serialport.h" 
#include "ccpackage.h"

class cctalk_code_protocol
{
private:
    std::string m_com_port_name;
    int m_last_error;
    bool m_disposed;
    bool m_is_connected;
    bool m_is_power_up;
    bool m_is_listening;
    bool m_is_enable_bills;
    bool m_return_bill;
    serial_port *com_port;
    CCTalkPackage pack;

    // Time-out ожидания ответа от считывателя
    const int POLL_TIMEOUT = 200;

    // Тайм-аут ожидания снятия блокировки
    const int EVENT_WAIT_HANDLER_TIMEOUT = 10000;

    vec_bytes ENABLE_BILL_TYPES_WITH_ESCROW = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    vec_bytes DISABLE_BILL_TYPES_WITH_ESCROW = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    vec_bytes SECURITY_CODE = { 0x00, 0x00, 0x00 };

    // Таблица кодов валют
    int cash_code_table(byte code);

public:
    cctalk_code_protocol();

    bool is_connected();

    int m_cash_received;

    vec_bytes send_command(validator_commands_CCTALK cmd, vec_bytes data = {});

    std::string find_port();

    // Инициализация и открытие COM-порта для работы с CashCode
    int connect_validator(std::string port_path);

    int power_up_validator(); 

    // Проверка ответов купюро приемника на наличие ошибок
    bool check_errors(vec_bytes &result) ; 

    void print_b(std::string msg, vec_bytes);
};

#endif // cctalk_code_protocol_H
 
