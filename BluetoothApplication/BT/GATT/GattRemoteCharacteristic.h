#pragma once
#include <cstdint>
#include <vector>

#include "BT/GATT/GattRemoteDescriptor.h"

class GattRemoteCharacteristic;

typedef void ( *dataSourceNotifyCallbackType )(
    GattRemoteCharacteristic* pDataSourceCharacteristic,
    uint8_t* pData,
    size_t length,
    bool isNotify );

class GattRemoteCharacteristic
{

public:

    void registerForNotify( dataSourceNotifyCallbackType a_callback )
    {
        m_notifyCallback = a_callback;
    }

    GattRemoteDescriptor* getDescriptor( std::vector<uint8_t> a_uuid );

    void writeValue( uint8_t* value, uint32_t len, bool );

    void SetHandle( uint16_t a_handle )
    {
        m_handle = a_handle;
    }

private:

    dataSourceNotifyCallbackType m_notifyCallback = nullptr;
    uint16_t m_handle = 0;
    std::vector<uint8_t> m_uuid;
    std::vector< std::pair< std::vector<uint8_t>, GattRemoteDescriptor > > m_descriptors;
};

