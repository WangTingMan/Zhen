#include "PageKanban.h"
#include <BT/stackLayer/BluetoothGattServerInterface.h>
#include <BT/StackLayer/BluetoothGattClientInterface.h>
#include <BT/BluetoothCommonDefs.h>
#include <BT/common/BluetoothUuid.h>

#include <bitset>

#include <Zhen/logging.h>
#include <Zhen/PageManager.h>

static std::vector<uint8_t> s_kanban_service_uuid{ 0x00, 0x02, 0x03, 0x73, 0x9b, 0xf0, 0x46, 0x1f, 0xb5, 0x6a, 0x54, 0x4a, 0xbf, 0x69, 0xe9, 0xe5 };
static std::vector<uint8_t> s_kanban_screen_power_uuid{ 0x01, 0x02, 0x03, 0x73, 0x9b, 0xf0, 0x46, 0x1f, 0xb5, 0x6a, 0x54, 0x4a, 0xbf, 0x69, 0xe9, 0xe5 };
static std::vector<uint8_t> s_kanban_media_src_uuid{ 0x01, 0x02, 0x07, 0x73, 0x9b, 0xf0, 0x46, 0x1f, 0xb5, 0x6a, 0x54, 0x4a, 0xbf, 0x69, 0xe9, 0xe5 };

#define CHARACTERISTIC_USER_DESCRIPTION_UUID 0x2901
#define CHARACTERISTIC_CLIENT_CONFIGURATION_UUID 0x2902

class kanban_client
{

public:

    ConnectionStatus m_connection_status = ConnectionStatus::ServiceDisconnected;
    BluetoothAddress m_client_addr{};
    int m_connection_id = 0x00;
    bool m_media_source_notification_enabled = false;
    bool m_screen_on_off_notification_enabled = false;
};

/**
 * 这是一个示例GATT服务，用途是生产车间的看板系统。
 * 这个看板可以显示多个生产线的状况；
 * 还可以新增或者移除一个交通信号灯（是的，没错，车间里面也有交通信号灯！)
 */
PageKanban::PageKanban()
    : m_kanbanRegisterUuid{ 0x19, 0x44, 0x69, 0x73, 0x9b, 0xf0, 0x46, 0x1f, 0xb5, 0x6a, 0x54, 0x4a, 0xbf, 0x69, 0xe9, 0xe5 }
{
    constexpr uint16_t value1 = PROPERTY_READ | PROPERTY_WRITE | PROPERTY_WRITE_WITHOUT_RSP | PROPERTY_NOTIFY | PROPERTY_INDICATION;

    SetTitle( "Kanban Server" );

    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Register Kanban Service", std::bind( &PageKanban::RegisterService, this ) );
    m_content->AddOption( "Stop Kanban Service", std::bind( &PageKanban::stop_service, this ) );
    m_content->AddOption( "Change screen to on", std::bind( &PageKanban::set_sreen_on_off, this, true ) );
    m_content->AddOption( "Change screen to off", std::bind( &PageKanban::set_sreen_on_off, this, false ) );
    m_content->AddOption( "Start media test", std::bind( &PageKanban::start_media_test, this, true ) );
    m_content->AddOption( "Stop media test", std::bind( &PageKanban::start_media_test, this, false ) );

    connect_signals();

    m_screen_on_off_dsp_string.assign( "This is a switch to turn the screen on and off." );
    m_media_dsp_string.assign( "This is a media source." );
}

PageKanban::~PageKanban()
{

}

std::string PageKanban::GetPrintableString()const
{
    std::string str = BasePage::GetPrintableString();
    str.append( m_content->GetPrintableString() );
    return str;
}

bool PageKanban::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = false;

    if (!r)
    {
        r = m_content->OnEvent( a_event );
    }

    if (!r)
    {
        BasePage::OnEvent( a_event );
    }

    return r;
}

