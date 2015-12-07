#ifndef OUT_STREAM_H
#define OUT_STREAM_H

#include <string>

class out_stream{
 public:
  /* constructor */
  out_stream(enum AVCodecID Id);
  /* destructor */
  virtual ~out_stream();

  /* open and close stream */
  int open(AVFormatContext *av_format_context);
  int close();

  /* push a frame for writing */
  virtual int saveFrame(AVFrame*, AVFormatContext*){ return 1;};

  /* setup codec, virtual, depends of the codec */
  virtual int setup_codec(){ return 1;};

  /* setup_processor - setup scaler or resampler */
  virtual int init_processor(AVCodecContext *){ return 1;};


  /* get and set error messages for external use */
  std::string getLastErrorMessage(){ return this->errorMessage;};
  std::string getErrorMessage(){ return this->errorMessage;};

  int64_t cur_dts(){
    return this->stream->cur_dts;
  }



  //output 
  friend std::ostream &operator<<(std::ostream &stream, out_stream o);

  // We need to count the skipped  frames, in order to sync audio and video
  int   skipped_frames;

  //The output packet, will be used when saving packets to the file
  AVPacket   packet;


 protected:
  void setErrorMessage(std::string error){ this->errorMessage = error ;};
  AVStream      *stream;  
  
 private:
  out_stream(){};

  int is_open;
  enum AVCodecID codecId;

  //Error messages handling
  std::string errorMessage;

  

};


#endif
