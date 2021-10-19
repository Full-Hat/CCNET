#ifndef CASH_CODE_PROTOCOL_H
#define CASH_CODE_PROTOCOL_H

#include <string>
#include <vector>
#include <iomanip>
#include <boost/thread.hpp>

#include <boost/chrono.hpp>

#include "commands.h"
#include "../serialport.h"
#include "cashcodeerrors.h"
#include "ccpackage.h"

class cash_code_protocol
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
    CCNetPackage pack;

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
    cash_code_protocol();
    enum class bill_recieved_status { Accepted, Rejected };
    enum class bill_cassette_status { Inplace, Removed };

    int m_cash_received;

    bool is_connected();

    vec_bytes send_command(validator_commands_CCNET cmd, vec_bytes data = {});

    std::string find_port();

    // Инициализация и открытие COM-порта для работы с CashCode
    int connect_validator(std::string port_path);

    int power_up_validator();

    // Включение режима приема купюр
    int enable_validator();

    // Выключение режима приема купюр
    int disable_validator();

    // Проверка ответов купюро приемника на наличие ошибок
    bool check_errors(vec_bytes &result) ;

    // Enable sequence
    // Включение режима для приема купюр
    int enable_sequence(void);

    // Disable sequence
    // Выключение режима для приема купюр
    int disable_sequence(void);

    // Функция прослушка купюро приемника
    int validator_listener();
    bool validator_listener_without_bill(int& amount);

    void stack_bill(); 
    void unstack_bill();
    void hold_bill();

    void print_b(std::string msg, vec_bytes);
};

#endif // cash_code_protocol_H
