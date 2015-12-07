#include "out_stream_wmav2.h"

#include <iostream>

int out_stream_wmav2::setup_codec(){
      AVRational  time_base;
      AVCodecContext *codec_ctx;

      time_base.num   = 1 ;
      time_base.den   = AUDIO_OUT_SAMPLE_RATE ;

      this->samples_count = 0;

      this->target_dts = 0;

      this->stream->time_base = time_base; //for video frames is 1/framerate

      //Populate the video stream video codec_context with our options
      codec_ctx             = this->stream->codec;

      //Populate the Audio codec context
      codec_ctx->codec_type  = AVMEDIA_TYPE_AUDIO;
      codec_ctx->codec_id    = AUDIO_OUT_CODEC_ID;
      codec_ctx->sample_rate = AUDIO_OUT_SAMPLE_RATE;
      codec_ctx->bit_rate    = AUDIO_OUT_BITRATE;
      codec_ctx->channels    = AUDIO_OUT_CHANNELS; //Number of channels
      codec_ctx->channel_layout = av_get_default_channel_layout(AUDIO_OUT_CHANNELS); //stereo
      codec_ctx->sample_fmt  = AUDIO_OUT_SAMPLE_FMT; //samples format

      std::cout << "Audio Codec Opened "  << std::endl;

      return 0;
  }

int out_stream_wmav2::init_processor(AVCodecContext *input_audio_codec_ctx){
  return this->initResampler(input_audio_codec_ctx);
}


int out_stream_wmav2::initResampler(AVCodecContext *input_codec_ctx){

  std::string errorMessage;

 
  //An empty frame - this frame will be our output frame, it will hold the audio samples
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
  this->pFrame = av_frame_alloc();
#else
  this->pFrame = avcodec_alloc_frame();
#endif

  this->pFrame->format         = AUDIO_OUT_SAMPLE_FMT;
  this->pFrame->sample_rate    = AUDIO_OUT_SAMPLE_RATE;
  this->pFrame->channel_layout = av_get_default_channel_layout(AUDIO_OUT_CHANNELS);//stereo
  this->pFrame->nb_samples     = AUDIO_OUT_SAMPLES_PER_FRAME;
  
  int numBytes;
  //We need to init the data part of the audio frame
  numBytes  = AUDIO_OUT_SAMPLES_PER_FRAME * av_get_bytes_per_sample(AUDIO_OUT_SAMPLE_FMT)*AUDIO_OUT_CHANNELS;
  numBytes += FF_INPUT_BUFFER_PADDING_SIZE;
  
  uint8_t *buffer            = NULL;
  buffer   = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  int ret;
  ret = avcodec_fill_audio_frame(this->pFrame,
				 AUDIO_OUT_CHANNELS,
				 AUDIO_OUT_SAMPLE_FMT,
				 buffer,
				 numBytes,
				 1);


 
  if(ret<0){
    errorMessage = "Error filling output frame.";
    this->setErrorMessage(errorMessage);
    return 1;
  }


#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
  this->pFrameCodec = av_frame_alloc();
#else
  this->pFrameCodec = avcodec_alloc_frame();
#endif

  this->pFrameCodec->format         = AUDIO_OUT_SAMPLE_FMT;
  this->pFrameCodec->sample_rate    = AUDIO_OUT_SAMPLE_RATE;
  this->pFrameCodec->channel_layout = av_get_default_channel_layout(AUDIO_OUT_CHANNELS);//stereo
  
  //This is the difference, the output codec need frames with a different number of samples
  this->pFrameCodec->nb_samples     = this->stream->codec->frame_size ;
  
  numBytes  = this->stream->codec->frame_size * av_get_bytes_per_sample(AUDIO_OUT_SAMPLE_FMT)*AUDIO_OUT_CHANNELS;
  numBytes += FF_INPUT_BUFFER_PADDING_SIZE;
  buffer   = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  ret = avcodec_fill_audio_frame(this->pFrameCodec,
				 AUDIO_OUT_CHANNELS,
				 AUDIO_OUT_SAMPLE_FMT,
				 buffer,
				 numBytes,
				 1);

  if(ret<0){
    errorMessage = "Error filling output codec frame.";
    this->setErrorMessage(errorMessage);
    return 1;
  }

  
  this->resample_ctx = swr_alloc();

  if(!this->resample_ctx){
    errorMessage = "Could not allocate resample context.";
    this->setErrorMessage(errorMessage);
    return 1;
  }

  

  //input
  av_opt_set_int(this->resample_ctx, "in_channel_layout",  av_get_default_channel_layout(input_codec_ctx->channels),0);
  av_opt_set_int(this->resample_ctx, "in_sample_rate",     input_codec_ctx->sample_rate,                0);
  av_opt_set_int(this->resample_ctx, "in_sample_fmt",      input_codec_ctx->sample_fmt, 0);

  //output
  av_opt_set_int(this->resample_ctx, "out_channel_layout", av_get_default_channel_layout(AUDIO_OUT_CHANNELS),  0);
  av_opt_set_int(this->resample_ctx, "out_sample_rate",    AUDIO_OUT_SAMPLE_RATE,                0);
  av_opt_set_int(this->resample_ctx, "out_sample_fmt",     AUDIO_OUT_SAMPLE_FMT, 0);

  /** Open the resampler with the specified parameters. */
  ret = swr_init(this->resample_ctx);

  if (ret < 0) {
    swr_free(&this->resample_ctx);
    errorMessage = "Could not open resample context.";
    this->setErrorMessage(errorMessage);
    return 1;
  }

  this->samples_fifo   = av_audio_fifo_alloc(AUDIO_OUT_SAMPLE_FMT, 
						   AUDIO_OUT_CHANNELS, 
						   1);
  if(!this->samples_fifo){
    errorMessage = "Could not open fifo queue.";
    this->setErrorMessage(errorMessage);
    return 1;
  }

  return 0;
}

