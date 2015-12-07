#ifndef OUT_STREAM_WMV2_H
#define OUT_STREAM_WMV2_H

#include <string>
#include "common.h"
#include "out_stream.h"


#define VIDEO_OUT_CODEC_ID    AV_CODEC_ID_WMV2 
#define VIDEO_OUT_WIDTH   320
#define VIDEO_OUT_HEIGHT  240
#define VIDEO_OUT_BITRATE 7500000
#define VIDEO_OUT_PIX_FMT AV_PIX_FMT_YUV420P
#define VIDEO_OUT_FRAMERATE_NUM 25
#define VIDEO_OUT_FRAMERATE_DEN 1

class out_stream_wmv2:public out_stream {
  
public:
  out_stream_wmv2():out_stream(AV_CODEC_ID_WMV2){}

 protected:

  /* virtuals */
  /* push a frame for writing */
  int saveFrame(AVFrame*, AVFormatContext*);

  /* setup codec, virtual, depends of the codec */
  int setup_codec();
  int init_processor(AVCodecContext *);


 private:	    

  /*for video streams*/
  int initScaler(AVCodecContext *);
  int scaleFrame(AVFrame *pFrameIn);

  AVFrame *pFrame;
  struct SwsContext *pRescale_ctx;
  int frames_skipped;
  
};

#endif
