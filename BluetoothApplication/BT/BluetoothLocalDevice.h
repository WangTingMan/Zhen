#pragma once
#include "BluetoothDevice.h"

class BluetoothLocalDevice : public BluetoothDevice
{
public:

    //! Get local device settings
    bool GetLocalDeviceSettings
        (
        bool& a_pairable,     //!< [out] whether BT module is pairable
        bool& a_discoverable, //!< [out] whether BT module is discoverable
        bool& a_connectable   //!< [out] whether BT module is connectable
        ) const
    {
        a_pairable = m_pairable;
        a_discoverable = m_discoverable;
        a_connectable = m_connectable;
        return true;
    }

    //! Set local device settings
    //! @return whether the request was passed successfully to stack
    bool SetLocalDeviceSettingsInternal
        (
        bool a_pairable,     //!< [in] whether BT module is pairable
        bool a_discoverable, //!< [in] whether BT module is discoverable
        bool a_connectable   //!< [in] whether BT module is connectable
        )
    {
        m_pairable = a_pairable;
        m_discoverable = a_discoverable;
        m_connectable = a_connectable;
        return true;
    }


    void SetDiscoverable
        (
        bool a_discoverable = false
        )
    {
        m_discoverable = a_discoverable;
    }

    void SetConnectable
        (
        bool a_connectable = false
        )
    {
        m_connectable = a_connectable;
    }

    void SetPairable
        (
        bool a_pairable = false
        )
    {
        m_pairable = a_pairable;
    }

private:

    bool m_pairable = false;      //!< [in] whether BT module is pairable
    bool m_discoverable = false;  //!< [in] whether BT module is discoverable
    bool m_connectable = false;   //!< [in] whether BT module is connectable
};

