#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include "cashcodeprotocol.h"

using namespace std;

int main(int argc, char *argv[])
{
    CashCodeProtocol CCValidator;

    // try find port
    std::cout << std::string(10, '~') << " Try find port " << std::string(10, '~') << "\n";
    std::string portPath = CCValidator.FindPort();
    if(portPath == "")
    {
        return 1;
    }
    std::cout << "Port found! portPath=" << portPath << "\n";
    std::cout << std::string(10, '~') << " \tEnd\t " << std::string(10, '~') << "\n\n";

    // work with port
    CCValidator.ConnectValidator(portPath.c_str());
    CCValidator.PowerUpValidator();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    bool is_thread = false;

    std::string stopWord = "";
    const int SLEEP_TIME = 200;
    for(int i = 0; stopWord != "n"; i++)
    {
        cout << "-----------[ TRY " << i << " ]------------\n";
        CCValidator.EnableSequence();
        std::cout << "Sleep to start listening\n";
        std::this_thread::sleep_for(std::chrono::seconds(10));

        if( ! is_thread) {
            std::cout << "Start listening\n";
            CCValidator.StartListening();
            is_thread = true;
        }

        std::cout << "Listen port... Push bill\n";
        std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIME));

        cout << "Total Cash: " << dec << CCValidator.m_CashReceived << endl;

        //std::this_thread::sleep_for(std::chrono::seconds(35));

        std::cout << "Stop listening\n";
        CCValidator.DisableSequence();
        std::this_thread::sleep_for(std::chrono::seconds(10));

        std::cout << "Do you want to continue? (y/n) : ";
        std::cin >> stopWord;
    }
    cout << "Done\n";

    return 0;
}
