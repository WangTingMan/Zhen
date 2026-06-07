#include "PageAppleANCS.h"

#include <Bt/Adaptor.h>
#include <BT/BluetoothHF.h>
#include <BT/stackLayer/BluetoothGattClientInterface.h>
#include <BT\GATT\BluetoothGattCommon.h>
#include <BT\GATT\GattRemoteCharacteristic.h>

#include "Zhen/OptionContent.h"
#include "Zhen/logging.h"
#include "Zhen/StringContent.h"

#include <base\strings\utf_string_conversions.h>
#include <base\strings\sys_string_conversions.h>

#include <iostream>

#define ANCSPrimaryServiceUuid { 0x79, 0x05, 0xF4, 0x31, 0xB5, 0xCE, 0x4E, 0x99, 0xA4, 0x0F, 0x4B, 0x1E, 0x12, 0x2D, 0x00, 0xD0 }

#define ANCSNotificationSourceUuid { 0x9F, 0xBF, 0x12, 0x0D, 0x63, 0x01, 0x42, 0xD9, 0x8C, 0x58, 0x25, 0xE6, 0x99, 0xA2, 0x1D, 0xBD }

#define ANCSControlPointUuid { 0x69, 0xD1, 0xD8, 0xF3, 0x45, 0xE1, 0x49, 0xA8, 0x98, 0x21, 0x9B, 0xBD, 0xFD, 0xAA, 0xD9, 0xD9 }

#define ANCSDataSourceUuid { 0x22, 0xEA, 0xC6, 0xE9, 0x24, 0xD6, 0x4B, 0xB5, 0xBE, 0x44, 0xB3, 0x6A, 0xCE, 0x7C, 0x7B, 0xFB }

PageAppleANCS::PageAppleANCS()
    : m_uuidArray{ 0x19, 0x44, 0x69, 0x73, 0x9b, 0xf0, 0x46, 0x1f, 0xb5, 0x6a, 0x54, 0x4a, 0xbf, 0x69, 0xe9, 0x15 }
{
    SetTitle( "ANCS Client" );
    using namespace std::placeholders;

    m_connectStatus = ConnectionStatus::ServiceDisconnected;
    m_remoteAddress = BluetoothAddress::kEmpty;

    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Connect",
        [this]()mutable
        {
            m_commandScheduler.SetExecutor
                (
                    std::bind
                    (
                    &PageAppleANCS::HandleConnectRequest,
                    this,
                    _1
                    )
                );
            m_commandScheduler.start();
        } );

    m_content->AddOption( "Register Notification", std::bind( &PageAppleANCS::ConfigureANCSNotification, this ) );

    BluetoothGattClientInterface* gatt_client = &BluetoothGattClientInterface::GetInterface();
    if( gatt_client->IsInitlialized() )
    {
        RegiterANCSClient();
    }
    else
    {
        m_connections.emplace_back( gatt_client->ConnectToSignalGattClientInited(
            std::bind( &PageAppleANCS::RegiterANCSClient, this ) ) );
    }

    auto client_registered_fun = std::bind( &PageAppleANCS::HandleClientRegistered, this, _1, _2, _3 );
    m_connections.emplace_back( gatt_client->ConnectToClientRegistered( client_registered_fun ) );

    auto fun = std::bind( &PageAppleANCS::HandleClientCallbackMessage, this, _1 );
    m_connections.emplace_back( gatt_client->ConnectToSignalGattClientMessageEmiited( fun ) );

    auto connection_status_registered_fun = std::bind(&PageAppleANCS::HandleANCSConnectionStatusChanged, this, _1, _2,
        _3, _4);
    m_connections.emplace_back( gatt_client->ConnectToConnectionStatusChanged(connection_status_registered_fun));

    auto service_detail_searched = std::bind(&PageAppleANCS::HandleServiceDetailSearched, this, _1, _2);
    m_connections.emplace_back( gatt_client->ConnectToServiceSearchCompleted(service_detail_searched));

    auto descriptor_read_completed_slot = std::bind(&PageAppleANCS::HandleDescriptorReadCompleted, this, _1, _2, _3);
    m_connections.emplace_back( gatt_client->ConnectToDescriptorReadCompleted(descriptor_read_completed_slot));

    auto characteristic_write_completed_slot = std::bind(&PageAppleANCS::HandleCharacteristicWriteCompleted, this, _1, _2,
        _3, _4, _5);
    m_connections.emplace_back( gatt_client->ConnectToCharacteristicWriteCompleted(characteristic_write_completed_slot));

    m_statusContent = std::make_shared<StringContent>();
    m_statusContent->SetTitle( "Stauts" );

    m_NotificationContent = std::make_shared<StringContent>();
    m_NotificationContent->SetTitle( "Notification" );

    m_timerConnection = PageManager::GetInstance().connectTimerTo(
        std::bind( &PageAppleANCS::HandelNextGattCommand, this ), s_longTerm, true );
    m_currentTimerInterval = s_longTerm;

    m_nextAction = std::make_shared<Action>( "Next", 'N' );
    m_preAction = std::make_shared<Action>( "Previous", 'P' );
    this->AddMenuAction( m_preAction );
    this->AddMenuAction( m_nextAction );
    m_nextAction->SetVisiable( false );
    m_preAction->SetVisiable( false );

    m_preAction->ConnectActionActived( [this]( std::string )
        {
            if( m_currentNotifyIndex > 0 )
            {
                m_currentNotifyIndex--;
            }
            PageNeedFresh();
            UpdateNextPreActionVisiable();
        } );
    m_nextAction->ConnectActionActived( [this]( std::string )
        {
            if( m_currentNotifyIndex + 1 < m_detailMessages.size() )
            {
                m_currentNotifyIndex++;
            }
            PageNeedFresh();
            UpdateNextPreActionVisiable();
        } );
}

