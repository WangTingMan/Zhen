
#ifndef BluetoothCodecConfig_hpp_
#define BluetoothCodecConfig_hpp_

#include <string>

/**
 * Represents the codec configuration for a Bluetooth A2DP source device.
 *
 * {@see BluetoothA2dp}
 *
 * {@hide}
 */
class BluetoothCodecConfig
{

public:

    // Add an entry for each source codec here.
    // NOTE: The values should be same as those listed in the following file:
    //   hardware/libhardware/include/hardware/bt_av.h

    /** @hide */
    enum SourceCodecType
    {
        SOURCE_CODEC_TYPE_SBC = 0,
        SOURCE_CODEC_TYPE_AAC = 1,
        SOURCE_CODEC_TYPE_APTX = 2,
        SOURCE_CODEC_TYPE_APTX_HD = 3,
        SOURCE_CODEC_TYPE_LDAC = 4,
        SOURCE_CODEC_TYPE_MAX = 5,
        SOURCE_CODEC_TYPE_INVALID = 1000 * 1000
    };

    enum CodecPriority
    {
        CODEC_PRIORITY_DISABLED = -1,
        CODEC_PRIORITY_DEFAULT = 0,
        CODEC_PRIORITY_HIGHEST = 1000 * 1000
    };

    enum SampleRate
    {
        SAMPLE_RATE_NONE = 0,
        SAMPLE_RATE_44100 = 0x1 << 0,
        SAMPLE_RATE_48000 = 0x1 << 1,
        SAMPLE_RATE_88200 = 0x1 << 2,
        SAMPLE_RATE_96000 = 0x1 << 3,
        SAMPLE_RATE_176400 = 0x1 << 4,
        SAMPLE_RATE_192000 = 0x1 << 5
    };

    enum BitsPerSample
    {
        BITS_PER_SAMPLE_NONE = 0,
        BITS_PER_SAMPLE_16 = 0x1 << 0,
        BITS_PER_SAMPLE_24 = 0x1 << 1,
        BITS_PER_SAMPLE_32 = 0x1 << 2
    };

    enum ChannelMode
    {
        CHANNEL_MODE_NONE = 0,
        CHANNEL_MODE_MONO = 0x1 << 0,
        CHANNEL_MODE_STEREO = 0x1 << 1
    };

private:

    SourceCodecType  mCodecType;
    CodecPriority mCodecPriority;
    SampleRate mSampleRate;
    BitsPerSample mBitsPerSample;
    ChannelMode mChannelMode;
    long mCodecSpecific1;
    long mCodecSpecific2;
    long mCodecSpecific3;
    long mCodecSpecific4;

public:

    BluetoothCodecConfig( SourceCodecType codecType, CodecPriority codecPriority,
            SampleRate sampleRate, BitsPerSample bitsPerSample,
            ChannelMode channelMode, long codecSpecific1,
            long codecSpecific2, long codecSpecific3,
            long codecSpecific4 )
    {
        mCodecType = codecType;
        mCodecPriority = codecPriority;
        mSampleRate = sampleRate;
        mBitsPerSample = bitsPerSample;
        mChannelMode = channelMode;
        mCodecSpecific1 = codecSpecific1;
        mCodecSpecific2 = codecSpecific2;
        mCodecSpecific3 = codecSpecific3;
        mCodecSpecific4 = codecSpecific4;
    }

    void Assign( SourceCodecType codecType, CodecPriority codecPriority,
        SampleRate sampleRate, BitsPerSample bitsPerSample,
        ChannelMode channelMode, long codecSpecific1,
        long codecSpecific2, long codecSpecific3,
        long codecSpecific4 )
    {
        mCodecType = codecType;
        mCodecPriority = codecPriority;
        mSampleRate = sampleRate;
        mBitsPerSample = bitsPerSample;
        mChannelMode = channelMode;
        mCodecSpecific1 = codecSpecific1;
        mCodecSpecific2 = codecSpecific2;
        mCodecSpecific3 = codecSpecific3;
        mCodecSpecific4 = codecSpecific4;
    }

