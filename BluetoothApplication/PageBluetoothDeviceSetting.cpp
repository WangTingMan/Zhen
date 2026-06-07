#include "PageBluetoothDeviceSetting.h"
#include "PageDeviceInformation.h"
#include "Zhen/OptionContent.h"
#include "BT/Adaptor.h"
#include <iostream>

PageBluetoothDeviceSetting::PageBluetoothDeviceSetting()
{
    SetPageName( "PageBluetoothDeviceSetting" );
    SetTitle( "Device Settings" );
    m_optionContent = std::make_shared<OptionContent>();
    m_optionContent->SetTitle( "Device Options" );
    m_optionContent->AddOption
                        ( "Device infomation",
                        []()mutable
                        {
                            PageManager::GetInstance().PushPage( std::make_shared<PageDeviceInformation>() );
                        }
                        );
    m_optionContent->AddOption
                        ( "Power On",
                        std::bind( &Adaptor::Enable, &Adaptor::GetInstance() )
                        );
    m_optionContent->AddOption
                        ( "Power Off",
                        std::bind( &Adaptor::deinit, &Adaptor::GetInstance() )
                        );
    m_optionContent->AddOption
                        ( "Set Local Device Name",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind( 
                                 &PageBluetoothDeviceSetting::HandleSetLocalDeviceName,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        }
                        );
    m_optionContent->AddOption( "Set Local Device Address" );
    m_optionContent->AddOption( "Set Discoverable",
                        [this]()mutable
                        {
                            bool discoverable = false;
                            bool connectable = false;
                            bool pairable = false;
                            Adaptor::GetInstance().GetLocalDeviceSettings( pairable, discoverable, connectable );
                            Adaptor::GetInstance().SetLocalDeviceSettings( pairable, true, true );
                        } );
    m_optionContent->AddOption( "Set Non-discoverable",
                        [this]()mutable
                        {
                            bool discoverable = false;
                            bool connectable = false;
                            bool pairable = false;
                            Adaptor::GetInstance().GetLocalDeviceSettings( pairable, discoverable, connectable );
                            Adaptor::GetInstance().SetLocalDeviceSettings( pairable, false, connectable );
                        } );
    m_optionContent->AddOption( "Set Pairable",
                        [this]()mutable
                        {
                            bool discoverable = false;
                            bool connectable = false;
                            bool pairable = false;
                            Adaptor::GetInstance().GetLocalDeviceSettings( pairable, discoverable, connectable );
                            Adaptor::GetInstance().SetLocalDeviceSettings( true, discoverable, connectable );
                        } );
    m_optionContent->AddOption( "Set Non-pairable",
                        [this]()mutable
                        {
                            bool discoverable = false;
                            bool connectable = false;
                            bool pairable = false;
                            Adaptor::GetInstance().GetLocalDeviceSettings( pairable, discoverable, connectable );
                            Adaptor::GetInstance().SetLocalDeviceSettings( false, discoverable, connectable );
                        } );
    m_optionContent->AddOption( "Set Connectable",
                        [this]()mutable
                        {
                            bool discoverable = false;
                            bool connectable = false;
                            bool pairable = false;
                            Adaptor::GetInstance().GetLocalDeviceSettings( pairable, discoverable, connectable );
                            Adaptor::GetInstance().SetLocalDeviceSettings( pairable, discoverable, true );
                        } );
    m_optionContent->AddOption( "Set Non-connectable",
                        [this]()mutable
                        {
                            bool discoverable = false;
                            bool connectable = false;
                            bool pairable = false;
                            Adaptor::GetInstance().GetLocalDeviceSettings( pairable, discoverable, connectable );
                            Adaptor::GetInstance().SetLocalDeviceSettings( pairable, discoverable, false );
                        } );
    m_optionContent->AddOption( "Test Lib",
                        [this]()mutable
                        {
                            Adaptor::GetInstance().Test();
                        } );
}

std::string PageBluetoothDeviceSetting::GetPrintableString()const
{
    std::string ret = BasePage::GetPrintableString();
    ret.append( m_optionContent->GetPrintableString() );
    return ret;
}

bool PageBluetoothDeviceSetting::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = false;
    r = m_commandScheduler.OnEvent( a_event );
    if( r )
    {
        return r;
    }

    r = m_optionContent->OnEvent( a_event );
    if( r )
    {
        return r;
    }

    r = BasePage::OnEvent( a_event );
    return r;
}

bool PageBluetoothDeviceSetting::HandleSetLocalDeviceName( std::string const& a_input )
{
    bool done = false;
    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::cout << "Enter device name: " << std::endl;
        m_commandScheduler.SetStepNumber( 1 );
    }
    else if( 1 == m_commandScheduler.GetStepNumber() )
    {
        m_commandScheduler.AddParameter( a_input );
    }

    if( !done && 1 == m_commandScheduler.GetNumParameters() )
    {
        std::string name = std::any_cast< std::string >( m_commandScheduler.GetParameter( 0 ) );
        Adaptor::GetInstance().SetLocalName( name );
        done = true;
    }
    return done;
}