PageAppleANCS::~PageAppleANCS()
{

}

std::string PageAppleANCS::GetPrintableString()const
{
    (const_cast< PageAppleANCS *>(this))->UpdateStatusContent();
    std::string str = BasePage::GetPrintableString();
    str.append( m_content->GetPrintableString() );
    str.append( m_NotificationContent->GetPrintableString() );
    str.append( m_statusContent->GetPrintableString() );
    return str;
}

bool PageAppleANCS::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = m_commandScheduler.OnEvent( a_event );

    if( !r )
    {
        r = m_content->OnEvent( a_event );
    }

    if( !r )
    {
        BasePage::OnEvent( a_event );
    }

    return r;
}

void PageAppleANCS::RegiterANCSClient()
{
    if (m_interfaceNumber == 0)
    {
        BluetoothGattClientInterface::GetInterface().RegisterClientUuid(m_uuidArray, true);
    }
    else
    {
        LogDebug("ANCS client already registered");
    }
}

bool PageAppleANCS::HandleConnectRequest
    (
    std::string const& a_input
    )
{
    bool done = false;

    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::list<RemoteDevice>const& pairedDevices = Adaptor::GetInstance().GetPairedDevices();
        if( pairedDevices.empty() )
        {
            done = true;
            std::cout << "There is no paired device to connect\n";
        }
        else
        {
            int i = 1;
            for( auto it = pairedDevices.begin(); it != pairedDevices.end(); ++it, ++i )
            {
                std::cout << i << ". " << it->name << std::endl;
            }
            std::cout << "Enter the number of remote device:\n";
            m_commandScheduler.SetStepNumber( 1 );
        }
    }
    else if( 1 == m_commandScheduler.GetStepNumber() )
    {
        m_commandScheduler.AddParameter( a_input );
        m_commandScheduler.SetStepNumber( 2 );
    }

    if( !done && 1 == m_commandScheduler.GetNumParameters() )
    {
        std::string numberString = std::any_cast< std::string >( m_commandScheduler.GetParameter( 0 ) );
        int number = 0;
        try
        {
            number = std::stoi( numberString );
            if( number < 1 )
            {
                std::cout << "Wrong index, enter again:\n";
                m_commandScheduler.RemoveLastParameter();
                m_commandScheduler.SetStepNumber( 1 );
                done = false;
                return done;
            }

            BluetoothAddress address;
            std::list<RemoteDevice>const& pairedDevices = Adaptor::GetInstance().GetPairedDevices();
            if( pairedDevices.size() < number )
            {
                std::cout << "Wrong index. Enter again:\n";
                m_commandScheduler.RemoveLastParameter();
                m_commandScheduler.SetStepNumber( 1 );
                done = false;
                return done;
            }
            std::list<RemoteDevice>::const_iterator it = pairedDevices.begin();
            std::advance( it, number - 1 );
            address = it->address;

            int client_if = m_interfaceNumber;
            bool is_direct = true;
            int transport = 0;
            bool opportunistic = false;
            int initiating_phys = 2;
            BluetoothGattClientInterface::GetInterface().Connect(address, m_interfaceNumber);

            done = true;
        }
        catch( std::exception& e )
        {
            std::cout << "Error: " << e.what() << ", enter again:";
            m_commandScheduler.RemoveLastParameter();
            m_commandScheduler.SetStepNumber( 1 );
            done = false;
        }
    }
    return done;
}

