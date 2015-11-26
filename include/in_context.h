#ifndef IN_CONTEXT_H
#define IN_CONTEXT_H

#include <string>

//Forward declaration
class in_video;
class in_audio;

class in_context{
 public:
  /* init calls common ffmpeg procedures needed to setup the environment */
  in_context();

  /* open open the file name and extracts the input audio and video streams */
  int open(std::string filename);

  std::string getLastErrorMessage(){ return this->errorMessage;};
  friend std::ostream &operator<<(std::ostream &stream, in_context o);

  std::string getFileName(){ return this->filename;}

 private:
  /* get and set error messages for external use */
  std::string getErrorMessage(){ return this->errorMessage;};
  void setErrorMessage(std::string error){ this->errorMessage = error ;};

  //The input file name
  std::string filename;
  std::string errorMessage;

  in_audio *audio;
  in_video *video;
  
};


#endif