void PageKanban::connect_signals()
{
    using namespace std::placeholders;
    auto interface_ = &BluetoothGattServerInterface::GetInterface();
    boost_ns::signals2::connection connection;

    connection = interface_->ConnectToServiceRegistered(
        std::bind( &PageKanban::handle_register_service_result, this, _1, _2, _3 ) );
    m_connections.push_back( connection );

    connection = interface_->connect_to_service_body_added(
        std::bind( &PageKanban::handle_service_body_added_result, this, _1, _2, _3 ) );
    m_connections.push_back( connection );

    connection = interface_->connect_to_client_connection_status_changed(
        std::bind( &PageKanban::handle_client_connection_status_changed, this, _1, _2, _3, _4 ) );
    m_connections.push_back( connection );

    connection = interface_->connect_to_client_read_desciptor_request(
        std::bind( &PageKanban::handle_client_read_descriptor_request, this, _1, _2, _3, _4, _5, _6 ) );
    m_connections.push_back( connection );

    connection = interface_->connect_to_client_read_characteristic_request(
        std::bind( &PageKanban::handle_client_read_characteristic_request, this, _1, _2, _3, _4, _5, _6 ) );
    m_connections.push_back( connection );

    connection = interface_->connect_to_client_write_characteristic_request(
        std::bind( &PageKanban::handle_client_write_characteristic_request, this, _1, _2, _3, _4, _5, _6, _7, _8 ) );
    m_connections.push_back( connection );

    connection = interface_->connect_to_client_request_write_descriptor_request(
        std::bind( &PageKanban::handle_client_request_write_descriptor_request, this, _1, _2, _3, _4, _5, _6, _7, _8 ) );
    m_connections.push_back( connection );
}

void PageKanban::RegisterService()
{
    if (m_server_if != 0)
    {
        LogInfo() << "we already registered";
        return;
    }

    BluetoothGattServerInterface::GetInterface().RegisterGattServer( m_kanbanRegisterUuid, false );
}

void PageKanban::unregister_service()
{

}

void PageKanban::stop_service()
{
    BluetoothGattServerInterface::GetInterface().StopGattService( m_server_if,
    m_characteristic_index_handle_map[m_gatt_service_handle_index]);
}

void PageKanban::set_sreen_on_off( bool a_on )
{
    if( m_screen_on_off == a_on )
    {
        return;
    }

    m_screen_on_off = a_on;
    std::vector<uint8_t> value;
    value.push_back( a_on ? 0x01 : 0x00 );
    for( auto _client : m_kanban_clients )
    {
        if( _client->m_connection_status == ConnectionStatus::ServiceConnected )
        {
            if( _client->m_screen_on_off_notification_enabled )
            {
                BluetoothGattServerInterface::GetInterface().SendIndicationOrNotification
                    (
                    m_server_if,
                    m_characteristic_index_handle_map[m_screen_on_off_handle_index],
                    _client->m_connection_id,
                    0,
                    value
                    );
            }
            break;
        }
    }
}

void PageKanban::start_media_test( bool a_start )
{
    if( m_media_test_enabled == a_start )
    {
        return;
    }

    m_media_test_enabled = a_start;
    if( m_media_test_enabled )
    {
        if( !m_media_test_timer_connection.connected() )
        {
            m_media_test_timer_connection = PageManager::GetInstance().connectTimerTo
                (
                std::bind(&PageKanban::handle_media_test_timer_event, this),
                5000
                );
        }
    }
    else
    {
        if( m_media_test_timer_connection.connected() )
        {
            m_media_test_timer_connection.disconnect();
        }
    }
}

