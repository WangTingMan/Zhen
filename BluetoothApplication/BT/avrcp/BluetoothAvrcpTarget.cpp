#include "BluetoothAvrcpTarget.h"
#include "../Adaptor.h"
#define AVRCP_TARGET_MODULE "AVRCP_TARGET_MODULE"

namespace Bluetooth
{

class BluetoothAvrcpTargetImpl
{

public:

    bool m_inited = false;
};


BluetoothAvrcpTarget::BluetoothAvrcpTarget()
{
    SetModuleName( AVRCP_TARGET_MODULE );
}

std::shared_ptr<BluetoothAvrcpTarget> BluetoothAvrcpTarget::GetMoudle()
{
    auto hf = Adaptor::GetInstance().FindModule( AVRCP_TARGET_MODULE );
    std::shared_ptr< BluetoothAvrcpTarget > hfModule = std::dynamic_pointer_cast< BluetoothAvrcpTarget >( hf );
    return hfModule;
}

int BluetoothAvrcpTarget::Init()
{
    if( m_impl && m_impl->m_inited )
    {
        return 0;
    }

    m_impl = std::make_shared<BluetoothAvrcpTargetImpl>();
    return 0;
}

bool BluetoothAvrcpTarget::Connect
    (
    BluetoothAddress a_address
    )
{
    return 0;
}

}
