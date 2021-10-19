#include "cctalk_protocol.h"

cctalk_code_protocol::cctalk_code_protocol() : m_cash_received(0) {
    this->com_port = new serial_port(200);
}

int cctalk_code_protocol::connect_validator(std::string port_path)
{
    try {
        if(this->com_port->start(port_path.c_str(), 9600)) {
            this->m_is_connected = true;
            std::cout << "COM CONNECTED!" << std::endl;
        }
        return 1;
    }
    catch(std::exception &e) {
        this->m_is_connected = false;
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}

vec_bytes cctalk_code_protocol::send_command(validator_commands_CCTALK cmd, vec_bytes data) {
    vec_bytes response;

    if(validator_commands_CCTALK::ACK == cmd || validator_commands_CCTALK::NAK == cmd) {
        vec_bytes bytes;
        if(cmd == validator_commands_CCTALK::ACK)
            bytes = pack.create_response(validator_commands_CCTALK::ACK);
        if(cmd == validator_commands_CCTALK::NAK)
            bytes = pack.create_response(validator_commands_CCTALK::NAK);

        if(bytes.size() != 0)
            this->com_port->write_data(bytes);
    }
    else {
        pack.set_cmd((byte)cmd);
        if(data.size() != 0)
            pack.set_data(data);

        this->com_port->write_data(pack.get_bytes());
        response = this->com_port->read_line();
    }
    return response;
}

std::string cctalk_code_protocol::find_port() {
    const int MAX_PORT_NUMBER = 31;
    for (int i = 0; i < MAX_PORT_NUMBER; ++i) {
        std::cout << "\n111111111111112\n";
        std::string port_path = std::string("/dev/ttyS") + std::to_string(i);
        std::cout << "\n11111111111111\n";
        int err = connect_validator(port_path.c_str());
        std::cout << "\n111111111111113\n";
        if(!err) // connection failed
        {
            continue;
        }

        std::cout << "\n111111111111114\n";

        // Try get device status
        vec_bytes result;

        std::cout << "\n111111111111115\n";

        if(!this->m_is_connected){
            continue;
        }

        std::cout << "\n111111111111116\n";

        print_b("ACK: ", this->send_command(validator_commands_CCTALK::ACK, {}));
        std::cout << "\n\n";
        usleep(5000000);

        print_b("STA: ", this->send_command(validator_commands_CCTALK::GET_STATUS, {}));
        std::cout << "\n\n";
        usleep(5000000);

        print_b("NUM: ", this->send_command(validator_commands_CCTALK::NUM, {}));
        std::cout << "\n\n";
        usleep(5000000);

        exit(0);

        std::cout << "Found device :: portPath=" << port_path << "\n";
        return port_path;
    }

    return "";
}

bool cctalk_code_protocol::check_errors(vec_bytes &result)
{
    if(result.size() == 0) {
        std::cout << "Empty result string :: ERROR\n";
        return true;
    }

    bool is_error = false;

    switch(result[3]) {
    case 0x30:
        this->m_last_error = 0x30;
        is_error = true;
        break;
    case 0x40:
        m_last_error = 0x40;
        is_error = true;
        break;
    case 0x42:
        m_last_error = 0x42;
        is_error = true;
        break;
    case 0x43:
        m_last_error = 0x43;
        is_error = true;
        break;
    case 0x44:
        m_last_error = 0x44;
        is_error = true;
        break;
    case 0x45:
        m_last_error = 0x45;
        is_error = true;
        break;
    case 0x46:
        m_last_error = 0x46;
        is_error = true;
        break;
    case 0x47:
        switch(result[4]) {
            case 0x50: m_last_error = 0x50; break;
            case 0x51: m_last_error = 0x51; break;
            case 0x52: m_last_error = 0x52; break;
            case 0x53: m_last_error = 0x53; break;
            case 0x54: m_last_error = 0x54; break;
            case 0x55: m_last_error = 0x55; break;
            case 0x56: m_last_error = 0x56; break;
            case 0x5F: m_last_error = 0x5F; break;
        default:
            m_last_error = 0x47;
        }
        is_error = true;
        break;
    case 0x1C:
        switch (result[4]) {
            case 0x60: m_last_error = 0x60; break;
            case 0x61: m_last_error = 0x61; break;
            case 0x62: m_last_error = 0x62; break;
            case 0x63: m_last_error = 0x63; break;
            case 0x64: m_last_error = 0x64; break;
            case 0x65: m_last_error = 0x65; break;
            case 0x66: m_last_error = 0x66; break;
            case 0x67: m_last_error = 0x67; break;
            case 0x68: m_last_error = 0x68; break;
            case 0x69: m_last_error = 0x69; break;
            case 0x6A: m_last_error = 0x6A; break;
            case 0x6C: m_last_error = 0x6C; break;
            case 0x6D: m_last_error = 0x6D; break;
            case 0x92: m_last_error = 0x92; break;
            case 0x93: m_last_error = 0x93; break;
            case 0x94: m_last_error = 0x94; break;
            case 0x95: m_last_error = 0x95; break;
        default:
            m_last_error = 0x1C;
            break;
        }
        is_error = true;
        break;
    }

    if(is_error)
        std::cout << "ERROR: " << std::setw(2) << std::setfill('0') << std::hex << m_last_error << std::endl;
    return is_error;
}

void cctalk_code_protocol::print_b(std::string msg, vec_bytes data){
    std::cout << msg;
    for(auto byte : data)
        std::cout << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)byte;
    std::cout << std::endl;
}
