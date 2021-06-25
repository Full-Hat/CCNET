# CCNET CashCode bill validator
Implementation of the CCNET protocol for CashCode bill validator written in C++

## Requirements
Uses boost libraries (curr version=1.67)  
Linux OS (software use Linux file system)

## Interface
file CCNET.h:
  1) static int try_find_port(std::string &port_path_out);
     |                                                   |
     |  Call all "dev/ttyS.." files.                     |
     |  Found --------> return full port path            |
     |  Didn't find --> path or empty string             |
     -----------------------------------------------------
     
  2) port_CCNET(const std::string &port_path);
     |                                                   |
     |  Init bill validator and prepair for listening.   |
     |  After this actions validator can take bills      |
     |  By using funct (3) you can analyze bills         |
     -----------------------------------------------------

  3) int listen_port();
     |                                                   |
     |  Analyze validator messages and return pushed     |
     |  bill's value & calculate total_cash value        |
     |  total_cash value = sum(pushed bill's values)     |
     -----------------------------------------------------

  4) int get_total_cash() const;
     |                                                   |
     |  return total_cash value (see (3))                |
     -----------------------------------------------------

  5) ~port_CCNET();
     |                                                   |
     |  Disable bill validator                           |
     -----------------------------------------------------
