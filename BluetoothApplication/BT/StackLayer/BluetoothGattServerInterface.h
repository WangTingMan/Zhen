#pragma once

#include <BT/BluetoothAddress.h>
#include <BT/BluetoothCommonDefs.h>
#include <BT/GATT/BluetoothGattCommon.h>

#include <Zhen/global.h>

#include <vector>

#define PROPERTY_BRAODCAST          0x01
#define PROPERTY_READ               0x02
#define PROPERTY_WRITE_WITHOUT_RSP  0x04
#define PROPERTY_WRITE              0x08
#define PROPERTY_NOTIFY             0x10
#define PROPERTY_INDICATION         0x20
#define PROPERTY_AUTH_SIGN_WRITE    0x40
#define PROPERTY_EXT                0x80

#define ATTRIBUTE_PERM_READ (1 << 0)              /* bit 0 */
#define ATTRIBUTE_PERM_READ_ENCRYPTED (1 << 1)    /* bit 1 */
#define ATTRIBUTE_PERM_READ_ENC_MITM (1 << 2)     /* bit 2 */
#define ATTRIBUTE_PERM_WRITE (1 << 4)             /* bit 4 */
#define ATTRIBUTE_PERM_WRITE_ENCRYPTED (1 << 5)   /* bit 5 */
#define ATTRIBUTE_PERM_WRITE_ENC_MITM (1 << 6)    /* bit 6 */
#define ATTRIBUTE_PERM_WRITE_SIGNED (1 << 7)      /* bit 7 */
#define ATTRIBUTE_PERM_WRITE_SIGNED_MITM (1 << 8) /* bit 8 */

struct GATTResponseContent
{
    uint8_t* value = nullptr;
    uint16_t len;
    uint16_t handle;
    uint16_t offset;
};

class BluetoothGattServerInterface
{

public:

    static BluetoothGattServerInterface& GetInterface();

    virtual void Init() = 0;

    virtual void RegisterGattServer
        (
        std::vector<uint8_t> a_uuid,
        bool a_support_eatt
        ) = 0;

    virtual void UnregisterGattServer
        (
        int a_server_id
        ) = 0;

    virtual void StopGattService
        (
        int server_if,
        int service_handle
        ) = 0;

    virtual void Connect
        (
        int server_if,
        const BluetoothAddress& bd_addr,
        bool is_direct,
        int transport
        ) = 0;

    virtual void Disconnect
        (
        int server_if,
        const BluetoothAddress& bd_addr,
        int conn_id
        ) = 0;

    virtual void AddServiceBody
        (
        int server_if,
        std::vector<GATT_DB_ELEMENT> service_elements
        ) = 0;

    virtual void SendResponse
        (
        int conn_id,
        int trans_id,
        int status,
        GATTResponseContent response
        ) = 0;

    virtual void SendIndicationOrNotification
        (
        int server_if,
        int attribute_handle,
        int conn_id,
        int confirm,
        std::vector<uint8_t> value
        ) = 0;

    virtual void TryRequestMaxDataLengthForLE( const BluetoothAddress& bd_addr ) = 0;

public:

    virtual boost_ns::signals2::connection ConnectToServiceRegistered
        (
        std::function<void( int, int, std::vector<uint8_t> )> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection connect_to_service_body_added
        (
        std::function<void( int status, int server_if, std::vector<GATT_DB_ELEMENT> )> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection connect_to_client_connection_status_changed
        (
        std::function<void( int conn_id, int server_if, ConnectionStatus status, BluetoothAddress addr )> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection connect_to_client_read_desciptor_request
        (
            std::function<void(
                int conn_id,
                int trans_id,
                const BluetoothAddress& bda,
                int attr_handle,
                int offset,
                bool is_long )> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection connect_to_client_read_characteristic_request
        (
        std::function<void(
            int conn_id,
            int trans_id,
            const BluetoothAddress& bda,
            int attr_handle,
            int offset,
            bool is_long )> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection connect_to_client_write_characteristic_request
        (
        std::function<void(
            int conn_id,
            int trans_id,
            const BluetoothAddress& bda,
            int attr_handle,
            int offset,
            bool need_rsp,
            bool is_prep,
            std::vector<uint8_t> value )> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection connect_to_client_request_write_descriptor_request
        (
        std::function<void(
            int conn_id,
            int trans_id,
            const BluetoothAddress& bda,
            int attr_handle,
            int offset,
            bool need_rsp,
            bool is_prep,
            std::vector<uint8_t> value )> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection ConnectToIndicationSent
        (
        std::function<void( int, int )> a_fun
        ) = 0;

};

