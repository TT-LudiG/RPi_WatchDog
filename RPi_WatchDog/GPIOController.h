#ifndef GPIOCONTROLLER_H
#define GPIOCONTROLLER_H

#include <string>

/* GPIO Class
 * Purpose: Each object instantiated from this class will control a GPIO pin
 * The GPIO pin number must be passed to the overloaded class constructor
 */

class GPIOController
{
private:
	// GPIO number associated with the instance of an object.
	
    std::string _gpioNum;
	
public:	
    GPIOController(void);	
    GPIOController(const std::string gpioNum);  
    ~GPIOController(void);
	
    // Exports GPIO.
	
    void exportGPIO(void) const;
	
    // Unexports GPIO.
	
    void unexportGPIO(void) const;
	
     // Set GPIO Direction.
	
    void setGPIODirection(const std::string direction) const;
	
    // Set GPIO Value (output pins).
	
    void setGPIOValue(const bool value) const;
	
    // Get GPIO Value (input/ output pins).
	
    bool getGPIOValue(void) const;
	
    // Return the GPIO number associated with the instance of an object.
	
    std::string getGPIONum(void) const;
};

#endif