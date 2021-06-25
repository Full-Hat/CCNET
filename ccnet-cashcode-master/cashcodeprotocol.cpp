#include "cashcodeprotocol.h"
#include <stdio.h>

/*
 * Возвращает номинал принятой/распознаной и отправленной в стек купюры.
 */
int cash_code_protocol::cash_code_table(byte code) {
    if(code == 0x00)
        return 50;
    else if(code == 0x01)
        return 100;
    else if(code == 0x02)
        return 200;
    else if(code == 0x03)
        return 500;
    else if(code == 0x04)
        return 1000;
    else if(code == 0x05)
        return 5000;
    return 0;
}

cash_code_protocol::cash_code_protocol() : m_cash_received(0) {
    this->com_port = new serial_port(200);
}

int cash_code_protocol::enable_sequence() {
    vec_bytes result;
    if( ! this->m_is_connected) {
        std::cout << "COM Port is not open!" << std::endl;
        return 1;
    }

    this->send_command(validator_commands::ENABLE_BILL_TYPES, ENABLE_BILL_TYPES_WITH_ESCROW);
    return 0;
}

int cash_code_protocol::disable_sequence() {
    vec_bytes result;
    if( ! this->m_is_connected) {
        std::cout << "COM Port is not open!" << std::endl;
        return 1;
    }

    result = this->send_command(validator_commands::ENABLE_BILL_TYPES, DISABLE_BILL_TYPES_WITH_ESCROW);
    return 0;
}

int cash_code_protocol::validator_listener() {
    vec_bytes result;

    // POLL
    result = this->send_command(validator_commands::POLL);

    if(result.size() < 3) {
        this->send_command(validator_commands::NAK);
        return -1;
    }

    // Проверка на ошибки, если были обнаружены ошибки посылаем команду NAK
    if(this->check_errors(result)) {
        this->send_command(validator_commands::NAK);
        return -1;
    }

    if(result[3] == 0x43){
        std::cout << "DROP CASSETTE OUT OF POSITION" << std::endl;
        return -1;
    }

    // ACK
    if(result[3] == 0x00) {
        this->send_command(validator_commands::POLL);
        return -1;
    }

    // IDLING
    if(result[3] == 0x14) {
        this->send_command(validator_commands::ACK);
        return -1;
    }

    // Если 4 байт не равен 0х14 (IDLING)
    if(result[3] != 0x14)
    {
        if(result[3] == 0x15)
        {
            // ACCEPTING 0x15
            this->send_command(validator_commands::ACK);
        }
        else if(result[3] == 0x1C)
        {
            // ESCROW POSITION
            // Если 0x1C (Rejection), купюроприемник скорее всего не распознал купюру

            std::cout << "0x1C :: unknown... :: can't identify\n";
            this->send_command(validator_commands::ACK);
        }
        else if(result[3] == 0x80)
        {
            // ESCROW POSITION
            // Купюра распознана и находится в отсеке хранения
            std::cout << "0x80 :: has pushed into stack\n";
            this->send_command(validator_commands::ACK);

            /****** СДЕСЬ В ДАЛЬНЕЙШЕМ БУДЕТ ПРОВЕРКА СОСТОЯНИЯ СЕТИ ******/

            // STACK
            result = this->send_command(validator_commands::STACK);
        }
        else if(result[3] == 0x17)
        {
            // STACKING 0x17 отправка купюры в стек
            this->send_command(validator_commands::ACK);
        }
        else if(result[3] == 0x81)
        {
            // Bill stacked 0x81
            // купюра попала в стек
            this->send_command(validator_commands::ACK);
            print_b("Bill added :: code=", result);
            m_cash_received += this->cash_code_table(result[4]) / 10;
            std::cout << "\nCASH: " << std::dec << this->cash_code_table(result[4]) / 10 << std::endl;
            print_b("STACKED: ", result);
            return this->cash_code_table(result[4]) / 10;
        }
        else if(result[3] == 0x18)
        {
            // RETURNING
            // Если четвертый бит 18h, следовательно идет процесс возврата
            this->send_command(validator_commands::ACK);
        }
        else if(result[3] == 0x82)
        {
            // BILL RETURNING
            // Если четвертый бит 82h, следовательно купюра возвращена
            this->send_command(validator_commands::ACK);
        }
    }

    return -1;
}

