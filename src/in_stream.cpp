#include "in_stream.h"

/*Constructor
  gets the type of the stream (audio or video)
  sets the stream as closed
*/
in_stream::in_stream(enum AVMediaType type){
  this->type      = type;
  this->is_open   = 0;
  this->has_frame = 0;
  
  //Init the input packet, one will do for both video and audio
  av_init_packet(&(this->packet));
  this->packet.data = NULL;
  this->packet.size = 0;

  this->pFrame = NULL;

  this->frames_skipped = 0;
}

/* Destructor
   close the stream, if necessary
*/
in_stream::~in_stream(){
  this->close();
}

/* open
   open the file name, if its possible
*/
int in_stream::open(std::string filename){

  int ret;


#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
  this->pFrame = av_frame_alloc();
#else
  this->pFrame = avcodec_alloc_frame();
#endif

  //Open the input file  
  ret = avformat_open_input(&this->av_format_context, filename.c_str(), NULL, NULL);
  if (ret <0){
    this->setErrorMessage(CANT_OPEN_INPUT_FILE, filename);
    return 1;
  }

  //Read packets of a media file to get the stream information. 
  ret = avformat_find_stream_info(this->av_format_context, 0);
  if (ret < 0) {
    this->setErrorMessage(CANT_OPEN_INPUT_FILE, filename);
    return 1;
  }

  //Open the video codec and get the appropriate stream id (audio or video)
  this->stream_id  = this->open_codec_context();
  if(this->stream_id < 0){
    this->setErrorMessage(CANT_OPEN_CODEC);
    return 1;
  }

  this->is_open = 1;
  return 0;
}

/* close 
   closes the open stream, if necessary;
*/
int in_stream::close(){
  if(this->is_open){
    avformat_close_input(&this->av_format_context);
  }
  return 0;
}


std::ostream &operator<<(std::ostream &stream, in_stream *my_stream){
  /* Print detailed information about the input or output format, 
     such as duration, bitrate, streams, container, programs, metadata, side data, 
     codec and time base. 
  */

  //avformat_find_stream_info(my_stream->av_format_context, 0);
  //av_dump_format(my_stream->av_format_context, my_stream->stream_id, "", 0);

  if(my_stream->type == AVMEDIA_TYPE_AUDIO){
    stream << "-------------------------" << std::endl;
    stream << "| INPUT AUDIO STREAM: " << av_get_media_type_string(my_stream->type) << " Id :" << my_stream->stream_id << std::endl;
  }else{
    stream << "-------------------------" << std::endl;
    stream << "| INPUT VIDEO STREAM " << av_get_media_type_string(my_stream->type) << " Id :" << my_stream->stream_id << std::endl;

    AVCodecContext *input_codec_ctx =   my_stream->getCodecContext(); // av_format_context->streams[my_stream->stream_id]->codec;
    stream << "| Width: "  << input_codec_ctx->width << " - "  << std::endl;
    stream << "| Height: " << input_codec_ctx->height << " - " << std::endl;
    stream << "| Frame Format: " << input_codec_ctx->pix_fmt  << std::endl;
  }
  stream << "-------------------------" << std::endl;  
  return stream;
}


/* Private Methods down below */

int in_stream::open_codec_context(){
  int stream_id = -1;

  // Now pFormatCtx->streams is just an array of pointers, 
  // of size pFormatCtx->nb_streams, so let's walk through it until we find a suitable (audio or video) stream.

  unsigned int i;
  AVStream       *input_st;
  AVCodecContext *input_decoder_ctx = NULL;
  AVCodec        *input_decoder     = NULL;

  AVFormatContext *pInputFormatCtx;

  pInputFormatCtx = this->av_format_context;

  // Find the first matching stream
  for(i=0; i<pInputFormatCtx->nb_streams; i++){
    if(pInputFormatCtx->streams[i]->codec->codec_type == this->type) {
      stream_id=i;
      break;
    }
  }
  
  //We have the stream_id, now we need to open the corresponding codec
  if(stream_id >= 0){

    input_st          = pInputFormatCtx->streams[stream_id];                //The input stream
    input_decoder_ctx = input_st->codec;                                    //His codec context - holds various infos
    input_decoder     = avcodec_find_decoder(input_decoder_ctx->codec_id);  //His codec - we need to open this

    if (!input_decoder) {
      return -1;
    }

    //Some debug string
    std::cout << "Opening inpyt " << av_get_media_type_string(this->type) <<  " codec - " <<  input_decoder_ctx->codec_id << std::endl ;

    // We open the codec input_decoder, 
    // wich will be accessed through the input_stream's -> codec 
    // Which is itself a codec_context.
    // Be warned! It's easy to be confused by codec and codec_context, often they are exchanged
    if (avcodec_open2(input_st->codec, input_decoder, NULL) < 0) {
      return -1;
    }else{
      std::cout << "Opened input codec " << av_get_media_type_string(type) << std::endl;
    }
  }else{
    return -1;
  }

  return stream_id;
}

void in_stream::setErrorMessage(int_stream_error_code code, std::string payload){
  std::string errorMessage;

  switch(code){
  case CANT_OPEN_CODEC:
    errorMessage = "Filed to open codec: ";
    errorMessage.append(av_get_media_type_string(this->type));
    this->setErrorMessage(errorMessage);
    break;
  case CANT_OPEN_INPUT_FILE:
    errorMessage = "Could not open input file: " ;
    errorMessage.append(payload);
    this->setErrorMessage(errorMessage);
    break;
  };
}


