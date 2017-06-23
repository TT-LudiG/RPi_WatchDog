#ifndef GPIOEXCEPTIONS_H
#define GPIOEXCEPTIONS_H

#include <exception>
#include <string>

namespace GPIOExceptions
{
    // EXCEPTION_GPIO_0

    class GPIOExportException: public std::exception
    {
    private:
        const std::string _gpioNum;
	
    public:
        GPIOExportException(const std::string gpioNum): _gpioNum(gpioNum) {}
    
        virtual const char* what() const throw()
        {
            std::string message =  "EXCEPTION_GPIO_0: Failed to export GPIO" + _gpioNum;
        
            return message.c_str();
        }
    };

    // EXCEPTION_GPIO_1

    class GPIOUnexportException: public std::exception
    {
    private:
        const std::string _gpioNum;
	
    public:
        GPIOUnexportException(const std::string gpioNum): _gpioNum(gpioNum) {}
    
        virtual const char* what() const throw()
        {
            std::string message =  "EXCEPTION_GPIO_1: Failed to unexport GPIO" + _gpioNum;
        
            return message.c_str();
        }
    };
    
    // EXCEPTION_GPIO_2

    class GPIODirectionSetException: public std::exception
    {
    private:
        const std::string _gpioNum;
	
    public:
        GPIODirectionSetException(const std::string gpioNum): _gpioNum(gpioNum) {}
    
        virtual const char* what() const throw()
        {
            std::string message =  "EXCEPTION_GPIO_2: Failed to set the direction of GPIO" + _gpioNum;
        
            return message.c_str();
        }
    };

    // EXCEPTION_GPIO_3

    class GPIOValueSetException: public std::exception
    {
    private:
        const std::string _gpioNum;
	
    public:
        GPIOValueSetException(const std::string gpioNum): _gpioNum(gpioNum) {}
    
        virtual const char* what() const throw()
        {
            std::string message =  "EXCEPTION_GPIO_3: Failed to set the value of GPIO" + _gpioNum;
        
            return message.c_str();
        }
    };
    
    // EXCEPTION_GPIO_4

    class GPIOValueGetException: public std::exception
    {
    private:
        const std::string _gpioNum;
	
    public:
        GPIOValueGetException(const std::string gpioNum): _gpioNum(gpioNum) {}
    
        virtual const char* what() const throw()
        {
            std::string message =  "EXCEPTION_GPIO_4: Failed to get the value of GPIO" + _gpioNum;
        
            return message.c_str();
        }
    };
}
#endif