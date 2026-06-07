#pragma once
#include "Zhen/BasePage.h"

class StringContent;
class PageDeviceInformation : public BasePage
{

public:

    PageDeviceInformation();

    ~PageDeviceInformation();

    std::string GetPrintableString()const override;

private:

    std::string GetDeviceInformation()const;
    boost_ns::signals2::connection discoverySignalConnection;
    std::shared_ptr< StringContent > m_content;
};

