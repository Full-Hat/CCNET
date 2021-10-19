#ifndef PROTOCOL_PACKAGE
#define PROTOCOL_PACKAGE 

#define POLYNOMIAL 0x08408

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>  

typedef std::vector<unsigned char> vec_bytes;
typedef unsigned char byte;

class package {
protected: 
    byte m_cmd;
    vec_bytes m_data; 
    
public:
    package();
    package(byte cmd, vec_bytes data);
    
    void set_cmd(byte cmd);
    byte get_cmd();
    
    vec_bytes get_data();
    void set_data(vec_bytes data);
    
    virtual vec_bytes get_bytes() = 0;  
    
    int get_length();
    
    vec_bytes getCRC16(vec_bytes bufData, int sizeData); 
    
    //virtual vec_bytes create_response(validator_commands type) = 0;
};

#endif
