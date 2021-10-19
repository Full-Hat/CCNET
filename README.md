# CCNET CashCode bill validator
Implementation of the CCNET protocol for CashCode bill validator written in C++

## Requirements
Uses boost libraries (curr version=1.67)  
Linux OS (software use port name = "/ttyS..")

## Overview 
Software gives to C/C++ developer an interface for working with bill validator

+ Software can automatically try to find port with bill validator (on different ports, with different baud rate) 
+ Connect to bill validator (open port and make connection with args)
+ disconnect (disable validator and close port)
+ Listen port for any action (and any errors too)
+ Hold bill (send this command in cycle to hold bill in validator)
+ Stack bill (require one command for stacking) 
+ Unstack bill (if validator hold bill, to unstack it send this command)
+ Thread locking ('set_mutex(...)', if mutex==nullptr => program will skip locking

IMPORTANT!
You should to use 'bill_interface_new' class
It gives you ability to make connection, reconnect validator and disable it 
Also, this class have 'get_instance(...)' and 'destroy_instance(...)' methods
They are easy to use!

## Supports
Protocols: 
  1) CCNET 

Baud rates:
  1) 19200
  2) 9600

Serial ports:
  1) /dev/ttyS{0..31} (exmpl: /dev/ttyS0)
