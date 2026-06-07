#pragma once
#include <BT/BluetoothAddress.h>
#include <BT/BluetoothCommonDefs.h>
#include <BT/GATT/BluetoothGattCommon.h>

#include <cstdint>
#include <functional>
#include <vector>

#include <Zhen/global.h>

struct GattClientBaseMessage
{
    enum class Type
    {
        Unknown,
        RegisterClient,
        ConnectStatusChanged,
        SearchComplete,
        RegisterForNotification,
        Notify,
        ReadCharacteristic,
        WriteCharacteristic,
        ReadDescriptor,
        WriteDescriptor,
        ExecuteWrite,
        ReadRemoteRssi,
        ConfigureMtu,
        CongestionClient,
        GetGattDb,
        ServicesRemoved,
        ServicesAdded,
        PhyClientUpdated,
        ConnClientUpdated,
        DiscoveryDone
    };
    Type m_type = Type::Unknown;

    virtual ~GattClientBaseMessage() {}
};

struct RegisterClientMessage : public GattClientBaseMessage
{
    RegisterClientMessage() { m_type = GattClientBaseMessage::Type::RegisterClient; }
    int status = 0;
    int client_if = 0;
    std::vector<uint8_t> uuid;
};

struct ConnectStatusMessage : public GattClientBaseMessage
{
    ConnectStatusMessage() { m_type = GattClientBaseMessage::Type::ConnectStatusChanged; }
    int conn_id = 0;
    ConnectionStatus status = ConnectionStatus::ServiceDisconnected;
    int client_if = 0;
    BluetoothAddress address;
};

struct SearchCompleteMessage : public GattClientBaseMessage
{
    SearchCompleteMessage() { m_type = GattClientBaseMessage::Type::SearchComplete; }
    int conn_id = 0;
    int status = 0;
};

struct RegisterForNotificationMessage : public GattClientBaseMessage
{
    RegisterForNotificationMessage() { m_type = GattClientBaseMessage::Type::RegisterForNotification; }
    int conn_id = 0;
    int registered = 0;
    int status = 0;
    uint16_t handle = 0;
};

struct GetGattDbMessage : public GattClientBaseMessage
{
    GetGattDbMessage() { m_type = GattClientBaseMessage::Type::GetGattDb; }
    int conn_id = 0;
    std::vector<GATT_DB_ELEMENT> db_elements;
};

struct NotifyMessage : public GattClientBaseMessage
{
    static constexpr uint32_t MAX_ATTR_SIZE = 600;
    NotifyMessage() { m_type = GattClientBaseMessage::Type::Notify; }
    int conn_id = 0;
    uint8_t value[MAX_ATTR_SIZE] = {};
    BluetoothAddress bda{};
    uint16_t handle = 0;
    uint16_t len = 0;
    uint8_t is_notify = 0;
};

struct btgatt_unformatted_value_type
{
    uint8_t value[512];
    uint16_t len;
};

/** Parameters for GATT read operations */
struct btgatt_read_params_type
{
    uint16_t handle;
    btgatt_unformatted_value_type value;
    uint16_t value_type;
    uint8_t status;
};

struct ReadCharacteristicMessage : public GattClientBaseMessage
{
    ReadCharacteristicMessage() { m_type = GattClientBaseMessage::Type::ReadCharacteristic; }
    int conn_id = 0;
    int status = 0;
    btgatt_read_params_type data;
};

struct WriteCharacteristicMessage : public GattClientBaseMessage
{
    WriteCharacteristicMessage() { m_type = GattClientBaseMessage::Type::WriteCharacteristic; }
    int conn_id = 0;
    int status = 0;
    uint16_t handle;
};

struct ReadDescriptorMessage : public GattClientBaseMessage
{
    ReadDescriptorMessage() { m_type = GattClientBaseMessage::Type::ReadDescriptor; }
    int conn_id = 0;
    int status = 0;
    btgatt_read_params_type data;
};

struct WriteDescriptorMessage : public GattClientBaseMessage
{
    WriteDescriptorMessage() { m_type = GattClientBaseMessage::Type::WriteDescriptor; }
    int conn_id = 0;
    int status = 0;
    uint16_t handle = 0;
};

struct ExecuteWriteMessage : public GattClientBaseMessage
{
    ExecuteWriteMessage() { m_type = GattClientBaseMessage::Type::ExecuteWrite; }
    int conn_id = 0;
    int status = 0;
};

struct ReadRemoteRssiMessage : public GattClientBaseMessage
{
    ReadRemoteRssiMessage() { m_type = GattClientBaseMessage::Type::ReadRemoteRssi; }
    int client_if = 0;
    BluetoothAddress bda;
    int rssi = 0;
    int status = 0;
};