    BluetoothCodecConfig( SourceCodecType codecType = SOURCE_CODEC_TYPE_INVALID )
    {
        mCodecType = codecType;
        mCodecPriority = CODEC_PRIORITY_DEFAULT;
        mSampleRate = SAMPLE_RATE_NONE;
        mBitsPerSample = BITS_PER_SAMPLE_NONE;
        mChannelMode = CHANNEL_MODE_NONE;
        mCodecSpecific1 = 0;
        mCodecSpecific2 = 0;
        mCodecSpecific3 = 0;
        mCodecSpecific4 = 0;
    }

    BluetoothCodecConfig( BluetoothCodecConfig const& other );

    BluetoothCodecConfig& operator=( BluetoothCodecConfig const& other );

    void CopyFrom( BluetoothCodecConfig const& other );

    void clear()
    {
        mCodecType = SOURCE_CODEC_TYPE_INVALID;
        mCodecPriority = CODEC_PRIORITY_DISABLED;
        mSampleRate = SAMPLE_RATE_NONE;
        mBitsPerSample = BITS_PER_SAMPLE_NONE;
        mChannelMode = CHANNEL_MODE_NONE;
        mCodecSpecific1 = 0;
        mCodecSpecific2 = 0;
        mCodecSpecific3 = 0;
        mCodecSpecific4 = 0;
    }

    bool equals( BluetoothCodecConfig const& other )
    {
        return ( other.mCodecType == mCodecType
            && other.mCodecPriority == mCodecPriority
            && other.mSampleRate == mSampleRate
            && other.mBitsPerSample == mBitsPerSample
            && other.mChannelMode == mChannelMode
            && other.mCodecSpecific1 == mCodecSpecific1
            && other.mCodecSpecific2 == mCodecSpecific2
            && other.mCodecSpecific3 == mCodecSpecific3
            && other.mCodecSpecific4 == mCodecSpecific4 );
    }

    /**
     * Returns a hash based on the config values
     *
     * @return a hash based on the config values
     * @hide
     */
    int hashCode()
    {
        auto ret = std::hash<std::string>{}( toString() );
        return ret;
    }

    /**
     * Checks whether the object contains valid codec configuration.
     *
     * @return true if the object contains valid codec configuration, otherwise false.
     * @hide
     */
    bool isValid()
    {
        return ( mSampleRate != SAMPLE_RATE_NONE )
            && ( mBitsPerSample != BITS_PER_SAMPLE_NONE )
            && ( mChannelMode != CHANNEL_MODE_NONE );
    }

private:

    /**
     * Adds capability string to an existing string.
     *
     * @param prevStr the previous string with the capabilities. Can be a null pointer.
     * @param capStr the capability string to append to prevStr argument.
     * @return the result string in the form "prevStr|capStr".
     */
    static std::string appendCapabilityToString( std::string prevStr,
        std::string capStr );

public:

    std::string toString();

    /**
     * Gets the codec name.
     *
     * @return the codec name
     */
    std::string getCodecName();

    /**
     * Gets the codec type.
     * See {@link android.bluetooth.BluetoothCodecConfig#SOURCE_CODEC_TYPE_SBC}.
     *
     * @return the codec type
     */int getCodecType()
    {
        return mCodecType;
    }

    /**
     * Checks whether the codec is mandatory.
     *
     * @return true if the codec is mandatory, otherwise false.
     */
    bool isMandatoryCodec()
    {
        return mCodecType == SOURCE_CODEC_TYPE_SBC;
    }

    /**
     * Gets the codec selection priority.
     * The codec selection priority is relative to other codecs: larger value
     * means higher priority. If 0, reset to default.
     *
     * @return the codec priority
     */
    int getCodecPriority()
    {
        return mCodecPriority;
    }

    /**
     * Sets the codec selection priority.
     * The codec selection priority is relative to other codecs: larger value
     * means higher priority. If 0, reset to default.
     *
     * @param codecPriority the codec priority
     * @hide
     */
    void setCodecPriority( CodecPriority codecPriority )
    {
        mCodecPriority = codecPriority;
    }

