#pragma once
#include <BT/BluetoothAddress.h>

class BluetoothHFInterface
{

public:

    static BluetoothHFInterface& GetInterface();

    virtual ~BluetoothHFInterface(){}

    virtual void Init() = 0;

    virtual bool Connect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool disconnect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool setActiveDevice
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool setSilenceDevice
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool dialNumber
        (
        BluetoothAddress a_address,
        std::string a_number
        ) = 0;

    virtual void answerIncomingCall
        (
        BluetoothAddress& a_address,
        bool a_answered = true
        ) = 0;

    // Send AT+CHLD=2 to phone.
    virtual void swapCalls
        (
        BluetoothAddress a_address
        ) = 0;

    virtual void joinCalls
        (
        BluetoothAddress a_address
        ) = 0;

    virtual void splitCall
        (
        BluetoothAddress a_address,
        int a_index
        ) = 0;

    virtual bool QueryCallList
        (
        BluetoothAddress a_address
        ) = 0;

};

