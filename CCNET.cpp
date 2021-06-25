#include "CCNET.h"

int port_CCNET::try_find_port(std::string &port_path_out)
{
    CashCodeProtocol CCValidator;

    std::cout << std::string(10, '~') << " Try find port " << std::string(10, '~') << "\n";
    port_path_out = CCValidator.FindPort();
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
    CCValidator.ConnectValidator(port_path.c_str());
    CCValidator.PowerUpValidator();
    std::this_thread::sleep_for(std::chrono::seconds(delay_power_up));
}

int port_CCNET::listen_port()
{

}
