#include "OptionContent.h"
#include "PageManager.h"
#include "KeyBoardInputEvent.h"

#include <iostream>

bool OptionContent::OnEvent( std::shared_ptr<Event> a_event )
{
    bool ret = false;
    if( Event::EventType::KeyBoardInputEvent != a_event->GetEventType() )
    {
        return ret;
    }

    std::shared_ptr<KeyBoardInputEvent> event = std::dynamic_pointer_cast< KeyBoardInputEvent >( a_event );
    for( auto & ele : m_options )
    {
        if( ele.visiable &&
            ( ele.index == event->GetInputedText() ) )
        {
            if( ele.callback )
            {
                ele.callback();
            }
            ret = true;
            break;
        }
    }
    return ret;
}

std::string OptionContent::GetPrintableString()const
{
    std::string ret;
    if( m_options.empty() )
    {
        return ret;
    }

    ret = TitleContent::GetPrintableString();

    for( int i = 0; i < m_options.size(); ++i )
    {
        if( !m_options[i].visiable )
        {
            continue;
        }

        if( m_executable )
        {
            ret.append( "[" ).append( m_options[i].index ).append( "]" );
        }
        else
        {
            ret.append( m_options[i].index );
        }
        ret.append( ". " ).append( m_options[i].option_title ).append( "\n" );
    }
    return ret;
}

bool OptionContent::SetVisiable
    (
    std::size_t a_id,
    bool a_visiable
    )
{
    if( a_id == s_invalid_id )
    {
        return false;
    }

    bool status = false;
    for( auto& ele : m_options )
    {
        if( ele.id == a_id )
        {
            if( a_visiable == ele.visiable )
            {
                return false;
            }
            ele.visiable = a_visiable;
            status = true;
            break;
        }
    }

    if( !status )
    {
        return status;
    }

    std::size_t current_index = 1;
    for( auto& ele : m_options )
    {
        if( ele.visiable )
        {
            ele.index = std::to_string( current_index++ );
        }
    }

    return status;
}

std::size_t OptionContent::AddOption
    (
    std::string a_option,
    std::function<void()> a_optionSelectCallBack
    )
{
    Option option;
    std::size_t id = m_options.size() + 1;
    option.callback = a_optionSelectCallBack;
    option.option_title = a_option;
    option.index = std::to_string( id );
    option.id = m_current_id++;
    option.visiable = true;

    m_options.push_back( option );
    return option.id;
}

