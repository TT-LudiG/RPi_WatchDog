#ifndef WATCHDOGCONTROLLER_H
#define WATCHDOGCONTROLLER_H

#include <string>

class WatchDogController
{
private:
    long int _deviceHandle;
    
public:
    WatchDogController(const std::string devicePath);
    ~WatchDogController();
    
    void kick(void) const;
};
#endif