    /**
     * Gets the codec sample rate. The value can be a bitmask with all
     * supported sample rates:
     * {@link android.bluetooth.BluetoothCodecConfig#SAMPLE_RATE_NONE} or
     * {@link android.bluetooth.BluetoothCodecConfig#SAMPLE_RATE_44100} or
     * {@link android.bluetooth.BluetoothCodecConfig#SAMPLE_RATE_48000} or
     * {@link android.bluetooth.BluetoothCodecConfig#SAMPLE_RATE_88200} or
     * {@link android.bluetooth.BluetoothCodecConfig#SAMPLE_RATE_96000} or
     * {@link android.bluetooth.BluetoothCodecConfig#SAMPLE_RATE_176400} or
     * {@link android.bluetooth.BluetoothCodecConfig#SAMPLE_RATE_192000}
     *
     * @return the codec sample rate
     */
    SampleRate getSampleRate()
    {
        return mSampleRate;
    }

    /**
     * Gets the codec bits per sample. The value can be a bitmask with all
     * bits per sample supported:
     * {@link android.bluetooth.BluetoothCodecConfig#BITS_PER_SAMPLE_NONE} or
     * {@link android.bluetooth.BluetoothCodecConfig#BITS_PER_SAMPLE_16} or
     * {@link android.bluetooth.BluetoothCodecConfig#BITS_PER_SAMPLE_24} or
     * {@link android.bluetooth.BluetoothCodecConfig#BITS_PER_SAMPLE_32}
     *
     * @return the codec bits per sample
     */
    BitsPerSample getBitsPerSample()
    {
        return mBitsPerSample;
    }

    /**
     * Gets the codec channel mode. The value can be a bitmask with all
     * supported channel modes:
     * {@link android.bluetooth.BluetoothCodecConfig#CHANNEL_MODE_NONE} or
     * {@link android.bluetooth.BluetoothCodecConfig#CHANNEL_MODE_MONO} or
     * {@link android.bluetooth.BluetoothCodecConfig#CHANNEL_MODE_STEREO}
     *
     * @return the codec channel mode
     * @hide
     */
    ChannelMode getChannelMode()
    {
        return mChannelMode;
    }

    /**
     * Gets a codec specific value1.
     *
     * @return a codec specific value1.
     */
    long getCodecSpecific1()
    {
        return mCodecSpecific1;
    }

    /**
     * Gets a codec specific value2.
     *
     * @return a codec specific value2
     * @hide
     */
    long getCodecSpecific2()
    {
        return mCodecSpecific2;
    }

    /**
     * Gets a codec specific value3.
     *
     * @return a codec specific value3
     * @hide
     */
    long getCodecSpecific3()
    {
        return mCodecSpecific3;
    }

    /**
     * Gets a codec specific value4.
     *
     * @return a codec specific value4
     * @hide
     */
    long getCodecSpecific4()
    {
        return mCodecSpecific4;
    }

private:

    /**
     * Checks whether a value set presented by a bitmask has zero or single bit
     *
     * @param valueSet the value set presented by a bitmask
     * @return true if the valueSet contains zero or single bit, otherwise false.
     * @hide
     */
    static bool hasSingleBit( int valueSet );

public:

    /**
     * Checks whether the object contains none or single sample rate.
     *
     * @return true if the object contains none or single sample rate, otherwise false.
     * @hide
     */
    bool hasSingleSampleRate();

    /**
     * Checks whether the object contains none or single bits per sample.
     *
     * @return true if the object contains none or single bits per sample, otherwise false.
     * @hide
     */
    bool hasSingleBitsPerSample();

    /**
     * Checks whether the object contains none or single channel mode.
     *
     * @return true if the object contains none or single channel mode, otherwise false.
     * @hide
     */
    bool hasSingleChannelMode();

    /**
     * Checks whether the audio feeding parameters are same.
     *
     * @param other the codec config to compare against
     * @return true if the audio feeding parameters are same, otherwise false
     * @hide
     */
    bool sameAudioFeedingParameters( BluetoothCodecConfig other );

    /**
     * Checks whether another codec config has the similar feeding parameters.
     * Any parameters with NONE value will be considered to be a wildcard matching.
     *
     * @param other the codec config to compare against
     * @return true if the audio feeding parameters are similar, otherwise false.
     * @hide
     */
    bool similarCodecFeedingParameters( BluetoothCodecConfig other );

    /**
     * Checks whether the codec specific parameters are the same.
     *
     * @param other the codec config to compare against
     * @return true if the codec specific parameters are the same, otherwise false.
     * @hide
     */
    bool sameCodecSpecificParameters( BluetoothCodecConfig other );
};

#endif

