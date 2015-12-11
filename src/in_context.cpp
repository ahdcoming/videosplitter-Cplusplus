#include "in_context.h"
#include "in_stream.h"
#include "in_audio.h"
#include "in_video.h"

/*Constructor
*/
in_context::in_context(){
  
  this->audio = new in_audio();
  this->video = new in_video();
  this->error = new errorClass();
}

in_context::~in_context(){
}

int in_context::open(std::string filename){
  this->filename = filename;

  int ret = 0;

  //Open video stream
  ret = this->video->open(filename);
  IF_VAL_REPORT_ERROR_AND_RETURN(ret, this->video->error->getLastMessage());

  //Open audio stream
  ret = this->audio->open(filename);
  IF_VAL_REPORT_ERROR_AND_RETURN(ret, this->audio->error->getLastMessage());

  return 0;
}

std::ostream &operator<<(std::ostream &stream, in_context my_ctx){

  std::string filename = my_ctx.getFileName();

  stream << "-------------" << std::endl;
  stream << "Input Context: " << filename << std::endl;
  stream << "-------------" << std::endl;

  /* Print detailed information about the input or output format, such as duration, bitrate, streams, container, programs, metadata, side data, codec and time base. */
  av_dump_format(my_ctx.video->getFormatContext(), 0, filename.c_str(), 0);


  stream <<  my_ctx.video;
  stream <<  my_ctx.audio;

  return stream;
}
