#pragma once
#include <memory>

#include "BT/BluetoothAddress.h"
#include "BT/BluetoothCommonDefs.h"

namespace Bluetooth
{

class BluetoothAvrcpInterface
{
public:

    static std::shared_ptr<BluetoothAvrcpInterface> CreateAvrcpInterface();

    virtual ~BluetoothAvrcpInterface() {};

    virtual bool SendGetPlayBackStateCmd(BluetoothAddress const&) = 0;

    virtual bool SendPassThroughCmd
        (
        BluetoothAddress const& a_address,
        AvrcpPassThroughKeyState a_state,
        AvrcpPassThroughKeyCode a_key
        ) = 0;

    virtual bool init() = 0;

private:

};

}
