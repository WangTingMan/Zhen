#pragma once

#include "BT/BluetoothLocalDevice.h"
#include "BT/BluetoothAddress.h"
#include "BT/BluetoothCommonDefs.h"

#include <memory>
#include <vector>

class BluetoothBaseInterface
{

public:

    static BluetoothBaseInterface& GetInterface();

    virtual ~BluetoothBaseInterface()
    {

    }

    /**
     * Init platform related features.
     * These features are not related to bluetooth.
     * So will before initialize bluetooth, will invoke this interface
     * to initialize platform related features first.
     */
    virtual void InitPlatform() = 0;

    virtual void Init() = 0;

    virtual void Enable(bool a_enable = true) = 0;

    virtual void StartSearch(bool a_search = true) = 0;

    virtual void PairWithDevice(BluetoothAddress a_address) = 0;

    virtual void DeletePairedDevice(BluetoothAddress a_address) = 0;

    virtual void SetLocalName(std::string const& a_name) = 0;

    virtual bool SetLocalDeviceSettings
        (
        bool a_pairable,     //!< [in] whether BT module is pairable
        bool a_discoverable, //!< [in] whether BT module is discoverable
        bool a_connectable   //!< [in] whether BT module is connectable
        ) = 0;

    virtual bool SspPairingReply
        (
        BluetoothAddress a_address,
        bool             a_accept,
        SppPairingMethod a_pairingMethod,
        uint32_t a_passkey
        ) = 0;

    virtual bool PincodeReply
        (
        BluetoothAddress     a_address,
        bool                 a_accept,
        std::vector<uint8_t> a_pin
        ) = 0;

    virtual void test() = 0;
};

