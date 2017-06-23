#include <cstring>

#include <fcntl.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "WatchDogController.h"
#include "WatchDogExceptions.h"

WatchDogController::WatchDogController(const std::string ttyDevice)
{
    _deviceHandle = open(ttyDevice.c_str(), O_RDWR | O_NOCTTY);
    
    if (_deviceHandle < 0)
    {
        WatchDogExceptions::TerminalOpenException e(std::string(std::strerror(errno)));
        throw e;
    }
}

void WatchDogController::kick(void) const
{
    ioctl(_deviceHandle, WDIOC_KEEPALIVE, 0);
}

WatchDogController::~WatchDogController()
{
    close(_deviceHandle);
}