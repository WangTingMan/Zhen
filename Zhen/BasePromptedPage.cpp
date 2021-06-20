#include "BasePromptedPage.h"
#include "Zhen/StringContent.h"

BasePromptedPage::BasePromptedPage()
{
    SetPageName( "PromptedPage" );
    m_content = std::make_shared<StringContent>();
    m_content->SetTitle( "Detail" );
}

BasePromptedPage::~BasePromptedPage()
{

}

std::string BasePromptedPage::GetPrintableString()const
{
    std::string ret = BasePage::GetPrintableString();
    ret.append( m_content->GetPrintableString() );
    return ret;
}
