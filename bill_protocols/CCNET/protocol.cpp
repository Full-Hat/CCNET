#include "protocol.h"

vec_bytes CCNET_protocol::send_command(validator_commands_CCNET cmd, vec_bytes data) {
    vec_bytes response;

    if(validator_commands_CCNET::ACK == cmd || validator_commands_CCNET::NAK == cmd) {
        vec_bytes bytes;
        if(cmd == validator_commands_CCNET::ACK)
            bytes = m_pack.create_response(validator_commands_CCNET::ACK);
        if(cmd == validator_commands_CCNET::NAK)
            bytes = m_pack.create_response(validator_commands_CCNET::NAK);

        if(bytes.size() != 0)
            this->m_port->write_data(bytes);
    }
    else {
        m_pack.set_cmd((byte)cmd);
        if(data.size() != 0)
            m_pack.set_data(data);

        this->m_port->write_data(m_pack.get_bytes());
        response = this->m_port->read_line();
    }
    return response;
}

void CCNET_protocol::print_b(std::string msg, vec_bytes data){
    std::cout << msg;
    for(auto byte : data)
        std::cout << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)byte;
    std::cout << std::endl;
}

bool CCNET_protocol::check_errors(vec_bytes &result) {
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

int CCNET_protocol::cash_code_table(byte code) {
    if(code == 0x03)
        return 50;
    else if(code == 0x04)
        return 100;
    else if(code == 0x0C)
        return 200;
    else if(code == 0x05)
        return 500;
    else if(code == 0x06)
        return 1000;
    return 0;
}

bool CCNET_protocol::connect_port(const std::string &port_name, const int baud_rate) {
    lock_thread();

    settings.m_port_name = port_name;
    settings.m_baud_rate = baud_rate;
    settings.m_protocol_code = CCNET;

    if (m_port == nullptr) {
        m_port = new serial_port(200);
    }

    try {
        std::cout << "Prepaer for start\n";
        if(this->m_port->start(settings.m_port_name.c_str(), baud_rate)) { // 9600 / 19200
            this->m_is_connected = true;
            std::cout << "SERIAL PORT CONNECTED!" << std::endl;
        }
        unlock_thread();
        return false;
    }
    catch(std::exception &e) {
        this->m_is_connected = false;
        std::cout << "Error: " << e.what() << std::endl;
    }

    unlock_thread();
    return true;
}

bool CCNET_protocol::connect_validator() {
    lock_thread();
    m_last_error = 0;

    vec_bytes result;

    if(!this->m_is_connected) {
        std::cout << "Port isn't connected\n";
        unlock_thread();
        return 202;
    }

    // Power UP
    result = this->send_command(validator_commands_CCNET::POLL);
    print_b("POWER UP: ", result);

    // Check result for errors
    if(this->check_errors(result)){
        std::cout << "\n\nError in check result value\n\n";
        this->send_command(validator_commands_CCNET::NAK);
        unlock_thread();
        return 404;
    }

    // Если CashCode вернул в 4 байте 0х19 значит он уже включен
    if(result[3] == 0x19) {
        std::cout << "Validator ready to work!" << std::endl;
        unlock_thread();
        return 0;
    }

    // Если все хорошо, отправляет команду подтверждения
    this->send_command(validator_commands_CCNET::ACK);

    // RESET
    result = this->send_command(validator_commands_CCNET::RESET);
     print_b("RESET: ", result);

    // Если купюроприемник не ответил сигналом ACK
    if(result[3] != 0x00) {
        m_last_error = 0x00;
        unlock_thread();
        return m_last_error;
    }


    // Опрос купюроприемника процедура инициализации
    result = this->send_command(validator_commands_CCNET::POLL);
    print_b("POLL: ", result);
    if(this->check_errors(result)){
        this->send_command(validator_commands_CCNET::NAK);
    }
    this->send_command(validator_commands_CCNET::ACK);


    // Получение статуса GET_STATUS
    result = this->send_command(validator_commands_CCNET::GET_STATUS);
    print_b("GET_STATUS: ", result);

    // Команда GET STATUS возвращает 6 байт ответа. Если все равны 0, то статус ok и можно работать дальше, иначе ошибка
    if(result[3] != 0x00 || result[4] != 0x00 || result[5] != 0x00 ||
       result[6] != 0x00 || result[7] != 0x00 || result[8] != 0x00 )
    {
        this->m_last_error = 0x70;
    }

    // Подтверждает если все хорошо
    this->send_command(validator_commands_CCNET::ACK);

    // SET_SECURITY (в тестовом примере отправояет 3 байта (0 0 0)
    result = this->send_command(validator_commands_CCNET::SET_SECURITY, SECURITY_CODE);
    print_b("SET_SECURITY: ", result);

    // Если не получили от купюроприемника сигнал ACK
    if(result[3] != 0x00) {
        m_last_error = 0x00;
        unlock_thread();
        return this->m_last_error;
    }

    // IDENTIFICATION
    result = this->send_command(validator_commands_CCNET::IDENTIFICATION);
    this->send_command(validator_commands_CCNET::ACK);
    print_b("IDENTIFICATION: ", result);

    // Опрашиваем купюроприемник должны получить команду INITIALIZE
    result = this->send_command(validator_commands_CCNET::POLL);
    print_b("INITIALIZE: ", result);

    if(this->check_errors(result)) {
        this->send_command(validator_commands_CCNET::NAK);
    }

    this->send_command(validator_commands_CCNET::ACK);

    // POLL Должны получить команду UNIT DISABLE
    result = this->send_command(validator_commands_CCNET::POLL);
    print_b("UNIT DISABLE: ", result);

    if(this->check_errors(result)) {
        std::cout << "UNIT DISABLE :: ERROR\n";
        this->send_command(validator_commands_CCNET::NAK);
        unlock_thread();
        return m_last_error;
    }

    this->send_command(validator_commands_CCNET::ACK);
    this->m_is_power_up = true;

    //std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "INIT DONE :: ERROR VALUE=[" << m_last_error << "]\n";
    unlock_thread();
    return this->m_last_error;
}

bool CCNET_protocol::connect(const std::string &port_name, const int baud_rate) {
    lock_thread();

    std::cout << "Try connect\n";
    std::cout << "\tPort connection\n";
    bool res = connect_port(port_name, baud_rate);
    if (res) {
        std::cout << "Err in CCNET connect() :: connect_port :: terminate \n";
        unlock_thread();
        return false;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "\tValidator connection\n";
    res = connect_validator();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    if (res) {
        std::cout << "Err in CCNET connect() :: connect_validator :: terminate \n";
        disconnect();
        std::cout << "Dissconnected\n";
        unlock_thread();
        return false;
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));

    unlock_thread();
    return true;
}

bool CCNET_protocol::disconnect() {
    lock_thread();
    try {
        vec_bytes result;
        if(!this->m_is_connected) {
            std::cout << "COM Port is not open!" << std::endl;
            this->m_port->stop();
            std::cout << "Stopped port!\n";
            unlock_thread();
            return true;
        }

        result = this->send_command(validator_commands_CCNET::ENABLE_BILL_TYPES, DISABLE_BILL_TYPES_WITH_ESCROW);
    }
    catch (...) {
    }

    this->m_port->stop();
    unlock_thread();
    return true;
}

bool CCNET_protocol::on() {
    lock_thread();

    vec_bytes result;
    if(!this->m_is_connected) {
        std::cout << "COM Port is not open!" << std::endl;
        unlock_thread();
        return false;
    }

    this->send_command(validator_commands_CCNET::ENABLE_BILL_TYPES, ENABLE_BILL_TYPES_WITH_ESCROW);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    unlock_thread();
    return true;
}

bool CCNET_protocol::off() {
    lock_thread();

    vec_bytes result;
    if(!this->m_is_connected) {
        std::cout << "COM Port is not open!" << std::endl;
        unlock_thread();
        return false;
    }

    result = this->send_command(validator_commands_CCNET::ENABLE_BILL_TYPES, DISABLE_BILL_TYPES_WITH_ESCROW);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    unlock_thread();
    return true;
}

int CCNET_protocol::listen_port(int &amount) {
    //std::cout << "CCNET_protocol::listen_port\n";
    lock_thread();
    //std::cout << "CCNET_protocol::listen_ported\n";

    vec_bytes result;

    // POLL
    result = this->send_command(validator_commands_CCNET::POLL);

    amount = this->cash_code_table(result[4]);

    if(result.size() < 3) {
        this->send_command(validator_commands_CCNET::NAK);
        unlock_thread();
        return false;
    }

    if(result[3] == 0x42) {
        //m_is_connected = true;
        //while (true) {
            std::cout << "DROP CASSETTE OUT OF POSITION" << std::endl;
            /*std::this_thread::sleep_for(std::chrono::seconds(1));
            try {
                off();
                disconnect();
            }
            catch (...) {
                std::cout << "Can't dissconnect\n";
            }

            std::cout << "Connect\n";
            if (connect(m_port_name)) {
                "Yes\n";
                on();
                break;
            }
            std::cout << "Return\n";

            std::this_thread::sleep_for(std::chrono::seconds(5));
        }*/
        unlock_thread();
        return 42;
    }

    // ACK
    if(result[3] == 0x00) {
        this->send_command(validator_commands_CCNET::POLL);
        unlock_thread();
        return false;
    }

    // IDLING
    if(result[3] == 0x14) {
        this->send_command(validator_commands_CCNET::ACK);
        unlock_thread();
        return false;
    }

    // Проверка на ошибки, если были обнаружены ошибки посылаем команду NAK
    if(this->check_errors(result)) {
        this->send_command(validator_commands_CCNET::NAK);
        unlock_thread();
        return false;
    }

    // Если 4 байт не равен 0х14 (IDLING)
    if(result[3] != 0x14) {
        if(result[3] == 0x15) {
            // ACCEPTING 0x15
            this->send_command(validator_commands_CCNET::ACK);
        }
        else if(result[3] == 0x1C) {
            // ESCROW POSITION
            // Если 0x1C (Rejection), купюроприемник скорее всего не распознал купюру

            std::cout << "0x1C :: unknown... :: can't identify\n";
            this->send_command(validator_commands_CCNET::ACK);
        }
        else if(result[3] == 0x80) {
            // ESCROW POSITION
            // Купюра распознана и находится в отсеке хранения
            std::cout << "0x80 :: has pushed into stack\n";
            this->send_command(validator_commands_CCNET::ACK);

            //// STACK
            //result = this->send_command(validator_commands_CCNET::STACK);
            unlock_thread();
            return -1;
        }
        else if(result[3] == 0x17) {
            // STACKING 0x17 отправка купюры в стек
            this->send_command(validator_commands_CCNET::ACK);
        }
        else if(result[3] == 0x81) {
            // Bill stacked 0x81
            // купюра попала в стек
            this->send_command(validator_commands_CCNET::ACK);
            print_b("Bill added :: code=", result);
            m_cash_received += this->cash_code_table(result[4]);
            std::cout << "\nCASH: " << std::dec << this->cash_code_table(result[4]) << std::endl;
            print_b("STACKED: ", result);
            unlock_thread();
            return this->cash_code_table(result[4]);
        }
        else if(result[3] == 0x18) {
            // RETURNING
            // Если четвертый бит 18h, следовательно идет процесс возврата
            this->send_command(validator_commands_CCNET::ACK);
        }
        else if(result[3] == 0x82) {
            // BILL RETURNING
            // Если четвертый бит 82h, следовательно купюра возвращена
            this->send_command(validator_commands_CCNET::ACK);
        }
    }

    unlock_thread();
    return false;
}

void CCNET_protocol::hold() {
    lock_thread();
    this->send_command(validator_commands_CCNET::HOLD);
    unlock_thread();
}

void CCNET_protocol::stack() {
    lock_thread();
    this->send_command(validator_commands_CCNET::STACK);
    unlock_thread();
}

void CCNET_protocol::unstack() {
    lock_thread();
    this->send_command(validator_commands_CCNET::RETURN);
    unlock_thread();
}

CCNET_protocol::~CCNET_protocol() {
    lock_thread();
    disconnect();
    unlock_thread();
}
