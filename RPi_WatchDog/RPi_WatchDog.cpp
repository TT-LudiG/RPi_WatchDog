#include <exception>
#include <iostream>
#include <thread>

#include "BaseController_RPi.h"

#define SERVERNAME "41.185.23.172"
#define USERNAME "RPi-Dev"
#define PASSWORD "jacoistehbaws0YEAH!"

#define DIRECTORY_CHECK_INTERVAL_IN_SEC 60

int main(int argc, char *argv[])
{
    std::string servername = SERVERNAME;
    std::string username = USERNAME;
    std::string password = PASSWORD;
    
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
            
            if (currentParam == "-s")
            {
                try
                {
                    servername = std::string(argv[i + 1]);
                }
                
                catch (...)
                {
                    servername = SERVERNAME;
                }
            }
            
            else if (currentParam == "-u")
            {
                try
                {
                    username = std::string(argv[i + 1]);
                }
                
                catch (...)
                {
                    username = USERNAME;
                }
            }
            
            else if (currentParam == "-p")
            {
                try
                {
                    password = std::string(argv[i + 1]);
                }
                
                catch (...)
                {
                    password = PASSWORD;
                }
            }
            
            else if (currentParam == "-t")
            {             
                try
                {
                    directoryCheckIntervalInSec = std::stoul(std::string(argv[i + 1]));
                }
                
                catch (...)
                {
                    directoryCheckIntervalInSec = DIRECTORY_CHECK_INTERVAL_IN_SEC;
                }
            }
        }
    }
    
    BaseController_RPi* baseControllerPtr = nullptr;
    
    try
    {
        baseControllerPtr = new BaseController_RPi(servername, username, password, directoryCheckIntervalInSec);
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