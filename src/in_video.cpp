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

  // We keep reading from the input context until we have a complete frame.
  // A frame can be divided over many packets
  while(av_read_frame(pFormatCtx, &(this->packet))>=0){
    //The packet now contains some of our input stream data - we need to fill the correct fields
    if(this->packet.stream_index == input_video_stream_id) {

      result = avcodec_decode_video2(this->av_format_context->streams[input_video_stream_id]->codec,
				     this->pFrame,
				     &frameFinished,
				     &(this->packet));


      if(result <= 0){
	this->error->setMessage("Read an Empty video frame!");
	//We need to keep track of the skipped video frames 
	//this->frames_skipped += this->av_format_context->streams[input_video_stream_id]->codec->ticks_per_frame;
	std::cout << std::endl << "Skipped video Frame " << std::endl;
	av_free_packet(&(this->packet));
	continue;
      }

      //Look, a malformed video_frame, let's ignore it
      if(this->pFrame->pkt_pts == 0){
	std::cout << std::endl << "\nSkip a BAD video frame!" << std::endl;
	//this->frames_skipped += this->av_format_context->streams[input_video_stream_id]->codec->ticks_per_frame;
	this->has_frame = 0;
	av_free_packet(&(this->packet));
	continue;
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

  return decoded;
}
