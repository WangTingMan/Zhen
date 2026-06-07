#include "BluetoothCommonDefs.h"

std::string ConnectionStatusToString( ConnectionStatus a_status )
{
    switch( a_status )
    {
    case ConnectionStatus::ServiceDisconnected:
        return "disconnected";
        break;
    case ConnectionStatus::ServiceConnecting:
        return "connecting";
        break;
    case ConnectionStatus::ServiceConnected:
        return "connected";
        break;
    case ConnectionStatus::ServiceDisconnecting:
        return "disconnecting";
        break;
    default:
        break;
    }
    return "";
}

std::string CallStateToString( CallState a_status )
{
    std::string str;
    switch( a_status )
    {
    case CallState::ACTIVE:
        str.assign( "active" );
        break;
    case CallState::HELD:
        str.assign( "held" );
        break;
    case CallState::DIALING:
        str.assign( "dialing" );
        break;
    case CallState::ALERTING:
        str.assign( "alerting" );
        break;
    case CallState::INCOMING:
        str.assign( "incoming" );
        break;
    case CallState::WAITING:
        str.assign( "waiting" );
        break;
    default:
        break;
    }
    return str;
}

std::string AvrcpMediaAttributeTypeToString( AvrcpMediaAttributeType a_status )
{
    std::string str;
    switch( a_status )
    {
    case AvrcpMediaAttributeType::TITLE:
        str = "Title";
        break;
    case AvrcpMediaAttributeType::ARTIST:
        str = "Artist";
        break;
    case AvrcpMediaAttributeType::ALBUM:
        str = "Album";
        break;
    case AvrcpMediaAttributeType::TRACK_NUM:
        str = "track number";
        break;
    case AvrcpMediaAttributeType::NUM_TRACKS:
        str = "Number tracks";
        break;
    case AvrcpMediaAttributeType::GENRE:
        str = "Genre";
        break;
    case AvrcpMediaAttributeType::PLAYING_TIME:
        str = "Playing time";
        break;
    default:
        break;
    }
    return str;
}

std::string AvrcpPlayStatusToString( AvrcpPlayStatus  a_status )
{
    std::string str;
    switch( a_status )
    {
    case AvrcpPlayStatus::STOPPED:
        str = "Stopped";
        break;
    case AvrcpPlayStatus::PLAYING:
        str = "Playing";
        break;
    case AvrcpPlayStatus::PAUSED:
        str = "Paused";
        break;
    case AvrcpPlayStatus::FWD_SEEK:
        str = "Forward seek";
        break;
    case AvrcpPlayStatus::REV_SEEK:
        str = "Reverse seek";
        break;
    case AvrcpPlayStatus::ERROR_STATUS:
        str = "Error status";
        break;
    default:
        break;
    }
    return str;
}

std::string GetPbapPceUuid()
{
    return "0000112e-0000-1000-8000-00805F9B34FB";
}
