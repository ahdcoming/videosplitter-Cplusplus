#ifndef OUT_STREAM_H
#define OUT_STREAM_H

#include "common.h"

class out_stream{
 public:

  /* out_stream
     constructor - param: the codecId
   */
  out_stream(enum AVCodecID Id);
  /* 
     ~out_stream
     destructor: has to be virtual
  */
  virtual ~out_stream();

  /* open
     opens the current output stream, audio or video
  */
  int open(AVFormatContext *av_format_context);

  /* close
     closes the current output stream
  */
  int close();

  /* saveFrame
     get's a frame, and then process it in order to get it ready for the output stream
     audio frames get resampled and stored, video frames get scaled and sent to the output stream
  */
  virtual int saveFrame(AVFrame*, AVFormatContext*, int){ return 1;};

  /* setup_codec
     sets the output codec settings and then start the codec
  */
  virtual int setup_codec(){ return 1;};

  /* init_processor
     resamplers and scalers need some initialization, and need the input codec context 
     this is the real link between the input and the output streams
  */
  virtual int init_processor(AVCodecContext *){ return 1;};

  /* isBlackFrame 
     returns true if the current out frame is black - valid for video streams
  */
  virtual int isBlackFrame(){ return 1; }

  /* isSilentFrame
     returns true if the current out frame is silent - valid for audio streams
  */
  virtual int isSilentFrame(){ return 1; }

  /* getVolume
     returns the audio volume, if it's an audio stream
  */
  virtual float getVolume(){ return 0; }

  /* cur_dts
     it returns the current dts of the output stream
     it is used externally to keep audio and video in sync
  */
  int64_t cur_dts(){
    return this->stream->cur_dts;
  }

  //output 
  friend std::ostream &operator<<(std::ostream &stream, out_stream o);

  //The output data packet, will be used when saving packets to the file
  AVPacket   packet;

  //The output data frame
  AVFrame *pFrame;

  errorClass *error;

 protected:
  AVStream      *stream;  
  
 private:
  out_stream(){};

  int is_open;
  enum AVCodecID codecId;

  

};


#endif