struct ConfigureMtuMessage : public GattClientBaseMessage
{
    ConfigureMtuMessage() { m_type = GattClientBaseMessage::Type::ConfigureMtu; }
    int conn_id = 0;
    int status = 0;
    int mtu = 0;
};

struct CongestionClientMessage : public GattClientBaseMessage
{
    CongestionClientMessage() { m_type = GattClientBaseMessage::Type::CongestionClient; }
    int conn_id = 0;
    bool congested;
};

struct ServicesRemovedMessage : public GattClientBaseMessage
{
    ServicesRemovedMessage() { m_type = GattClientBaseMessage::Type::ServicesRemoved; }
    int conn_id = 0;
    uint16_t start_handle = 0;
    uint16_t end_handle = 0;
};

struct ServicesAddedMessage : public GattClientBaseMessage
{
    ServicesAddedMessage() { m_type = GattClientBaseMessage::Type::ServicesAdded; }
    int conn_id = 0;
    GATT_DB_ELEMENT added;
    int added_count = 0;
};

struct PhyClientUpdatedMessage : public GattClientBaseMessage
{
    PhyClientUpdatedMessage() { m_type = GattClientBaseMessage::Type::PhyClientUpdated; }
    int conn_id = 0;
    uint8_t tx_phy = 0;
    uint8_t rx_phy = 0;
    uint8_t status = 0;
};

struct ConnClientUpdatedMessage : public GattClientBaseMessage
{
    ConnClientUpdatedMessage() { m_type = GattClientBaseMessage::Type::ConnClientUpdated; }
    int conn_id = 0;
    uint16_t interval = 0;
    uint16_t latency = 0;
    uint16_t timeout = 0;
    uint8_t status = 0;
};

struct DiscoveryDoneMessage : public GattClientBaseMessage
{
    DiscoveryDoneMessage() { m_type = GattClientBaseMessage::Type::DiscoveryDone; }
    BluetoothAddress address;
};

class BluetoothGattClientInterface
{

public:

    static BluetoothGattClientInterface& GetInterface();

    virtual ~BluetoothGattClientInterface(){}

    virtual void Init() = 0;

    virtual bool Connect
        (
        BluetoothAddress a_address,
        int a_client_if
        ) = 0;

    virtual bool disconnect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool IsInitlialized() const = 0;

    virtual void RegisterClientUuid
        (
        std::vector<uint8_t> a_uuid,
        bool a_support_eatt
        ) = 0;

    virtual void SearchServiceByUuid
        (
        std::vector<uint8_t> a_uuid,
        int a_connection_id
        ) = 0;

    virtual void RegisterNotification
        (
        int client_if,
        BluetoothAddress a_address,
        int16_t a_handle
        ) = 0;

    virtual void WriteCharacteristic
        (
        int conn_id, uint16_t handle,
        int write_type, int auth_req,
        const uint8_t* value, size_t length
        ) = 0;

    virtual void ReadDescriptor
        (
        int conn_id, uint16_t handle, int auth_req
        ) = 0;

    virtual void GetDbByConnectId( int conn_id ) = 0;

    virtual void UpdateConnectionParameters
        (
        const BluetoothAddress& bd_addr,
        int min_interval,
        int max_interval,
        int latency,
        int timeout,
        uint16_t min_ce_len,
        uint16_t max_ce_len
        ) = 0;

    virtual void ConfigureMtuSize
        (
        int connd_id,
        int mtu
        ) = 0;

public:

    virtual boost_ns::signals2::connection ConnectToSignalGattClientInited
        (
        std::function<void()> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection ConnectToSignalGattClientMessageEmiited
        (
        std::function<void(std::shared_ptr< GattClientBaseMessage>) > a_fun
        ) = 0;

    virtual boost_ns::signals2::connection ConnectToClientRegistered
        (
        std::function<void(int, int, std::vector<uint8_t>)> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection ConnectToConnectionStatusChanged
        (
        std::function<void(int, ConnectionStatus/*a_status*/, int/*client_if*/, BluetoothAddress)> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection ConnectToServiceSearchCompleted
        (
        std::function<void(std::vector<GATT_DB_ELEMENT>, int)> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection ConnectToDescriptorReadCompleted
        (
        std::function<void(int, int, btgatt_read_params_type)> a_fun
        ) = 0;

    virtual boost_ns::signals2::connection ConnectToCharacteristicWriteCompleted
        (
        std::function<void(int conn_id, int status, uint16_t handle, uint16_t len, std::vector<uint8_t> value)>
        ) = 0;
};

