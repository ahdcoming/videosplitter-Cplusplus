#include "in_video.h"
#include <string>
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}


/* Print detailed information about the input or output format, 
   such as duration, bitrate, streams, container, programs, metadata, side data, 
   codec and time base. 
*/
std::ostream &operator<<(std::ostream &stream, in_video *video){

  avformat_find_stream_info(video->av_format_context, 0);
  av_dump_format(video->av_format_context, video->stream_id, "", 0);

  stream << "-------------------------" << std::endl;
  stream << "Stream " << av_get_media_type_string(video->type) << " Id :" << video->stream_id << std::endl;
  AVCodecContext *input_codec_ctx =   video->av_format_context->streams[video->stream_id]->codec;
  stream << "Width: " << input_codec_ctx->width << " - ";
  stream << "Height: " << input_codec_ctx->height << " - ";
  stream << "Frame Format: " << input_codec_ctx->pix_fmt << std::endl;
  
  return stream;
}

