#include "out_stream_wmv2.h"

int out_stream_wmv2::setup_codec(){
      AVRational  time_base;
      AVCodecContext *codec_ctx;

      time_base.num = VIDEO_OUT_FRAMERATE_DEN ;
      time_base.den = VIDEO_OUT_FRAMERATE_NUM ;

      //Populate the video stream video codec_context with our options
      codec_ctx             = this->stream->codec;

      this->stream->time_base = time_base; //for video frames is 1/framerate
      codec_ctx->time_base    = time_base; //for video frames is 1/framerate

      codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
      codec_ctx->codec_id   = VIDEO_OUT_CODEC_ID ;
      codec_ctx->bit_rate   = VIDEO_OUT_BITRATE;
      codec_ctx->width      = VIDEO_OUT_WIDTH;
      codec_ctx->height     = VIDEO_OUT_HEIGHT;
      codec_ctx->pix_fmt    = VIDEO_OUT_PIX_FMT;

      codec_ctx->gop_size = 12;
      codec_ctx->mb_decision = 1;
      
      return 1;
  }
  

