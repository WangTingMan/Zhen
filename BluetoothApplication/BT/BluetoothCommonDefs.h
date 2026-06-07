#pragma once
#include <string>

enum class DeviceControllerType
{
    BREDR,
    LEONLY,
    DUAL
};

enum class BondState
{
    NoneBond,
    Bonding,
    BondCompleted
};

enum class AdaptorEnableState
{
    Disabled,
    Enabling,
    Enabled,
    Disabling
};

enum class DiscoveryState
{
    BT_DISCOVERY_STOPPED,
    BT_DISCOVERY_STARTED
};

enum class SppPairingMethod
{
    BT_SSP_VARIANT_PASSKEY_CONFIRMATION,
    BT_SSP_VARIANT_PASSKEY_ENTRY,
    BT_SSP_VARIANT_CONSENT,
    BT_SSP_VARIANT_PASSKEY_NOTIFICATION
};

enum class ConnectionStatus
{
    ServiceDisconnected,
    ServiceConnecting,
    ServiceConnected,
    ServiceDisconnecting
};

enum class CallMode
{
    Voice,
    Data,
    Fax
};

enum class CallDirection
{
    CallOutgoing,
    CallIncoming
};

enum class CallState
{
    ACTIVE = 0,
    HELD,
    DIALING,
    ALERTING,
    INCOMING,
    WAITING,
    IDLE,
    DISCONNECTED,
};

enum class CallType
{
    SINGLE = 0,
    MULTI
};

enum class CallAddrType
{
    UNKNOWN,
    INTERNATIONAL,
};

enum class AudioPathState
{
    SUSPEND,
    STARTED,
    STOPPED
};

/* Define the Media Attribute IDs
*/
enum class AvrcpMediaAttributeType
{
    TITLE,
    ARTIST,
    ALBUM,
    TRACK_NUM,
    NUM_TRACKS,
    GENRE,
    PLAYING_TIME /* in miliseconds */
};

enum class AvrcpPlayStatus
{
    STOPPED = 0x00,  /* Stopped */
    PLAYING = 0x01,  /* Playing */
    PAUSED = 0x02,   /* Paused  */
    FWD_SEEK = 0x03, /* Fwd Seek*/
    REV_SEEK = 0x04, /* Rev Seek*/
    ERROR_STATUS = 0xFF     /* Error   */
};

enum class AvrcpRemoteFeature
{
    NONE = 0x00,            /* AVRCP 1.0 */
    METADATA = 0x01,        /* AVRCP 1.3 */
    ABSOLUTE_VOLUME = 0x02, /* Supports TG role and volume sync */
    BROWSE = 0x04,          /* AVRCP 1.4 and up, with Browsing support */
};

enum class AvrcpPassThroughKeyCode : uint8_t
{
    PASS_THRU_CMD_ID_INVALID_KEY = 0x00,
    PASS_THRU_CMD_ID_PLAY = 0x44,
    PASS_THRU_CMD_ID_PAUSE = 0x46,
    PASS_THRU_CMD_ID_VOL_UP = 0x41,
    PASS_THRU_CMD_ID_VOL_DOWN = 0x42,
    PASS_THRU_CMD_ID_STOP = 0x45,
    PASS_THRU_CMD_ID_FF = 0x49,
    PASS_THRU_CMD_ID_REWIND = 0x48,
    PASS_THRU_CMD_ID_FORWARD = 0x4B,
    PASS_THRU_CMD_ID_BACKWARD = 0x4C
};

enum class AvrcpPassThroughKeyState : uint8_t
{
    KEY_STATE_PRESSED = 0,
    KEY_STATE_RELEASED = 1
};

enum class BtSocketType
{
    SOCK_RFCOMM = 1,
    SOCK_SCO = 2,
    SOCK_L2CAP = 3,
    SOCK_L2CAP_LE = 4
};

std::string ConnectionStatusToString( ConnectionStatus a_status );

std::string CallStateToString( CallState a_status );

std::string AvrcpMediaAttributeTypeToString( AvrcpMediaAttributeType a_status );

std::string AvrcpPlayStatusToString( AvrcpPlayStatus  a_status );
