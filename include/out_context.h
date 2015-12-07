#ifndef OUT_CONTEXT_H
#define OUT_CONTEXT_H

#define OUTPUT_FORMAT ".wmv" 

#include <string>
#include <iostream>

extern "C"{
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include "out_stream.h"

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

  out_stream * getAudioStream(){return this->audio;}
  out_stream * getVideoStream(){return this->video;}

  int saveAudioFrame(AVFrame *audioFrame){ return this->audio->saveFrame(audioFrame, this->av_format_context);}
  int saveVideoFrame(AVFrame *videoFrame){ return this->video->saveFrame(videoFrame, this->av_format_context);}

  int64_t cur_video_dts(){ return this->video->cur_dts();};
  int64_t cur_audio_dts(){ return this->audio->cur_dts();};

  int initAudioProcessor(AVCodecContext *input_audio_codec_ctx){ return this->audio->init_processor(input_audio_codec_ctx);}
  int initVideoProcessor(AVCodecContext *input_video_codec_ctx){ return this->video->init_processor(input_video_codec_ctx);}


  std::string getErrorMessage(){ return this->errorMessage;};

 private:
  /* get and set error messages for external use */

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

  // We need to count the skipped  frames, in order to sync audio and video
  int         video_frames_skipped;


  // The container for the audio video output streams
  AVFormatContext *av_format_context;

  out_stream *audio;
  out_stream *video;

};

#endif
