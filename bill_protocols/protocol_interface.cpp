#include "protocol_interface.h"

// New logic

#include "CCNET/protocol.h"

void bill_protocol::set_mutex(std::recursive_mutex* const mutex) {
    m_mutex = mutex;
}

void bill_protocol::lock_thread() {
    if (m_mutex) {
        m_mutex->lock();
        //thread_locked = true;
    }
}

void bill_protocol::unlock_thread() {
    if (m_mutex) { // thread locked
        m_mutex->unlock();
        //thread_locked = false;
    }
}

bill_protocol* bill_interface_new::m_protocol = nullptr;
bill_protocols_codes bill_interface_new::m_protocol_code = bill_protocols_codes::MAX_BPC;

const std::vector<std::string> bill_interface_new::linux_port_names_default = {
    "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4",
    "/dev/ttyS5", "/dev/ttyS6", "/dev/ttyS7", "/dev/ttyS8", "/dev/ttyS9",
    "/dev/ttyS10", "/dev/ttyS11", "/dev/ttyS12", "/dev/ttyS13", "/dev/ttyS14",
    "/dev/ttyS15", "/dev/ttyS16", "/dev/ttyS17", "/dev/ttyS18", "/dev/ttyS19",
    "/dev/ttyS20", "/dev/ttyS21", "/dev/ttyS22", "/dev/ttyS23", "/dev/ttyS24",
    "/dev/ttyS25", "/dev/ttyS26", "/dev/ttyS27", "/dev/ttyS28", "/dev/ttyS29",
    "/dev/ttyS30", "/dev/ttyS31"
};

const std::vector<int> bill_interface_new::baud_rates_default = {
    19200, 9600
};

bill_protocol* bill_interface_new::get_instance(const bill_protocols_codes &protocol_code) {
    if (m_protocol && m_protocol_code != protocol_code) {
        delete m_protocol;
    }
    else if (m_protocol) {
        return m_protocol;
    }

    switch (protocol_code) {
    case bill_protocols_codes::CCNET:
    {
        m_protocol = new CCNET_protocol();
        m_protocol_code = protocol_code;
        return m_protocol;
    }
    }

    std::cout << "ERROR :: Can't find required protocol \n";
    return nullptr;
}

void bill_interface_new::destroy_instance() {
    delete m_protocol;
    m_protocol = nullptr;
    m_protocol_code = bill_protocols_codes::MAX_BPC;
}

bool bill_interface_new::try_call_port(const bill_protocols_codes &protocol_code,
                                       const std::string &port_name,
                                       const int baud_rate) {
    bill_protocol* protocol = bill_interface_new::get_instance(protocol_code);

    std::cout << "Try call port, portname=[" << port_name << "]\n";
    bool res = protocol->connect(port_name, baud_rate);
    if (!res) {
        std::cout << "Bad connection on port [" << port_name << "] \n";
        destroy_instance();
        std::cout << "Instance destroyed\n";
        return false;
    }

    std::cout << "Successful call! portName=[" << port_name << "]\n";
    destroy_instance();
    return true;
}

