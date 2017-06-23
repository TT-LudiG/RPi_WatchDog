#include <chrono>
#include <cstring>
#include <fstream>
#include <sstream>
#include <thread>

#include "GSMController.h"
#include "GSMExceptions.h"

#include <iostream>

#define GSM_FTP_FILESIZE_MAX 1000000

#define GSM_GETRESPONSES_ATTEMPTS_MAX 20

GSMController::GSMController(void)
{
    _uartControllerPtr = new UARTController("//dev/serial0");
    _gpioPowerControllerPtr = new GPIOController("17");
    
    _fileBuffer = nullptr;
    
    _responseBuffer = nullptr;
    
    _dumpBuffer = nullptr;
    
    _isGPRSInitialised = false;
    _isConnectedToServer = false;
    
    _gpioPowerControllerPtr->exportGPIO();
    _gpioPowerControllerPtr->setGPIODirection("out");
}

GSMController::~GSMController(void)
{ 
    if (_isConnectedToServer)
        disconnectFromServer();
    
    _gpioPowerControllerPtr->unexportGPIO();
    
    if (_fileBuffer != nullptr)
        delete[] _fileBuffer;
    
    if (_responseBuffer != nullptr)
        delete[] _responseBuffer;
    
    if (_dumpBuffer != nullptr)
        delete[] _dumpBuffer;
    
    if (_gpioPowerControllerPtr != nullptr)
        delete _gpioPowerControllerPtr;
    
    if (_uartControllerPtr != nullptr)
        delete _uartControllerPtr;
}

void GSMController::initialiseGSM(void)
{
    for (unsigned long int i = 0; i < 2; ++i)
    {      
        // AT Command: AT
    
        unsigned char command[] = {"AT\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            _gpioPowerControllerPtr->setGPIOValue(true);
            
            std::this_thread::sleep_for(std::chrono::minutes(1));
            
            continue;
        }
    
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            _gpioPowerControllerPtr->setGPIOValue(true);
            
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
        
        else
            break;
    }
}

