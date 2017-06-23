#include <fcntl.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "WatchDogController.h"

WatchDogController::WatchDogController(const std::string devicePath)
{
    if ((_deviceHandle = open("//dev/watchdog", O_RDWR | O_NOCTTY)) < 0)
    {
        // Throw exception.
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