void PageAppleANCS::HandleClientCallbackMessage( std::shared_ptr<GattClientBaseMessage> a_message )
{
    switch( a_message->m_type )
    {
    case GattClientBaseMessage::Type::DiscoveryDone:
        {
            auto detailMessage = std::dynamic_pointer_cast< DiscoveryDoneMessage >( a_message );
            LogDebug() << "service discovery completed.";
            //interface_->get_gatt_db( m_connectionID );
        }
        break;
    case GattClientBaseMessage::Type::RegisterForNotification:
        {
            auto detailMessage = std::dynamic_pointer_cast< RegisterForNotificationMessage >( a_message );
            LogDebug() << "Has register notification for handle: " << detailMessage->handle;
        }
        break;
    case GattClientBaseMessage::Type::WriteCharacteristic:
        {
            auto detailMessage = std::dynamic_pointer_cast< WriteCharacteristicMessage >( a_message );
            m_cmdExecuting = false;
            LogDebug() << "Characteristic has been wrote for handle: " << detailMessage->handle;
        }
        break;
    case GattClientBaseMessage::Type::Notify:
        {
            LogDebug() << "A notify has been received.";
            auto detailMessage = std::dynamic_pointer_cast< NotifyMessage >( a_message );
            HandleANCSNotifySourceNotifyEvent( detailMessage );
        }
        break;
    default:
        LogWarning() << "No handle process for message type: " << static_cast<uint32_t>( a_message->m_type );
        break;
    }
}

void PageAppleANCS::HandleDiscoveryANCSService()
{
    std::vector<uint8_t> targetUUID = ANCSPrimaryServiceUuid;
    BluetoothGattClientInterface::GetInterface().SearchServiceByUuid(targetUUID, m_connectionID);
}

bool PageAppleANCS::HandelNextGattCommand()
{
    if( m_GattCommandQueue.empty() )
    {
        m_timerConnection.disconnect();
        return false;
    }
    else
    {
        if( m_cmdExecuting )
        {
            return true;
        }

        auto front = std::move( m_GattCommandQueue.front() );
        m_GattCommandQueue.pop_front();
        m_cmdExecuting = true;
        LogDebug() << front.description;
        front.task();
    }
    return true;
}

