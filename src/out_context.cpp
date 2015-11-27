#include "out_context.h"
#include "out_stream_wmav2.h"
#include "out_stream_wmv2.h"

out_context::out_context(){
  this->is_open              = 0;
  this->video_frames_skipped = 0;

  this->video = NULL;
  this->audio = NULL;

}

out_context::~out_context(){
  if(!this->is_open){
    this->close();
  }
}

std::ostream &operator<<(std::ostream &stream, out_context my_ctx){

  std::cout << "-------------" << std::endl;
  std::cout << "Output Context: " << my_ctx.getFileName() << std::endl;
  std::cout << "--------------" << std::endl;

  return stream;
}

int out_context::open(std::string filename){
  this->filename = filename;

  //We need to do this every time we open a new output file
  this->audio_samples_count     = 0 ;
  
  /* allocate the output media context for the new output stream */
  this->av_format_context = avformat_alloc_context();
  if (!av_format_context) {
    std::string error("Memory error");
    this->setErrorMessage(error);
    return 1;
  }

  //try to guess the output file format
  this->av_format_context->oformat = av_guess_format(NULL,OUTPUT_FORMAT,NULL);
  if(this->av_format_context->oformat == NULL){
    std::string error("Can't find a suitable output format");
    this->setErrorMessage(error);
    return 1;
  }

  //we need the output audio and video codec
  this->video = new out_stream_wmv2;
  this->audio = new out_stream_wmav2;

  is_open = 1;

  return 0;
}


int out_context::close(){
  if(!this->is_open){
    return 0;
  }

  //write the tail of the file
  av_write_trailer(this->av_format_context);

  delete(this->video);
  delete(this->audio);

  if (!(this->av_format_context->oformat->flags & AVFMT_NOFILE)) {

    /* close the output file */
    avio_flush(this->av_format_context->pb);
    avio_close(this->av_format_context->pb);
  }

  /* free the stream */
  av_free(this->av_format_context);

  this->video_frames_skipped = 0 ;
  this->audio_samples_count = 0;


  this->is_open = 0;
  return 0;
}
