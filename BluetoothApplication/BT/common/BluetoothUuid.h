
#pragma once

#include <array> 
#include <cstdint> 
#include <iostream>
#include <string>

// This class is representing Bluetooth UUIDs across whole stack.
// Here are some general endianness rules:
// 1. UUID is internally kept as as Big Endian.
// 2. Bytes representing UUID coming from upper layers, Java or Binder, are Big
//    Endian.
// 3. Bytes representing UUID coming from lower layer, HCI packets, are Little
//    Endian.
// 4. UUID in storage is always string.
class BluetoothUuid final {
public:
    static constexpr size_t kNumBytes128 = 16;
    static constexpr size_t kNumBytes32 = 4;
    static constexpr size_t kNumBytes16 = 2;

    static constexpr size_t kString128BitLen = 36;

    static BluetoothUuid kEmpty;  // 00000000-0000-0000-0000-000000000000
    static BluetoothUuid kBase;   // 00000000-0000-1000-8000-00805f9b34fb

    using UUID128Bit = std::array<uint8_t, kNumBytes128>;

    // Creates and returns a random 128-bit UUID.
    static BluetoothUuid GetRandom();

    BluetoothUuid() = default;

    BluetoothUuid(uint8_t* a_data)
    {
        memcpy(uu, a_data, kNumBytes128);
    }

    // Returns the shortest possible representation of this UUID in bytes. Either
    // kNumBytes16, kNumBytes32, or kNumBytes128
    size_t GetShortestRepresentationSize() const;

    // Returns true if this UUID can be represented as 16 bit.
    bool Is16Bit() const;

    // Returns 16 bit Little Endian representation of this UUID. Use
    // GetShortestRepresentationSize() or Is16Bit() before using this method.
    uint16_t As16Bit() const;

    // Returns 32 bit Little Endian representation of this UUID. Use
    // GetShortestRepresentationSize() before using this method.
    uint32_t As32Bit() const;

    // Converts string representing 128, 32, or 16 bit UUID in
    // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx, xxxxxxxx, or xxxx format to UUID. If
    // set, optional is_valid parameter will be set to true if conversion is
    // successfull, false otherwise.
    static BluetoothUuid FromString(const std::string& uuid, bool* is_valid = nullptr);

    // Converts 16bit Little Endian representation of UUID to UUID
    static BluetoothUuid From16Bit(uint16_t uuid16bit);

    // Converts 32bit Little Endian representation of UUID to UUID
    static BluetoothUuid From32Bit(uint32_t uuid32bit);

    // Converts 128 bit Big Endian array representing UUID to UUID.
    static BluetoothUuid From128BitBE(const UUID128Bit& uuid) {
#if( _MSC_VER <= 1900 )
        return Uuid(uuid);
#else
        BluetoothUuid u;
        u.Construt(uuid);
        return u;
#endif
    }

    // Converts 128 bit Big Endian array representing UUID to UUID. |uuid| points
    // to beginning of array.
    static BluetoothUuid From128BitBE(const uint8_t* uuid);

    // Converts 128 bit Little Endian array representing UUID to UUID.
    static BluetoothUuid From128BitLE(const UUID128Bit& uuid);

    // Converts 128 bit Little Endian array representing UUID to UUID. |uuid|
    // points to beginning of array.
    static BluetoothUuid From128BitLE(const uint8_t* uuid);

    // Returns 128 bit Little Endian representation of this UUID
    const UUID128Bit To128BitLE() const;

    // Returns 128 bit Big Endian representation of this UUID
    const UUID128Bit To128BitBE() const;

    // Returns string representing this UUID in
    // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx format, lowercase.
    std::string ToString() const;

    // Returns true if this UUID is equal to kEmpty
    bool IsEmpty() const;

    bool operator<(const BluetoothUuid& rhs) const;
    bool operator==(const BluetoothUuid& rhs) const;
    bool operator!=(const BluetoothUuid& rhs) const;

    void Construt(const UUID128Bit& val)
    {
        for (int i = 0; i < kNumBytes128; ++i)
        {
            uu[i] = val[i];
        }
    }

public:
    // Network-byte-ordered ID (Big Endian).
    uint8_t uu[kNumBytes128];
};


inline std::ostream& operator<<(std::ostream& os, const BluetoothUuid& a) {
    os << a.ToString();
    return os;
}

// Custom std::hash specialization so that bluetooth::UUID can be used as a key
// in std::unordered_map.
namespace std {

    template <>
    struct hash<BluetoothUuid> {
        std::size_t operator()(const BluetoothUuid& key) const {
            const auto uuid_bytes = key.To128BitBE();
            std::hash<std::string> hash_fn;
            return hash_fn(std::string(reinterpret_cast<const char*>(uuid_bytes.data()),
                uuid_bytes.size()));
        }
    };

}  // namespace std
