#pragma once
#include <functional>

class GuardWatcher
{
public:

    GuardWatcher( std::function<void()> a_fun )
        : m_fun( a_fun )
    { }

    ~GuardWatcher()
    {
        if( m_fun )
        {
            m_fun();
        }
    }

    void Cancel()
    {
        m_fun = std::function<void()>();
    }

private:

    std::function<void()> m_fun;
};

