#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/bind.hpp>

typedef std::vector<unsigned char> vec_bytes;
typedef unsigned char byte;

// Размер буфера чтения для COP-порта
#define SERIAL_PORT_READ_BUF_SIZE 256

typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;

class serial_port {
private:
    serial_port_ptr p_port;
    boost::asio::io_service m_io_service;
    boost::asio::deadline_timer *timer;

    size_t timeout;
    bool read_error;
    byte c;

    // Called when an async read completes or has been cancelled
    void read_complete(const boost::system::error_code& error, size_t bytes_transferred);
    // Called when the timer's deadline expires.
    void time_out(const boost::system::error_code& error);

public:
    std::string m_port_name;
    int m_baud_rate;

    serial_port(size_t timeout);
    ~serial_port(void);

    virtual bool start(const char *com_port_name, int baud_rate = 9600);
    virtual void stop();

    // Reads a character or times out
    bool read_char(unsigned char& val);

    // Read Data
    vec_bytes read_line(void);

    void write_data(vec_bytes buf);
    static std::string get_port_name(void);

};

#endif // SERIALPORT_H
