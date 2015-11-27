#ifndef OUT_STREAM_H
#define OUT_STREAM_H

#include <string>





class out_stream{
 public:
  out_stream(enum AVCodecID Id);
  virtual ~out_stream();
  int open(AVFormatContext *av_format_context);
  int close();

  virtual int setup_codec();

  /* get and set error messages for external use */
  std::string getLastErrorMessage(){ return this->errorMessage;};
  std::string getErrorMessage(){ return this->errorMessage;};

  int open(AVOutputFormat *av_format_context);

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
