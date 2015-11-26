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

  /* get and set error messages for external use */
  std::string getLastErrorMessage(){ return this->errorMessage;};
  void setLastErrorMessage(std::string error){ this->errorMessage = error ;};

  friend std::ostream &operator<<(std::ostream &stream, in_stream* o);
  
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

 private:
  in_stream(){}; // Default constructor we don't want


};


#endif
