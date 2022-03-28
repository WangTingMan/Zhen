#pragma once
#include "Event.h"

#include <functional>

class ExecutbleEvent : public Event
{

public:

    ExecutbleEvent( std::function<void()> a_fun )
        : m_fun( a_fun )
    {
        SetEventType( Event::EventType::ExecutbleEvent );
    }

    ExecutbleEvent()
    {
        SetEventType( Event::EventType::ExecutbleEvent );
    }

    void operator()()
    {
        if( m_fun )
        {
            m_fun();
        }
    }

    void SetExecutableFunction( std::function<void()> a_fun )
    {
        m_fun = a_fun;
    }

private:

    std::function<void()> m_fun;
};

