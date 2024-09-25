#include "menu.h"
#include "PageManager.h"
#include "KeyBoardInputEvent.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <iostream>

void Action::GenerateShowString()
{
    auto it = std::find_if( m_name.begin(), m_name.end(), 
                  []( const std::string::value_type& ch )
                    {
                        return '[' == ch;
                    } );
    if( it != m_name.end() )
    {
        return;
    }

    it = std::find_if( m_name.begin(), m_name.end(),
                  [this]( const std::string::value_type& ch )
                    {
                        return std::toupper( ch ) == std::toupper( m_target );
                    } );
    if( it != m_name.end() )
    {
        it = m_name.insert( it, '[' );
        std::advance( it, 2 );
        m_name.insert( it, ']' );
    }
}

std::shared_ptr<Action> Action::MakeBackAction()
{
    static std::shared_ptr<Action> action;
    if( !action )
    {
        action = std::make_shared<Action>( "Back", 'B' );
        action->ConnectActionActived( []( std::string )
        {
            PageManager::GetInstance().PopPage( PageManager::GetInstance().GetTopPage() );
        }
        );
    }
    return action;
}

std::shared_ptr<Action> Action::MakeQuitAction()
{
    static std::shared_ptr<Action> action;
    if( !action )
    {
        action = std::make_shared<Action>( "Quit", 'q' );
        action->ConnectActionActived( []( std::string )
        {
            PageManager::GetInstance().quit();
        }
        );
    }
    return action;
}

std::string Menu::GetPrintableString()const
{
    std::string ret;
    std::string str;
    int width = PageManager::GetInstance().GetDisplayAreaWidth();
    for( auto& ele : m_actions )
    {
        if( ele->GetVisiable() )
        {
            str.append( ele->GetName() ).append( 4, ' ' );
            if( str.size() >= width )
            {
                str.append( "\n" );
            }
        }
    }

    if( !str.empty() )
    {
        ret = TitleContent::GetPrintableString();
        ret.append( str ).append( "\n" );
    }
    return ret;
}

void Menu::show()
{
    int width = PageManager::GetInstance().GetDisplayAreaWidth();

    std::string str;
    for( auto& ele : m_actions )
    {
        str.append( ele->GetName() ).append( 4, ' ' );
        if( str.size() >= width )
        {
            std::cout << str << std::endl;
            str.clear();
        }
    }
    if( !str.empty() )
    {
        std::cout << str << std::endl;
    }
}

bool Menu::OnEvent( std::shared_ptr<Event> a_event )
{
    bool ret = false;
    char ch = '\0';
    if( Event::EventType::KeyBoardInputEvent == a_event->GetEventType() )
    {
        std::shared_ptr<KeyBoardInputEvent> detailEvent = std::dynamic_pointer_cast< KeyBoardInputEvent >( a_event );
        if( 1 == detailEvent->GetInputedText().size() )
        {
            ch = detailEvent->GetInputedText().at( 0 );
        }
    }

    if( '\0' != ch )
    {
        for( auto & ele : m_actions )
        {
            if( ele->GetVisiable() )
            {
                ret = ele->HandleEventChar( ch );
                if( ret )
                {
                    break;
                }
            }
        }
    }
    return ret;
}
