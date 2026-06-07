#include "PageDeviceInformation.h"
#include "Zhen/StringContent.h"
#include "BT/Adaptor.h"

PageDeviceInformation::PageDeviceInformation()
{
    SetPageName( "PageDeviceInformation" );
    SetTitle( "Device Information" );
    m_content = std::make_shared<StringContent>();
    m_content->SetTitle( "Detail" );
    SetMenuPos();

    discoverySignalConnection = 
        Adaptor::GetInstance().ConnectDiscoveryStateChanged
        (
        [ this ](DiscoveryState a_state )mutable
        {
            this->PageNeedFresh();
        }
        );
}

PageDeviceInformation::~PageDeviceInformation()
{
    discoverySignalConnection.disconnect();
}

std::string PageDeviceInformation::GetPrintableString()const
{
    std::string r = BasePage::GetPrintableString();
    m_content->SetString( GetDeviceInformation() );
    r.append( m_content->GetPrintableString() );
    return r;
}

std::string PageDeviceInformation::GetDeviceInformation()const
{
    std::string r;

    r.append( "Bluetooth power status:" ).append( ( Adaptor::GetInstance().isPowerOn() ? "on" : "off" ) );
    r.append( "\n" );

    r.append( "Search status: " ).append( ( Adaptor::GetInstance().GetSearchState() == DiscoveryState::BT_DISCOVERY_STARTED ? "Searching" : "Stopped" ) );
    r.push_back( '\n' );

    r.append( "Device Name: " ).append( Adaptor::GetInstance().GetLocalName() );
    r.push_back( '\n' );


    bool discoverable = false;
    bool connectable = false;
    bool pairable = false;
    Adaptor::GetInstance().GetLocalDeviceSettings( pairable, discoverable, connectable );
    r.append( "Others: " ).append( ( discoverable ? "discoverable" : "non-discoverable" ) ).push_back( ' ' );
    r.append( ( connectable ? "connectable" : "non-connectable" ) ).push_back( ' ' );
    r.append( ( pairable ? "pairable" : "non-pairable" ) );
    r.push_back( '\n' );

    return r;
}
