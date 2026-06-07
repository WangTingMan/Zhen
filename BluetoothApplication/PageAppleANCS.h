#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/OptionContent.h"
#include "Zhen/StrandCommandScheduler.h"

#include <BT/BluetoothCommonDefs.h>
#include <BT/BluetoothAddress.h>
#include <BT/stackLayer/BluetoothGattClientInterface.h>
#include <BT/GATT/BluetoothGattCommon.h>

#include <vector>
#include <bitset>
#include <deque>

enum class notification_attribute_id_t : uint8_t
{
    NotificationAttributeIDAppIdentifier = 0, /**< ie com.facebook.Messenger, or some other app */
    NotificationAttributeIDTitle = 1,    // (Needs to be followed by a 2-bytes max length parameter)
    NotificationAttributeIDSubtitle = 2, // (Needs to be followed by a 2-bytes max length parameter)
    NotificationAttributeIDMessage = 3,  // (Needs to be followed by a 2-bytes max length parameter)
    NotificationAttributeIDMessageSize = 4,
    NotificationAttributeIDDate = 5,
    NotificationAttributeIDPositiveActionLabel = 6,
    NotificationAttributeIDNegativeActionLabel = 7
};

enum class command_id_t : uint8_t
{
    CommandIDGetNotificationAttributes = 0,
    CommandIDGetAppAttributes = 1,
    CommandIDPerformNotificationAction = 2
};

class ANCSNotifyMessage
{
public:

    enum class EventID : uint8_t
    {
        Added = 0,
        Modified = 1,
        Removed = 2,
        ReservedID = 3
    };

    enum class EventFlags : uint8_t
    {
        Silent =  1 << 0,
        Importent = 1 << 1,
        PreExisting = 1 << 2,
        PositiveAction = 1 << 3,
        NegativeAction = 1 << 4,
        ReservedID = 1 << 5
    };

    enum class CategoryID : uint8_t
    {
        CategoryIDOther = 0,
        CategoryIDIncomingCall = 1,
        CategoryIDMissedCall = 2,
        CategoryIDVoicemail = 3,
        CategoryIDSocial = 4,
        CategoryIDSchedule = 5,
        CategoryIDEmail = 6,
        CategoryIDNews = 7,
        CategoryIDHealthAndFitness = 8,
        CategoryIDBusinessAndFinance = 9,
        CategoryIDLocation = 10,
        CategoryIDEntertainment = 11,
        CategoryIDReserved
    };

    ANCSNotifyMessage
        (
        EventID a_eventID,
        std::bitset<8> a_flags,
        CategoryID a_categoryID,
        uint8_t a_categoryCount,
        uint32_t a_notifyID
        )
        : m_eventID( a_eventID )
        , m_flags( a_flags )
        , m_categoryID( a_categoryID )
        , m_categoryCount( a_categoryCount )
        , m_notifyID( a_notifyID )
        {}

    uint32_t GetNotifyID()const
    {
        return m_notifyID;
    }

private:

    EventID m_eventID;
    CategoryID m_categoryID;
    uint8_t m_categoryCount = 0;
    uint32_t m_notifyID = 0u;
    std::bitset<8> m_flags;
};

struct DetailNotificationMessage
{
    uint32_t uid = 0;
    std::map< notification_attribute_id_t, std::string > elements;
};

class StringContent;

class PageAppleANCS : public BasePage
{

public:

    PageAppleANCS();

    ~PageAppleANCS();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

    //Handle the user input event
private:

    bool HandleConnectRequest
        (
        std::string const& a_input
        );

    void RegiterANCSClient();

    void HandleClientCallbackMessage( std::shared_ptr< GattClientBaseMessage> );

    void HandleDiscoveryANCSService();

    bool HandelNextGattCommand();

    void UpdateStatusContent();

    void ConfigureANCSNotification();

    void HandleANCSNotifySourceNotifyEvent( std::shared_ptr< NotifyMessage> a_notfiy );

    void ParseDataSource( uint8_t* a_raw, uint32_t a_size );

    void ParseApplicationAttribute( uint8_t* a_raw, uint32_t a_size );

    void EnqueueCommandTask( std::string a_description, std::function<void()> a_task, bool a_postToLast = true );

    void RetrieveDetailMassge();

    void UpdateNextPreActionVisiable();

    // Handle event from low level stack.
private:

    void HandleClientRegistered
        (
        int status,
        int client_if,
        std::vector<uint8_t> uuid
        );

    void HandleANCSConnectionStatusChanged
        (
        int conn_id,
        ConnectionStatus a_status,
        int client_if,
        BluetoothAddress addr
        );

    void HandleServiceDetailSearched
        (
        std::vector<GATT_DB_ELEMENT> a_db_elements,
        int a_connect_id
        );

    void HandleDescriptorReadCompleted
        (
        int conn_id, int status,
        btgatt_read_params_type a_descriptor
        );

    void HandleCharacteristicWriteCompleted
        (
        int conn_id, int status,
        uint16_t handle, uint16_t len,
        std::vector<uint8_t> value
        );

private:

    static constexpr uint32_t s_longTerm = 3000;
    static constexpr uint32_t s_shortTerm = 200;
    struct GattCommandTask
    {
        GattCommandTask() {}
        GattCommandTask( GattCommandTask&& right ) noexcept
            : description( std::move( right.description ) )
            , task( std::move( right.task ) )
        {

        }

        GattCommandTask( std::string&& a_description, std::function<void()>&& a_task )
            : description( std::move( a_description ) )
            , task( std::move( a_task ) )
        {

        }

        std::string description;
        std::function<void()> task;
    };

    std::vector<uint8_t> m_uuidArray; // GATT client uuid used to register this client.
    int m_interfaceNumber = 0;        // the interface number, from stack
    int m_connectionID = -10;         // connection ID , from stack
    BluetoothAddress m_remoteAddress; // Connected remote address
    ConnectionStatus m_connectStatus; // Current connection status
    uint16_t m_notifyHandle = 0;      // ANCS notification attribute's value handle
    uint16_t m_notifyConfigHandle = 0;// The handle to configure the ANCS notify charactistic
    uint16_t m_ControlPointHandle = 0;// ANCS control point handle
    uint16_t m_DataSourceHandle = 0;  // ANCS data source handle
    uint16_t m_DataSourceConfigHandle = 0;
    bool m_hasANCSService = false;    // Whether connected remote device has ANCS service
    bool m_notifyRegisteredRemote = false; // 
    std::vector<GATT_DB_ELEMENT> m_dbElement; // Queried gatt database elements.
    std::vector<ANCSNotifyMessage> m_notifyMsgs;
    std::vector<std::shared_ptr< DetailNotificationMessage > > m_detailMessages;
    uint32_t m_currentTimerInterval;
    std::deque<GattCommandTask> m_GattCommandQueue;
    std::map<std::string, std::string> m_appNames;
    bool m_cmdExecuting = false;

    std::shared_ptr<OptionContent> m_content;
    StrandCommandScheduler m_commandScheduler;
    std::shared_ptr< StringContent > m_statusContent;
    uint32_t m_currentNotifyIndex = 0;
    std::shared_ptr< StringContent > m_NotificationContent;
    std::vector< boost_ns::signals2::connection > m_connections;
    std::shared_ptr<Action> m_nextAction;
    std::shared_ptr<Action> m_preAction;
    boost_ns::signals2::connection m_timerConnection;
};