void PageKanban::register_service_body()
{
    std::vector<GATT_DB_ELEMENT> service_elements;
    GATT_DB_ELEMENT element;
    uint16_t start_handle = 0x0100;
    uint16_t end_handle = 0x0105;
    uint16_t index = 0;
    auto drscptor_uuid = BluetoothUuid::From16Bit( CHARACTERISTIC_USER_DESCRIPTION_UUID );
    auto client_config_uuid = BluetoothUuid::From16Bit( CHARACTERISTIC_CLIENT_CONFIGURATION_UUID );

    /*1. 添加服务声明属性*/
    element.clear();
    element.uuid = s_kanban_service_uuid;
    element.type = gatt_db_attribute_type::BTGATT_DB_PRIMARY_SERVICE;
    element.extended_properties = 0x00;
    service_elements.push_back( element );
    m_gatt_service_handle_index = index++;

    /*2. 添加特性(特征)定义*/
    /*2.1. 添加一个看板的显示或者关闭的开关特性，客户端可以控制看板是否亮灭屏幕*/
    element.clear();
    element.uuid = s_kanban_screen_power_uuid;
    element.type = gatt_db_attribute_type::BTGATT_DB_CHARACTERISTIC;
    element.properties = PROPERTY_READ | PROPERTY_WRITE | PROPERTY_WRITE_WITHOUT_RSP | PROPERTY_NOTIFY | PROPERTY_INDICATION;
    element.permissions = ATTRIBUTE_PERM_READ | ATTRIBUTE_PERM_WRITE_ENC_MITM;
    service_elements.push_back( element );
    m_screen_on_off_handle_index = index++;

    /*2.1.1 添加上面屏幕亮灭屏的描述，一个字符串*/
    element.clear();
    element.uuid.assign( drscptor_uuid.uu, drscptor_uuid.uu + BluetoothUuid::kNumBytes128 );
    element.type = gatt_db_attribute_type::BTGATT_DB_DESCRIPTOR;
    element.properties = PROPERTY_READ;
    element.permissions = ATTRIBUTE_PERM_READ;
    service_elements.push_back( element );
    m_screen_on_off_dsp_handle_index = index++;

    element.clear();
    element.uuid.assign( client_config_uuid.uu, client_config_uuid.uu + BluetoothUuid::kNumBytes128 );
    element.type = gatt_db_attribute_type::BTGATT_DB_DESCRIPTOR;
    element.properties = PROPERTY_READ;
    element.permissions = ATTRIBUTE_PERM_READ | ATTRIBUTE_PERM_WRITE_ENC_MITM;
    service_elements.push_back( element );
    m_screen_on_off_client_conf_handle_index = index++;

    /*3. 添加特性(特征)定义*/
    /*3.1. 添加一个看板的多媒体数据特性，可以给客户端推送媒体数据*/
    element.clear();
    element.uuid = s_kanban_media_src_uuid;
    element.type = gatt_db_attribute_type::BTGATT_DB_CHARACTERISTIC;
    element.properties = PROPERTY_NOTIFY | PROPERTY_INDICATION;
    element.permissions = ATTRIBUTE_PERM_READ | ATTRIBUTE_PERM_WRITE_ENC_MITM;
    service_elements.push_back( element );
    m_media_handle_index = index++;

    /*3.1.1 添加上面多媒体特性的描述，一个字符串*/
    element.clear();
    element.uuid.assign( drscptor_uuid.uu, drscptor_uuid.uu + BluetoothUuid::kNumBytes128 );
    element.type = gatt_db_attribute_type::BTGATT_DB_DESCRIPTOR;
    element.properties = PROPERTY_READ;
    element.permissions = ATTRIBUTE_PERM_READ;
    service_elements.push_back( element );
    m_media_dsc_handle_index = index++;

    element.clear();
    element.uuid.assign( client_config_uuid.uu, client_config_uuid.uu + BluetoothUuid::kNumBytes128 );
    element.type = gatt_db_attribute_type::BTGATT_DB_DESCRIPTOR;
    element.properties = PROPERTY_READ;
    element.permissions = ATTRIBUTE_PERM_READ | ATTRIBUTE_PERM_WRITE_ENC_MITM;
    service_elements.push_back( element );
    m_media_client_conf_handle_index = index++;

    BluetoothGattServerInterface::GetInterface().AddServiceBody( m_server_if, service_elements );
}

bool PageKanban::handle_media_test_timer_event()
{
    uint8_t index = 0;
    uint16_t buffer_size = 244;

    std::vector<uint8_t> buffer;
    buffer.reserve(buffer_size);

    for( uint16_t i = 0; i < buffer_size; ++i )
    {
        buffer.push_back(index++);
    }

    std::shared_ptr<kanban_client> client;
    for( auto _client : m_kanban_clients )
    {
        if( _client->m_connection_status == ConnectionStatus::ServiceConnected )
        {
            if( _client->m_media_source_notification_enabled )
            {
                BluetoothGattServerInterface::GetInterface().SendIndicationOrNotification
                    (
                    m_server_if,
                    m_characteristic_index_handle_map[m_media_handle_index],
                    _client->m_connection_id,
                    0,
                    buffer
                    );
            }
            break;
        }
    }

    return true;
}

