#pragma once
#include "Event.h"
#include <type_traits>
#include <string>

class KeyBoardInputEvent : public Event
{

public:

    template<typename T,
             typename = typename std::enable_if<
                !std::is_same<KeyBoardInputEvent, typename std::decay<T>::type>::value &&
                !std::is_base_of<KeyBoardInputEvent, T>::value &&
                !std::is_base_of<Event,T>::value &&
                std::is_constructible<std::string, T>::value,
                std::true_type
                >::type
            >
        KeyBoardInputEvent( T&& a_input )
        : m_textInput( std::forward<T&&>( a_input ) )
    {
        SetEventType( Event::EventType::KeyBoardInputEvent );
        static_assert( std::is_constructible<std::string, T>::value, "std::string cannot construct with a_input!" );
    }

    KeyBoardInputEvent( KeyBoardInputEvent&& r )
        : m_textInput( std::move( r.m_textInput ) )
    {
    }

    KeyBoardInputEvent( const KeyBoardInputEvent& r )
        : m_textInput( r.m_textInput )
    {

    }

    inline std::string&& ExtractInputedText()
    {
        return std::move( m_textInput );
    }

    inline const std::string& GetInputedText()const
    {
        return m_textInput;
    }

private:

    std::string m_textInput;
};

