#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include <dirent.h>
#include <fcntl.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "BaseController_RPi.h"

BaseController_RPi::BaseController_RPi(const unsigned long int directoryCheckIntervalInSec):
    _directoryCheckIntervalInSec(directoryCheckIntervalInSec)
{
    _watchDogControllerPtr = nullptr;
    
    logToFileWithSubdirectory("Boot");
    
    try
    {
		_watchDogControllerPtr = new WatchDogController("//dev/watchdog");
    }
    
    catch (const std::exception& e)
    {
        logToFileWithSubdirectory(e.what(), "WatchDog");
        
        throw e;
    }
    
    _isReady = false;
    _isWaiting = false;
    _hasWoken = false;
}

BaseController_RPi::~BaseController_RPi()
{  
    if (_watchDogControllerPtr != nullptr)
        delete _watchDogControllerPtr;
}

void BaseController_RPi::monitorKickerThread(void)
{
    std::unique_lock<std::mutex> lock(_mutex);

    while (!_isDone)
    {
        // Reset the conditional wait barrier for all client-listener threads.

        if ((_isReady) && (_hasWoken))
        {
            lock.lock();

            _isReady = false;
            
            _hasWoken = false;
        }

        if ((!_isReady) && (_isWaiting))
        {
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            
            // All client-listener threads wait for DELAY_SENDER_POST_IN_SEC seconds (interruptible).
            
            while (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - start).count() < DELAY_KICKER_LOOP_IN_SEC)
                if (_isDone)
                    break;
            
            _isReady = true;

            lock.unlock();

            _cv.notify_all();

            _isWaiting = false;
        }
    }
    
    _isReady = true;
}

void BaseController_RPi::kickWatchDogLoop(void)
{  
    std::unique_lock<std::mutex> lock(_mutex);
    
    lock.unlock();
    
    emptyDirectory("//home/pi/LOGS/RPi_Updater/Network");
    emptyDirectory("//home/pi/LOGS/RPi_Updater/FTP");
    emptyDirectory("//home/pi/LOGS/RPi_Updater/Base");
    
    emptyDirectory("//home/pi/LOGS/RPi_BLE_Scanner/Network");
    emptyDirectory("//home/pi/LOGS/RPi_BLE_Scanner/Bluetooth");
    emptyDirectory("//home/pi/LOGS/RPi_BLE_Scanner/GSM");
    
    bool isEmpty = true;
    
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    
    while (!_isDone)
    {
        if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - start).count() > _directoryCheckIntervalInSec)
        {
            if ((!isDirectoryEmpty("//home/pi/LOGS/RPi_Updater/Network")) || (!isDirectoryEmpty("//home/pi/LOGS/RPi_Updater/FTP")) || (!isDirectoryEmpty("//home/pi/LOGS/RPi_Updater/Base")))
                isEmpty = false;
            
            else if ((!isDirectoryEmpty("//home/pi/LOGS/RPi_BLE_Scanner/Network")) || (!isDirectoryEmpty("//home/pi/LOGS/RPi_BLE_Scanner/Bluetooth")) || (!isDirectoryEmpty("//home/pi/LOGS/RPi_BLE_Scanner/GSM")))
                isEmpty = false;
            
            start = std::chrono::steady_clock::now();
        }
        
        if (isEmpty)
            _watchDogControllerPtr->kick();
        
        else
            break;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
        _isWaiting = true;
            
        lock.lock();

        while (!_isReady)
            _cv.wait(lock);
            
        _hasWoken = true;
            
        lock.unlock();
    }
    
    _isDone = true;
}

void BaseController_RPi::setFinalised(void)
{
    _isDone = true;
}

bool BaseController_RPi::getFinalised(void) const
{
    return _isDone;
}

bool BaseController_RPi::isDirectoryEmpty(const std::string directoryPath) const
{
    bool isEmpty = true;
    
    DIR* directoryPtr = opendir(directoryPath.c_str());
    
    if (directoryPtr != nullptr)
    {      
        struct dirent* directoryEntryPtr = readdir(directoryPtr);
        
        while (directoryEntryPtr != nullptr)
        {
            std::string fileName = std::string(directoryEntryPtr->d_name);
            
            if ((fileName != ".") && (fileName != ".."))
            {
                isEmpty = false;
                
                break;
            }
            
            directoryEntryPtr = readdir(directoryPtr);
        }
        
        closedir(directoryPtr);
    }
    
    if (!isEmpty)
        return false;
    
    return true;
}

void BaseController_RPi::emptyDirectory(const std::string directoryPath) const
{
    DIR* directoryPtr = opendir(directoryPath.c_str());
    
    if (directoryPtr != nullptr)
    {    
        struct dirent* directoryEntryPtr = readdir(directoryPtr);
    
        while (directoryEntryPtr != nullptr)
        {
            std::string fileName = std::string(directoryEntryPtr->d_name);
            
            if ((fileName != ".") && (fileName != ".."))
            {
                std::string filePath = directoryPath + "/" + fileName;
                
                remove(filePath.c_str());
            }
        
            directoryEntryPtr = readdir(directoryPtr);
        }
        
        closedir(directoryPtr);
    }
}

void BaseController_RPi::logToFileWithSubdirectory(std::string subdirectoryName) const
{
    std::stringstream fileLogNameStream;
        
    fileLogNameStream << "//home/pi/LOGS/RPi_WatchDog/" << subdirectoryName;
    
    umask(0);
    mkdir("//home/pi/LOGS", 0755);
    mkdir("//home/pi/LOGS/RPi_WatchDog", 0755);
    mkdir(fileLogNameStream.str().c_str(), 0755);
    
    time_t timeRaw;
            
    std::time(&timeRaw);
            
    struct tm timeInfo = *std::localtime(&timeRaw);
            
    char time[20];
            
    std::strftime(time, 20, "%F_%T", &timeInfo);
        
    fileLogNameStream << "/" << time << ".log";
        
    std::ofstream fileLog(fileLogNameStream.str());
        
    fileLog.close();
}

void BaseController_RPi::logToFileWithSubdirectory(const std::string message, std::string subdirectoryName) const
{
    std::stringstream fileLogNameStream;
        
    fileLogNameStream << "//home/pi/LOGS/RPi_WatchDog/" << subdirectoryName;
    
    umask(0);
    mkdir("//home/pi/LOGS", 0755);
    mkdir("//home/pi/LOGS/RPi_WatchDog", 0755);
    mkdir(fileLogNameStream.str().c_str(), 0755);
    
    time_t timeRaw;
            
    std::time(&timeRaw);
            
    struct tm timeInfo = *std::localtime(&timeRaw);
            
    char time[20];
            
    std::strftime(time, 20, "%F_%T", &timeInfo);
        
    fileLogNameStream << "/" << time << ".log";
        
    std::ofstream fileLog(fileLogNameStream.str());
    
    if (fileLog.is_open())
        fileLog << message << std::endl;
        
    fileLog.close();
}