#pragma once
#include "TitleContent.h"

class StringContent : public TitleContent
{

public:

    StringContent() {};

    template<typename T,
        typename = typename std::enable_if<
        std::is_constructible<std::string, T>::value,
        std::true_type>::type
        >
        void SetString( T&& a_string )
    {
        static_assert( std::is_constructible<std::string, T>::value, "std::string cannot construct with a_input!" );
        m_string = std::string( std::forward<T>( a_string ) );
        m_formated = false;
        FormatContentString();
    }

    std::string GetPrintableString()const override
    {
        std::string ret = TitleContent::GetPrintableString();
        ret.append( m_string );
        return ret;
    }

    bool OnEvent( std::shared_ptr<Event> a_event ) override
    {
        return false;
    }

private:

    void FormatContentString();

    std::string m_string;
    bool m_formated = false;
};

