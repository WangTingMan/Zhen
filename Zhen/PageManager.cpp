#include "PageManager.h"
#include "KeyBoardInputEvent.h"
#include "ExecutbleEvent.h"
#include "GuardWatcher.h"
#include "GeneralTimer.h"

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <future>
#include <memory>
#include <mutex>

#ifndef _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
#endif

#ifdef _MSC_VER
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#endif

#include <conio.h>

class TimerImpl
{
    friend class PageManager;

public:

    TimerImpl();

    void run();

    void setTimeout( uint32_t a_milliseconds );

    void stop();

private:

    std::chrono::steady_clock::time_point getWaitDuration();

    std::atomic_bool m_running_flag;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::chrono::steady_clock::time_point m_nextAlarmTimepoint;
    std::shared_ptr<Timer> m_timerHanlder;
    std::future<void> m_stopFuture;
};

TimerImpl::TimerImpl()
{
    m_running_flag.store(true);
    m_nextAlarmTimepoint = std::chrono::steady_clock::now() + std::chrono::hours(10);
    m_timerHanlder = std::make_shared<Timer>();
    m_timerHanlder->SetTimerSetCallback( std::bind( &TimerImpl::setTimeout,
        this, std::placeholders::_1 ) );
}

void TimerImpl::run()
{
    std::promise<void> promise_;
    m_stopFuture = promise_.get_future();
    std::chrono::steady_clock::time_point start;
    while (m_running_flag)
    {
        std::chrono::steady_clock::time_point wait_time_point = getWaitDuration();
        std::unique_lock locker(m_mutex);
        start = std::chrono::steady_clock::now();
        std::chrono::milliseconds duration(std::chrono::hours(10));
        if (start < wait_time_point)
        {
            duration = std::chrono::duration_cast<std::chrono::milliseconds>
                ( wait_time_point - start );
        }
        m_condition.wait_for(locker, duration);
        if (std::chrono::steady_clock::now() - start < duration)
        {
            continue;
        }

        std::shared_ptr<ExecutbleEvent> event = std::make_shared<ExecutbleEvent>(
            std::bind(&Timer::operator(), m_timerHanlder));
        PageManager::GetInstance().PostEvent(event);
    }
    promise_.set_value();
}

void TimerImpl::setTimeout( uint32_t a_milliseconds )
{
    std::lock_guard locker(m_mutex);
    m_nextAlarmTimepoint = Timer::GetStartPoint() + std::chrono::milliseconds(a_milliseconds);
    m_condition.notify_all();
}

void TimerImpl::stop()
{
    m_running_flag.store(false);
    m_condition.notify_all();
    if( m_stopFuture.valid() )
    {
        m_stopFuture.wait();
    }
}

std::chrono::steady_clock::time_point TimerImpl::getWaitDuration()
{
    std::lock_guard locker(m_mutex);
    return m_nextAlarmTimepoint;
}

PageManager& PageManager::GetInstance()
{
    static PageManager instance;
    return instance;
}

PageManager::PageManager()
{
    m_timerImpl = std::make_shared<TimerImpl>();
    m_timerThread = std::thread( &TimerImpl::run, m_timerImpl );
}

int PageManager::run()
{
    int ret = 0;
    std::vector<std::shared_ptr<Event>> msgs;
    std::unique_lock<std::mutex> locker( m_messageMutex, std::defer_lock );
    m_running = true;
    if( !m_pages.empty() )
    {
        m_pages.back()->show();
    }

    m_runningThread = std::this_thread::get_id();

    while( m_running )
    {
        locker.lock();
        if( m_messages.empty() )
        {
            m_notifyCondition.wait( locker, [ this ]()
            {
                return !m_messages.empty();
            }
            );
        }
        msgs = std::move( m_messages );
        locker.unlock();

        for( auto&& ele : msgs )
        {
            if( ele )
            {
                HandleEvent( std::move( ele ) );
            }
        }
    }

    m_timerImpl->stop();
    return ret;
}

void PageManager::quit()
{
    m_running = false;
    m_notifyCondition.notify_all();
}

void PageManager::PostEvent( Event&& a_event )
{
    {
        std::lock_guard<std::mutex> locker( m_messageMutex );
        m_messages.push_back( std::make_shared<Event>( a_event ) );
    }
    m_notifyCondition.notify_all();
}

void PageManager::PostEvent( std::shared_ptr<Event>&& a_event )
{
    {
        std::lock_guard<std::mutex> locker( m_messageMutex );
        m_messages.push_back( std::forward<std::shared_ptr<Event>>( a_event ) );
    }
    m_notifyCondition.notify_all();
}

int PageManager::GetDisplayAreaWidth()const
{
    return m_displayAreaWidth;
}

