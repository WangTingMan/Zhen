#pragma once
#include <BT/BluetoothAddress.h>
#include <BT/StackLayer/bluetooth_interface.h>

class BluetoothPANInterface : public bluetooth_interface
{

public:

	static BluetoothPANInterface& GetInterface();

    BluetoothPANInterface()
        : bluetooth_interface( bluetooth_interface_type::pan )
    {

    }

	virtual ~BluetoothPANInterface() {}

	virtual void Init() = 0;

    virtual bool Connect(BluetoothAddress a_address) = 0;

    virtual bool disconnect(BluetoothAddress a_address) = 0;

};

