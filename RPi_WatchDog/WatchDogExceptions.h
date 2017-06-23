#ifndef WATCHDOGEXCEPTIONS_H
#define WATCHDOGEXCEPTIONS_H

#include <exception>
#include <string>

namespace WatchDogExceptions
{
    // EXCEPTION_WATCHDOG_0

    class TerminalOpenException: public std::exception
    {
    private:
        const std::string _error;
	
    public:
        TerminalOpenException(const std::string error): _error(error) {}
    
        virtual const char* what() const throw()
        {
            std::string message =  "EXCEPTION_WATCHDOG_0: Failed to open the POSIX terminal file: " + _error;
        
            return message.c_str();
        }
    };
}
#endif