void GSMController::initialiseGPRS(void)
{
    // AT Command: AT+CGATT?
    
    {
        unsigned char command[] = {"AT+CGATT?\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
        
        const unsigned long int responseCount = 3;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "+CGATT: 1") || (responses.at(2) != "OK"))
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+CIPSHUT
    
    {
        unsigned char command[] = {"AT+CIPSHUT\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
    
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "SHUT OK"))
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+CIPMUX=0
    
    {
        unsigned char command[] = {"AT+CIPMUX=0\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
    
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
    }
    
        // AT Command: AT+SAPBR=3,1,"CONTYPE","GPRS"
    
    {
        unsigned char command[] = {"AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+SAPBR=3,1,"APN","internet"
    
    {
        unsigned char command[] = {"AT+SAPBR=3,1,\"APN\",\"internet\"\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+SAPBR=1,1
    
    {
        unsigned char command[] = {"AT+SAPBR=1,1\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) && (responses.at(1) == "OK"))
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+SAPBR=2,1
    
    {
        unsigned char command[] = {"AT+SAPBR=2,1\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
        
        const unsigned long int responseCount = 3;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(2) != "OK"))
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
        
        std::string response = responses[1];
        
        unsigned long int status = std::stoul(response.substr(10, 1));
        
        if (status != 1)
        {
            GSMExceptions::GPRSInitialiseException e;
            throw e;
        }
        
        _addressLocal = response.substr(13, response.length() - 14);
    }
    
    _isGPRSInitialised = true;
}

void GSMController::initialiseFTP(const std::string servername, const std::string username, const std::string password)
{  
    // AT Command: AT+FTPCID=1
    
    {
        unsigned char command[] = {"AT+FTPCID=1\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+FTPMODE=1
    
    {
        unsigned char command[] = {"AT+FTPMODE=1\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+FTPSERV="servername"
    
    {
        std::stringstream commandStream;
    
        commandStream << "AT+FTPSERV=\"" << servername << "\"\r";
    
        std::string commandString = commandStream.str();
    
        unsigned long int commandLength = commandString.length();
    
        unsigned char command[commandLength];
    
        std::memcpy(static_cast<void*>(command), static_cast<const void*>(commandString.c_str()), commandLength);
    
        if (_uartControllerPtr->sendBuffer(command, commandLength) < 0)
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+FTPUN="username"
    
    {
        std::stringstream commandStream;
    
        commandStream << "AT+FTPUN=\"" << username << "\"\r";
    
        std::string commandString = commandStream.str();
    
        unsigned long int commandLength = commandString.length();
    
        unsigned char command[commandLength];
    
        std::memcpy(static_cast<void*>(command), static_cast<const void*>(commandString.c_str()), commandLength);
    
        if (_uartControllerPtr->sendBuffer(command, commandLength) < 0)
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
    
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
    }
    
    // AT Command: AT+FTPPW="password"
    
    {
        std::stringstream commandStream;
    
        commandStream << "AT+FTPPW=\"" << password << "\"\r";
    
        std::string commandString = commandStream.str();
    
        unsigned long int commandLength = commandString.length();
    
        unsigned char command[commandLength];
    
        std::memcpy(static_cast<void*>(command), static_cast<const void*>(commandString.c_str()), commandLength);
    
        if (_uartControllerPtr->sendBuffer(command, commandLength) < 0)
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
    
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::FTPInitialiseException e;
            throw e;
        }
    }
}

void GSMController::connectToServer(const std::string servername, const unsigned short int port)
{
    // AT Command: AT+CIPSTART="TCP","servername","port"
    
    std::stringstream commandStream;
    
    commandStream << "AT+CIPSTART=\"TCP\",\"" << servername << "\",\"" << port << "\"\r";
    
    std::string commandString = commandStream.str();
    
    unsigned long int commandLength = commandString.length();
    
    unsigned char command[commandLength];
    
    std::memcpy(static_cast<void*>(command), static_cast<const void*>(commandString.c_str()), commandLength);
    
    if (_uartControllerPtr->sendBuffer(command, commandLength) < 0)
    {
        GSMExceptions::ServerConnectException e(servername, port);
        throw e;
    }
    
    const unsigned long int responseCount = 3;
        
    std::vector<std::string> responses = getResponses(responseCount, false, false);
    
    if ((responses.size() != responseCount) || (responses.at(1) != "OK") || (responses.at(2) != "CONNECT OK"))
    {
        GSMExceptions::ServerConnectException e(servername, port);
        throw e;
    }
    
    _servername = servername;
    _port = port;
    
    _isConnectedToServer = true;
}

void GSMController::disconnectFromServer(void)
{  
    // AT Command: AT+CIPCLOSE
    
    unsigned char command[] = {"AT+CIPCLOSE\r"};
    
    if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
    {
        GSMExceptions::ServerDisconnectException e;
        throw e;
    }
    
    const unsigned long int responseCount = 2;
        
    std::vector<std::string> responses = getResponses(responseCount, false, false);
    
    if ((responses.size() != responseCount) || (responses.at(1) != "CLOSE OK"))
    {
        GSMExceptions::ServerDisconnectException e;
        throw e;
    }
}

void GSMController::sendBuffer(const unsigned char* inputBuffer, const unsigned long int bufferLength)
{
    // AT Command: AT+CIPSEND
    
    {
        unsigned char command[] = {"AT+CIPSEND\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::BufferSendException e;
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "> "))
        {
            GSMExceptions::BufferSendException e;
            throw e;
        }
    }
    
    {
        unsigned char buffer[bufferLength + 1];
    
        std::memcpy(static_cast<void*>(buffer), static_cast<const void*>(inputBuffer), bufferLength);
    
        buffer[bufferLength] = 0x1A;
    
        if (_uartControllerPtr->sendBuffer(buffer, bufferLength + 1) < 0)
        {
            GSMExceptions::BufferSendException e;
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "SEND OK"))
        {
            GSMExceptions::BufferSendException e;
            throw e;
        }
    }
}

void GSMController::receiveFile(const std::string filename, const std::string filepath)
{
    // AT Command: AT+FTPGETNAME="filename"
    
    {
        std::stringstream commandStream;
        
        commandStream << "AT+FTPGETNAME=\"" << filename << "\"\r";
        
        std::string commandString = commandStream.str();
    
        unsigned long int commandLength = commandString.length();
    
        unsigned char command[commandLength];
    
        std::memcpy(static_cast<void*>(command), static_cast<const void*>(commandString.c_str()), commandLength);
    
        if (_uartControllerPtr->sendBuffer(command, commandLength) < 0)
        {
            GSMExceptions::FileReceiveInitialiseException e(filename, filepath);
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::FileReceiveInitialiseException e(filename, filepath);
            throw e;
        }
    }
    
    // AT Command: AT+FTPGETPATH="filepath"
    
    {
        std::stringstream commandStream;
    
        commandStream << "AT+FTPGETPATH=\"" << filepath << "\"\r";
    
        std::string commandString = commandStream.str();
    
        unsigned long int commandLength = commandString.length();
    
        unsigned char command[commandLength];
    
        std::memcpy(static_cast<void*>(command), static_cast<const void*>(commandString.c_str()), commandLength);
    
        if (_uartControllerPtr->sendBuffer(command, commandLength) < 0)
        {
            GSMExceptions::FileReceiveInitialiseException e(filename, filepath);
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::FileReceiveInitialiseException e(filename, filepath);
            throw e;
        }
    }
    
    // AT Command: AT+FTPGET=1
    
    {
        unsigned char command[] = {"AT+FTPGET=1\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::FileReceiveException e(filename, "ERROR_SEND");
            throw e;
        }
        
        const unsigned long int responseCount = 2;
        
        std::vector<std::string> responses = getResponses(responseCount, false, false);
    
        if ((responses.size() != responseCount) || (responses.at(1) != "OK"))
        {
            GSMExceptions::FileReceiveException e(filename, "ERROR_RECEIVE");
            throw e;
        }
    }
    
    // AT Command: AT+FTPGET=2,1024
 
    {       
        unsigned char command[] = {"AT+FTPGET=2,1024\r"};
    
        if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
        {
            GSMExceptions::FileReceiveException e(filename, "ERROR_SEND");
            throw e;
        }
    }
    
    _fileBuffer = new unsigned char[GSM_FTP_FILESIZE_MAX];
    
    unsigned long int fileLength = 0;
    
    unsigned long int lastBufferLength = 0;
    
    unsigned long int status = 1;
    
    while (status)
    {
        const unsigned long int responseCount = 20;
        
        std::vector<std::string> responses = getResponses(responseCount, true, true);
        
        std::vector<std::string>::const_iterator it;
        
        for (it = responses.begin(); it != responses.end(); ++it)
        {
            std::cout << *it << std::endl;
            
            if (lastBufferLength > 0)
            {
                std::memcpy(static_cast<void*>(_fileBuffer + fileLength), static_cast<const void*>(it->c_str()), lastBufferLength);
                
                fileLength += lastBufferLength;
                
                std::cout << "Downloaded bytes: " << fileLength << std::endl;
                
                lastBufferLength = 0;
            }
            
            if (it->substr(0, 9) != "+FTPGET: ")
                continue;
            
            if (it->substr(9, 1) == "1")
            {
                status = std::stoul(it->substr(11, it->length() - 11));
                
                if (status == 1)
                {
                    // AT Command: AT+FTPGET=2,1024
     
                    unsigned char command[] = {"AT+FTPGET=2,1024\r"};
    
                    if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
                    {
                        GSMExceptions::FileReceiveException e(filename, "ERROR_SEND");
                        throw e;
                    }
                }
                
                else if (status == 0)
                    break;
                    
                else if (status != 86)
                {   
                    bool isOK = false;
                    
                    while (!isOK)
                    {
                        // AT Command: AT+FTPREST=n
    
                        {
                            std::stringstream commandStream;
    
                            commandStream << "AT+FTPREST=" << fileLength << "\r";
    
                            std::string commandString = commandStream.str();
    
                            unsigned long int commandLength = commandString.length();
    
                            unsigned char command[commandLength];
    
                            std::memcpy(static_cast<void*>(command), static_cast<const void*>(commandString.c_str()), commandLength);
    
                            if (_uartControllerPtr->sendBuffer(command, commandLength) < 0)
                            {
                                GSMExceptions::FileReceiveException e(filename, "ERROR_SEND");
                                throw e;
                            }
                        }
                        
                        const unsigned long int responseCount = 20;
        
                        std::vector<std::string> responses = getResponses(responseCount, true, false);
                        
                        std::vector<std::string>::const_iterator it;
                        
                        for (it = responses.begin(); it != responses.end(); ++it)
                        {
                            if (*it == "OK")
                            {
                                isOK = true;
                                
                                break;
                            }
                        }
                    }
                    
                    isOK = false;
                    
                    while (!isOK)
                    {
                        // AT Command: AT+FTPGET=1
    
                        {
                            unsigned char command[] = {"AT+FTPGET=1\r"};
    
                            if (_uartControllerPtr->sendBuffer(command, sizeof(command) - 1) < 0)
                            {
                                GSMExceptions::FileReceiveException e(filename, "ERROR_SEND");
                                throw e;
                            }
                        }
                        
                        const unsigned long int responseCount = 20;
        
                        std::vector<std::string> responses = getResponses(responseCount, true, false);
                        
                        std::vector<std::string>::const_iterator it;
                        
                        for (it = responses.begin(); it != responses.end(); ++it)
                        {
                            if (*it == "OK")
                            {
                                isOK = true;
                                
                                break;
                            }
                        }
                    }
                        
                    status = 1;
                }
                
                else
                {
                    GSMExceptions::FileReceiveException e(filename, "ERROR_STATUS: " + std::to_string(status));
                    throw e;
                }
            }
            
            else
                lastBufferLength = std::stoul(it->substr(11, it->length() - 11));
        }
    }
    
    std::ofstream file(filename, std::ofstream::binary);
    
    if (!file.is_open())
    {
        GSMExceptions::FileWriteException e(filename);
        throw e;
    }
    
    // REINTERPRET_CAST!
    
    file.write(reinterpret_cast<const char*>(_fileBuffer), fileLength);
    
    file.close();
    
    if (_fileBuffer != nullptr)
        delete[] _fileBuffer;
    
    _fileBuffer = nullptr;
}

std::vector<std::string> GSMController::getResponses(const unsigned long int expectedCount, const bool mustReturn, const bool isExpectingFileData)
{   
    std::vector<std::string> responses;
    
    unsigned long int responseCount;
    
    _responseBuffer = new unsigned char[GSM_FTP_FILESIZE_MAX];
    
    std::memset(static_cast<void*>(_responseBuffer), 0, GSM_FTP_FILESIZE_MAX);
    
    unsigned long int bufferLength = 0;
    
    unsigned long int attempt = 1;
    
    bool isDone = false;
    
    while ((!isDone) && (attempt <= GSM_GETRESPONSES_ATTEMPTS_MAX))
    {
        responses.clear();
        
        long int receiveLength = _uartControllerPtr->receiveBuffer(_responseBuffer + bufferLength, GSM_FTP_FILESIZE_MAX);
        
        if (receiveLength < 0)
            break;
        
        bufferLength += receiveLength;
        
        std::stringstream responseStream;
        
        // REINTERPRET_CAST!
        
        responseStream.write(reinterpret_cast<const char*>(_responseBuffer), bufferLength);
        
        std::string currentResponse;
        
        responseCount = 0;
        
        while (std::getline(responseStream, currentResponse))
        {
            if ((responseStream.eof()) || (currentResponse.size() == 0))
                continue;
            
            bool isFailedAttempt = false;
            
            if ((isExpectingFileData) && (currentResponse.substr(0, 11) == "+FTPGET: 2,"))
            {
                bool isDoneInner = false;
                
                unsigned long int responseLength = currentResponse.length();
                
                unsigned long int dataLength = std::stoul(currentResponse.substr(11, responseLength - 11));
                
                if (dataLength > 0)
                {
                    for (unsigned long int i = 0; i < bufferLength; ++i)
                    {
                        if (isDoneInner)
                            break;
                    
                        unsigned long int afterEndPositionBlock = i + responseLength;
                    
                        std::string currentBlock(_responseBuffer + i, _responseBuffer + afterEndPositionBlock);
                    
                        if (currentBlock != currentResponse)
                            continue;
                        
                        if ((bufferLength - afterEndPositionBlock) < dataLength)
                        {
                            isFailedAttempt = true;
                            
                            break;
                        }
                        
                        responses.push_back(currentResponse);
                                
                        // Remove the leading LF char.
                                
                        responseStream.get();
                                
                        _dumpBuffer = new char[dataLength];
                                
                        responseStream.read(_dumpBuffer, dataLength);
                                
                        responses.push_back(std::string(_dumpBuffer, dataLength));
                                
                        if (_dumpBuffer != nullptr)
                            delete[] _dumpBuffer;
                                
                        _dumpBuffer = nullptr;
                                
                        responseCount += 2;
                                
                        isDoneInner = true;
                                
                        break;
                    }
                }
                
                else
                {
                    responses.push_back(currentResponse);
            
                    ++responseCount;
                }
            }
            
            else
            {
                responses.push_back(currentResponse);
            
                ++responseCount;
            }
            
            if (isFailedAttempt)
                break;
            
            if (responseCount >= expectedCount)
            {
                isDone = true;
                
                break;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        ++attempt;
    }
    
    if (_responseBuffer != nullptr)
        delete[] _responseBuffer;
    
    _responseBuffer = nullptr;
    
    if ((!isDone) && (!mustReturn))
        responses.clear();
    
    return responses;
}