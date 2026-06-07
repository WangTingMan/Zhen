#pragma once
#include <BT/BluetoothAddress.h>
#include <BT/BluetoothCommonDefs.h>

class BluetoothHFAGInterface
{

public:

    static BluetoothHFAGInterface& GetInterface();

    virtual ~BluetoothHFAGInterface(){}

    virtual void Init() = 0;

    virtual bool Connect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool disconnect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool connectAudio(BluetoothAddress a_address) = 0;

    virtual bool DialNewNumber( std::string a_number ) = 0;

    virtual void ResponseCind
        (
        BluetoothAddress a_address,
        bool a_cell_network_available,
        int a_active_call_count,
        int a_held_call_count,
        CallState a_call_state,
        int a_signal_level,
        bool a_is_roaming,
        int a_bettery_level
        ) = 0;

    virtual void ResponseCops
        (
        BluetoothAddress a_address,
        std::string a_cops
        ) = 0;

    virtual void ResponseCnum
        (
        BluetoothAddress a_address,
        std::string a_cnum
        ) = 0;

    virtual void ResponseClcc
        (
        BluetoothAddress a_address,
        int a_index,
        CallDirection a_direction,
        CallState a_call_state,
        CallMode a_call_mode,
        CallType a_call_type,
        std::string a_number,
        CallAddrType a_call_addr_type
        ) = 0;

    virtual void AtResponse
        (
        bool a_result,
        int a_code,
        BluetoothAddress a_address
        ) = 0;

    virtual void PhoneStateChange
        (
        BluetoothAddress address,
        int num_active,
        int num_held,
        CallState call_setup_state,
        std::string number,
        CallAddrType type,
        std::string name
        ) = 0;

public:


};

