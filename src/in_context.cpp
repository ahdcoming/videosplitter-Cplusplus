#include "in_context.h"

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
  if(ret = this->video->open(filename)){
    this->setLastErrorMessage(this->video->getLastErrorMessage());
    return 1;
  }

  //Open audio stream
  if(ret = this->audio->open(filename)){
    this->setLastErrorMessage(this->audio->getLastErrorMessage());
    return 1;
  }

  return 0;
}

std::ostream &operator<<(std::ostream &stream, in_context my_ctx){
  std::cout << my_ctx.video;
  std::cout << my_ctx.audio;
}