/*
 * Функция отправки команды купюроприемнику, после чего возвращает полученных результат от
 * купюроприемника в виде вектора байтов.
 */
vec_bytes cash_code_protocol::send_command(validator_commands cmd, vec_bytes data)
{
    vec_bytes response;

    if(validator_commands::ACK == cmd || validator_commands::NAK == cmd)
    {
        vec_bytes bytes;
        if(cmd == validator_commands::ACK)
            bytes = pack.create_response(validator_commands::ACK);
        if(cmd == validator_commands::NAK)
            bytes = pack.create_response(validator_commands::NAK);

        if(bytes.size() != 0)
            this->com_port->write_data(bytes);
    }
    else
    {
        pack.set_cmd((byte)cmd);
        if(data.size() != 0)
            pack.set_data(data);

        this->com_port->write_data(pack.get_bytes());
        response = this->com_port->read_line();
    }
    return response;
}

std::string cash_code_protocol::find_port()
{
    const int MAX_PORT_NUMBER = 31;
    for(int i = 0; i < MAX_PORT_NUMBER; ++i)
    {
        std::string port_path = std::string("/dev/ttyS") + std::to_string(i);
        int err = connect_validator(port_path.c_str());
        if(!err) // connection failed
        {
            continue;
        }

        // Try get device status
        vec_bytes result;

        if (!this->m_is_connected)
        {
            std::cout << "Test if(){}";
        }

        if(!this->m_is_connected){
            continue;
        }

        // Power UP
        result = this->send_command(validator_commands::POLL);
        //print_b("POWER UP: ", result);

        // Check result for errors
        if(this->check_errors(result)){
            std::cout << "\n\nError in check result value\n\n";
            this->send_command(validator_commands::NAK);
        }

        // Если CashCode вернул в 4 байте 0х19 значит он уже включен
        if(result[3] == 0x19) {
            //std::cout << "Validator ready to work!" << std::endl;
            return port_path;
        }

        // Если все хорошо, отправляет команду подтверждения
        this->send_command(validator_commands::ACK);

        // RESET
        result = this->send_command(validator_commands::RESET);
         print_b("RESET: ", result);

        // Если купюроприемник не ответил сигналом ACK
        if(result[3] != 0x00){
            m_last_error = 0x00;
            continue;
        }


        // Опрос купюроприемника процедура инициализации
        result = this->send_command(validator_commands::POLL);
        print_b("POLL: ", result);
        if(this->check_errors(result)){
            this->send_command(validator_commands::NAK);
        }
        this->send_command(validator_commands::ACK);


        // Получение статуса GET_STATUS
        result = this->send_command(validator_commands::GET_STATUS);
        print_b("GET_STATUS: ", result);

        // Команда GET STATUS возвращает 6 байт ответа. Если все равны 0, то статус ok и можно работать дальше, иначе ошибка
        if(result[3] != 0x00 || result[4] != 0x00 || result[5] != 0x00 ||
           result[6] != 0x00 || result[7] != 0x00 || result[8] != 0x00 )
        {
            continue;
        }

        // Подтверждает если все хорошо
        this->send_command(validator_commands::ACK);

        std::cout << "Found device :: portPath=" << port_path << "\n";
        return port_path;
    }

    std::cout << "WARNING :: Can't find any port...\n";
    return "";
}

/*
 * Функция инициации SerialPort для работы с купюроприемником
 */
