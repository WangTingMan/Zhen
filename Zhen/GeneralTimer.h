
#ifndef General_Timer_h__
#define General_Timer_h__

#include "global.h"
#include <functional>
#include <cstdint>
#include <chrono>

class LIBZHEN_API Timer
{

public:

    //! Timer signal type. Return true if the timer has been handled.
    //! Return false if you no longer want to receive the signal
    using TimerSignal = boost_ns::signals2::signal<bool()>;

    boost_ns::signals2::connection connectTo
        (
        const TimerSignal::slot_type& aSlot,
        unsigned int                  aMilliseconds,
        bool                          aCombine = true
        );

    //! Get the time from the first call this function.
    //! If it's the first time to call this function, it will return zero.
    static uint64_t GetCurrentMilliseconds();

    static std::chrono::steady_clock::time_point GetStartPoint();

private:

    typedef std::function<void( uint32_t )> SetTimerCallback;

public:

    Timer();

    void SetTimerSetCallback
        ( const SetTimerCallback& aSetTimer )
    {
        mSetTimer = aSetTimer;
    }

    void operator()();

private:

    SetTimerCallback mSetTimer;
};

#endif
