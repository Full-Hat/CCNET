#ifndef CCNET_H
#define CCNET_H

#include "cashcodeprotocol.h"

#include <iostream>

class port_CCNET {
protected:
    cash_code_protocol CCValidator;
    std::string port_path;

    // time settings (sec)
    const int delay_power_up = 5;
    const int delay_start_listening = 10;
    const int delay_stop_listening = 10;

public:
    // try find port and return port path & errCode = 0, else errCode = -1
    static int try_find_port(std::string &port_path_out);

    port_CCNET(const std::string &port_path);

    // make one iteration
    int listen_port();

    int get_total_cash() const;

    ~port_CCNET();
};

class port_CCNET_static {
protected:
    static cash_code_protocol CCValidator;
    static std::string port_path;

    // time settings (sec)
    static const int delay_power_up = 5;
    static const int delay_start_listening = 10;
    static const int delay_stop_listening = 10;

public:
    // try find port and return port path & errCode = 0, else errCode = -1
    static int try_find_port(std::string &port_path_out);

    static void init(const std::string &port_path);

    static void on();
    static void off();

    static int get_total_cash();

    static void destroy();

    // make one iteration
    static int listen_port();
};

#endif // CCNET_H