int out_stream_wmav2::resampleFrame(AVFrame *pFrameIn){

    std::string errorMessage;
    
    AVFrame *pFrameOut            = this->pFrame;  

    AVPacket  *pktPtr = NULL;
    //Output Packet, will hold the data
    pktPtr = &this->packet;

    int ret;
    //Let's resample the audio output (no need for buffering , nb_samples out is big enough )
    ret = swr_convert(this->resample_ctx,
		      (uint8_t  **)pFrameOut->data,
		      AUDIO_OUT_SAMPLES_PER_FRAME,
		      (const uint8_t  **)pFrameIn->data,
		      pFrameIn->nb_samples);
    
      if (ret < 0) {
	errorMessage = "Error when resampling audio packet\n";
	//fprintf(stderr, "\nError String: %d %s \n", ret, get_error_text(ret));
	av_free_packet(pktPtr);
	return 1;
      }

      av_free_packet(pktPtr);
      return 0;
}

int out_stream_wmav2::saveFrame(AVFrame *pFrameIn, AVFormatContext *av_format_context){

  std::string errorMessage;

  //The result will go in this->pFrame
  this->resampleFrame(pFrameIn);

  int result = 0;
  //Push the samples in the audio fifo
  result = av_audio_fifo_write(this->samples_fifo, (void **)this->pFrame->data,  this->pFrame->nb_samples);

  if(result < this->pFrame->nb_samples){
    errorMessage = "Error writing data to audio fifo.";
    this->setErrorMessage(errorMessage);
    return 1;
  }


  std::cout << "------- Fifo Frames ! " 
	    << av_audio_fifo_size(this->samples_fifo) 
	    << " - " 
	    <<  this->stream->codec->frame_size  
	    << std::endl;


  std::cout <<  "Samples count " << this->samples_count << std::endl;

  //If we have enough samples in the fifo, we can send them to the encoder
  //while (((this->target_dts - this->stream->cur_dts) >= AUDIO_VIDEO_MAX_DELAY)
  while(av_audio_fifo_size(this->samples_fifo) >= this->stream->codec->frame_size){


    std::cout << "------- Audio Packet Write 1 !"  << std::endl;

    av_audio_fifo_read(this->samples_fifo,
		       (void **)this->pFrameCodec->data,
		       this->stream->codec->frame_size);

    //Set the pts
    this->samples_count += this->pFrameCodec->nb_samples;
    this->pFrameCodec->pts = this->samples_count;

    std::cout <<  "Samples count " << this->samples_count << std::endl;

    int got_output = 0;

    AVPacket  *pktPtr;
    //Output Packet, will hold the data
    pktPtr = &this->packet;
    av_init_packet(pktPtr);

    int ret;
    ret = avcodec_encode_audio2(this->stream->codec, pktPtr, this->pFrameCodec, &got_output);

    /* encode the audio frame */
    if (ret < 0) {
      errorMessage = "Error encoding output frame.";
      //fprintf(stderr, "\nError String: %d %s \n", ret, get_error_text(ret));

      this->setErrorMessage(errorMessage);
      //fprintf(stderr, "\nError encoding audio frame\n");

      av_free_packet(pktPtr);
      return 1;
    }
    
    if (got_output) {

      std::cout <<  "AUDIO out pts " << pktPtr->pts << std::endl;
      std::cout <<  "AUDIO out dts " << pktPtr->dts << std::endl;

      //The packet pts has to be converted from the codec timebase to the stream time base
      av_packet_rescale_ts(pktPtr, this->stream->codec->time_base, this->stream->time_base);
      pktPtr->dts = pktPtr->pts;
      pktPtr->stream_index  = this->stream->index;

      std::cout <<  "AUDIO POST out pts " << pktPtr->pts << std::endl;
      std::cout <<  "AUDIO POST dts " << pktPtr->dts << std::endl;


    //Write to file
#if INTERLEAVED == 1	
    ret = av_interleaved_write_frame(av_format_context, pktPtr);
#else
    ret = av_write_frame(av_format_context, pktPtr);
#endif
      if(ret <0){
	fprintf(stderr, "\nError saving audio frame\n");
	return 0;
      }

      av_free_packet(pktPtr);
    }
  }
  return 0;
}
