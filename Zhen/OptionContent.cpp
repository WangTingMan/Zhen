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
    for( auto & ele : m_opthions )
    {
        if( ele.cmd_index == event->GetInputedText() )
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
    if( m_opthions.empty() )
    {
        return ret;
    }

    ret = TitleContent::GetPrintableString();

    for( int i = 0; i < m_opthions.size(); ++i )
    {
        if( !m_opthions[i].enabled )
        {
            continue;
        }

        if( m_executable )
        {
            ret.append( "[" ).append( m_opthions[i].cmd_index ).append( "]" );
        }
        else
        {
            ret.append( m_opthions[i].cmd_index );
        }
        ret.append( ". " ).append( m_opthions[i].opthion ).append( "\n" );
    }
    return ret;
}

std::string OptionContent::AddOption
    (
    std::string a_option,
    std::function<void()> a_optionSelectCallBack
    )
{
    Opthion opthion;
    opthion.callback = a_optionSelectCallBack;
    opthion.opthion = a_option;
    opthion.index = std::to_string( m_opthions.size() + 1 );
    opthion.cmd_index = opthion.index;

    m_opthions.push_back( opthion );

    GenerateCommandIndex();
    return opthion.index;
}

void OptionContent::SetOptionEnable( std::string a_index, bool a_enable )
{
    bool changed = false;
    for( auto& ele : m_opthions )
    {
        if( ele.index == a_index )
        {
            if( ele.enabled != a_enable )
            {
                ele.enabled = a_enable;
                changed = true;
                break;
            }
        }
    }

    if( changed )
    {
        GenerateCommandIndex();
    }
}

void OptionContent::ChangedOptionTitle( std::string a_index, std::string a_tile )
{
    for( auto& ele : m_opthions )
    {
        if( ele.index == a_index )
        {
            ele.opthion = a_tile;
            break;
        }
    }
}

void OptionContent::ChangeOptionAction( std::string a_index, std::function<void()> a_action )
{
    for( auto& ele : m_opthions )
    {
        if( ele.index == a_index )
        {
            ele.callback = a_action;
            break;
        }
    }
}

void OptionContent::GenerateCommandIndex()
{
    int i = 0;
    for( auto& ele : m_opthions )
    {
        if( ele.enabled )
        {
            ele.cmd_index = std::to_string( ++i );
        }
    }
}

