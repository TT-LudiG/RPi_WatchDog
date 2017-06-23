#ifndef GSMCONTROLLER_H
#define GSMCONTROLLER_H

#include <vector>

#include "GPIOController.h"
#include "UARTController.h"

class GSMController
{
private:
    UARTController* _uartControllerPtr;
    GPIOController* _gpioPowerControllerPtr;
    
    unsigned char* _fileBuffer;
    
    unsigned char* _responseBuffer;
    
    char* _dumpBuffer;
    
    std::string _addressLocal;
    
    std::string _servername;
    unsigned short int _port;
    
    bool _isGPRSInitialised;
    bool _isConnectedToServer;
    
    std::vector<std::string> getResponses(const unsigned long int expectedCount, const bool mustReturn, const bool isExpectingFileData);
    
public:
    GSMController(void);
    ~GSMController(void);
    
    void initialiseGSM(void);
    void initialiseGPRS(void);
    void initialiseFTP(const std::string servername, const std::string username, const std::string password);
    
    void connectToServer(const std::string servername, const unsigned short int port);
    void disconnectFromServer(void);
    
    void sendBuffer(const unsigned char* inputBuffer, const unsigned long int bufferLength);
    
    void receiveFile(const std::string filename, const std::string filepath);
};
#endif