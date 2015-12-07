#ifndef IN_CONTEXT_H
#define IN_CONTEXT_H

#include <string>
#include "in_stream.h"

class in_context{
 public:
  /* init calls common ffmpeg procedures needed to setup the environment */
  in_context();
  ~in_context();

  /* open open the file name and extracts the input audio and video streams */
  int open(std::string filename);

  std::string getLastErrorMessage(){ return this->errorMessage;};
  friend std::ostream &operator<<(std::ostream &stream, in_context o);

  std::string getFileName(){ return this->filename;}

  //in_stream * getAudioStream(){return this->audio;}
  //in_stream * getVideoStream(){return this->video;}

  int readAudioFrame(){return this->audio->readFrame();}
  int readVideoFrame(){return this->video->readFrame();}

  int hasAudioFrame(){return this->audio->hasFrame();}
  int hasVideoFrame(){return this->video->hasFrame();}

  void resetAudioFrame(){return this->audio->resetFrame();}
  void resetVideoFrame(){return this->video->resetFrame();}

  AVFrame * getAudioFrame(){return this->audio->getFrame();}
  AVFrame * getVideoFrame(){return this->video->getFrame();}

  AVCodecContext * getAudioCodecContext(){return this->audio->getCodecContext();}
  AVCodecContext * getVideoCodecContext(){return this->video->getCodecContext();}

 private:
  /* get and set error messages for external use */
  std::string getErrorMessage(){ return this->errorMessage;};
  void setErrorMessage(std::string error){ this->errorMessage = error ;};

  //The input file name
  std::string filename;
  std::string errorMessage;

  in_stream *audio;
  in_stream *video;
  
};


#endif
