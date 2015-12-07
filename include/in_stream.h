#ifndef IN_STREAM_H
#define IN_STREAM_H

#include <string>
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
}

enum int_stream_error_code {CANT_OPEN_CODEC, CANT_OPEN_INPUT_FILE };

class in_stream{
 public:

  in_stream(enum AVMediaType type);
  ~in_stream();

  /* open open the file name and extracts the input audio and video streams */
  int open(std::string filename);
  int close();

  int hasFrame(){ return this->has_frame;};
  void resetFrame(){ this->has_frame = 0;};
  
  AVFrame * getFrame(){ return this->pFrame;};

  AVCodecContext * getCodecContext(){ return this->av_format_context->streams[this->stream_id]->codec;}

  /* get and set error messages for external use */
  std::string getErrorMessage(){ return this->errorMessage;};
  std::string getLastErrorMessage(){ return this->errorMessage;};
  void setErrorMessage(std::string error){ this->errorMessage = error ;};

  friend std::ostream &operator<<(std::ostream &stream, in_stream* o);

  virtual int readFrame(){return 1;};
  
  //A flag, used once in the init_stream
  static int initialized;

 protected:

  void setErrorMessage(int_stream_error_code code, std::string payload = std::string(" "));

  int open_codec_context();

  //Error messages handling
  std::string errorMessage;

  //The type of the stream  (AVMEDIA_TYPE_VIDEO or AVMEDIA_TYPE_AUDIO)
  enum AVMediaType type;

  AVFormatContext *av_format_context;
  int stream_id;

  //a flag, tell us if the stream is open
  int is_open;

  //if there is a frame waiting to be processed
  int has_frame;

  //The input packet, will be used when saving packets to the file
  AVPacket   packet;

  // The last read audio frame
  AVFrame    *pFrame;

  int frames_skipped;

 private:
  in_stream(){}; // Default constructor we don't want



};


#endif