void PageAppleANCS::UpdateStatusContent()
{
    std::string str;

    str.append( "Connection status: " ).append( ConnectionStatusToString( m_connectStatus ) );
    if( m_connectStatus == ConnectionStatus::ServiceConnected )
    {
        str.append( "; Address: " ).append( m_remoteAddress.ToString() );
        str.append( "; " ).append( ( m_hasANCSService ? "Has" : "Has not" ) ).append( " ANCS service." );
        str.append( ( m_notifyRegisteredRemote ? " Has" : "Has not" ) ).append( " registered notification.\n" );
        str.append( "Got " ).append( std::to_string( m_detailMessages.size() ) ).append( " notifications." );
    }

    str.append( "\n" );
    m_statusContent->SetString( std::move( str ) );

    str = std::string();
    if( m_detailMessages.empty() )
    {
        str.append( "No new notification." );
    }
    else
    {
        if( m_currentNotifyIndex >= m_detailMessages.size() )
        {
            m_currentNotifyIndex = 0;
            PageNeedFresh();
            UpdateNextPreActionVisiable();
        }

        std::shared_ptr<DetailNotificationMessage>& msg = m_detailMessages[m_currentNotifyIndex];
        str.append( "Notify UID: " ).append( std::to_string( msg->uid ) ).append( "\n" );
        std::string appId = msg->elements[notification_attribute_id_t::NotificationAttributeIDAppIdentifier];
        str.append( "Notify application ID: " ).append( appId ).append( "\n" );
        if( m_appNames[appId].empty() )
        {
            //Retrie application name.
            std::vector<uint8_t> value;
            value.push_back( static_cast< uint8_t >( command_id_t::CommandIDGetAppAttributes ) );
            for( auto ele : appId )
            {
                value.push_back( ele );
            }
            value.push_back( '\0' );

            value.push_back( '\0' ); // 0 is AppAttributeIDDisplayName
            EnqueueCommandTask( "Retrieve application name", [this,value]() {
                BluetoothGattClientInterface::GetInterface().WriteCharacteristic(m_connectionID, m_ControlPointHandle, 2, 2, value.data(), value.size());
                } );
        }
        else
        {
            str.append( "Notify application: " ).append( m_appNames[appId] ).append( "\n" );
        }
        str.append( "Notify title: " ).append( msg->elements[notification_attribute_id_t::NotificationAttributeIDTitle] ).append( "\n" );
        str.append( "Notify message: " ).append( msg->elements[notification_attribute_id_t::NotificationAttributeIDMessage] ).append( "\n" );
        str.append( "Notify date time: " ).append( msg->elements[notification_attribute_id_t::NotificationAttributeIDDate] );
    }
    str.append( "\n" );
    m_NotificationContent->SetString( std::move( str ) );
}

void PageAppleANCS::ConfigureANCSNotification()
{
    if( m_connectStatus != ConnectionStatus::ServiceConnected )
    {
        return;
    }

    std::vector<uint8_t> value = { 0x01, 0x00 };

    EnqueueCommandTask( "config data source handle.", [this, value]()
        {
            BluetoothGattClientInterface::GetInterface().WriteCharacteristic( m_connectionID, m_notifyConfigHandle, 2, 2, value.data(), value.size() );
        },
        false );

    EnqueueCommandTask( "config data source handle.", [this, value]()
            {
                BluetoothGattClientInterface::GetInterface().WriteCharacteristic( m_connectionID, m_DataSourceConfigHandle, 2, 2, value.data(), value.size() );
            },
            false );

    m_notifyRegisteredRemote = true;
    PageNeedFresh();
    LogDebug() << "Configuration for ANCS notification to notify value changed.";
}

void PageAppleANCS::HandleANCSNotifySourceNotifyEvent( std::shared_ptr< NotifyMessage> a_notfiy )
{
    if( a_notfiy->handle == m_notifyHandle )
    {
        LogDebug() << "Received a ANCS notify source notify message.";
        ANCSNotifyMessage::EventID eventID = ANCSNotifyMessage::EventID::ReservedID;
        if( a_notfiy->value[0] <= static_cast<uint8_t>( ANCSNotifyMessage::EventID::ReservedID ) )
        {
            eventID = static_cast< ANCSNotifyMessage::EventID >( a_notfiy->value[0] );
        }
        ANCSNotifyMessage::CategoryID categoryID = ANCSNotifyMessage::CategoryID::CategoryIDReserved;
        if( a_notfiy->value[2] <= static_cast< uint8_t >( ANCSNotifyMessage::CategoryID::CategoryIDReserved ) )
        {
            categoryID = static_cast< ANCSNotifyMessage::CategoryID >( a_notfiy->value[2] );
        }
        uint8_t categoryCount = a_notfiy->value[3];
        uint32_t notifyID;
        memcpy( &notifyID, a_notfiy->value + 4, 4 );
        std::bitset<8> flags( a_notfiy->value[1]);

        m_notifyMsgs.emplace_back( eventID, flags, categoryID, categoryCount, notifyID );
        RetrieveDetailMassge();
    }
    else if( a_notfiy->handle == m_DataSourceHandle )
    {
        LogDebug() << "Received a ANCS data source notify message.";
        uint8_t* rawMsg = a_notfiy->value;
        command_id_t cmdID = static_cast< command_id_t >( rawMsg[0] );
        switch( cmdID )
        {
        case command_id_t::CommandIDGetNotificationAttributes:
            {
                ParseDataSource( rawMsg, a_notfiy->len );
            }
            break;
        case command_id_t::CommandIDGetAppAttributes:
            {
                ParseApplicationAttribute( rawMsg, a_notfiy->len );
            }
            break;
        case command_id_t::CommandIDPerformNotificationAction:
            break;
        default:
            break;
        }
    }
}

