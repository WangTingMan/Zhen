#pragma once
#include "BluetoothLocalDevice.h"
#include "BluetoothAddress.h"
#include "BluetoothCommonDefs.h"
#include "AbstractModule.h"
#include "ModuleManager.h"
#include "StackLayer/BluetoothBaseInterface.h"
#include "common/BluetoothUuid.h"
#include <BT/StackLayer/bluetooth_interface.h>

#include <Zhen/global.h>

#include <list>
#include <memory>
#include <vector>

struct RemoteDevice
{
    std::string name;
    BluetoothAddress address;
    std::vector<BluetoothUuid> uuids;
    uint32_t cod = 0u;
    DeviceControllerType type = DeviceControllerType::BREDR;
    int8_t rssi = 0;
};

class Adaptor : protected AbstractModule
{

public:

    Adaptor();

    static Adaptor& GetInstance();

    bool isPowerOn()
    {
        return m_enableState == AdaptorEnableState::Enabled;
    }

    void deinit();

    void Enable();

    void disable();

    void StartSearch();

    void StopSearch();

    DiscoveryState GetSearchState()const
    {
        return m_discoveryState;
    }

    std::list< RemoteDevice >const & GetFoundDevices()const
    {
        return m_foundDevices;
    }
    
    std::list< RemoteDevice >const & GetPairedDevices()const
    {
        return m_pairedDevices;
    }

    void PairWithDevice( BluetoothAddress a_address );

    void DeletePairedDevice( BluetoothAddress a_address );

    void SetLocalName( std::string const& a_name );

    std::string const& GetLocalName()const
    {
        return m_localDevice.GetName();
    }

    BluetoothAddress GetLocalAddress()const
    {
        return m_localDevice.GetAddress();
    }

    //! Set local device settings
    //! @return whether the request was passed successfully to stack
    bool SetLocalDeviceSettings
        (
        bool a_pairable,     //!< [in] whether BT module is pairable
        bool a_discoverable, //!< [in] whether BT module is discoverable
        bool a_connectable   //!< [in] whether BT module is connectable
        );

    //! Get local device settings
    bool GetLocalDeviceSettings
        (
        bool& a_pairable,     //!< [out] whether BT module is pairable
        bool& a_discoverable, //!< [out] whether BT module is discoverable
        bool& a_connectable   //!< [out] whether BT module is connectable
        ) const;

    bool SspPairingReply
        (
        BluetoothAddress a_address,
        bool             a_accept = true
        );

    bool PincodeReply
        (
        BluetoothAddress     a_address,
        bool                 a_accept,
        std::vector<uint8_t> a_pin
        );

    void GenerateOobData();

    BondState GetPairingState()
    {
        return m_bondState;
    }

    DiscoveryState GetSearchState()
    {
        return m_discoveryState;
    }

    void LogToFile
        (
        const char* a_log,
        uint32_t a_line,
        const char* a_file,
        uint8_t a_level
        );

    void Test();

    std::shared_ptr<AbstractModule> FindModule( std::string const& a_name );

    std::shared_ptr<ModuleManager> GetModuleManager()const
    {
        return m_moduleManager;
    }

    std::shared_ptr<bluetooth_interface> GetLowLevelInterface( bluetooth_interface_type a_type )
    {
        for( auto& ele : m_low_level_interfaces )
        {
            if( ele->get_interface_type() == a_type )
            {
                return ele;
            }
        }
        return nullptr;
    }

// The signals connection functions
public:

    boost_ns::signals2::connection ConnectDiscoveryStateChanged
        (
        std::function<void(DiscoveryState) > a_fun
        )
    {
        return m_discoveryStateChanged.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectDeviceFound
        (
        std::function<void() > a_fun
        )
    {
        return m_deviceFounSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectPairingRequest
        (
        std::function<void( BluetoothAddress, std::string, uint32_t )> a_fun
        )
    {
        return m_sppPairingRequestSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectPincodeRequest
        (
        std::function<void(BluetoothAddress, std::string, bool)> a_fun
        )
    {
        return m_pincodeRequestSignal.connect(a_fun);
    }

    boost_ns::signals2::connection ConnectPairedDeviceChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_devicePairedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectPairingStateChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_pairingStateChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectEnableStateChanged
        (
        std::function<void( AdaptorEnableState )> a_fun
        )
    {
        return m_enableStateChangedSignal.connect( a_fun );
    }

// Process the callbacks event from stack. These function should not be invoked by external modules
public:

    void OnDeviceFound( const RemoteDevice& a_device );

    void OnDiscoveryStateChanged(DiscoveryState a_discoveryState )
    {
        m_discoveryState = a_discoveryState;
        m_discoveryStateChanged( a_discoveryState );
    }

    void OnBondStateChanged
        (
        BondState a_state,
        BluetoothAddress a_address
        );

    void OnSppPairingConfRequest
        (
        BluetoothAddress a_address,
        std::string      a_deviceName,
        uint32_t         a_passkey,
        SppPairingMethod a_pairingMethod
        );

    void OnPincodeRequest
        (
        BluetoothAddress a_address,
        std::string      a_deviceName,
        bool             a_min_16_digit
        );

    void OnLocalNameChanged
        (
        std::string a_name
        )
    {
        m_localDevice.SetNameInternal( a_name );
    }

    void OnLocalAddressChanged
        (
        BluetoothAddress a_address
        )
    {
        m_localDevice.SetAddressInternal( a_address );
    }

    void OnLocalSettingsChanged
        (
        bool a_discoveriable,
        bool a_connectable
        )
    {
        m_localDevice.SetDiscoverable( a_discoveriable );
        m_localDevice.SetConnectable( a_connectable );
    }

    void OnPairedDeviceReceived
        (
        RemoteDevice a_device
        );

    void AdapterStateChanged( bool a_isOn );

private:

    struct PairingRequestData
    {
        BluetoothAddress address;
        std::string deviceName;
        uint32_t passkey;
        SppPairingMethod pairingMethod;
    };

    bool m_initialized = false;
    AdaptorEnableState m_enableState = AdaptorEnableState::Disabled;
    std::list<RemoteDevice> m_foundDevices;
    std::list<RemoteDevice> m_pairedDevices;
    BluetoothLocalDevice m_localDevice;
    PairingRequestData m_pairingData;
    std::shared_ptr<ModuleManager> m_moduleManager;

    DiscoveryState m_discoveryState = DiscoveryState::BT_DISCOVERY_STOPPED;
    BondState m_bondState = BondState::NoneBond;
    BluetoothAddress m_bondingAddress;
    RemoteDevice m_pairingDevice;

    boost_ns::signals2::signal<void(DiscoveryState)> m_discoveryStateChanged;
    boost_ns::signals2::signal<void()> m_deviceFounSignal;
    boost_ns::signals2::signal<void()> m_devicePairedSignal;
    boost_ns::signals2::signal<void()> m_pairingStateChangedSignal;
    boost_ns::signals2::signal<void(BluetoothAddress, std::string, uint32_t)> m_sppPairingRequestSignal;
    boost_ns::signals2::signal<void(BluetoothAddress, std::string, bool)> m_pincodeRequestSignal;
    boost_ns::signals2::signal<void( AdaptorEnableState )> m_enableStateChangedSignal;

    std::vector<std::shared_ptr<bluetooth_interface>> m_low_level_interfaces;
};

