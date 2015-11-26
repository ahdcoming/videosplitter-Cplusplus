#ifndef OUT_CONTEXT_H
#define OUT_CONTEXT_H

#define OUTPUT_FORMAT     ".wmv" 
#define VIDEO_OUT_CODEC_ID    AV_CODEC_ID_WMV2 
#define VIDEO_OUT_WIDTH   320
#define VIDEO_OUT_HEIGHT  240
#define VIDEO_OUT_BITRATE 7500000
#define VIDEO_OUT_PIX_FMT AV_PIX_FMT_YUV420P
#define VIDEO_OUT_FRAMERATE_NUM 25
#define VIDEO_OUT_FRAMERATE_DEN 1

#define AUDIO_VIDEO_MAX_DELAY 1

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

//This works, but I don't know why.
#define MAGIC_NUMBER_ALPHA 2

#define DEBUG 1

#define INTERLEAVED 1


#include <string>
#include <iostream>

extern "C"{
  #include <libavutil/avutil.h>
  #include <libavformat/avformat.h>
}

class out_context{
 public:
  /* init calls common ffmpeg procedures needed to setup the environment */
  out_context();
  ~out_context();

  /* open open the file name and extracts the input audio and video streams */
  int open(std::string filename);
  int close();
  std::string getFileName(){ return this->filename;}

  std::string getLastErrorMessage(){ return this->errorMessage;};
  friend std::ostream &operator<<(std::ostream &stream, out_context o);

 private:
  /* get and set error messages for external use */
  std::string getErrorMessage(){ return this->errorMessage;};
  void setErrorMessage(std::string error){ this->errorMessage = error ;};

  //The input file name
  std::string filename;
  std::string errorMessage;

  //a flag, tell us if the stream is open
  int is_open;

  // How many samples have we processed until now?
  // Note Samples, not frames
  // Counting the number of audio samples is our key to keep audio and video in sinc
  int64_t      audio_samples_count;

  // We need to count the skipped video frames, in order to sync audio and video
  int         video_frames_skipped;


  // The container for the audio video output streams
  AVFormatContext *av_format_context;


};

#endif
