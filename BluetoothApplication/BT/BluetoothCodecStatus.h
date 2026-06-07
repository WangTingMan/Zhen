#pragma once
#include "BluetoothCodecConfig.hpp"

#include <vector>

class BluetoothCodecStatus
{

public:

    std::vector<BluetoothCodecConfig> getCodecsSelectableCapabilities()
    {
        return std::vector<BluetoothCodecConfig>();
    }

    BluetoothCodecConfig getCodecConfig()
    {
        return BluetoothCodecConfig();
    }

};