void PageAppleANCS::ParseDataSource( uint8_t* a_raw, uint32_t a_size )
{
    uint32_t notifyID = 0;
    memcpy( &notifyID, a_raw + 1, 4 );
    std::shared_ptr< DetailNotificationMessage > msg;
    for( auto& ele : m_detailMessages )
    {
        if( ele->uid == notifyID )
        {
            msg = ele;
        }
    }

    if( !msg )
    {
        msg = std::make_shared<DetailNotificationMessage>();
        m_detailMessages.emplace_back( msg );
        msg->uid = notifyID;
    }

    uint8_t* p_rawData = a_raw + 5;
    uint32_t parsedSize = 5;
    notification_attribute_id_t attributeID;
    uint16_t attributeSize = 0;
    uint8_t* pSize = reinterpret_cast< uint8_t* > ( &attributeSize );
    while ( parsedSize < a_size )
    {
        attributeID = static_cast< notification_attribute_id_t >( p_rawData[0] );
        pSize[0] = p_rawData[1];
        pSize[1] = p_rawData[2];
        if( attributeID != notification_attribute_id_t::NotificationAttributeIDMessageSize )
        {
            std::wstring names = base::SysUTF8ToWide( base::StringPiece( reinterpret_cast< char* >( p_rawData + 3 ), attributeSize ) );
            std::string MultiBytes = base::SysWideToNativeMB( names );
            msg->elements[attributeID] = std::move( MultiBytes );
        }

        p_rawData += attributeSize + 3;
        parsedSize += attributeSize + 3;
    }
    PageNeedFresh();
    UpdateNextPreActionVisiable();
}

void PageAppleANCS::ParseApplicationAttribute( uint8_t* a_raw, uint32_t a_size )
{
    if( a_size < 2 )
    {
        return;
    }

    uint32_t pos = 1;
    std::string appID( reinterpret_cast< char* >( a_raw ) + pos );
    pos += appID.size();
    pos++;

    if( 0 == a_raw[pos] )
    {
        ++pos;
        uint16_t attributeSize = 0;
        uint8_t* pSize = reinterpret_cast< uint8_t* > ( &attributeSize );
        pSize[0] = a_raw[pos++];
        pSize[1] = a_raw[pos++];

        std::wstring names = base::SysUTF8ToWide( base::StringPiece( reinterpret_cast< char* >( a_raw ) + pos, attributeSize ) );
        std::string appName = base::SysWideToNativeMB( names );
        if( appName.empty() || '\0' == appName[0] )
        {
            appName = "Unknown";
        }
        m_appNames[appID] = std::move( appName );
    }
}

void PageAppleANCS::EnqueueCommandTask( std::string a_description, std::function<void()> a_task, bool a_postToLast )
{
    uint32_t interval = s_longTerm;
    if( m_notifyRegisteredRemote )
    {
        interval = s_shortTerm;
    }

    if( !m_timerConnection.connected() )
    {
        m_timerConnection = PageManager::GetInstance().connectTimerTo
            ( std::bind( &PageAppleANCS::HandelNextGattCommand, this ), interval, true );
    }

    if( a_postToLast )
    {
        m_GattCommandQueue.emplace_back( std::move( a_description ), std::move( a_task ) );
    }
    else
    {
        m_GattCommandQueue.emplace_front( std::move( a_description ), std::move( a_task ) );
    }

    if( m_notifyRegisteredRemote && !m_cmdExecuting )
    {
        HandelNextGattCommand();
    }
}

