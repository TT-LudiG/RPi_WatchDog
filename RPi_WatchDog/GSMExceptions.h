#ifndef GSMEXCEPTIONS_H
#define GSMEXCEPTIONS_H

#include <exception>
#include <string>

namespace GSMExceptions
{
    // EXCEPTION_GSM_0

    class BaseInitialiseException: public std::exception
    {	
    public:	
        virtual const char* what() const throw()
        {
            return "EXCEPTION_GSM_0: Failed to initialise the GSM controller.";
        }
    };

    // EXCEPTION_GSM_1

    class GPRSInitialiseException: public std::exception
    {	
    public:	
        virtual const char* what() const throw()
        {
            return "EXCEPTION_GSM_1: Failed to initialise GPRS.";
        }
    };
    
    // EXCEPTION_GSM_2

    class FTPInitialiseException: public std::exception
    {	
    public:	
        virtual const char* what() const throw()
        {
            return "EXCEPTION_GSM_2: Failed to initialise FTP.";
        }
    };

    // EXCEPTION_GSM_3

    class ServerConnectException: public std::exception
    {
    private:
        const std::string _serverName;
        const unsigned short int _port;
	
    public:
        ServerConnectException(const std::string serverName, const unsigned short int port): _serverName(serverName), _port(port) {}
	
        virtual const char* what() const throw()
        {
            std::string message = "EXCEPTION_GSM_3: Failed to connect to the server: " + _serverName + ", at port: " + std::to_string(_port);
		
            return message.c_str();
        }
    };
    
    // EXCEPTION_GSM_4

    class ServerDisconnectException: public std::exception
    {	
    public:	
        virtual const char* what() const throw()
        {
            return "EXCEPTION_GSM_4: Failed to disconnect from the server.";
        }
    };

    // EXCEPTION_GSM_5

    class BufferSendException: public std::exception
    {	
    public:	
        virtual const char* what() const throw()
        {
            return "EXCEPTION_GSM_5: Failed to send data.";
        }
    };
    
    // EXCEPTION_GSM_6

    class FileReceiveInitialiseException: public std::exception
    {
    private:
        const std::string _filename;
        const std::string _filepath;
        
    public:
        FileReceiveInitialiseException(const std::string filename, const std::string filepath): _filename(filename), _filepath(filepath) {}
        
        virtual const char* what() const throw()
        {
            std::string message = "EXCEPTION_GSM_6: Failed to initialise the receiving of file: " + _filename + ", from path: " + _filepath;
            
            return message.c_str();
        }
    };

    // EXCEPTION_GSM_7

    class FileReceiveException: public std::exception
    {
    private:
        const std::string _filename;
        const std::string _error;
        
    public:
        FileReceiveException(const std::string filename, const std::string error): _filename(filename), _error(error) {}
        
        virtual const char* what() const throw()
        {
            std::string message = "EXCEPTION_GSM_7: Failed to receive file: " + _filename + ", with error: " + _error;
            
            return message.c_str();
        }
    };
    
    // EXCEPTION_GSM_8

    class FileWriteException: public std::exception
    {
    private:
        const std::string _filename;
        
    public:
        FileWriteException(const std::string filename): _filename(filename) {}
        
        virtual const char* what() const throw()
        {
            std::string message = "EXCEPTION_GSM_8: Failed to write the file: " + _filename + " to disk.";
            
            return message.c_str();
        }
    };
}
#endif