void PageKanban::handle_register_service_result
    (
    int status,
    int server_if,
    const std::vector<uint8_t>& app_uuid
    )
{
    if (app_uuid != m_kanbanRegisterUuid)
    {
        return;
    }

    if (status == 0)
    {
        m_server_if = server_if;
        register_service_body();
        return;
    }

    LogError() << "register Kanban service then received a error! code: " << status;
}

void PageKanban::handle_service_body_added_result
    (
    int status,
    int server_if,
    const std::vector<GATT_DB_ELEMENT>& service
    )
{
    if (server_if != m_server_if)
    {
        return;
    }

    if (status == 0)
    {
        LogInfo() << "Kanban service body added";
        m_characteristic_index_handle_map.clear();
        m_characteristic_handle_index_map.clear();
        for (int i = 0; i < service.size(); ++i)
        {
            m_characteristic_index_handle_map[i] = service[i].attribute_handle;
            m_characteristic_handle_index_map[service[i].attribute_handle] = i;
        }
        return;
    }

    LogError() << "Kanban service added failed. Error code: " << status;
}

void PageKanban::handle_client_connection_status_changed
    (
    int connection_id,
    int server_if,
    ConnectionStatus status,
    const BluetoothAddress& client_addr
    )
{
    if (server_if != m_server_if)
    {
        return;
    }

    LogInfo() << "Kanban client connection status changed. remote: " << client_addr.ToString()
        << ", status: " << ConnectionStatusToString( status );
    std::shared_ptr<kanban_client> client;
    for (auto _client_it = m_kanban_clients.begin(); _client_it != m_kanban_clients.end(); ++_client_it )
    {
        std::shared_ptr<kanban_client> _client;
        _client = *_client_it;
        if (_client->m_client_addr == client_addr)
        {
            if (status == ConnectionStatus::ServiceDisconnected)
            {
                m_kanban_clients.erase( _client_it );
                return;
            }

            client = _client;
            break;
        }
    }

    if (!client)
    {
        client = std::make_shared<kanban_client>();
        client->m_client_addr = client_addr;
        m_kanban_clients.push_back( client );
    }

    client->m_connection_status = status;
    client->m_connection_id = connection_id;

    PageManager::GetInstance().connectOneShotTimerTo
        (
        std::bind
            (
            &BluetoothGattClientInterface::UpdateConnectionParameters,
            std::ref( BluetoothGattClientInterface::GetInterface() ),
            client_addr,
            6,
            6,
            0,
            0x60,
            0x01,
            0x7CFF
            ),
        1000
        );

    PageManager::GetInstance().connectOneShotTimerTo
        (
        std::bind
            (
            &BluetoothGattServerInterface::TryRequestMaxDataLengthForLE,
            std::ref( BluetoothGattServerInterface::GetInterface() ),
            client_addr
            ),
        500
        );
}

void PageKanban::handle_client_read_descriptor_request
    (
    int conn_id,
    int trans_id,
    const BluetoothAddress& bda,
    int attr_handle,
    int offset,
    bool is_long
    )
{
    std::shared_ptr<kanban_client> client;
    for (auto _client : m_kanban_clients)
    {
        if (_client->m_client_addr == bda)
        {
            client = _client;
            break;
        }
    }

    if (!client)
    {
        return;
    }

    uint16_t index = m_characteristic_handle_index_map[attr_handle];
    if ( index == m_screen_on_off_dsp_handle_index )
    {
        GATTResponseContent response;
        int total_size = m_screen_on_off_dsp_string.size();
        if( total_size >= offset )
        {
            response.value = (uint8_t*)m_screen_on_off_dsp_string.data();
            response.value += offset;
            response.len = total_size - offset;
        }
        else
        {
            response.value = nullptr;
            response.len = 0;
        }
        response.offset = 0;
        response.handle = attr_handle;
        BluetoothGattServerInterface::GetInterface().SendResponse( conn_id, trans_id, 0, response );
        return;
    }

    if( index == m_media_dsc_handle_index )
    {
        GATTResponseContent response;
        int total_size = m_media_dsp_string.size();
        if( total_size >= offset )
        {
            response.value = ( uint8_t* )m_media_dsp_string.data();
            response.value += offset;
            response.len = total_size - offset;
        }
        else
        {
            response.value = nullptr;
            response.len = 0;
        }
        response.offset = 0;
        response.handle = attr_handle;
        BluetoothGattServerInterface::GetInterface().SendResponse( conn_id, trans_id, 0, response );
        return;
    }
}

