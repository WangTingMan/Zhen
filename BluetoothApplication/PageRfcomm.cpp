#include "PageRfcomm.h"

PageRfcomm::PageRfcomm()
{

}

PageRfcomm::~PageRfcomm()
{

}

std::string PageRfcomm::GetPrintableString()const
{
    std::string str = BasePage::GetPrintableString();
    return str;
}

bool PageRfcomm::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = m_commandScheduler.OnEvent( a_event );

    if( !r )
    {
        BasePage::OnEvent( a_event );
    }

    return r;
}

void PageRfcomm::UpdateStatusContent()
{

}
