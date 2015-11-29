#include "in_audio.h"
#include <string>
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}


std::ostream &operator<<(std::ostream &stream, in_audio *audio){
  /* Print detailed information about the input or output format, 
     such as duration, bitrate, streams, container, programs, metadata, side data, 
     codec and time base. 
  */

  stream << "-------------------------" << std::endl;
  stream << "Stream " << av_get_media_type_string(audio->type) << " Id :" << audio->stream_id << std::endl;
  
  return stream;
}

