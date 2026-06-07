#pragma once

class BluetoothAdvScanningInterface
{

public:

    static BluetoothAdvScanningInterface& GetInterface();

    virtual ~BluetoothAdvScanningInterface(){}

    virtual void Init() = 0;

    virtual void StartFullScan(bool a_start) = 0;

    virtual void StartFilterScan() = 0;
};

