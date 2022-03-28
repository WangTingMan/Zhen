#pragma once
#include "BasePage.h"
#include "PageManager.h"
#include "OptionContent.h"
#include "Zhen/StringContent.h"

class StringContent;
class BasePromptedPage : public BasePage
{

public:

    BasePromptedPage();

    template<typename T,
        typename = typename std::enable_if<
        std::is_constructible<std::string, T>::value,
        std::true_type>::type>
        void SetContentString( T&& a_string )
    {
        static_assert( std::is_constructible<std::string, T>::value, "std::string cannot construct with a_string!" );
        m_content->SetString( std::forward<T>( a_string ) );
    }

    std::string GetPrintableString()const override;

    ~BasePromptedPage();

private:

    std::shared_ptr<StringContent> m_content;
    boost::signals2::connection m_pageToPopedConnection;
};

