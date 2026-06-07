#include "BluetoothCodecConfig.hpp"

BluetoothCodecConfig::BluetoothCodecConfig( BluetoothCodecConfig const& other )
{
    CopyFrom( other );
}

void BluetoothCodecConfig::CopyFrom( BluetoothCodecConfig const& other )
{
    mCodecType = other.mCodecType;
    mCodecPriority = other.mCodecPriority;
    mSampleRate = other.mSampleRate;
    mBitsPerSample = other.mBitsPerSample;
    mChannelMode = other.mChannelMode;
    mCodecSpecific1 = other.mCodecSpecific1;
    mCodecSpecific2 = other.mCodecSpecific2;
    mCodecSpecific3 = other.mCodecSpecific3;
    mCodecSpecific4 = other.mCodecSpecific4;
}

BluetoothCodecConfig& BluetoothCodecConfig::operator=( BluetoothCodecConfig const& other )
{
    CopyFrom( other );
    return *this;
}

/**
 * Checks whether a value set presented by a bitmask has zero or single bit
 *
 * @param valueSet the value set presented by a bitmask
 * @return true if the valueSet contains zero or single bit, otherwise false.
 * @hide
 */
 bool BluetoothCodecConfig::hasSingleBit( int valueSet )
{
    return ( valueSet == 0 || ( valueSet & ( valueSet - 1 ) ) == 0 );
}

    /**
     * Checks whether the object contains none or single sample rate.
     *
     * @return true if the object contains none or single sample rate, otherwise false.
     * @hide
     */
    bool BluetoothCodecConfig::hasSingleSampleRate()
    {
        return hasSingleBit( mSampleRate );
    }

    /**
     * Checks whether the object contains none or single bits per sample.
     *
     * @return true if the object contains none or single bits per sample, otherwise false.
     * @hide
     */
    bool BluetoothCodecConfig::hasSingleBitsPerSample()
    {
        return hasSingleBit( mBitsPerSample );
    }

    /**
     * Checks whether the object contains none or single channel mode.
     *
     * @return true if the object contains none or single channel mode, otherwise false.
     * @hide
     */
    bool BluetoothCodecConfig::hasSingleChannelMode()
    {
        return hasSingleBit( mChannelMode );
    }

    /**
     * Checks whether the audio feeding parameters are same.
     *
     * @param other the codec config to compare against
     * @return true if the audio feeding parameters are same, otherwise false
     * @hide
     */
    bool BluetoothCodecConfig::sameAudioFeedingParameters( BluetoothCodecConfig other )
    {
        return ( other.mSampleRate == mSampleRate
            && other.mBitsPerSample == mBitsPerSample
            && other.mChannelMode == mChannelMode );
    }

    /**
     * Checks whether another codec config has the similar feeding parameters.
     * Any parameters with NONE value will be considered to be a wildcard matching.
     *
     * @param other the codec config to compare against
     * @return true if the audio feeding parameters are similar, otherwise false.
     * @hide
     */
    bool BluetoothCodecConfig::similarCodecFeedingParameters( BluetoothCodecConfig other )
    {
        if( mCodecType != other.mCodecType )
        {
            return false;
        }
        int sampleRate = other.mSampleRate;
        if( mSampleRate == SAMPLE_RATE_NONE
            || sampleRate == SAMPLE_RATE_NONE )
        {
            sampleRate = mSampleRate;
        }
        int bitsPerSample = other.mBitsPerSample;
        if( mBitsPerSample == BITS_PER_SAMPLE_NONE
            || bitsPerSample == BITS_PER_SAMPLE_NONE )
        {
            bitsPerSample = mBitsPerSample;
        }
        int channelMode = other.mChannelMode;
        if( mChannelMode == CHANNEL_MODE_NONE
            || channelMode == CHANNEL_MODE_NONE )
        {
            channelMode = mChannelMode;
        }

        return sameAudioFeedingParameters( BluetoothCodecConfig(
            mCodecType, /* priority */ ( CodecPriority)0, ( SampleRate )sampleRate, ( BitsPerSample )bitsPerSample,
            ( ChannelMode )channelMode,
            /* specific1 */ 0, /* specific2 */ 0, /* specific3 */ 0,
            /* specific4 */ 0 ) );
    }

    /**
     * Checks whether the codec specific parameters are the same.
     *
     * @param other the codec config to compare against
     * @return true if the codec specific parameters are the same, otherwise false.
     * @hide
     */
    bool BluetoothCodecConfig::sameCodecSpecificParameters( BluetoothCodecConfig other )
    {
        if( mCodecType != other.mCodecType )
        {
            return false;
        }
        // Currently we only care about the AAC VBR and LDAC Playback Quality at CodecSpecific1
        switch( mCodecType )
        {
        case SOURCE_CODEC_TYPE_AAC:
        case SOURCE_CODEC_TYPE_LDAC:
            if( mCodecSpecific1 != other.mCodecSpecific1 )
            {
                return false;
            }
            // fall through
        default:
            return true;
        }
    }

    std::string BluetoothCodecConfig::appendCapabilityToString( std::string prevStr,
        std::string capStr )
    {
        if( prevStr.empty() )
        {
            return capStr;
        }
        std::string ret;
        ret.append( prevStr ).append( "|" ).append( capStr );
        return ret;
    }

    std::string BluetoothCodecConfig::toString()
    {
        std::string sampleRateStr;
        if( mSampleRate == SAMPLE_RATE_NONE )
        {
            sampleRateStr = appendCapabilityToString( sampleRateStr, "NONE" );
        }
        if( ( mSampleRate & SAMPLE_RATE_44100 ) != 0 )
        {
            sampleRateStr = appendCapabilityToString( sampleRateStr, "44100" );
        }
        if( ( mSampleRate & SAMPLE_RATE_48000 ) != 0 )
        {
            sampleRateStr = appendCapabilityToString( sampleRateStr, "48000" );
        }
        if( ( mSampleRate & SAMPLE_RATE_88200 ) != 0 )
        {
            sampleRateStr = appendCapabilityToString( sampleRateStr, "88200" );
        }
        if( ( mSampleRate & SAMPLE_RATE_96000 ) != 0 )
        {
            sampleRateStr = appendCapabilityToString( sampleRateStr, "96000" );
        }
        if( ( mSampleRate & SAMPLE_RATE_176400 ) != 0 )
        {
            sampleRateStr = appendCapabilityToString( sampleRateStr, "176400" );
        }
        if( ( mSampleRate & SAMPLE_RATE_192000 ) != 0 )
        {
            sampleRateStr = appendCapabilityToString( sampleRateStr, "192000" );
        }

        std::string bitsPerSampleStr;
        if( mBitsPerSample == BITS_PER_SAMPLE_NONE )
        {
            bitsPerSampleStr = appendCapabilityToString( bitsPerSampleStr, "NONE" );
        }
        if( ( mBitsPerSample & BITS_PER_SAMPLE_16 ) != 0 )
        {
            bitsPerSampleStr = appendCapabilityToString( bitsPerSampleStr, "16" );
        }
        if( ( mBitsPerSample & BITS_PER_SAMPLE_24 ) != 0 )
        {
            bitsPerSampleStr = appendCapabilityToString( bitsPerSampleStr, "24" );
        }
        if( ( mBitsPerSample & BITS_PER_SAMPLE_32 ) != 0 )
        {
            bitsPerSampleStr = appendCapabilityToString( bitsPerSampleStr, "32" );
        }

        std::string channelModeStr;
        if( mChannelMode == CHANNEL_MODE_NONE )
        {
            channelModeStr = appendCapabilityToString( channelModeStr, "NONE" );
        }
        if( ( mChannelMode & CHANNEL_MODE_MONO ) != 0 )
        {
            channelModeStr = appendCapabilityToString( channelModeStr, "MONO" );
        }
        if( ( mChannelMode & CHANNEL_MODE_STEREO ) != 0 )
        {
            channelModeStr = appendCapabilityToString( channelModeStr, "STEREO" );
        }

        std::string ret;
        ret.reserve( 500 );
        ret += "{codecName:" + getCodecName()
            + ",mCodecType:" + std::to_string( mCodecType )
            + ",mCodecPriority:" + std::to_string( mCodecPriority )
            + ",mSampleRate:" + std::to_string( mSampleRate )
            + "(" + sampleRateStr + ")"
            + ",mBitsPerSample:" + std::to_string( mBitsPerSample )
            + "(" + bitsPerSampleStr + ")"
            + ",mChannelMode:" + std::to_string( mChannelMode )
            + "(" + channelModeStr + ")"
            + ",mCodecSpecific1:" + std::to_string( mCodecSpecific1 )
            + ",mCodecSpecific2:" + std::to_string( mCodecSpecific2 )
            + ",mCodecSpecific3:" + std::to_string( mCodecSpecific3 )
            + ",mCodecSpecific4:" + std::to_string( mCodecSpecific4 ) + "}";
        return ret;
    }

    std::string BluetoothCodecConfig::getCodecName()
    {
        switch( mCodecType )
        {
        case SOURCE_CODEC_TYPE_SBC:
            return "SBC";
        case SOURCE_CODEC_TYPE_AAC:
            return "AAC";
        case SOURCE_CODEC_TYPE_APTX:
            return "aptX";
        case SOURCE_CODEC_TYPE_APTX_HD:
            return "aptX HD";
        case SOURCE_CODEC_TYPE_LDAC:
            return "LDAC";
        case SOURCE_CODEC_TYPE_INVALID:
            return "INVALID CODEC";
        default:
            break;
        }
        return "UNKNOWN CODEC(" + std::to_string( mCodecType ) + ")";
    }

