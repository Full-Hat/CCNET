#include "CCNET.h"

int port_CCNET::try_find_port(std::string &port_path_out)
{
    cash_code_protocol CCValidator;

    std::cout << std::string(10, '~') << " Try find port " << std::string(10, '~') << "\n";
    port_path_out = CCValidator.find_port();
    if(port_path_out == "")
    {
        return -1;
    }
    std::cout << "Port found! portPath=" << port_path_out << "\n";
    std::cout << std::string(10, '~') << " \tEnd\t " << std::string(10, '~') << "\n\n";

    return 0;
}

port_CCNET::port_CCNET(const std::string &port_path_) :
    port_path { port_path_ }
{
    // power up validator
    CCValidator.connect_validator(port_path.c_str());
    CCValidator.power_up_validator();
    std::this_thread::sleep_for(std::chrono::seconds(delay_power_up));

    // start listening
    CCValidator.enable_sequence();
    std::this_thread::sleep_for(std::chrono::seconds(delay_start_listening));
}

int port_CCNET::listen_port()
{
    return CCValidator.validator_listener();
}

int port_CCNET::get_total_cash() const
{
    return CCValidator.m_cash_received;
}

port_CCNET::~port_CCNET()
{
    CCValidator.disable_sequence();
    std::this_thread::sleep_for(std::chrono::seconds(delay_stop_listening));
}

/// Static version

cash_code_protocol port_CCNET_static::CCValidator;
std::string port_CCNET_static::port_path;

// time settings (sec)
const int port_CCNET_static::delay_power_up;
const int port_CCNET_static::delay_start_listening;
const int port_CCNET_static::delay_stop_listening;

void port_CCNET_static::init(const std::string &port_path)
{
    using port = port_CCNET_static;

    port::port_path = port_path;
    // power up validator
    port::CCValidator.connect_validator(port::port_path.c_str());
    port::CCValidator.power_up_validator();
    std::this_thread::sleep_for(std::chrono::seconds(port::delay_power_up));
}

void port_CCNET_static::destroy()
{
    using port = port_CCNET_static;

    port::CCValidator.disable_sequence();
    std::this_thread::sleep_for(std::chrono::seconds(port::delay_stop_listening));
}

int port_CCNET_static::get_total_cash()
{
    return port_CCNET_static::CCValidator.m_cash_received;
}

int port_CCNET_static::listen_port()
{
    return port_CCNET_static::CCValidator.validator_listener();
}

void port_CCNET_static::on()
{
    using port = port_CCNET_static;

    // start listening
    port::CCValidator.enable_sequence();
    std::this_thread::sleep_for(std::chrono::seconds(port::delay_start_listening));
}

void port_CCNET_static::off()
{
    using port = port_CCNET_static;

    port::CCValidator.disable_sequence();
    std::this_thread::sleep_for(std::chrono::seconds(port::delay_stop_listening));
}
