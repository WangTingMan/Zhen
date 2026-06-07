#include "BluetoothHidHost.h"
#include "BT/Adaptor.h"
#include "BT/StackLayer/BluetoothHidHostInterface.h"

#define HID_HOST_SERVICE_MODULE "HID_HOST_ROLE"

namespace Bluetooth {

BluetoothHidHost::BluetoothHidHost()
{
    m_status = ConnectionStatus::ServiceDisconnected;
    SetModuleName(HID_HOST_SERVICE_MODULE);
}

std::shared_ptr<BluetoothHidHost> BluetoothHidHost::GetMoudle()
{
    auto hf = Adaptor::GetInstance().FindModule(HID_HOST_SERVICE_MODULE);
    std::shared_ptr<BluetoothHidHost> hfModule = std::dynamic_pointer_cast<BluetoothHidHost>(hf);
    return hfModule;
}

int BluetoothHidHost::Init()
{
    BluetoothHidHostInterface::GetInterface().Init();
    return 0;
}

bool BluetoothHidHost::Connect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothHidHostInterface::GetInterface().Connect(a_address);
}

bool BluetoothHidHost::Disconnect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothHidHostInterface::GetInterface().disconnect(a_address);
}

void BluetoothHidHost::GetReport
    (
    BluetoothAddress a_address
    )
{
    return BluetoothHidHostInterface::GetInterface().GetReport(a_address);
}

std::vector<BluetoothAddress> BluetoothHidHost::GetConnectedDevices()
{
    std::vector<BluetoothAddress> devices;
    return devices;
}

ConnectionStatus BluetoothHidHost::GetConnectionState(BluetoothAddress a_address)
{
    return m_status;
}

ConnectionStatus BluetoothHidHost::GetConnectionState()const
{
    return m_status;
}

void BluetoothHidHost::GetConnectionStatus
    (
    BluetoothAddress& a_address,
    ConnectionStatus& a_status
    )
{
    a_address = m_address;
    a_status = m_status;
}

void BluetoothHidHost::HandleConnectionStatusChanged
    (
    BluetoothAddress a_address,
    ConnectionStatus a_status
    )
{
    m_address = a_address;
    m_status = a_status;
    m_connectionStatusChangedSignal(m_address, m_status);
}

}
