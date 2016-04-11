#ifndef OUT_CONTEXT_H
#define OUT_CONTEXT_H

#define OUTPUT_FORMAT ".wmv" 


#include "common.h"
#include "out_stream.h"


class out_context{
 public:

  /* out_context 
     constructor
  */
  out_context( std::string );

  /* ~out_context 
     destructor
  */
  ~out_context();

  /* open 
     open the file name and extracts the input audio and video streams 
     the file name is from the prefix received in the constructor
  */
  int open();

  /* close
     closes the output audio and video stream
   */
  int close();

  /* getFileName 
     returns the name of the currently opened file.
  */
  std::string getFileName(){ return this->filename;}

  friend std::ostream &operator<<(std::ostream &stream, out_context o);

  /* saveAudioFrame 
     the input audio frame gets resampled and saved to the output stream
  */
  int saveAudioFrame(AVFrame *audioFrame, int skippedFrames){ return this->audio->saveFrame(audioFrame, this->av_format_context, skippedFrames);}

  /* saveVideoFrame
     the input video frame gets scaled and saved to the output stream
  */
  int saveVideoFrame(AVFrame *videoFrame, int skippedFrames){ return this->video->saveFrame(videoFrame, this->av_format_context, skippedFrames);}

  /* cur_video_dts 
     return the video dts, tipically is time stamp of the last frame expres in 1/1000 seconds
  */
  int64_t cur_video_dts(){ return this->video->cur_dts();};

  /* cur_audio_dts 
     return the audio dts, tipically is time stamp of the last frame expres in 1/1000 seconds
  */
  int64_t cur_audio_dts(){ return this->audio->cur_dts();};

  /* initAudioProcessor
     the audio resampler needs some initialization activity
  */
  int initAudioProcessor(AVCodecContext *input_audio_codec_ctx){ return this->audio->init_processor(input_audio_codec_ctx);}

  /* initVideoProcessor
     the video rescaler needs some initialization activity
  */
  int initVideoProcessor(AVCodecContext *input_video_codec_ctx){ return this->video->init_processor(input_video_codec_ctx);}

  /* isBlackFrame
     returns 1 if the current video frame is black
  */
  int isBlackFrame(){ if(this->video->isBlackFrame()){return 1;}else{return 0;}};

  /* isSilentFrame
     returns 1 if the current audio frame is silent
  */
  int isSilentFrame(){ if(this->audio->isSilentFrame()){return 1;}else{return 0;}};

  /* getAudioVolume 
     returns the audio volume
   */
  int getAudioVolume(){ return this->audio->getVolume();};

  // the progressive file counter name
  int    fileCounter;
  
  // the prefix for the output files
  std::string filePrefix;

  errorClass *error;

 private:

  /* the constructor which we don't want */
  out_context(){};

  //The input file name
  std::string filename;

  //a flag, tell us if the stream is open
  int is_open;


  // The container for the audio video output streams
  AVFormatContext *av_format_context;

  out_stream *audio;
  out_stream *video;

};

#endif
