#pragma once
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <list>
#include <functional>
#include <thread>

#include <boost/signals2.hpp>

#include "Event.h"
#include "BasePage.h"
#include "KeyboardReader.h"

class BasePage;
class Timer;
class TimerImpl;

class PageManager
{

public:

    enum class AddType
    {
        ADD_FIRST,
        ADD_LAST,
        ADD_BEFORE,
        ADD_AFTER
    };

    enum class PageChangedSignalType
    {
        START,
        FINISHED
    };

    static PageManager& GetInstance();

    int run();

    void PostEvent( Event&& a_event );

    void PostEvent( std::shared_ptr<Event>&& a_event );

    int GetDisplayAreaWidth()const;

    void PushPage
        (
        std::shared_ptr<BasePage>&& a_page,
        AddType                     a_addType = AddType::ADD_LAST,
        std::shared_ptr<BasePage>   a_referencePage = nullptr
        );

    void PopToPage( std::shared_ptr<BasePage> const& a_page );

    void PopPage( std::shared_ptr<BasePage> const& a_page );

    std::shared_ptr<BasePage> GetRootPage()const;

    std::shared_ptr<BasePage> GetTopPage()const;

    boost::signals2::connection ConnectPageChanged( std::function<void( PageChangedSignalType, std::shared_ptr<BasePage>, bool ) > a_fun );

    void FreshTopPageShow();

    boost::signals2::connection connectTimerTo
        (
        const boost::signals2::signal<bool()>::slot_type& a_slot,
        unsigned int                                      a_milliseconds,
        bool                                              a_combine = true
        );

    boost::signals2::connection connectOneShotTimerTo
        (
        const std::function<void()>& a_slot,
        unsigned int                 a_milliseconds,
        bool                         a_combine = true
        );

    bool IsPageManagerRunningThread();

    virtual ~PageManager();

private:

    PageManager();

    bool HandleEvent( std::shared_ptr<Event>&& a_event );

    void clearScreen();

    std::mutex m_messageMutex;
    std::condition_variable m_notifyCondition;
    std::vector<std::shared_ptr<Event>> m_messages;

    std::list<std::shared_ptr<BasePage>> m_pages;
    boost::signals2::signal<void( PageChangedSignalType, std::shared_ptr<BasePage>, bool )>  m_pageChangedSignal;
    KeyboardReader reader;
    bool m_running = false;
    std::thread::id m_runningThread;

    std::shared_ptr< TimerImpl> m_timerImpl;
    std::thread  m_timerThread;
};

