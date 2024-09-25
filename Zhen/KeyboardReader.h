#ifndef KEYBOARD_READER_H__
#define KEYBOARD_READER_H__
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

class KeyboardReader 
{
public:

    KeyboardReader();

    ~KeyboardReader();

    void start();

    void stop();

    void registerLineHandler( std::function<void( std::string&& ) > aHandler )
    {
        mLineHandler = aHandler;
    }

private:

    std::function<void( std::string&& )> mLineHandler;

    bool isRunning = false;

    bool isGoingToStop = false;

    void read(std::shared_ptr<std::promise<void>> a_promise);

    void handleNewCharRead( char a_ch );

    std::thread mThread;

    std::mutex mMutex;
    std::string mReadString;
};

#endif

