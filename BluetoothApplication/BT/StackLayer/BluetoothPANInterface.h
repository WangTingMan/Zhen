#pragma once
#include <BT/BluetoothAddress.h>

class BluetoothPANInterface
{

public:

	static BluetoothPANInterface& GetInterface();

	virtual ~BluetoothPANInterface() {}

	virtual void Init() = 0;

    virtual bool Connect(BluetoothAddress a_address) = 0;

    virtual bool disconnect(BluetoothAddress a_address) = 0;

};

