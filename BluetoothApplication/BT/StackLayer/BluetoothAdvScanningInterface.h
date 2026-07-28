#pragma once 
#include <BT/StackLayer/bluetooth_interface.h>

class BluetoothAdvScanningInterface : public bluetooth_interface
{

public:

    static BluetoothAdvScanningInterface& GetInterface();

    BluetoothAdvScanningInterface()
        : bluetooth_interface( bluetooth_interface_type::adv_scanning )
    {

    }

    virtual ~BluetoothAdvScanningInterface(){}

    virtual void Init() = 0;

    virtual void StartFullScan(bool a_start) = 0;

    virtual void StartFilterScan() = 0;
};

