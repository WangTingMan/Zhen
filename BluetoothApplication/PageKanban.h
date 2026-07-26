#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/OptionContent.h"
#include "Zhen/StrandCommandScheduler.h"

#include <BT/StackLayer/BluetoothGattServerInterface.h>

class kanban_client;/*who connected with us*/
class PageKanban : public BasePage
{

public:

    PageKanban();

    ~PageKanban();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

private:

    void connect_signals();

    void RegisterService();

    void unregister_service();

    void stop_service();

    void set_sreen_on_off( bool a_on = true );

    void start_media_test(bool a_start = true);

    void register_service_body();

    bool handle_media_test_event();

    void handle_register_service_result
        (
        int status,
        int server_if,
        const std::vector<uint8_t>& app_uuid
        );

    void handle_service_body_added_result
        (
        int status,
        int server_if,
        const std::vector<GATT_DB_ELEMENT>& service
        );

    void handle_client_connection_status_changed
        (
        int connection_id,
        int server_if,
        ConnectionStatus status,
        const BluetoothAddress& service
        );

    void handle_client_read_descriptor_request
        (
        int conn_id,
        int trans_id,
        const BluetoothAddress& bda,
        int attr_handle,
        int offset,
        bool is_long
        );

    void handle_client_read_characteristic_request
        (
        int conn_id,
        int trans_id,
        const BluetoothAddress& bda,
        int attr_handle,
        int offset,
        bool is_long
        );

    void handle_client_write_characteristic_request
        (
        int conn_id,
        int trans_id,
        const BluetoothAddress& bda,
        int attr_handle,
        int offset,
        bool need_rsp,
        bool is_prep,
        std::vector<uint8_t> value
        );

    void handle_client_request_write_descriptor_request
        (
        int conn_id,
        int trans_id,
        const BluetoothAddress& bda,
        int attr_handle,
        int offset,
        bool need_rsp,
        bool is_prep,
        std::vector<uint8_t> value
        );

    void handle_indication_notify_sent_done
        (
        int trans_id,
        int status
        );

    std::shared_ptr<OptionContent> m_content;
    std::vector<uint8_t> m_kanbanRegisterUuid;
    std::vector<GATT_DB_ELEMENT> service_body_elements;
    std::vector<std::shared_ptr<kanban_client>> m_kanban_clients;
    int m_server_if = 0;
    std::vector<boost_ns::signals2::connection> m_connections;

    uint16_t m_gatt_service_handle_index = 0;
    uint16_t m_screen_on_off_handle_index = 0;
    uint16_t m_screen_on_off_dsp_handle_index = 0;
    uint16_t m_screen_on_off_client_conf_handle_index = 0;
    uint16_t m_media_handle_index = 0;
    uint16_t m_media_dsc_handle_index = 0;
    uint16_t m_media_client_conf_handle_index = 0;
    uint16_t m_signal_handle_index = 0;
    uint16_t m_signal_dsc_handle_index = 0;
    uint16_t m_siganl_config_handle_index = 0;
    std::map<uint16_t/*index*/, uint16_t/*handle*/> m_characteristic_index_handle_map;
    std::map<uint16_t/*handle*/, uint16_t/*index*/> m_characteristic_handle_index_map;

    std::string m_screen_on_off_dsp_string;
    std::string m_media_dsp_string;
    bool m_screen_on_off = true; /*the screen on/off state currently*/
    bool m_media_test_enabled = false;
    bool m_media_packt_notifing = false;
    uint16_t m_media_packet_count = 0;
};


