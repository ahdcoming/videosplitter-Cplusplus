#include "out_stream_wmv2.h"

#include <iostream>

int out_stream_wmv2::setup_codec(){
      AVRational  time_base;
      AVCodecContext *codec_ctx;

      time_base.num = VIDEO_OUT_FRAMERATE_DEN ;
      time_base.den = VIDEO_OUT_FRAMERATE_NUM ;

      //Populate the video stream video codec_context with our options
      codec_ctx             = this->stream->codec;

      this->stream->time_base = time_base; //for video frames is 1/framerate
      codec_ctx->time_base    = time_base; //for video frames is 1/framerate

      codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
      codec_ctx->codec_id   = VIDEO_OUT_CODEC_ID ;
      codec_ctx->bit_rate   = VIDEO_OUT_BITRATE;
      codec_ctx->width      = VIDEO_OUT_WIDTH;
      codec_ctx->height     = VIDEO_OUT_HEIGHT;
      codec_ctx->pix_fmt    = VIDEO_OUT_PIX_FMT;

      codec_ctx->gop_size = 12;
      codec_ctx->mb_decision = 1;


      std::cout << "Video Codec Opened "  << std::endl;
      
      return 0;
  }
  

int out_stream_wmv2::init_processor(AVCodecContext *input_video_codec_ctx){
  return this->initScaler(input_video_codec_ctx);
}

int out_stream_wmv2::initScaler(AVCodecContext *input_video_codec_ctx){

  //We need an output frame and theen we prepare the scaler

  //An empty frame this frame will be our output frame, will hold each scaled frame.
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
  this->pFrame = av_frame_alloc();
#else
  this->pFrame = avcodec_alloc_frame();
#endif

  //Set the output video frame options
  this->pFrame->format = VIDEO_OUT_PIX_FMT;
  this->pFrame->width  = VIDEO_OUT_WIDTH    ;
  this->pFrame->height = VIDEO_OUT_HEIGHT   ;

  int numBytes;
  // The frame need a data buffer. 
  // We need to allocate a buffer with the right size.
  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(VIDEO_OUT_PIX_FMT,
			      VIDEO_OUT_WIDTH,
			      VIDEO_OUT_HEIGHT);

  numBytes += FF_INPUT_BUFFER_PADDING_SIZE;
  
  uint8_t *buffer            = NULL;
  
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  int ret;
  // Assign appropriate parts of buffer to image planes in pFrame
  // Note that pFrame is an AVFrame, but AVFrame is a superset
  // of AVPicture
  ret = avpicture_fill((AVPicture *)this->pFrame, 
		       buffer, 
		       VIDEO_OUT_PIX_FMT,
		       VIDEO_OUT_WIDTH,
		       VIDEO_OUT_HEIGHT);

  if(ret<0){
    return 1;
  }

  //initialize SWS context for software scaling
  this->pRescale_ctx = sws_getContext(input_video_codec_ctx->width,
				      input_video_codec_ctx->height,
				      input_video_codec_ctx->pix_fmt,
				      VIDEO_OUT_WIDTH,
				      VIDEO_OUT_HEIGHT,
				      VIDEO_OUT_PIX_FMT,
				      SWS_BILINEAR,
				      NULL,
				      NULL,
				      NULL
				      );

  if(this->pRescale_ctx == NULL){
    return 1;
  }

  std::cout << "Scaler Opened "  << std::endl;

  return 0;
}

int out_stream_wmv2::scaleFrame(AVFrame *pFrameIn){
  
  int ret;

  std::string errorMessage;

  ret = sws_scale(this->pRescale_ctx, 
		  (uint8_t const * const *)pFrameIn->data,
		  pFrameIn->linesize, 
		  0, 
		  pFrameIn->height,
		  this->pFrame->data, 
		  this->pFrame->linesize);

  if(ret != this->pFrame->height){
    errorMessage = "\nError during frame scaling \n";
    this->setErrorMessage(errorMessage);
    return 1;
  }
  
  return 0;
}


int out_stream_wmv2::saveFrame(AVFrame *pFrameIn, AVFormatContext *av_format_context){

  //The result will go in this->pFrame
  this->scaleFrame(pFrameIn);

  int ret, got_output;
  std::string errorMessage;

  AVPacket *pktPtr; 

  pktPtr = &(this->packet);

  //Encode the output frame into the packet
  ret = avcodec_encode_video2(this->stream->codec, 
			      pktPtr,
			      this->pFrame, 
			      &got_output);
    
  if (ret < 0) {
    errorMessage = "\nError encoding frame\n";
    this->setErrorMessage(errorMessage);
    return 0;
  }

  if (got_output) {
    
    /* Now, the frame has been converted in the output format 
       BUT  we need to set it's presentation time pts
       This is in the codec coded_frame->pts field
       BUT again it has to be rescaled to the output stream time_base (which is different)
       So:
    */
    
    if (this->stream->codec->coded_frame->pts != AV_NOPTS_VALUE){

      //Here we add the skipped_frames_count, to keep track of the skipped frames, otherwise the audio will not be in sinc with video
      
      pktPtr->pts = av_rescale_q(this->stream->codec->coded_frame->pts + this->skipped_frames , 
				 this->stream->codec->time_base, 
				 this->stream->time_base);
      
      pktPtr->dts = pktPtr->pts;
    }
    
    //If is a key frame
    if(this->stream->codec->coded_frame->key_frame){
      pktPtr->flags |= AV_PKT_FLAG_KEY;
    }

    pktPtr->stream_index  = this->stream->index;

   
#if INTERLEAVED == 1	
    ret = av_interleaved_write_frame(av_format_context, pktPtr);
#else
    ret = av_write_frame(av_format_context, pktPtr);
#endif
    
    if(ret <0){
      errorMessage = "Error saving video frame";
      this->setErrorMessage(errorMessage);
      return 1;
    }
    
    av_free_packet(pktPtr); 
  }
  
  return 0;
}
