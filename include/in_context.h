#ifndef IN_CONTEXT_H
#define IN_CONTEXT_H

#include <string>
#include "in_audio.h"
#include "in_video.h"


class in_context{
 public:
  /* init calls common ffmpeg procedures needed to setup the environment */
  in_context();

  /* open open the file name and extracts the input audio and video streams */
  int open(std::string filename);

  /* get and set error messages for external use */
  std::string getLastErrorMessage(){ return this->errorMessage;};
  void setLastErrorMessage(std::string error){ this->errorMessage = error ;};

  //A flag, used once in the init_stream
  static int initialized;

  friend std::ostream &operator<<(std::ostream &stream, in_context o);

 private:
  //The input file name
  std::string filename;
  std::string errorMessage;

  in_audio *audio;
  in_video *video;
  
};


#endif
