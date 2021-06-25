#include <map>
#include <string>

#ifndef CASHCODEERRORS_H
#define CASHCODEERRORS_H

class cash_code_error
{
    int error_code;
public:
    std::map<int, std::string> error_list;
    cash_code_error(int err_code)
    {
        error_code = err_code;
        error_list[0x00] = "Неизвестная ошибка.";
        error_list[0x01] = "Ошибка открытия COM-порта.";
        error_list[0x02] = "Ошибка отпраки команды включения купюроприемника.";
        error_list[0x03] = "Ошибка отпраки команды включения купюроприемника. От купюроприемник не прислал команду - POWER UP.";
        error_list[0x04] = "Ошибка отпраки команды включения купюроприемника. От купюроприемник не прислал команду - ACK.";
        error_list[0x05] = "Ошибка отпраки команды включения купюроприемника. От купюроприемник не прислал команду - INITIALIZE.";
        error_list[0x06] = "Ошибка проверки статуса купюроприемника. Stacker отсутствует.";
        error_list[0x07] = "Ошибка проверки статуса купюроприемника. Stacker переполнен.";
        error_list[0x08] = "Ошибка проверки статуса купюроприемника. В валидаторе застряла купюра.";
        error_list[0x09] = "Ошибка проверки статуса купюроприемника. В Stacker-e застряла купюра.";
        error_list[0x10] = "Ошибка проверки статуса купюроприемника. Фальшивая купюра.";

        error_list[0x50] = "Stack Motor Failure. Drop Cassette Motor failure";
        error_list[0x51] = "Transport Motor Speed Failure.";
        error_list[0x52] = "Transport Motor Failure";
        error_list[0x53] = "Aligning Motor Failure";
        error_list[0x54] = "Initial Cassette Status Failure";
        error_list[0x55] = "Optic Canal Failure";
        error_list[0x56] = "Magnetic Canal Failure";
        error_list[0x5F] = "Capacitance Canal Failure";

        error_list[0x60] = "REJECTING - Rejecting due to Insertion. Insertion error";
        error_list[0x61] = "REJECTING - Rejecting due to Magnetic. Magnetic error";
        error_list[0x62] = "REJECTING - Rejecting due to bill Remaining in the head. Bill remains in the head, and new bill is rejected.";
        error_list[0x63] = "REJECTING - Rejecting due to Multiplying. Compensation error/multiplying factor error";
        error_list[0x64] = "REJECTING - Rejecting due to Conveying. Conveying error";
        error_list[0x65] = "REJECTING - Rejecting due to Identification1. Identification error";
        error_list[0x66] = "REJECTING - Rejecting due to Verification. Verification error";
        error_list[0x67] = "REJECTING - Rejecting due to Optic. Optic error";
        error_list[0x68] = "REJECTING - Rejecting due to Inhibit. Returning by inhibit denomination error";
        error_list[0x69] = "REJECTING - Rejecting due to Capacity. Capacitance error";
        error_list[0x6A] = "REJECTING - Rejecting due to Operation. Operation error.";
        error_list[0x6C] = "REJECTING - Rejecting due to Length. Length error";
        error_list[0x6D] = "REJECTING - Rejecting due to UV optic. Banknote UV properties do not meet the predefined criteria";
        error_list[0x92] = "REJECTING - Rejecting due to unrecognised barcode. Bill taken was treated as a barcode but no reliable data can be read from it.";
        error_list[0x93] = "REJECTING - Rejecting due to incorrect number of characters in barcode. Barcode data was read (at list partially) but is inconsistent";
        error_list[0x94] = "REJECTING - Rejecting due to unknown barcode start sequence. Barcode was not read as no synchronization was established.";
        error_list[0x95] = "REJECTING - Rejecting due to unknown barcode stop sequence. Barcode was read but trailing data is corrupt.";

    }

    std::string get_message() {
        return this->error_list[this->error_code];
    }
};

#endif // CASHCODEERRORS_H
