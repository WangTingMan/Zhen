#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/StrandCommandScheduler.h"

class OptionContent;
class PageBluetoothDeviceSetting : public BasePage
{

public:

    PageBluetoothDeviceSetting();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

    bool HandleSetLocalDeviceName( std::string const& a_input );

private:

    std::shared_ptr< OptionContent> m_optionContent;
    StrandCommandScheduler m_commandScheduler;
};