validator_connection_settings bill_interface_new::try_find_port(const std::vector<bill_protocols_codes> &protocols_codes,
                          const std::vector<std::string> &port_names,
                          const std::vector<int> &baud_rates) {
    for (const bill_protocols_codes &pc : protocols_codes) {
        for (const std::string &pn : port_names) {
            for (const int br : baud_rates) {
                std::cout << "Try call [pc=[" << (int)pc << "], pn=[" << pn << "], br=[" << br << "]]\n";
                bool res = try_call_port(pc, pn, br);
                if (res) {
                    std::cout << "Port found! [pc=[" << (int)pc << "], pn=[" << pn << "], br=[" << br << "]]\n";
                    validator_connection_settings settings;
                    settings.m_protocol_code = pc;
                    settings.m_port_name = pn;
                    settings.m_baud_rate = br;
                    return settings;
                }
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        }
    }

    std::cout << "Can't identify any port\n";
    validator_connection_settings settings;
    settings.m_port_name = "";
    return settings;
}

void bill_interface_new::reconnect_port(const std::string &port_name, const int baud_rate) {
    bill_protocols_codes code = bill_interface_new::m_protocol_code;
    destroy_instance();
    while (!get_instance(code)->connect(port_name, baud_rate)) {
        destroy_instance();
        std::cout << "Can't reconnect, try again..\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Yes! Get connection\n";
    get_instance(m_protocol_code)->on();
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Reconnection done!\n";
}

// Old logic

bool bill_interface::try_find_port(std::string &port_path_out) {
    cash_code_protocol CCValidator;

    std::cout << std::string(10, '~') << " Try find port " << std::string(10, '~') << "\n";
    port_path_out = CCValidator.find_port();
    if(port_path_out == "")
    {
        return false;
    }
    std::cout << "Port found! portPath=" << port_path_out << "\n";
    std::cout << std::string(10, '~') << " \tEnd\t " << std::string(10, '~') << "\n\n";

    return true;
}

bill_interface::bill_interface(const std::string &port_path_) :
    port_path { port_path_ } {
    // power up validator
    CCValidator.connect_validator(port_path.c_str());
    CCValidator.power_up_validator();
    std::this_thread::sleep_for(std::chrono::seconds(delay_power_up));

    // start listening
    CCValidator.enable_sequence();
    std::this_thread::sleep_for(std::chrono::seconds(delay_start_listening));
}

int bill_interface::listen_port() {
    return CCValidator.validator_listener();
}

int bill_interface::get_total_cash() const {
    return CCValidator.m_cash_received;
}

bill_interface::~bill_interface() {
    CCValidator.disable_sequence();
    std::this_thread::sleep_for(std::chrono::seconds(delay_stop_listening));
}

/// Static version

cash_code_protocol bill_interface_static::CCNetValidator;
std::string bill_interface_static::port_path;

// time settings (sec)
const int bill_interface_static::delay_power_up;
const int bill_interface_static::delay_start_listening;
const int bill_interface_static::delay_stop_listening;

bill_interface_static::validators bill_interface_static::validator_type;

bool bill_interface_static::try_find_port(std::string &port_path_out, validators &v) {
    // CCNET
    cash_code_protocol CCNetValidator;
    v = validators::CCNET;
    std::cout << std::string(10, '~') << " Try find port " << std::string(10, '~') << "\nprotocol::CCNET\n";
    port_path_out = CCNetValidator.find_port();

    if (port_path_out != "") {
        std::cout << "Port found! portPath=" << port_path_out << "\n";
        std::cout << std::string(10, '~') << " \tEnd\t " << std::string(10, '~') << "\n\n";
        return true;
    }
    // CCNET _ END

    /*// CCTALK
    cctalk_code_protocol CCTalkValidator;
    v = validators::CCTALK;
    std::cout << std::string(10, '~') << " Try find port " << std::string(10, '~') << "\nprotocol::CCTALK\n";
    port_path_out = CCTalkValidator.find_port();

    if (port_path_out != "") {
        std::cout << "Port found! portPath=" << port_path_out << "\n";
        std::cout << std::string(10, '~') << " \tEnd\t " << std::string(10, '~') << "\n\n";
        return true;
    }
    // CCTALK _ END*/

    return false;
}

void bill_interface_static::init(const std::string &port_path, const validators &validator) {
    using port = bill_interface_static;

    port::port_path = port_path;
    port::validator_type = validator;

    // power up validator
    switch (port::validator_type) {
    case validators::CCNET:
        port::CCNetValidator.connect_validator(port::port_path.c_str());
        port::CCNetValidator.power_up_validator();

        break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(port::delay_power_up));
}

void bill_interface_static::destroy() {
    using port = bill_interface_static;

    switch (port::validator_type) {
    case validators::CCNET:
        port::CCNetValidator.disable_sequence();

        break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(port::delay_stop_listening));
}

int bill_interface_static::get_total_cash() {
    switch (bill_interface_static::validator_type) {
    case validators::CCNET:
        return bill_interface_static::CCNetValidator.m_cash_received;
    }

    return 0;
}

int bill_interface_static::listen_port() {
    switch (bill_interface_static::validator_type) {
    case validators::CCNET:
        return bill_interface_static::CCNetValidator.validator_listener();
    }

    return 0;
}

bool bill_interface_static::listen_port_without_bill(int& amount) {
    switch (bill_interface_static::validator_type) {
    case validators::CCNET:
        return bill_interface_static::CCNetValidator.validator_listener_without_bill(amount);
    }

    return false;
}

void bill_interface_static::stack_bill() {
    switch (bill_interface_static::validator_type) {
    case validators::CCNET:
        bill_interface_static::CCNetValidator.stack_bill();
    }
}

void bill_interface_static::unstack_bill() {
    switch (bill_interface_static::validator_type) {
    case validators::CCNET:
        bill_interface_static::CCNetValidator.unstack_bill();
    }
}

void bill_interface_static::hold_bill() {
    switch (bill_interface_static::validator_type) {
    case validators::CCNET:
        bill_interface_static::CCNetValidator.hold_bill();
    }
}

void bill_interface_static::on() {

    using port = bill_interface_static;

    // start listening
    switch (port::validator_type) {
    case validators::CCNET:
        port::CCNetValidator.enable_sequence();

        break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(port::delay_start_listening));
}

void bill_interface_static::off() {
    using port = bill_interface_static;

    switch (port::validator_type) {
    case validators::CCNET:
        port::CCNetValidator.disable_sequence();

        break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(port::delay_stop_listening));
}
