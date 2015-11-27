#include "out_stream_wmav2.h"

int out_stream_wmav2::setup_codec(){
      AVRational  time_base;
      AVCodecContext *codec_ctx;

      time_base.num   = 1 ;
      time_base.den   = AUDIO_OUT_SAMPLE_RATE ;

      this->stream->time_base = time_base; //for video frames is 1/framerate

      //Populate the video stream video codec_context with our options
      codec_ctx             = this->stream->codec;

      //Populate the Audio codec context
      codec_ctx->codec_type  = AVMEDIA_TYPE_AUDIO;
      codec_ctx->codec_id    = AUDIO_OUT_CODEC_ID;
      codec_ctx->sample_rate = AUDIO_OUT_SAMPLE_RATE;
      codec_ctx->bit_rate    = AUDIO_OUT_BITRATE;
      codec_ctx->channels    = AUDIO_OUT_CHANNELS; //Number of channels
      codec_ctx->channel_layout = av_get_default_channel_layout(AUDIO_OUT_CHANNELS); //stereo
      codec_ctx->sample_fmt  = AUDIO_OUT_SAMPLE_FMT; //samples format

      return 1;
  }
  

