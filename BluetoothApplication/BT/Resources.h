#pragma once

class Resources
{

public:

    enum
    {
        a2dp_source_codec_priority_sbc = 0,
        a2dp_source_codec_priority_aac,
        a2dp_source_codec_priority_aptx,
        a2dp_source_codec_priority_aptx_hd,
        a2dp_source_codec_priority_ldac
    };

public:

    static int GetResourceAsInt( int a_type );

};

