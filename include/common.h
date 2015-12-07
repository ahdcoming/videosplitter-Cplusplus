#ifndef COMMON_H
#define COMMON_H

#define DEBUG 1
#define AUDIO_VIDEO_MAX_DELAY 1
#define INTERLEAVED   1 

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h> 
#include <libswscale/swscale.h> 
}

class common{

 public:
//Useful procedure for error handling and reporting
static const char *get_error_text(const int error){
  static char error_buffer[255];
  av_strerror(error, error_buffer, sizeof(error_buffer));
  return error_buffer;
 }

};

#endif
