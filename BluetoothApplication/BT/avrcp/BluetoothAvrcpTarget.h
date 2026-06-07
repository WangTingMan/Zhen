#pragma once
#include "BT/ConnectableModule.h"

#include <memory>

namespace Bluetooth
{

class BluetoothAvrcpTargetImpl;

class BluetoothAvrcpTarget : public AbstractModule
{

public:

    BluetoothAvrcpTarget();

    static std::shared_ptr<BluetoothAvrcpTarget> GetMoudle();

    int Init()override;

    bool Connect
        (
        BluetoothAddress a_address
        );

private:

    std::shared_ptr<BluetoothAvrcpTargetImpl> m_impl;
};

}
