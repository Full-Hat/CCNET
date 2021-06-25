# CCNET CashCode bill validator
Implementation of the CCNET protocol for CashCode bill validator written in C++

## Requirements
Uses boost libraries (curr version=1.67)  
Linux OS (software use port name = "/ttyS..")

For Windows OS or other Linux ports change name in cashcodeprotocol.cpp

Port number must be <= 31 (to change it, change value in cashcodeprotocol.cpp). Example: ttyS32 -- software shall never find it
