#pragma once

class BluetoothLeAudioBroadcasterInterface
{

public:

    static BluetoothLeAudioBroadcasterInterface& GetInterface();

    virtual ~BluetoothLeAudioBroadcasterInterface() {}

    virtual void Init() = 0;

};

