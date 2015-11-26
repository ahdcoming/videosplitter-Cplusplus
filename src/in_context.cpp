#include "in_context.h"
#include "in_audio.h"
#include "in_video.h"

/*Constructor
*/
in_context::in_context(){
  
  this->audio = new in_audio();
  this->video = new in_video();
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

  std::cout << "-------------" << std::endl;
  std::cout << "Input Context: " << my_ctx.getFileName() << std::endl;
  std::cout << "-------------" << std::endl;

  std::cout << my_ctx.video;
  std::cout << my_ctx.audio;

  return stream;
}
