#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "BluetoothUuid.h"

#include <base/rand_util.h>
#include <base/strings/stringprintf.h>
#include <algorithm>

    static_assert(sizeof(BluetoothUuid) == 16, "BluetoothUuid must be 16 bytes long!");

    using UUID128Bit = BluetoothUuid::UUID128Bit;

    BluetoothUuid BluetoothUuid::kEmpty = BluetoothUuid::From128BitBE(UUID128Bit{ {0x00} });
    BluetoothUuid BluetoothUuid::kBase = BluetoothUuid::From128BitBE(
        UUID128Bit{ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00,
                    0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb} });

    size_t BluetoothUuid::GetShortestRepresentationSize() const {
        if (memcmp(uu + kNumBytes32, kBase.uu + kNumBytes32,
            kNumBytes128 - kNumBytes32) != 0) {
            return kNumBytes128;
        }

        if (uu[0] == 0 && uu[1] == 0) return kNumBytes16;

        return kNumBytes32;
    }

    bool BluetoothUuid::Is16Bit() const {
        return GetShortestRepresentationSize() == kNumBytes16;
    }

    uint16_t BluetoothUuid::As16Bit() const { return (((uint16_t)uu[2]) << 8) + uu[3]; }

    uint32_t BluetoothUuid::As32Bit() const {
        return (((uint32_t)uu[0]) << 24) + (((uint32_t)uu[1]) << 16) +
            (((uint32_t)uu[2]) << 8) + uu[3];
    }

    BluetoothUuid BluetoothUuid::FromString(const std::string& uuid, bool* is_valid) {
        if (is_valid) *is_valid = false;
        BluetoothUuid ret = kBase;

        if (uuid.empty()) return ret;

        uint8_t* p = ret.uu;
        if (uuid.size() == kString128BitLen) {
            char c;
            int rc =
                sscanf(uuid.c_str(),
                    "%02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx"
                    "-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%c",
                    &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7], &p[8],
                    &p[9], &p[10], &p[11], &p[12], &p[13], &p[14], &p[15], &c);
            if (rc != 16) return ret;
            if (c != '\0') return ret;

            if (is_valid) *is_valid = true;
        }
        else if (uuid.size() == 8) {
            char c;
            int rc = sscanf(uuid.c_str(), "%02hhx%02hhx%02hhx%02hhx%c", &p[0], &p[1],
                &p[2], &p[3], &c);
            if (rc != 4) return ret;
            if (c != '\0') return ret;

            if (is_valid) *is_valid = true;
        }
        else if (uuid.size() == 4) {
            char c;
            int rc = sscanf(uuid.c_str(), "%02hhx%02hhx%c", &p[2], &p[3], &c);
            if (rc != 2) return ret;
            if (c != '\0') return ret;

            if (is_valid) *is_valid = true;
        }

        return ret;
    }

    BluetoothUuid BluetoothUuid::From16Bit(uint16_t uuid16) {
        BluetoothUuid u = kBase;

        u.uu[2] = (uint8_t)((0xFF00 & uuid16) >> 8);
        u.uu[3] = (uint8_t)(0x00FF & uuid16);
        return u;
    }

    BluetoothUuid BluetoothUuid::From32Bit(uint32_t uuid32) {
        BluetoothUuid u = kBase;

        u.uu[0] = (uint8_t)((0xFF000000 & uuid32) >> 24);
        u.uu[1] = (uint8_t)((0x00FF0000 & uuid32) >> 16);
        u.uu[2] = (uint8_t)((0x0000FF00 & uuid32) >> 8);
        u.uu[3] = (uint8_t)(0x000000FF & uuid32);
        return u;
    }

    BluetoothUuid BluetoothUuid::From128BitBE(const uint8_t* uuid) {
        UUID128Bit tmp;
        memcpy(tmp.data(), uuid, kNumBytes128);
        return From128BitBE(tmp);
    }

    BluetoothUuid BluetoothUuid::From128BitLE(const UUID128Bit& uuid) {
        BluetoothUuid u;
        std::reverse_copy(uuid.data(), uuid.data() + kNumBytes128, u.uu);
        return u;
    }

    BluetoothUuid BluetoothUuid::From128BitLE(const uint8_t* uuid) {
        UUID128Bit tmp;
        memcpy(tmp.data(), uuid, kNumBytes128);
        return From128BitLE(tmp);
    }

    const UUID128Bit BluetoothUuid::To128BitLE() const {
        UUID128Bit le;
        std::reverse_copy(uu, uu + kNumBytes128, le.begin());
        return le;
    }

    const UUID128Bit BluetoothUuid::To128BitBE() const
    {
        UUID128Bit ret;
        std::copy(uu, uu + kNumBytes128, ret.begin());
        return ret;
    }

    BluetoothUuid BluetoothUuid::GetRandom() {
        BluetoothUuid uuid;
        base::RandBytes(uuid.uu, kNumBytes128);
        return uuid;
    }

    bool BluetoothUuid::IsEmpty() const { return *this == kEmpty; }

    bool BluetoothUuid::operator<(const BluetoothUuid& rhs) const {
        return std::lexicographical_compare(uu, uu + kNumBytes128, rhs.uu,
            rhs.uu + kNumBytes128);
    }

    bool BluetoothUuid::operator==(const BluetoothUuid& rhs) const { return uu == rhs.uu; }

    bool BluetoothUuid::operator!=(const BluetoothUuid& rhs) const { return uu != rhs.uu; }

    std::string BluetoothUuid::ToString() const {
        return base::StringPrintf(
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uu[0], uu[1], uu[2], uu[3], uu[4], uu[5], uu[6], uu[7], uu[8], uu[9],
            uu[10], uu[11], uu[12], uu[13], uu[14], uu[15]);
    }
