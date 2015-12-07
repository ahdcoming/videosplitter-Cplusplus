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

    void set_target_dts(uint64_t dts){ this->target_dts = dts;};

 protected:
  /* virtuals */
  /* push a frame for writing */
  int saveFrame(AVFrame*, AVFormatContext*);

  /* setup codec, virtual, depends of the codec */
  int setup_codec();
  int init_processor(AVCodecContext *);

private:	    
  
  //Audio resampling needs TWO frames in order to work
  AVFrame *pFrame;
  AVFrame *pFrameCodec;

  // How many samples have we processed until now?
  // Note Samples, not frames
  int64_t      samples_count;
  
  // The audio samples fifo
  /* Processing video frames is easy: one frame enters, one frame exits
     Processing audio frames is a messy thing
     A frame enters, his samples are extracted, and stored away.
     When you have enough samples, you will repack them in a new frame and
     send them to the codec.
     The Codec, tipically, wants a specific number of samples, which is not the
     one you get with every audio frame. This is true even in you are converting
     from and to the SAME format/bitrate/etc.
     So you will need an AVAudioFifo wich  will hold the samples queue
  */
  AVAudioFifo *samples_fifo ;

  // Resampling context for the audio resampling.
  // Resampling audio between formats is SLOW
  // When converting you will tipycally try to keep the same AUDIO sampling rate.
  SwrContext *resample_ctx;

  /*for audio frames*/
  int initResampler(AVCodecContext *);
  int resampleFrame(AVFrame *);

  uint8_t target_dts;
  
};

#endif
