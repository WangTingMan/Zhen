#pragma once
#include <BT/BluetoothAddress.h>
#include <BT/StackLayer/bluetooth_interface.h>

class bluetooth_l2cap_coc_interface : public bluetooth_interface
{

public:

    static bluetooth_l2cap_coc_interface& GetInterface();

    bluetooth_l2cap_coc_interface()
        : bluetooth_interface( bluetooth_interface_type::l2cap_coc )
    {

    }

    virtual ~bluetooth_l2cap_coc_interface() {}

    virtual void Init() = 0;

    virtual bool Connect
        (
        BluetoothAddress a_address,
        uint16_t a_psm,
        bool a_use_br_edr
        ) = 0;

    virtual bool disconnect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool listen(uint16_t a_psm, bool a_use_br_edr) = 0;

};

