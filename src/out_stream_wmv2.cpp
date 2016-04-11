#include "out_stream_wmv2.h"

#include <iostream>

int out_stream_wmv2::setup_codec(){
      AVRational  time_base;
      AVCodecContext *codec_ctx;

      //Time base for the video stream
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

#if DEBUG == 1
      std::cout << "Video Codec Opened "  << std::endl;
#endif      

      return 0;
}
  

int out_stream_wmv2::init_processor(AVCodecContext *input_video_codec_ctx){
  this->pFrameScene    = NULL;
  this->prevSceneScore = 0;
  return this->initScaler(input_video_codec_ctx);
}

int out_stream_wmv2::initScaler(AVCodecContext *input_video_codec_ctx){

  //We need an output frame and theen we prepare the scaler

  std::cout << "Init Scaler "  << std::endl;

  //An empty frame -  this frame will be our output frame, will hold each scaled frame.
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

#if DEBUG == 1
  std::cout << "Scaler Opened "  << std::endl;
#endif

  return 0;
}

int out_stream_wmv2::scaleFrame(AVFrame *pFrameIn){
  
  int ret;

  ret = sws_scale(this->pRescale_ctx, 
		  (uint8_t const * const *)pFrameIn->data,
		  pFrameIn->linesize, 
		  0, 
		  pFrameIn->height,
		  this->pFrame->data, 
		  this->pFrame->linesize);

  IF_VAL_REPORT_ERROR_AND_RETURN(ret != this->pFrame->height, "Error during frame scaling");
  
  return 0;
}


int out_stream_wmv2::saveFrame(AVFrame *pFrameIn, AVFormatContext *av_format_context, int skipped_frames){

  int ret, got_output;
  std::string errorMessage;

  AVPacket *pktPtr; 

  pktPtr = &(this->packet);

  //First we scale the input frame
  //The result will go in this->pFrame
  this->scaleFrame(pFrameIn);

  //Encode the output frame into the packet
  ret = avcodec_encode_video2(this->stream->codec, 
			      pktPtr,
			      this->pFrame, 
			      &got_output);

  IF_VAL_NEGATIVE_REPORT_ERROR_AND_RETURN(ret, "Error encoding frame");

  if (got_output) {
    
    /* Now, the frame has been converted in the output format 
       BUT  we need to set it's presentation time pts
       This is in the codec coded_frame->pts field
       BUT again it has to be rescaled to the output stream time_base (which is different)
       So:
    */
    
    if (this->stream->codec->coded_frame->pts != AV_NOPTS_VALUE){

      //Here we add the skipped_frames_count, to keep track of the skipped frames, otherwise the audio will not be in sinc with video
      
      pktPtr->pts = av_rescale_q(this->stream->codec->coded_frame->pts + skipped_frames , 
				 this->stream->codec->time_base, 
				 this->stream->time_base);
      
      pktPtr->dts = pktPtr->pts;
    }
    
    //If this is a key frame
    if(this->stream->codec->coded_frame->key_frame){
      pktPtr->flags |= AV_PKT_FLAG_KEY;
    }

    pktPtr->stream_index  = this->stream->index;

   
#if INTERLEAVED == 1	
    ret = av_interleaved_write_frame(av_format_context, pktPtr);
#else
    ret = av_write_frame(av_format_context, pktPtr);
#endif

    IF_VAL_NEGATIVE_REPORT_ERROR_AND_RETURN(ret, "Error saving video frame");
    
    av_free_packet(pktPtr); 
  }
  
  return 0;
}

/* Tell if we have  a blackframe 
   It counts the number of black pixels in centered squares at the middle of the screen
*/
int out_stream_wmv2::isBlackFrame(){

  int frameAvg = 0;

  int height,width;

  int result = 0;

  double sceneScore = this->getSceneScore();

  if(0){
  if(sceneScore > 0.8){
    std::cout << std::endl << "-> Scene score: "<< sceneScore << std:: endl;
    return 1;
  }else{
    return 0;
  }
  }

  height = this->pFrame->height;
  width  = this->pFrame->width;

  if(this->pFrame->format == AV_PIX_FMT_YUV420P ){
    //The frame has to be in this format 
    frameAvg = countBlackPixels(width/8, height/8, 50);
    if(frameAvg > 80){
      frameAvg = countBlackPixels(width, height, 50);
    }
    if(frameAvg > 80){
      result = 1;
    }
  }

  return result;
}


/* Counts the percentage of black pixels in the rectangle
   Depends to the pFrame format 
*/
int out_stream_wmv2::countBlackPixels(int rectW, int rectH, int threshold){
  int x, y, halfX, halfY;
  int Y,Cb,Cr;
  int frameSum = 0;
  int frameAvg = 0;

  int width  = this->pFrame->width;
  int height = this->pFrame->height;

  int offsetX = (width-rectW)/2;
  int offsetY = (height-rectH)/2;

  char colors[3];
  int R,G,B;

  frameSum = 0;
  for(y=offsetY; y<rectH+offsetY; y++){
    for(x=offsetX; x<rectW+offsetX; x++){
      
      Y  = this->pFrame->data[0][y * this->pFrame->linesize[0] + x];

      halfX = x/2;
      halfY = y/2;
      Cb = this->pFrame->data[1][halfY * this->pFrame->linesize[1] + halfX];
      Cr = this->pFrame->data[2][halfY * this->pFrame->linesize[2] + halfX];

      if(Y < threshold){

	common::YUV2RGB(Y,Cr,Cb,colors);

	R = colors[0];
	G = colors[1];
	B = colors[2];

	if(R<137 && G<137 && B<137){
	  frameSum += 1;
	}
      }
    }
  }
  
  frameAvg = (100.0*frameSum)/(rectW*rectH);
  return frameAvg;
    
}

double out_stream_wmv2::getSceneScore(){
  double result = 0;
  
  if(this->pFrameScene == NULL){

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
    this->pFrameScene = av_frame_alloc();
#else
    this->pFrameScene = avcodec_alloc_frame();
#endif

    this->pFrameScene->width  = this->pFrame->width;
    this->pFrameScene->height = this->pFrame->height;
    this->pFrameScene->format = this->pFrame->format;

    av_frame_get_buffer(this->pFrameScene, 32);
    av_frame_copy(this->pFrameScene, this->pFrame);
  }

  int width  = this->pFrame->width;
  int height = this->pFrame->height;
  
  int x, y;
  int Y,Y_prev,Y_delta;

  double sum = 0;
  for(x=0;x<width;x++){
    for(y=0;y<height;y++){
      Y       = this->pFrameScene->data[0][y * this->pFrameScene->linesize[0] + x];
      Y_prev  = this->pFrame->data[0][y * this->pFrame->linesize[0] + x];
      
      Y_delta = Y-Y_prev;
      if(Y_delta > 0){
	sum += Y_delta;
      }else{
	sum -= Y_delta;
      }
    }
  }

  double mafd = sum/(width*height);
  double diff =  fabs(mafd - this->prevSceneScore);
  result  = av_clipf(FFMIN(mafd, diff) / 100., 0, 1);
  
  this->prevSceneScore = result;

  av_frame_copy(this->pFrameScene, this->pFrame);  

  return result;
}
