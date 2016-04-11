#ifndef IN_CONTEXT_H
#define IN_CONTEXT_H

#include "in_stream.h"
#include "common.h"


class in_context{
 public:

  in_context();
  ~in_context();

  /* open 
     open the input filename and extracts the input audio and video streams 
  */
  int open(std::string filename);

  /* getFileName
     return the filename currently opened 
  */
  std::string getFileName(){ return this->filename;}

  /* operator<<
     friend class for printing the contents of the input context
  */
  
  friend std::ostream &operator<<(std::ostream &stream, in_context o);

  /* Read a frame from the input streams */
  int readAudioFrame(){return this->audio->readFrame();}
  int readVideoFrame(){return this->video->readFrame();}

  /* Tell if there is a frame waiting */
  int hasAudioFrame(){return this->audio->hasFrame();}
  int hasVideoFrame(){return this->video->hasFrame();}

  /* Clean the input Frame before reading another one */
  void resetAudioFrame(){return this->audio->resetFrame();}
  void resetVideoFrame(){return this->video->resetFrame();}

  /* Gets the frame for the data processing */
  AVFrame * getAudioFrame(){return this->audio->getFrame();}
  AVFrame * getVideoFrame(){return this->video->getFrame();}

  /* Get the codec context - used for video rescaling and audio resampling */
  AVCodecContext * getAudioCodecContext(){return this->audio->getCodecContext();}
  AVCodecContext * getVideoCodecContext(){return this->video->getCodecContext();}

  int skippedVideoFrames(){ return this->video->skippedFrames();}
  int skippedAudioFrames(){ return this->audio->skippedFrames();}

  errorClass *error;
 private:
  
  //The input file name
  std::string filename;

  in_stream *audio;
  in_stream *video;
  
};


#endif
