#include "PageMediaStatus.h"

#include "BT/BluetoothA2DPSink.h"  
#include "BT/avrcp/BluetoothAvrcpController.h"

PageMediaStatus::PageMediaStatus()
{
    SetTitle( "Media play status" );
    RefreshMediaStatus();
    m_postChangedConnection = Bluetooth::BluetoothAvrcpController::GetMoudle()->ConnectToPlayposChanged
        ( std::bind( &PageMediaStatus::HandleMediaStatusChanged, this ) );
}

PageMediaStatus::~PageMediaStatus()
{
    m_postChangedConnection.disconnect();
}

void PageMediaStatus::HandleMediaStatusChanged()
{
    RefreshMediaStatus();
    PageNeedFresh();
}

void PageMediaStatus::RefreshMediaStatus()
{
    std::string statusString;
    std::vector<std::pair< AvrcpMediaAttributeType, std::string> > trackInfo;
    BluetoothAddress address;
    auto sink = Bluetooth::BluetoothAvrcpController::GetMoudle();
    sink->GetPlayTrack( address, trackInfo );
    for( auto& ele : trackInfo )
    {
        statusString.append( AvrcpMediaAttributeTypeToString( ele.first ) ).append( ": " );
        statusString.append( ele.second ).push_back( '\n' );
    }

    statusString.append( "Play status: " ).append( AvrcpPlayStatusToString( sink->GetPlayStatus() ) ).push_back( '\n' );

    std::size_t length;
    std::size_t currentPos;
    sink->GetPlayPos( length, currentPos );
    auto secondToStringFuntor = []( std::size_t a_miliSeconds ) -> std::string
    {
        std::string ret;
        std::size_t a_seconds = a_miliSeconds;
        a_seconds /= 1000;
        std::size_t minutes = a_seconds / 60;
        a_seconds = a_seconds - minutes * 60;
        std::size_t hours = minutes / 60;
        auto formatTimeFunctor = []( std::size_t a_time )->std::string
        {
            std::string ret = std::to_string( a_time );
            if( ret.size() == 1 )
            {
                ret.insert( ret.begin(), '0' );
            }
            return ret;
        };

        if( hours > 0 )
        {
            minutes = minutes - hours * 60;
            ret.append( formatTimeFunctor( hours ) ).append( ":" );
        }
        ret.append( formatTimeFunctor( minutes ) ).append( ":" );
        ret.append( formatTimeFunctor( a_seconds ) );
        return ret;
    };
    statusString.append( "Current pos: " ).append( secondToStringFuntor( currentPos ) ).append( "\n" );
    statusString.append( "Total length: " ).append( secondToStringFuntor( length ) ).append( "\n" );

    SetContentString( statusString );
}

