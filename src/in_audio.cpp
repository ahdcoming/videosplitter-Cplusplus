#include "in_audio.h"

std::ostream &operator<<(std::ostream &stream, in_audio *audio){
  /* Print detailed information about the input or output format, 
     such as duration, bitrate, streams, container, programs, metadata, side data, 
     codec and time base. 
  */

  std::cout << "-------------------------" << std::endl;
  std::cout << "Stream " << av_get_media_type_string(audio->type) << " Id :" << audio->stream_id << std::endl;
  AVCodecContext *input_codec_ctx =   audio->av_format_context->streams[audio->stream_id]->codec;
  
  return stream;
}

