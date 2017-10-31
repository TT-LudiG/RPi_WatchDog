#include <exception>
#include <iostream>
#include <thread>

#include "BaseController_RPi.h"

#define DIRECTORY_CHECK_INTERVAL_IN_SEC 60

int main(int argc, char *argv[])
{
    unsigned long int directoryCheckIntervalInSec = DIRECTORY_CHECK_INTERVAL_IN_SEC;
    
    std::string currentParam;
    
    if (argc > 1)
    {
        for (long int i = 1; i < argc; ++i)
        {
            try
            {
                currentParam = argv[i];
            }
            
            catch (...)
            {
                continue;
            }
            
            std::size_t indexEqualsChar = currentParam.find('=');
            
            if (indexEqualsChar != std::string::npos)
            {
                std::string token = currentParam.substr(0, indexEqualsChar);
                std::string value = currentParam.substr(indexEqualsChar + 1);

                if (token == "-directory_check_interval_in_sec")
                    directoryCheckIntervalInSec = std::stoul(value);
            }
        }
    }
    
    BaseController_RPi* baseControllerPtr = nullptr;
    
    try
    {
        baseControllerPtr = new BaseController_RPi(directoryCheckIntervalInSec);
    }
    
    catch (const std::exception& e)
    {
        if (baseControllerPtr != nullptr)
            delete baseControllerPtr;
        
        return 1;
    }
    
    std::thread kickerThread(&BaseController_RPi::kickWatchDogLoop, baseControllerPtr);
    std::thread monitorThread(&BaseController_RPi::monitorKickerThread, baseControllerPtr);
    
    std::cout << "Enter 'q' (quit), 'e' (exit) or 'c' (close) to end the program..." << std::endl;
    
    std::string inputLine;
    
    std::getline(std::cin, inputLine);
    
    while ((!baseControllerPtr->getFinalised()) && (inputLine != "q") && (inputLine != "e") && (inputLine != "c"))
        std::getline(std::cin, inputLine);
    
    baseControllerPtr->setFinalised();
    
    monitorThread.join();
    kickerThread.join();
    
    if (baseControllerPtr != nullptr)
        delete baseControllerPtr;

    return 0;
}