bool PageManager::HandleEvent( std::shared_ptr<Event>&& a_event )
{
    bool handled = false;
    switch( a_event->GetEventType() )
    {
    case Event::EventType::ExecutbleEvent:
    {
        std::shared_ptr<ExecutbleEvent> event = std::dynamic_pointer_cast< ExecutbleEvent >( a_event );
        ( *event )( );
        handled = true;
    }
    break;
    default:
    {
        if( !m_pages.empty() )
        {
            auto last_ = m_pages.back();
            if( last_ )
            {
                handled = last_->OnEvent( a_event );
            }
        }
    }
    break;
    }

    return handled;
}

void PageManager::PushPage
    (
    std::shared_ptr<BasePage>&& a_page,
    AddType                     a_addType,
    std::shared_ptr<BasePage>   a_referencePage
    )
{
    if( !a_page )
    {
        return;
    }

    m_pageChangedSignal(PageChangedSignalType::START_PUSH_IN, a_page, true);

    GuardWatcher watcher( [&a_page, this ]()
        {
            m_pageChangedSignal(PageChangedSignalType::FINISHED_PUSH_IN, a_page, true);

            if( m_running )
            {
                clearScreen();
                m_pages.back()->show();
            }
        } );

    switch( a_addType )
    {
    case PageManager::AddType::ADD_FIRST:
        m_pages.push_front( std::move(a_page ) );
        return;
    case PageManager::AddType::ADD_LAST:
        m_pages.push_back( std::move( a_page ) );
        return;
    case PageManager::AddType::ADD_BEFORE:
    case PageManager::AddType::ADD_AFTER:
        break;
    default:
        return;
    }

    if( a_referencePage )
    {
        auto it = std::find( m_pages.begin(), m_pages.end(), a_referencePage );
        if( m_pages.end() != it )
        {
            if( PageManager::AddType::ADD_BEFORE == a_addType )
            {
                m_pages.insert( it, a_page );
            }
            else
            {
                std::advance( it, 1 );
                m_pages.insert( it, a_page );
            }
        }
        else
        {
            m_pages.push_back( std::move( a_page ) );
        }
    }
    else
    {
        m_pages.push_back( std::move( a_page ) );
    }
}

void PageManager::PopToPage( std::shared_ptr<BasePage> const& a_page )
{
    auto it = std::find( m_pages.begin(), m_pages.end(), a_page );
    if( m_pages.end() != it )
    {
        std::advance( it, 1 );
        std::list<std::shared_ptr<BasePage>> tmpList( it, m_pages.end() );
        for( auto& ele : tmpList )
        {
            m_pageChangedSignal(PageChangedSignalType::START_POP_OUT, ele, false);
        }
        m_pages.erase( it, m_pages.end() );
        for( auto& ele : tmpList )
        {
            m_pageChangedSignal(PageChangedSignalType::FINISHED_POP_OUT, ele, false);
        }
    }
}

void PageManager::PopPage( std::shared_ptr<BasePage> const& a_page )
{
    if( 1 == m_pages.size() )
    {
        return;
    }

    auto it = std::find( m_pages.begin(), m_pages.end(), a_page );
    if( m_pages.end() != it )
    {
        m_pageChangedSignal(PageChangedSignalType::START_POP_OUT, a_page, false);
        m_pages.erase( it );
        m_pageChangedSignal(PageChangedSignalType::FINISHED_POP_OUT, a_page, false);
    }

    if( m_running )
    {
        clearScreen();
        m_pages.back()->show();
    }
}

void PageManager::FreshTopPageShow()
{
    if( m_running )
    {
        clearScreen();
        m_pages.back()->show();
    }
}

std::shared_ptr<BasePage> PageManager::GetRootPage()const
{
    if( !m_pages.empty() )
    {
        return m_pages.front();
    }
    return nullptr;
}

std::shared_ptr<BasePage> PageManager::GetTopPage()const
{
    if( !m_pages.empty() )
    {
        return m_pages.back();
    }
    return nullptr;
}

boost_ns::signals2::connection PageManager::ConnectPageChanged( std::function<void( PageChangedSignalType, std::shared_ptr<BasePage>, bool ) > a_fun )
{
    boost_ns::signals2::connection con_ = m_pageChangedSignal.connect( a_fun );
    return con_;
}

void PageManager::clearScreen()
{
    std::system( "cls" );
}

boost_ns::signals2::connection PageManager::connectTimerTo
    (
    const boost_ns::signals2::signal<bool()>::slot_type& a_slot,
    unsigned int                                      a_milliseconds,
    bool                                              a_combine
    )
{
    return m_timerImpl->m_timerHanlder->connectTo( a_slot, a_milliseconds, a_combine );
}

boost_ns::signals2::connection PageManager::connectOneShotTimerTo
(
    const std::function<void()>& a_slot,
    unsigned int                 a_milliseconds,
    bool                         a_combine
    )
{
    auto fun = [a_slot]()->bool
    {
        a_slot();
        return false;
    };
    return connectTimerTo( fun, a_milliseconds, a_combine );
}

bool PageManager::IsPageManagerRunningThread()
{
    return std::this_thread::get_id() == m_runningThread;
}

PageManager::~PageManager()
{
    if( m_timerThread.joinable() )
    {
        m_timerThread.join();
    }
}