int cash_code_protocol::connect_validator(std::string port_path)
{
    try
    {
        if(this->com_port->start(port_path.c_str(), 9600)) {
            this->m_is_connected = true;
            std::cout << "COM CONNECTED!" << std::endl;
        }
        return 1;
    }
    catch(std::exception &e)
    {
        this->m_is_connected = false;
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}

/*
 * Включения купюроприемника
 */
int cash_code_protocol::power_up_validator()
{

    vec_bytes result;

    if (!this->m_is_connected)
    {
        std::cout << "Test if(){}";
    }

    std::cout << "Begin of powerupvalidator\n";


    if(!this->m_is_connected){

        std::cout << "Failed to connect\n";
        throw new cash_code_error(0);
    }

    // Power UP
    result = this->send_command(validator_commands::POLL);
    print_b("POWER UP: ", result);

    // Check result for errors
    if(this->check_errors(result)){
        std::cout << "\n\nError in check result value\n\n";
        this->send_command(validator_commands::NAK);
    }

    // Если CashCode вернул в 4 байте 0х19 значит он уже включен
    if(result[3] == 0x19) {
        std::cout << "Validator ready to work!" << std::endl;
        return 0;
    }

    // Если все хорошо, отправляет команду подтверждения
    this->send_command(validator_commands::ACK);

    // RESET
    result = this->send_command(validator_commands::RESET);
     print_b("RESET: ", result);

    // Если купюроприемник не ответил сигналом ACK
    if(result[3] != 0x00){
        m_last_error = 0x00;
        return m_last_error;
    }


    // Опрос купюроприемника процедура инициализации
    result = this->send_command(validator_commands::POLL);
    print_b("POLL: ", result);
    if(this->check_errors(result)){
        this->send_command(validator_commands::NAK);
    }
    this->send_command(validator_commands::ACK);


    // Получение статуса GET_STATUS
    result = this->send_command(validator_commands::GET_STATUS);
    print_b("GET_STATUS: ", result);

    // Команда GET STATUS возвращает 6 байт ответа. Если все равны 0, то статус ok и можно работать дальше, иначе ошибка
    if(result[3] != 0x00 || result[4] != 0x00 || result[5] != 0x00 ||
       result[6] != 0x00 || result[7] != 0x00 || result[8] != 0x00 )
    {
        this->m_last_error = 0x70;
    }

    // Подтверждает если все хорошо
    this->send_command(validator_commands::ACK);

    // SET_SECURITY (в тестовом примере отправояет 3 байта (0 0 0)
    result = this->send_command(validator_commands::SET_SECURITY, SECURITY_CODE);
    print_b("SET_SECURITY: ", result);

    // Если не получили от купюроприемника сигнал ACK
    if(result[3] != 0x00){
        m_last_error = 0x00;
        return this->m_last_error;
    }

    // IDENTIFICATION
    result = this->send_command(validator_commands::IDENTIFICATION);
    this->send_command(validator_commands::ACK);
    print_b("IDENTIFICATION: ", result);

    // Опрашиваем купюроприемник должны получить команду INITIALIZE
    result = this->send_command(validator_commands::POLL);
    print_b("INITIALIZE: ", result);

    if(this->check_errors(result)){
        this->send_command(validator_commands::NAK);
    }

    this->send_command(validator_commands::ACK);

    // POLL Должны получить команду UNIT DISABLE
    result = this->send_command(validator_commands::POLL);
    print_b("UNIT DISABLE: ", result);

    if(this->check_errors(result)){
        this->send_command(validator_commands::NAK);
        return m_last_error;
    }

    this->send_command(validator_commands::ACK);
    this->m_is_power_up = true;

    return this->m_last_error;
}


/*
 * Система проверки принятых данных от купюроприемника 
 * на наличие ошибок.
 */
bool cash_code_protocol::check_errors(vec_bytes &result)
{
    if(result.size() == 0)
    {
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


void cash_code_protocol::print_b(std::string msg, vec_bytes data){
    std::cout << msg;
    for(auto byte : data)
        std::cout << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)byte;
    std::cout << std::endl;
}
