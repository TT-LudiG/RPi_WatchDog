#include <fstream>

#include "GPIOController.h"
#include "GPIOExceptions.h"

GPIOController::GPIOController(void): _gpioNum("2") {}

GPIOController::GPIOController(const std::string gpioNum): _gpioNum(gpioNum) {}

GPIOController::~GPIOController(void) {}

void GPIOController::exportGPIO(void) const
{
    std::string exportPath = "//sys/class/gpio/export";
	
    // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames.
	
    std::ofstream exportGPIO(exportPath.c_str());
	
    if (!exportGPIO.is_open())
    {
        GPIOExceptions::GPIOExportException e(_gpioNum);
        throw e;
    }

    exportGPIO << _gpioNum;
	
    exportGPIO.close();
}

void GPIOController::unexportGPIO(void) const
{
    std::string unexportPath = "//sys/class/gpio/unexport";
	
     // Open unexport file.
	
    std::ofstream unexportGPIO(unexportPath.c_str());
	
    if (!unexportGPIO.is_open())
    {
        GPIOExceptions::GPIOUnexportException e(_gpioNum);
        throw e;
    }

    unexportGPIO << _gpioNum;
	
    unexportGPIO.close();
}

void GPIOController::setGPIODirection(const std::string direction) const
{
    std::string directionPath = "//sys/class/gpio/gpio" + _gpioNum + "/direction";
	
    // Open direction file for GPIO.
	
    std::ofstream setDirectionGPIO(directionPath.c_str());
	
    if (!setDirectionGPIO.is_open())
    {
        GPIOExceptions::GPIODirectionSetException e(_gpioNum);
        throw e;
    }
	
    setDirectionGPIO << direction;
	
    setDirectionGPIO.close();
}

void GPIOController::setGPIOValue(const bool value) const
{
    std::string valuePath = "//sys/class/gpio/gpio" + _gpioNum + "/value";
	
    // Open value file for GPIO.
	
    std::ofstream setValueGPIO(valuePath.c_str());
	
    if (!setValueGPIO.is_open())
    {
        GPIOExceptions::GPIOValueSetException e(_gpioNum);
        throw e;
    }
    
    if (value)
        setValueGPIO << "1";
    
    else
        setValueGPIO << "0";
	
    setValueGPIO.close();
}

bool GPIOController::getGPIOValue(void) const
{  
    std::string valuePath = "//sys/class/gpio/gpio" + _gpioNum + "/value";
	
    // Open value file for gpio.
	
    std::ifstream getValueGPIO(valuePath.c_str());
	
    if (!getValueGPIO.is_open())
    {
        GPIOExceptions::GPIOValueGetException e(_gpioNum);
        throw e;
    }
    
    std::string value;

    getValueGPIO >> value;
    
    getValueGPIO.close();

    if (value != "0")
        return true;
    
    return false;
}

std::string GPIOController::getGPIONum(void) const
{
    return _gpioNum;
}