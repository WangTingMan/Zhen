
#include "GeneralTimer.h"

#include <climits>
#include <list>
#include <chrono>
#include <iostream>
#include <cmath>
#include <string>

static auto start = std::chrono::steady_clock::now();

std::chrono::steady_clock::time_point Timer::GetStartPoint()
{
    return start;
}

uint64_t Timer::GetCurrentMilliseconds()
{
    return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::steady_clock::now() - start ).count();
}

//------------------------------------------------------------------------
//! Timer connection signal & connection
//------------------------------------------------------------------------
struct TimerConnectionType
{
    Timer::TimerSignal signal;               //!< The signal to pulse
    boost_ns::signals2::connection connection;  //!< The connection to the signal
};

typedef std::list<TimerConnectionType*> TimerConnectionListType;

//------------------------------------------------------------------------
//! Timer entry. Contains the list of timer connections and details about
//! when and how the timer fires
//------------------------------------------------------------------------
struct TimerEntryType
{
    TimerConnectionListType connections;    //!< List of timer connections
    int milliseconds;                       //!< Timer duration
    uint32_t timeToExecute;                 //!< Time to next fire the timer(s)
    bool combine;                           //!< Are we allowed to combine timers
    std::string name;                       //!< The timer entry name
};

//------------------------------------------------------------------------
//! Functor for comparison of TimerEntryType*.
//------------------------------------------------------------------------
class TimerEntryComparator
{
public:

    //--------------------------------------------------------------------
    //! Comparator for TimerEntryType*
    //! @return true if lhs is less than rhs
    //--------------------------------------------------------------------
    bool operator()
        (
        const TimerEntryType* lhs,  //!< Left hand side
        const TimerEntryType* rhs   //!< Right hand side
        ) const
    {
        return ( lhs->timeToExecute < rhs->timeToExecute );
    } // end of function TimerEntryComparator::operator()
};

//! Priority queue in ascending order by next time to execute
typedef std::list<TimerEntryType*> TimerEntryQueueType;

static TimerEntryQueueType gTimers; //!< Queue of timers

//----------------------------------------------------------------
//! Connect a callback to the timer
//!
//! This function adds a new timer to the timers list and inserts it into
//! the appropriate location.  Timers with the closest timeToExecute are
//! placed at the beginning of the list and timers with the farthest
//! timeToExecute are placed at the end of the list.
//!
//! The aCombine parameter indicates whether the timer should fire
//! with the previously scheduled to fire timer.
//! By default it is true and the new timer will fire with the previously
//! scheduled to fire timer if they expire at the same time.
//!
//! This is done as an optimization because in most cases we want the timer to
//! fire at specified intervals instead of a duration from connection time.
//! If you expect the timer to fire at the specified aMilliseconds duration
//! from connection time then aCombine should be set to false.
//!
//! @return the connection to the timer
//----------------------------------------------------------------
boost_ns::signals2::connection Timer::connectTo
    (
    const TimerSignal::slot_type& aSlot,    //!< Slot to make the connection with
    unsigned int aMilliseconds,                 //!< Time that you want the timer to execute its callback function
    bool aCombine                               //!< Boolean value stating whether or not this timer
                                                //!<  can be executed at the same time as another timer
    )
{
    uint32_t curTime = GetCurrentMilliseconds();

    TimerConnectionType* newConnection = new TimerConnectionType;
    newConnection->connection = newConnection->signal.connect( aSlot );

    TimerEntryType* timerEntry = NULL;

    // Combine timer with another one if possible
    if( aCombine )
    {
        TimerEntryQueueType::iterator iter;
        for( iter = gTimers.begin(); iter != gTimers.end(); ++iter )
        {
            TimerEntryType* timer = (*iter);
            if( timer->combine && timer->milliseconds == aMilliseconds )
            {
                timerEntry = timer;
                break;
            }
        }
    }

    // Not able to combine, create a new timer entry
    if( timerEntry == NULL )
    {
        uint32_t nextFire = gTimers.empty() ? 0xFFFFFFFF : gTimers.front()->timeToExecute;

        timerEntry = new TimerEntryType;
        timerEntry->combine       = aCombine;
        timerEntry->milliseconds  = aMilliseconds;
        timerEntry->timeToExecute = curTime + aMilliseconds;
        gTimers.push_front( timerEntry );
        gTimers.sort( TimerEntryComparator() );

        if( gTimers.front()->timeToExecute < nextFire )
        {
            mSetTimer( gTimers.front()->timeToExecute );
        }
    }

    // Add the new timer to the timer entry
    timerEntry->connections.push_back( newConnection );

    return newConnection->connection;
} // end of function Timer::connectTo()

//----------------------------------------------------------------
//! This function that gets called whenever the timer period elapses
//!
//! This function gets called whenever the marked event is tripped.  It
//! executes the callback function of the first item in the *timers
//! list.  It then cycles through the list and executes all of the
//! callback functions that have expired timers.  It removes
//! and reinserts the members of the list accordingly.  It then creates
//! another marked event to trip at the necessary time.
//----------------------------------------------------------------
void Timer::operator()()
{
    if( gTimers.empty() )
    {
        mSetTimer( UINT_MAX );
        return;
    }

    TimerEntryQueueType::iterator iter = gTimers.begin();
    while( iter != gTimers.end() )
    {
        TimerEntryType* timer = (*iter);
        int64_t curTime = GetCurrentMilliseconds();
        int64_t diff = curTime - static_cast< int64_t >( timer->timeToExecute );
        if( diff < -10 )
        {
            // We've processed all timers that have elapsed
            break;
        }

        // Perform the timer callback signal
        TimerConnectionListType::iterator connectionIter = timer->connections.begin();
        while( connectionIter != timer->connections.end() )
        {
            TimerConnectionType* connection = *connectionIter;
            if( connection->connection.connected() )
            {
                auto ret = connection->signal();
                bool isHandled = false;
                if( ret.has_value() )
                {
                    #if defined(USE_SIGNAL2_ALONE_LIB)
                        isHandled = ret.value();
                    #elif defined(USE_BOOST_ORG_LIB)
                        isHandled = ret.get();
                    #endif
                }
                if( !isHandled )
                {
                    // User no longer interested in getting timer callback
                    connection->connection.disconnect();

                    // Destroy this disconnected signal
                    delete (*connectionIter);
                    timer->connections.erase( connectionIter++ );
                    continue;
                }
            }
            else
            {
                // Destroy this disconnected signal
                delete (*connectionIter);
                timer->connections.erase( connectionIter++ );
                continue;
            }

            ++connectionIter;
        }

        // Setup the timer to run again with given interval (we'll force a re-sort of the
        // list later)
        timer->timeToExecute = curTime + timer->milliseconds;

        // If there are no remaining connections for this timer, remove it.
        if( timer->connections.empty() )
        {
            delete *iter;
            gTimers.erase( iter++ );
            continue;
        }
        ++iter;
    }

    // Re-sort the list so the next timer to fire is at the front, and set the timer
    gTimers.sort( TimerEntryComparator() );
    if( gTimers.size() != 0 )
    {
        mSetTimer( gTimers.front()->timeToExecute );
    }

} // end of function Timer::timerCallback()

Timer::Timer()
{
    mSetTimer = []( uint32_t ){};
}
