#include "BluetoothGattCommon.h"

std::vector<uint8_t> MakeBluetoothStandardUuid( std::vector<uint8_t> sourceUuid )
{
    std::vector<uint8_t> ret = BluetoothBaseUuid;
    if( 2 == sourceUuid.size() )
    {
        ret[2] = sourceUuid[0];
        ret[3] = sourceUuid[1];
    }
    return ret;
}

