#include "in_context.h"
#include "in_stream.h"
#include "in_audio.h"
#include "in_video.h"

/*Constructor
*/
in_context::in_context(){
  
  this->audio = new in_audio();
  this->video = new in_video();
}

in_context::~in_context(){
}

int in_context::open(std::string filename){
  this->filename = filename;

  int ret = 0;

  //Open video stream
  ret = this->video->open(filename);
  if(ret){
    this->setErrorMessage(this->video->getLastErrorMessage());
    return 1;
  }

  //Open audio stream
  ret = this->audio->open(filename);
  if(ret){
    this->setErrorMessage(this->audio->getLastErrorMessage());
    return 1;
  }

  return 0;
}

std::ostream &operator<<(std::ostream &stream, in_context my_ctx){

  stream << "-------------" << std::endl;
  stream << "Input Context: " << my_ctx.getFileName() << std::endl;
  stream << "-------------" << std::endl;

  stream <<  my_ctx.video;
  stream <<  my_ctx.audio;

  return stream;
}
