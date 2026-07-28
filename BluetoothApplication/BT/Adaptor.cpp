#include "Adaptor.h"
#include "BluetoothAddress.h"
#include "BluetoothDevice.h"
#include "BluetoothHF.h"
#include "BluetoothA2DPSink.h"

#include "Utils.h"

#include <Zhen\PageManager.h>
#include <Zhen\ExecutbleEvent.h>
#include <Zhen\logging.h>

#include <base\logging.h>
#include <base\strings\string_util.h>
#include <base\strings\stringprintf.h>

#include <windows.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iomanip>

using Bluetooth::BluetoothHF;

Adaptor::Adaptor()
{
    m_moduleManager = std::make_shared<ModuleManager>();
    BluetoothBaseInterface::GetInterface();
}

Adaptor& Adaptor::GetInstance()
{
    static Adaptor instance;
    return instance;
}

void Adaptor::OnDeviceFound( const RemoteDevice& a_device )
{
    bool foundPaired = false;
    for( auto it = m_pairedDevices.begin(); it != m_pairedDevices.end(); ++it )
    {
        if( it->address == a_device.address )
        {
            foundPaired = true;
            *it = a_device;
            break;
        }
    }

    if( foundPaired )
    {
        m_devicePairedSignal();
        return;
    }

    auto it = std::find_if( m_foundDevices.begin(), m_foundDevices.end(),
    [&]( const RemoteDevice& ele )
    {
        if( ele.address == a_device.address )
        {
            return true;
        }
        return false;
    } );

    LogDebug() << "device name: " << a_device.name;
    for( int j = 0; j < a_device.uuids.size(); ++j )
    {
    }

    if( it != m_foundDevices.end() )
    {
        *it = a_device;
    }
    else
    {
        m_foundDevices.push_back( a_device );
    }
    m_deviceFounSignal();
}

void Adaptor::OnBondStateChanged
    (
    BondState a_state,
    BluetoothAddress a_address
    )
{
    RemoteDevice remote_device;
    bool paired_found = false;
    for( auto it = m_pairedDevices.begin(); it != m_pairedDevices.end(); )
    {
        if( it->address == a_address )
        {
            remote_device = *it;
            it = m_pairedDevices.erase( it );
            paired_found = true;
        }
        else
        {
            ++it;
        }
    }

    if( m_bondState != a_state )
    {
        m_bondState = a_state;
        m_pairingStateChangedSignal();
    }

    if( BondState::Bonding == m_bondState )
    {
        m_bondingAddress = a_address;
    }
    else
    {
        memset( m_bondingAddress.address, 0x00, BluetoothAddress::kLength );
    }

    if( m_bondState == BondState::BondCompleted )
    {
        bool found = false;
        bool bond_added = false;
        for (auto it = m_foundDevices.begin(); it != m_foundDevices.end(); ++it)
        {
            if (it->address == a_address)
            {
                remote_device = *it;
                m_pairedDevices.push_back(remote_device);
                found = true;
                bond_added = true;
                break;
            }
        }

        if (!found && paired_found )
        {
            m_pairedDevices.push_back(remote_device);
            bond_added = true;
        }

        if (!bond_added)
        {
            remote_device.address = a_address;
            m_pairedDevices.push_back(remote_device);
        }
    }

    m_devicePairedSignal();

}

void Adaptor::OnSppPairingConfRequest
    (
    BluetoothAddress a_address,
    std::string      a_deviceName,
    uint32_t         a_passkey,
    SppPairingMethod a_pairingMethod
    )
{
    m_pairingData.address = a_address;
    m_pairingData.deviceName = a_deviceName;
    m_pairingData.passkey = a_passkey;
    m_pairingData.pairingMethod = a_pairingMethod;
    m_sppPairingRequestSignal( a_address, a_deviceName, a_passkey );
}

void Adaptor::OnPincodeRequest
    (
    BluetoothAddress a_address,
    std::string      a_deviceName,
    bool             a_min_16_digit
    )
{
    PincodeReply(a_address, true, { 0x30, 0x30, 0x30, 0x30 });
    LogDebug() << "Automatically accept pin code request from remote device..";
    m_pincodeRequestSignal(a_address, a_deviceName, a_min_16_digit);
}

