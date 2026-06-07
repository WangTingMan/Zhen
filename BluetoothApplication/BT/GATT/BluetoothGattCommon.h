#pragma once
#include <vector>
#include <cstdint>

#define BluetoothBaseUuid { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb }
#define ClientCharacteristicConfigUuid { 0x29, 0x02 }

std::vector<uint8_t> MakeBluetoothStandardUuid( std::vector<uint8_t> sourceUuid );

enum class gatt_db_attribute_type : uint8_t
{
    BTGATT_DB_PRIMARY_SERVICE,
    BTGATT_DB_SECONDARY_SERVICE,
    BTGATT_DB_INCLUDED_SERVICE,
    BTGATT_DB_CHARACTERISTIC,
    BTGATT_DB_DESCRIPTOR,
};

struct GATT_DB_ELEMENT
{

    void clear()
    {
        id = 0;
        type = gatt_db_attribute_type::BTGATT_DB_DESCRIPTOR;
        attribute_handle = 0x00;
        start_handle = 0x00;
        end_handle = 0x00;
        properties = 0x00;
        permissions = 0x00;
        extended_properties = 0x0000;
        uuid.clear();
    }

    uint16_t id;
    std::vector<uint8_t> uuid;
    gatt_db_attribute_type type;
    uint16_t attribute_handle;

    /*
     * If |type| is |BTGATT_DB_PRIMARY_SERVICE|, or
     * |BTGATT_DB_SECONDARY_SERVICE|, this contains the start and end attribute
     * handles.
     */
    uint16_t start_handle;
    uint16_t end_handle;

    /*
     * If |type| is |BTGATT_DB_CHARACTERISTIC|, this contains the properties of
     * the characteristic.
     */
    uint8_t properties;
    uint16_t extended_properties;
    uint16_t permissions;
};
