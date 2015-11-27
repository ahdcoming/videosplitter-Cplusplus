#ifndef OUT_CONTEXT_H
#define OUT_CONTEXT_H

#define OUTPUT_FORMAT     ".wmv" 

#include <string>
#include <iostream>

extern "C"{
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

class out_stream;

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

  out_stream *audio;
  out_stream *video;

};

#endif
