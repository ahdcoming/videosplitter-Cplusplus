
#include "out_context.h"
#include "out_stream_wmav2.h"
#include "out_stream_wmv2.h"
#include <sstream>


out_context::out_context(std::string prefix){

  this->video = NULL;
  this->audio = NULL;

  this->fileCounter = 0;
  this->filePrefix  = prefix;

  this->is_open     = 0;

  this->error = new errorClass;

}

out_context::~out_context(){
  if(!this->is_open){
    this->close();
  }
}

std::ostream &operator<<(std::ostream &stream, out_context my_ctx){

  stream << "-------------" << std::endl;
  stream << "Output Context: " << my_ctx.getFileName() << std::endl;
  stream << "--------------" << std::endl;
  stream << "Audio" << std::endl;
  stream << *my_ctx.audio << std::endl;

  stream << "--------------" << std::endl;
  stream << "Video" << std::endl;
  stream << *my_ctx.video << std::endl;  

  return stream;
}


int out_context::open(){

  std::stringstream ss;

  //let's build the output file name
  this->fileCounter++;
  ss << this->filePrefix << "_" << this->fileCounter << ".wmv";

  this->filename = ss.str();

  std::cout << "Output Filename: " << this->filename << std::endl;

  /* allocate the output media context for the new output stream */
  this->av_format_context = avformat_alloc_context();
  IF_VAL_REPORT_ERROR_AND_RETURN(!av_format_context, "Memory error");

  //try to guess the output file format
  this->av_format_context->oformat = av_guess_format(NULL,OUTPUT_FORMAT,NULL);
  IF_VAL_REPORT_ERROR_AND_RETURN(this->av_format_context->oformat == NULL, "Can't find a suitable output format");

  //we need the output audio and video stream for the current format (only .wmv for now)
  if(!this->video){
    this->video = new out_stream_wmv2;
  }
  
  if(!this->audio){
    this->audio = new out_stream_wmav2;
  }

  int ret;

  //let's open the output contexts
  ret = this->video->open(this->av_format_context);
  IF_VAL_REPORT_ERROR_AND_RETURN(ret, this->video->error->getLastMessage());

  ret = this->audio->open(this->av_format_context);
  IF_VAL_REPORT_ERROR_AND_RETURN(ret, this->audio->error->getLastMessage());

  /* open the output file */
  if (!(av_format_context->flags & AVFMT_NOFILE)) {
    ret = avio_open(&av_format_context->pb, filename.c_str(), AVIO_FLAG_WRITE);
    IF_VAL_NEGATIVE_REPORT_ERROR_AND_RETURN(ret, "Could not open output file");
  }
  
  /* write the stream header, if any */
  avformat_write_header(av_format_context ,NULL);

  std::cout <<  "\nOpened output file: " << filename << std::endl ;

  is_open = 1;

  return 0;
}


int out_context::close(){
  if(!this->is_open){
    return 0;
  }

  //write the tail of the file
  av_write_trailer(this->av_format_context);

  //cleanup
  //delete(this->video);
  //delete(this->audio);

  if (!(this->av_format_context->oformat->flags & AVFMT_NOFILE)) {

    /* close the output file */
    avio_flush(this->av_format_context->pb);
    avio_close(this->av_format_context->pb);
  }

  /* free the stream */
  av_free(this->av_format_context);

  this->is_open = 0;
  return 0;
}

