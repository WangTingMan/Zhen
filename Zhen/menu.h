#pragma once
#include "global.h"
#include "Event.h"
#include "TitleContent.h"

#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <cctype>

class Action
{
public:

    template<typename T,
        typename = typename std::enable_if<
        std::is_constructible<std::string, T>::value,
        std::true_type>::type
        >
        Action( T&& a_name, const char& a_target )
    {
        static_assert( std::is_constructible<std::string, T>::value, "std::string cannot construct with a_input!" );
        SetNameInternal( std::forward<T>( a_name ), a_target );
    }

    template<typename T,
        typename = typename std::enable_if<
        std::is_constructible<std::string, T>::value,
        std::true_type>::type
        >
    void SetName( T&& a_name, const char& a_target )
    {
        static_assert( std::is_constructible<std::string, T>::value, "std::string cannot construct with a_input!" );
        SetNameInternal( std::forward<T>( a_name ), a_target );
    }

    std::string GetName()const
    {
        return m_name;
    }

    bool HandleEventChar( const char& a_char )
    {
        bool ret = false;
        if( std::toupper( a_char ) == std::toupper( m_target ) )
        {
            if( !m_ActionActiveSignal.empty() )
            {
                m_ActionActiveSignal( m_name );
            }
            ret = true;
        }
        return ret;
    }

    boost_ns::signals2::connection ConnectActionActived( std::function<void(std::string)> a_slot )
    {
        return m_ActionActiveSignal.connect( a_slot );
    }

    void SetVisiable( bool a_visiable = true )
    {
        m_visiable = a_visiable;
    }

    bool GetVisiable()const
    {
        return m_visiable;
    }

    static std::shared_ptr<Action> MakeBackAction();

    static std::shared_ptr<Action> MakeQuitAction();

    Action( Action&& r )
    {
        m_name = std::move( r.m_name );
        m_ActionActiveSignal = std::move( r.m_ActionActiveSignal );
        m_target = r.m_target;
    }

private:

    template<typename T>
    void SetNameInternal( T&& a_name, const char& a_target )
    {
        m_name = std::string( std::forward<T>( a_name ) );
        m_target = a_target;
        GenerateShowString();
    }

    void GenerateShowString();

    boost_ns::signals2::signal<void( std::string )> m_ActionActiveSignal;
    std::string m_name;
    char        m_target = '\0';
    bool        m_visiable = true;
};

class Menu : public TitleContent
{

public:

    void AddAction( Action&& a_action )
    {
        m_actions.push_back( std::make_shared<Action>( std::forward<Action>( a_action ) ) );
    }

    void AddAction( std::shared_ptr<Action> a_action )
    {
        m_actions.push_back( a_action );
    }

    void show();

    std::string GetPrintableString()const override;

    bool empty()const
    {
        return m_actions.empty();
    }
 
    virtual bool OnEvent( std::shared_ptr<Event> a_event );

    std::vector< std::shared_ptr<Action> > GetActions()
    {
        return m_actions;
    }

private:

    std::vector< std::shared_ptr<Action> > m_actions;
};

