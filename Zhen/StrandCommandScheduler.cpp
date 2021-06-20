#include "StrandCommandScheduler.h"
#include "KeyBoardInputEvent.h"

#include <iostream>

StrandCommandScheduler::StrandCommandScheduler()
{

}

bool StrandCommandScheduler::OnEvent( std::shared_ptr<Event> a_event )
{
    bool ret = false;
    if( Event::EventType::KeyBoardInputEvent != a_event->GetEventType() )
    {
        return ret;
    }

    std::shared_ptr<KeyBoardInputEvent> event = std::dynamic_pointer_cast< KeyBoardInputEvent >( a_event );
    std::string input = event->GetInputedText();
    if( m_callBack )
    {
        if( m_executTimeCurrently++ > m_maxExecutionTimes )
        {
            std::cout << "Had tried more times. abort current operation.\n";
            clear();
            ret = true;
            return ret;
        }

        bool r = m_callBack( input );
        if( r )
        {
            clear();
        }
        ret = true;
    }
    return ret;
}

void StrandCommandScheduler::start()
{
    if( m_callBack )
    {
        bool r = m_callBack( "" );
        if( r )
        {
            clear();
        }
    }
}

void StrandCommandScheduler::AddParameter
    (
    const std::any& a_parameter
    )
{
    m_parameters.push_back( a_parameter );
}

std::any StrandCommandScheduler::GetParameter
    (
    const std::size_t a_index
    ) const
{
    std::any para;
    if( a_index < m_parameters.size() )
    {
        para = m_parameters[a_index];
    }
    return para;
}

void StrandCommandScheduler::clear()
{
    BackToBeginning();
    m_callBack = std::function<bool( std::string const& )>();
}

void StrandCommandScheduler::BackToBeginning()
{
    m_executTimeCurrently = 0;
    m_stepNumber = 0u;
    m_parameters.clear();
}
