#include "common.h"
#include "out_stream.h"

#include <iostream>

out_stream::out_stream(enum AVCodecID Id){

  this->codecId = Id;
  this->is_open = 0;

  this->stream = NULL;
  this->error  = NULL;

  this->skipped_frames = 0;

  //Init the output packet, one will do for both video and audio
  av_init_packet(&(this->packet));
  this->packet.data = NULL;
  this->packet.size = 0;

  this->pFrame = NULL;
};  

out_stream::~out_stream(){
  //free the context and close the stuff
  if(this->is_open){
    this->close();
  }
}

int out_stream::open(AVFormatContext *av_format_context){

  AVCodecContext *codec_ctx = NULL;
  AVCodec        *codec = NULL;

  int result = 0;

  std::string errorMessage;
  
  /* find the codec for encoding */
  codec = avcodec_find_encoder(this->codecId);
  IF_VAL_REPORT_ERROR_AND_RETURN(!codec, "Video codec not found");

  //create a new video stream
  /*We keep a direct pointer to the video stream, but you can still access
    it through the output_struct->av_format_context->streams structure */

  this->stream =  avformat_new_stream(av_format_context, codec);
  IF_VAL_REPORT_ERROR_AND_RETURN(!this->stream, "Could not alloc stream");

  /* setup_codec is a virtual call in the real audio/video stream */
  result = this->setup_codec();
  IF_VAL_REPORT_ERROR_AND_RETURN(result, "Could not setup codec");

  //Populate the video stream video codec_context with our options
  codec_ctx  = this->stream->codec;
 
  //some formats - like wmv - require a global header
  if (av_format_context->oformat->flags & AVFMT_GLOBALHEADER){
    codec_ctx->flags |= CODEC_FLAG_GLOBAL_HEADER;
  }

  /* open the codec */
  result = avcodec_open2(codec_ctx, codec, NULL);
  if (result < 0) {
    errorMessage =  "Video codec could not open codec\n";
    errorMessage.append("Error String: ");
    errorMessage.append(common::get_error_text(result));
    this->error->setMessage(errorMessage);
    return 1;
  }

  return 0;
}

int out_stream::close(){
  
  /* close the codec */
  if(this->is_open && this->stream){
    avcodec_close(this->stream->codec);
    av_freep(this->stream->codec);
    av_freep(this->stream);
  }
  
  this->is_open = 0;
  return 0;
}

std::ostream &operator<<(std::ostream &stream, out_stream my_stream){

  stream << "Codec: " << my_stream.codecId << std::endl;

  return stream;
}
