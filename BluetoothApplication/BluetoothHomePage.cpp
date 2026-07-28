#include "BluetoothHomePage.h"
#include "PageA2dpSource.h"
#include "PageBluetoothDeviceSetting.h"
#include "PageDeviceList.h"
#include "PageHandfree.h"
#include "PageMedia.h"
#include "PageHandfreeAg.h"
#include "PageGatt.h"
#include "PageHidHost.h"
#include "PagePan.h"
#include "PageRfcomm.h"
#include "PageL2CAP.h"

#include "Zhen/OptionContent.h"
#include "Zhen/PageManager.h"

#include "BT/avrcp/BluetoothAvrcpController.h"
#include "Bt/BluetoothHF.h"
#include <Bt/a2dp/BluetoothA2DPSource.h>
#include <BT/avrcp/BluetoothAvrcpTarget.h>
#include <Bt/BluetoothA2DPSink.h>
#include "BT/BluetoothHFAG.h"
#include "BT/BluetoothPAN.h"
#include <BT/rfcomm/BluetoothRfcomm.h>
#include <BT/HID/BluetoothHidHost.h>

BluetoothHomePage::BluetoothHomePage()
{
    SetPageName( "BluetoothHomePage" );
    std::shared_ptr<OptionContent> content = std::make_shared<OptionContent>();

    LoadBluetoothModules();

    content->AddOption( "Device Setting",
                        [this]()mutable
                        {
                            if( !m_bluetoothDeviceSettingPage )
                            {
                                m_bluetoothDeviceSettingPage = std::make_shared<PageBluetoothDeviceSetting>();
                            }

                            std::shared_ptr<BasePage> page = m_bluetoothDeviceSettingPage;
                            PageManager::GetInstance().PushPage( std::move( page ) );
                        } );

    content->AddOption( "Device List",
                        [this]()mutable
                        {
                            if( !m_deviceListPage )
                            {
                                m_deviceListPage = std::make_shared<PageDeviceList>();
                            }

                            std::shared_ptr<BasePage> page = m_deviceListPage;
                            PageManager::GetInstance().PushPage( std::move( page ) );
                        } );

    content->AddOption( "Handfree",
                        [this]()mutable
                        {
                            if( !m_handfreePage )
                            {
                                m_handfreePage = std::make_shared<PageHandfree>();
                            }

                            std::shared_ptr<BasePage> page = m_handfreePage;
                            PageManager::GetInstance().PushPage( std::move( page ) );
                        } );

    if (Bluetooth::BluetoothA2DPSink::GetMoudle())
    {
        content->AddOption("Media Sink",
            [this]()mutable
            {
                if (!m_mediaSinkPage)
                {
                    m_mediaSinkPage = std::make_shared<PageMedia>();
                }

                std::shared_ptr<BasePage> page = m_mediaSinkPage;
                PageManager::GetInstance().PushPage(std::move(page));
            });
    }

    if (Bluetooth::BluetoothA2DPSource::GetMoudle())
    {
        content->AddOption("Media Source",
            [this]()mutable
            {
                if (!m_mediaSourcePage)
                {
                    m_mediaSourcePage = std::make_shared<PageA2dpSource>();
                }

                std::shared_ptr<BasePage> page = m_mediaSourcePage;
                PageManager::GetInstance().PushPage(std::move(page));
            });
    }

    if( Bluetooth::BluetoothHFAG::GetMoudle() )
    {
        content->AddOption( "Handfree Service",
                            [ this ]()mutable
                            {
                                if( !m_handfreeAgPage )
                                {
                                    m_handfreeAgPage = std::make_shared<PageHandfreeAg>();
                                }

                                std::shared_ptr<BasePage> page = m_handfreeAgPage;
                                PageManager::GetInstance().PushPage( std::move( page ) );
                            } );
    }

    content->AddOption("GATT Pages",
        [this]()mutable
        {
            if (!m_gattPage)
            {
                m_gattPage = std::make_shared<PageGatt>();
            }

            std::shared_ptr<BasePage> page = m_gattPage;
            PageManager::GetInstance().PushPage(std::move(page));
        });

    content->AddOption("HID Host",
        [this]()mutable
        {
            if (!m_hidHostPage)
            {
                m_hidHostPage = std::make_shared<PageHidHost>();
            }

            std::shared_ptr<BasePage> page = m_hidHostPage;
            PageManager::GetInstance().PushPage(std::move(page));
        });

    content->AddOption("PAN",
        [this]()mutable
        {
            if (!m_panPage)
            {
                m_panPage = std::make_shared<PagePan>();
            }

            std::shared_ptr<BasePage> page = m_panPage;
            PageManager::GetInstance().PushPage(std::move(page));
        });

    content->AddOption( "FRCOMM",
        [ this ]()mutable
        {
            if( !m_rfcommPage )
            {
                m_rfcommPage = std::make_shared<PageRfcomm>();
            }

            std::shared_ptr<BasePage> page = m_rfcommPage;
            PageManager::GetInstance().PushPage( std::move( page ) );
        } );

    content->AddOption( "L2CAP COC",
        [ this ]()mutable
        {
            if( !m_l2capPage )
            {
                m_l2capPage = std::make_shared<PageL2CAP>();
            }

            std::shared_ptr<BasePage> page = m_l2capPage;
            PageManager::GetInstance().PushPage( std::move( page ) );
        } );

    content->SetTitle( "Basic Operations" );
    m_content = content;
    SetTitle( "Home Page" );
}

std::string BluetoothHomePage::GetPrintableString()const
{
    std::string ret = BasePage::GetPrintableString();
    ret.append( m_content->GetPrintableString() );
    return ret;
}

bool BluetoothHomePage::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = m_content->OnEvent( a_event );
    if( !r )
    {
        r = BasePage::OnEvent( a_event );
    }
    return r;
}

void BluetoothHomePage::LoadBluetoothModules()
{
    auto moduleMgr = Adaptor::GetInstance().GetModuleManager();

    std::shared_ptr<AbstractModule> module_ = std::make_shared<Bluetooth::BluetoothHF>();
    moduleMgr->AddNewModule( module_ );

    module_ = std::make_shared<Bluetooth::BluetoothHFAG>();
    moduleMgr->AddNewModule( module_ );

    //module_ = std::make_shared<Bluetooth::BluetoothA2DPSource>();
    //moduleMgr->AddNewModule( module_ );

    module_ = std::make_shared<Bluetooth::BluetoothA2DPSink>();
    moduleMgr->AddNewModule( module_ );

    module_ = std::make_shared<Bluetooth::BluetoothAvrcpTarget>();
    moduleMgr->AddNewModule( module_ );

    module_ = std::make_shared<Bluetooth::BluetoothAvrcpController>();
    moduleMgr->AddNewModule( module_ );

    module_ = std::make_shared<Bluetooth::BluetoothRfcomm>();
    moduleMgr->AddNewModule(module_);

    module_ = std::make_shared<Bluetooth::BluetoothHidHost>();
    moduleMgr->AddNewModule(module_);

    module_ = std::make_shared<Bluetooth::BluetoothPAN>();
    moduleMgr->AddNewModule(module_);
}

