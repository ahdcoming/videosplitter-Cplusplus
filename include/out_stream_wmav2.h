#ifndef OUT_STREAM_WMAV2_H
#define OUT_STREAM_WMAV2_H

#include <string>
#include "common.h"
#include "out_stream.h"

#define AUDIO_OUT_CODEC_ID    AV_CODEC_ID_WMAV2
#define AUDIO_OUT_SAMPLE_FMT  AV_SAMPLE_FMT_FLTP
#define AUDIO_OUT_CHANNELS    2

/*Audio Sample Rate - Bit Rate and samples per frame are strongly related
  You can't choose them as freely as you wish - TODO: explain and adapt these numbers
  The values choosen are meant to avoid extra buffering during the audio conversion
*/

#define AUDIO_OUT_BITRATE     128000
#define AUDIO_OUT_SAMPLE_RATE 48000
#define AUDIO_OUT_SAMPLES_PER_FRAME 1000

class out_stream_wmav2:public out_stream {
  
public:
  out_stream_wmav2():out_stream(AV_CODEC_ID_WMAV2){};

private:	    

  int setup_codec();
  
};

#endif
