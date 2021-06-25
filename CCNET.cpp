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
