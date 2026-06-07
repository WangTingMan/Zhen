#pragma once
#include "Zhen/BasePage.h"
#include "BT/Adaptor.h"

class OptionContent;
class PageBluetoothDeviceSetting;
class BluetoothHomePage : public BasePage
{

public:

    BluetoothHomePage();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

    void LoadBluetoothModules();

private:

    std::shared_ptr<BasePage> m_bluetoothDeviceSettingPage;
    std::shared_ptr<BasePage> m_deviceListPage;
    std::shared_ptr<BasePage> m_handfreeAgPage;
    std::shared_ptr<BasePage> m_handfreePage;
    std::shared_ptr<BasePage> m_mediaSinkPage;
    std::shared_ptr<BasePage> m_mediaSourcePage;
    std::shared_ptr<BasePage> m_phonebookPage;
    std::shared_ptr<BasePage> m_mainGattPage;
    std::shared_ptr<BasePage> m_hidDevicePage;
    std::shared_ptr<BasePage> m_gattPage;
    std::shared_ptr<BasePage> m_hidHostPage;
    std::shared_ptr<BasePage> m_panPage;
    std::shared_ptr<OptionContent> m_content;
};

