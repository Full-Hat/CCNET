#include "../protocol_interface.h"

#include <string>
#include <vector>
#include <iomanip>
#include <boost/thread.hpp>

#include <boost/chrono.hpp>

#include <thread>
#include <chrono>

#include "commands.h"
#include "../serialport.h"
#include "cashcodeerrors.h"

class CCNET_protocol : public bill_protocol {
protected:
    CCNetPackage m_pack;

    bool m_is_power_up;
    int m_last_error;

    vec_bytes send_command(validator_commands_CCNET cmd, vec_bytes data = {});

    void print_b(std::string msg, vec_bytes);
    bool check_errors(vec_bytes &result) ;

    int cash_code_table(byte code);

    vec_bytes SECURITY_CODE = { 0x00, 0x00, 0x00 };
    vec_bytes ENABLE_BILL_TYPES_WITH_ESCROW = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    vec_bytes DISABLE_BILL_TYPES_WITH_ESCROW = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
public:
    CCNET_protocol() {}

    virtual bool connect_port(const std::string &port_name, const int baud_rate) override;
    virtual bool connect_validator() override;
    virtual bool connect(const std::string &port_name, const int baud_rate) override;

    virtual bool disconnect() override;

    virtual bool on() override;
    virtual bool off() override;

    //!
    //! \brief listen_port
    //! \return bill sum (>0) || nothing happend (==0) || casset out possition (==404) || wait for command HOLD or STACK (==-1)
    //! \title if funct return -1 -> you must send HOLD or STACK without any delay
    //! \title make one iteration and listen for any action from bill validator
    //!
    virtual int listen_port(int &amount) override;

    virtual void hold() override;
    virtual void stack() override;
    virtual void unstack() override;

    virtual ~CCNET_protocol();
};

