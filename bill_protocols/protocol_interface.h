#ifndef PROTOCOL_INTERFACE_H
#define PROTOCOL_INTERFACE_H

#include <string>
#include <mutex>

#include "serialport.h"

enum bill_protocols_codes {
    CCNET,
    MAX_BPC,
};

struct validator_connection_settings {
    std::string m_port_name;
    int m_baud_rate;
    bill_protocols_codes m_protocol_code;
};

class bill_protocol {
protected:
    validator_connection_settings settings;

    serial_port *m_port = nullptr;
    bool m_is_connected;

    std::recursive_mutex* m_mutex = nullptr;

    bool thread_locked = false;
    void lock_thread();
    void unlock_thread();

    int m_cash_received;

public:
    int get_cash_received() const { return m_cash_received; }
    std::string get_port_name() const { return settings.m_port_name; }
    int get_baud_rate() const { return settings.m_baud_rate; }

    void set_mutex(std::recursive_mutex* const mutex);

    virtual bool connect_port(const std::string &port_name, const int baud_rate) = 0;
    virtual bool connect_validator() = 0;
    virtual bool connect(const std::string &port_name, const int baud_rate) = 0;
    virtual bool disconnect() = 0;

    virtual bool on() = 0;
    virtual bool off() = 0;

    virtual int listen_port(int &amount) = 0;

    virtual void hold() = 0;
    virtual void stack() = 0;
    virtual void unstack() = 0;

    virtual ~bill_protocol() {
        if (m_port) {
            std::cout << "START\n";
            delete m_port;
            std::cout << "DOUBLE START\n";
        }
    }

protected:
    bill_protocol() {}
};

class bill_interface_new {
protected:
    static bill_protocol* m_protocol;
    static bill_protocols_codes m_protocol_code;

private:
    static const std::vector<std::string> linux_port_names_default;
    static const std::vector<int> baud_rates_default;

public:
    bill_interface_new() = delete;

    static bill_protocol* get_instance(const bill_protocols_codes &protocol_code = m_protocol_code);
    static void destroy_instance();

    static bool try_call_port(const bill_protocols_codes &protocol_code,
                              const std::string &port_name,
                              const int baud_rate);
    static validator_connection_settings try_find_port(const std::vector<bill_protocols_codes> &protocols_codes,
                                     const std::vector<std::string> &port_names = linux_port_names_default,
                                     const std::vector<int> &baud_rates = baud_rates_default);

    static void reconnect_port(const std::string &port_name = get_instance()->get_port_name(),
                               const int baud_rate = get_instance()->get_baud_rate());
};

// Old logic
#include "CCNET/cashcodeprotocol.h"
#include "CCTalk/cctalk_protocol.h"

#include <iostream>

class bill_interface {
protected:
    cash_code_protocol CCValidator;
    std::string port_path;

    // time settings (sec)
    const int delay_power_up = 5;
    const int delay_start_listening = 10;
    const int delay_stop_listening = 10;

public:
    // try find port and return port path & errCode = 0, else errCode = -1
    static bool try_find_port(std::string &port_path_out);

    bill_interface(const std::string &port_path);

    // make one iteration
    int listen_port();

    int get_total_cash() const;

    ~bill_interface();
};

class bill_interface_static {
public:
    enum validators { 
        CCNET,
        CCTALK,  
    };
    static validators validator_type;

protected:
    static cash_code_protocol CCNetValidator;
    static cctalk_code_protocol CCTalkValidator;
    static std::string port_path;

    // time settings (sec)
    static const int delay_power_up = 5;
    static const int delay_start_listening = 10;
    static const int delay_stop_listening = 10;

public:
    // try find port and return port path & errCode = 0, else errCode = -1
    static bool try_find_port(std::string &port_path_out, validators &v);

    static void init(const std::string &port_path, const validators &validator);

    static void on();
    static void off();

    static int get_total_cash();

    static void destroy();

    // make one iteration
    static int listen_port();
    static bool listen_port_without_bill(int& amount);
    static void stack_bill();
    static void unstack_bill();
    static void hold_bill();
};

#endif // PROTOCOL_INTERFACE_H