void PageAppleANCS::RetrieveDetailMassge()
{
    if( m_notifyMsgs.empty() )
    {
        return;
    }

    ANCSNotifyMessage notify = m_notifyMsgs.front();
    m_notifyMsgs.erase( m_notifyMsgs.begin() );

    uint32_t id = notify.GetNotifyID();
    uint8_t* pNtyID = reinterpret_cast< uint8_t* >( &id );
    std::vector<uint8_t> value;
    value.push_back( static_cast<uint8_t>( command_id_t::CommandIDGetNotificationAttributes ) );
    value.push_back( pNtyID[0] );
    value.push_back( pNtyID[1] );
    value.push_back( pNtyID[2] );
    value.push_back( pNtyID[3] );

    value.push_back( static_cast< uint8_t >( notification_attribute_id_t::NotificationAttributeIDAppIdentifier ) );
    value.push_back( static_cast< uint8_t >( notification_attribute_id_t::NotificationAttributeIDMessageSize ) );
    value.push_back( static_cast< uint8_t >( notification_attribute_id_t::NotificationAttributeIDDate ) );
    value.push_back( static_cast< uint8_t >( notification_attribute_id_t::NotificationAttributeIDPositiveActionLabel ) );
    value.push_back( static_cast< uint8_t >( notification_attribute_id_t::NotificationAttributeIDNegativeActionLabel ) );

    value.push_back( static_cast< uint8_t >( notification_attribute_id_t::NotificationAttributeIDTitle ) );
    value.push_back( 0x00 );
    value.push_back( 0x10 );

    value.push_back( static_cast< uint8_t >( notification_attribute_id_t::NotificationAttributeIDSubtitle ) );
    value.push_back( 0x00 );
    value.push_back( 0x10 );

    value.push_back( static_cast< uint8_t >( notification_attribute_id_t::NotificationAttributeIDMessage ) );
    value.push_back( 0x00 );
    value.push_back( 0x10 );

    EnqueueCommandTask( "Retrieve detail message", [this, value]()
        {
            BluetoothGattClientInterface::GetInterface().WriteCharacteristic( m_connectionID, m_ControlPointHandle, 2, 2, value.data(), value.size() );
        } );
}

void PageAppleANCS::UpdateNextPreActionVisiable()
{
    if( m_detailMessages.size() <= 1 )
    {
        //Hide next/previous action
        m_nextAction->SetVisiable( false );
        m_preAction->SetVisiable( false );
    }
    else
    {
        if( m_currentNotifyIndex + 1 == m_detailMessages.size() )
        {
            //Hide next action
            m_nextAction->SetVisiable( false );
            m_preAction->SetVisiable();
        }
        else if( 0 == m_currentNotifyIndex )
        {
            //Hide previous action
            m_preAction->SetVisiable( false );
            m_nextAction->SetVisiable();
        }
        else
        {
            //Show the two action
            m_nextAction->SetVisiable();
            m_preAction->SetVisiable();
        }
    }
    PageNeedFresh();
}

void PageAppleANCS::HandleClientRegistered
    (
    int status,
    int client_if,
    std::vector<uint8_t> uuid
    )
{
    if (m_uuidArray == uuid)
    {
        m_interfaceNumber = client_if;
        LogDebug() << "Client registered result. client if: " << m_interfaceNumber
            << " status: " << status;
    }
}

void PageAppleANCS::HandleANCSConnectionStatusChanged
    (
    int conn_id,
    ConnectionStatus a_status,
    int client_if,
    BluetoothAddress addr
    )
{
    if (m_interfaceNumber != client_if)
    {
        return;
    }
    m_connectStatus = a_status;
    m_connectionID = conn_id;
    m_remoteAddress = addr;
    LogDebug() << "Connect status changed to: " << ConnectionStatusToString(m_connectStatus);
    PageNeedFresh();
    if (m_connectStatus == ConnectionStatus::ServiceConnected)
    {
        PageManager::GetInstance().connectOneShotTimerTo(
            std::bind( &PageAppleANCS::HandleDiscoveryANCSService, this ), 3000 );
    }
}