void Adaptor::OnPairedDeviceReceived
    (
    RemoteDevice a_device
    )
{
    for( auto it = m_foundDevices.begin(); it != m_foundDevices.end(); ++it)
    {
        if( it->address == a_device.address )
        {
            if( a_device.name.empty() )
            {
                a_device.name = it->name;
            }
            it = m_foundDevices.erase( it );
            break;
        }
    }

    bool foundPaired = false;
    for( auto it = m_pairedDevices.begin(); it != m_pairedDevices.end(); ++it )
    {
        if( it->address == a_device.address )
        {
            foundPaired = true;
            if( !a_device.name.empty() )
            {
                it->name = a_device.name;
            }
            break;
        }
    }

    if( !foundPaired )
    {
        m_pairedDevices.push_back( a_device );
    }

    m_devicePairedSignal();

}

void Adaptor::AdapterStateChanged
    (
    bool a_isOn
    )
{
    if( a_isOn )
    {
        m_moduleManager->Init();
        m_enableState = AdaptorEnableState::Enabled;
        m_enableStateChangedSignal( m_enableState );
    }
    else
    {
        m_enableState = AdaptorEnableState::Disabled;
        m_enableStateChangedSignal( m_enableState );
    }
}

void Adaptor::deinit()
{
    if( m_initialized )
    {
        BluetoothBaseInterface::GetInterface().Enable(false);
    }
}

void Adaptor::Enable()
{
    if( m_enableState != AdaptorEnableState::Disabled )
    {
        return;
    }

    m_enableState = AdaptorEnableState::Enabling;
    m_enableStateChangedSignal( m_enableState );

    m_low_level_interfaces = BluetoothBaseInterface::LoadLowLevelInterfaces();
    BluetoothBaseInterface::GetInterface().Enable();

    for( auto& inter : m_low_level_interfaces )
    {
        inter->Init();
    }
}

void Adaptor::disable()
{
    m_enableState = AdaptorEnableState::Disabling;
    BluetoothBaseInterface::GetInterface().Enable(false);
}

void Adaptor::StartSearch()
{
    BluetoothBaseInterface::GetInterface().StartSearch(true);
}

void Adaptor::StopSearch()
{
    BluetoothBaseInterface::GetInterface().StartSearch(false);
}

void Adaptor::PairWithDevice( BluetoothAddress a_address )
{
    BluetoothBaseInterface::GetInterface().PairWithDevice(a_address);
}

void Adaptor::DeletePairedDevice( BluetoothAddress a_address )
{
    BluetoothBaseInterface::GetInterface().DeletePairedDevice(a_address);
}

void Adaptor::SetLocalName( std::string const& a_name )
{
    BluetoothBaseInterface::GetInterface().SetLocalName(a_name);
}

bool Adaptor::SetLocalDeviceSettings
    (
    bool a_pairable,
    bool a_discoverable,
    bool a_connectable
    )
{
    return BluetoothBaseInterface::GetInterface().SetLocalDeviceSettings(
        a_pairable, a_discoverable, a_connectable);
}

bool Adaptor::GetLocalDeviceSettings
    (
    bool& a_pairable,
    bool& a_discoverable,
    bool& a_connectable
    ) const
{
    bool r = 0;
    r = m_localDevice.GetLocalDeviceSettings( a_pairable, a_discoverable, a_connectable );
    return r;
}

bool Adaptor::SspPairingReply
    (
    BluetoothAddress a_address,
    bool             a_accept
    )
{
    return BluetoothBaseInterface::GetInterface().SspPairingReply(a_address, a_accept,
        m_pairingData.pairingMethod, m_pairingData.passkey);
}

bool Adaptor::PincodeReply
    (
    BluetoothAddress     a_address,
    bool                 a_accept,
    std::vector<uint8_t> a_pin
    )
{
    return BluetoothBaseInterface::GetInterface().PincodeReply(a_address, a_accept,
        a_pin);
}

void Adaptor::GenerateOobData()
{
    //m_interface->get_local_oob_ex();
}

void Adaptor::LogToFile
    (
    const char* a_log,
    uint32_t a_line,
    const char* a_file,
    uint8_t a_level
    )
{
}

void Adaptor::Test()
{
    BluetoothBaseInterface::GetInterface().test();
}

std::shared_ptr<AbstractModule> Adaptor::FindModule( std::string const& a_name )
{
    return m_moduleManager->GetMoudle( a_name );
}

