#ifndef BASECONTROLLER_RPI_H
#define BASECONTROLLER_RPI_H

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "GSMController.h"
#include "WatchDogController.h"

#define DELAY_KICKER_LOOP_IN_SEC 5

class BaseController_RPi
{
private:
    WatchDogController* _watchDogControllerPtr;
    
    const unsigned long int _directoryCheckIntervalInSec;
    
    std::atomic_bool _isDone;
    
    std::mutex _mutex;
    std::atomic<bool> _isReady;
    std::atomic<bool> _isWaiting;
    std::atomic<bool> _hasWoken;

    std::condition_variable _cv;
    
    bool isDirectoryEmpty(const std::string directoryName) const;
    
    void emptyDirectory(const std::string directoryName) const;
    
    void logToFileWithSubdirectory(const std::string directoryName) const;
    void logToFileWithSubdirectory(const std::string logBody, const std::string directoryName) const;
    
public:
    BaseController_RPi(const unsigned long int directoryCheckIntervalInSec);
    ~BaseController_RPi(void);
    
    void monitorKickerThread(void);
    
    void kickWatchDogLoop(void);
    
    void setFinalised(void);
    bool getFinalised(void) const;
};
#endif