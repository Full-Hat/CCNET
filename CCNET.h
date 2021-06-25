#ifndef CCNET_H
#define CCNET_H

#include "cashcodeprotocol.h"

#include <iostream>

class port_CCNET {
protected:
    CashCodeProtocol CCValidator;
    std::string port_path;

    // time settings (sec)
    const int delay_power_up = 5;

public:
    // try find port and return port path & errCode = 0, else errCode = -1
    static int try_find_port(std::string &port_path_out);

    port_CCNET(const std::string &port_path);

    // make one iteration
    int listen_port();
    // make several iterations
    int listen_port_for(const int sec);

    ~port_CCNET();
};

#endif // CCNET_H