void PageAppleANCS::HandleServiceDetailSearched
    (
    std::vector<GATT_DB_ELEMENT> a_db_elements,
    int a_connect_id
    )
{
    //PageManager::GetInstance().connectOneShotTimerTo(std::bind(&PageAppleANCS::HandleDiscoveryANCSService, this), 3000);
    if (m_connectionID != a_connect_id)
    {
        return;
    }

    m_dbElement = a_db_elements;
    bool nextHandleIsNotificationConfigure = false;
    bool nextHandleIsDataSourceConfigure = false;
    for (auto& ele : m_dbElement)
    {
        BluetoothUuid uuid_(ele.uuid.data());
        std::string uuidStr = uuid_.ToString();
        LogDebug() << "Attribute ID: " << ele.id
            << ", uuid: " << uuidStr
            << ", handle: " << ele.attribute_handle;
        std::vector<uint8_t> uuid;
        uuid = ele.uuid;
        switch (ele.type)
        {
        case gatt_db_attribute_type::BTGATT_DB_CHARACTERISTIC:
            if (std::vector<uint8_t>(ANCSNotificationSourceUuid) == uuid)
            {
                m_notifyHandle = ele.attribute_handle;
                BluetoothGattClientInterface::GetInterface().RegisterNotification(m_interfaceNumber, m_remoteAddress, m_notifyHandle);
                nextHandleIsNotificationConfigure = true;
            }
            else if (std::vector<uint8_t>(ANCSControlPointUuid) == uuid)
            {
                m_ControlPointHandle = ele.attribute_handle;
            }
            else if (std::vector<uint8_t>(ANCSDataSourceUuid) == uuid)
            {
                m_DataSourceHandle = ele.attribute_handle;
                BluetoothGattClientInterface::GetInterface().RegisterNotification(m_interfaceNumber, m_remoteAddress, m_DataSourceHandle);
                nextHandleIsDataSourceConfigure = true;
            }
            else if (MakeBluetoothStandardUuid(ClientCharacteristicConfigUuid) == uuid)
            {
                std::vector<uint8_t> value = { 0x00, 0x00 };
                BluetoothGattClientInterface::GetInterface().WriteCharacteristic(m_connectionID, ele.attribute_handle, 2, 2, value.data(), value.size());
                LogDebug() << "Unexpected characteristic coming here.";
            }
            break;
        case gatt_db_attribute_type::BTGATT_DB_DESCRIPTOR:
            BluetoothGattClientInterface::GetInterface().ReadDescriptor(m_connectionID, ele.attribute_handle, true);
            if (MakeBluetoothStandardUuid(ClientCharacteristicConfigUuid) == uuid)
            {
                if (nextHandleIsNotificationConfigure)
                {
                    m_notifyConfigHandle = ele.attribute_handle;
                    nextHandleIsNotificationConfigure = false;
                    std::vector<uint8_t> value = { 0x01, 0x00 };
                    EnqueueCommandTask("config notify source handle.", [value, this]()
                        {
                            this->m_notifyRegisteredRemote = true;
                            BluetoothGattClientInterface::GetInterface().WriteCharacteristic(m_connectionID, m_notifyConfigHandle, 2, 2, value.data(), value.size());
                            PageNeedFresh();
                        }, false);
                }
                else if (nextHandleIsDataSourceConfigure)
                {
                    m_DataSourceConfigHandle = ele.attribute_handle;
                    nextHandleIsDataSourceConfigure = false;
                    std::vector<uint8_t> value = { 0x01, 0x00 };
                    auto fun = [value, this]()
                        {
                            BluetoothGattClientInterface::GetInterface().WriteCharacteristic(m_connectionID, m_DataSourceConfigHandle,
                                2, 2, value.data(), value.size());
                        };

                    EnqueueCommandTask("config data source handle.", fun, false);
                }
            }
            break;
        case gatt_db_attribute_type::BTGATT_DB_PRIMARY_SERVICE:
            if (std::vector<uint8_t>(ANCSPrimaryServiceUuid) == uuid)
            {
                m_hasANCSService = true;
            }
            break;
        default:
            break;
        };
        PageNeedFresh();
    }
}

void PageAppleANCS::HandleDescriptorReadCompleted
    (
    int conn_id, int status,
    btgatt_read_params_type a_descriptor
    )
{
    if (m_connectionID != conn_id)
    {
        return;
    }
    m_cmdExecuting = false;
    LogDebug() << "Descriptor read completed.";
}

void PageAppleANCS::HandleCharacteristicWriteCompleted
    (
    int conn_id, int status,
    uint16_t handle, uint16_t len,
    std::vector<uint8_t> value
    )
{
    m_cmdExecuting = false;
    LogDebug() << "Characteristic has been wrote for handle: " << handle;
}
