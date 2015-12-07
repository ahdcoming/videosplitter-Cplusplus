#include "in_video.h"
#include <string>
#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
}



int in_video::readFrame(){

  int input_video_stream_id    = this->stream_id;
  AVFormatContext *pFormatCtx  = this->av_format_context; 

  int result = 0;
  int frameFinished;
  int decoded = 0;


  while(av_read_frame(pFormatCtx, &(this->packet))>=0){
    //The packet now contains some of our input stream data - we need to fill the correct fields
    
    if(this->packet.stream_index == input_video_stream_id) {

      result = avcodec_decode_video2(this->av_format_context->streams[input_video_stream_id]->codec,
				     this->pFrame,
				     &frameFinished,
				     &(this->packet));



      if(result <= 0){
	std::cout << "Read an Empty video frame!: " << result << std::endl;
	//We need to keep track of the skipped video frames 
	this->frames_skipped += this->av_format_context->streams[input_video_stream_id]->codec->ticks_per_frame;
      }

      if(frameFinished){

	/* The size of the data read */
	decoded =  this->packet.size;

	this->has_frame = 1;
	av_free_packet(&(this->packet));
	return decoded;
      }
    }

    // Free the packet that was allocated by av_read_frame
    av_free_packet(&(this->packet));
  }

  printf("\nVideo read nothing\n");

  return decoded;
  
  
  return 0;
}