void PageKanban::handle_client_read_characteristic_request
    (
    int conn_id,
    int trans_id,
    const BluetoothAddress& bda,
    int attr_handle,
    int offset,
    bool is_long
    )
{
    std::shared_ptr<kanban_client> client;
    for (auto _client : m_kanban_clients)
    {
        if (_client->m_client_addr == bda)
        {
            client = _client;
            break;
        }
    }

    if (!client)
    {
        return;
    }

    uint16_t index = m_characteristic_handle_index_map[attr_handle];
    if ( index == m_screen_on_off_handle_index)
    {
        GATTResponseContent response;
        uint8_t value = m_screen_on_off ? 0x01 : 0x00;
        response.value = (uint8_t*)&value;
        response.value += offset;
        response.len = sizeof( uint8_t );
        response.offset = 0;
        response.handle = attr_handle;
        BluetoothGattServerInterface::GetInterface().SendResponse( conn_id, trans_id, 0, response );
        return;
    }
}

void PageKanban::handle_client_write_characteristic_request
    (
    int conn_id,
    int trans_id,
    const BluetoothAddress& bda,
    int attr_handle,
    int offset,
    bool need_rsp,
    bool is_prep,
    std::vector<uint8_t> value
    )
{
    std::shared_ptr<kanban_client> client;
    for( auto _client : m_kanban_clients )
    {
        if( _client->m_client_addr == bda )
        {
            client = _client;
            break;
        }
    }

    if( !client )
    {
        return;
    }

    uint16_t index = m_characteristic_handle_index_map[attr_handle];
    if( index == m_screen_on_off_handle_index )
    {
        GATTResponseContent response;
        if( value.size() > 0 )
        {
            m_screen_on_off = ( value[0] == 0x01 ? 0x01 : 0x00 );
        }
        uint8_t value = m_screen_on_off ? 0x01 : 0x00;
        response.value = ( uint8_t* )&value;
        response.value += offset;
        response.len = sizeof( uint8_t );
        response.offset = 0;
        response.handle = attr_handle;
        BluetoothGattServerInterface::GetInterface().SendResponse( conn_id, trans_id, 0, response );
        return;
    }
}

void PageKanban::handle_client_request_write_descriptor_request
    (
    int conn_id,
    int trans_id,
    const BluetoothAddress& bda,
    int attr_handle,
    int offset,
    bool need_rsp,
    bool is_prep,
    std::vector<uint8_t> value
    )
{
    uint16_t index = m_characteristic_handle_index_map[attr_handle];
    std::shared_ptr<kanban_client> client;
    for( auto _client : m_kanban_clients )
    {
        if( _client->m_client_addr == bda )
        {
            client = _client;
            break;
        }
    }

    if( !client )
    {
        return;
    }

    if( index == m_media_client_conf_handle_index )
    {
        std::bitset<8> sets( value[0] );
        bool notify_enable = sets[0];
        client->m_media_source_notification_enabled = notify_enable;

        GATTResponseContent response;
        response.value = ( uint8_t* )value.data();
        response.len = value.size();
        response.offset = 0;
        response.handle = attr_handle;
        BluetoothGattServerInterface::GetInterface().SendResponse( conn_id, trans_id, 0, response );
        return;
    }

    if( index == m_screen_on_off_client_conf_handle_index )
    {
        std::bitset<8> sets( value[0] );
        bool notify_enable = sets[0];
        client->m_screen_on_off_notification_enabled = notify_enable;

        GATTResponseContent response;
        response.value = ( uint8_t* )value.data();
        response.len = value.size();
        response.offset = 0;
        response.handle = attr_handle;
        BluetoothGattServerInterface::GetInterface().SendResponse( conn_id, trans_id, 0, response );
        return;
    }
}

