#ifndef IN_STREAM_H
#define IN_STREAM_H

#include "common.h"


enum int_stream_error_code {CANT_OPEN_CODEC, CANT_OPEN_INPUT_FILE };

class in_stream{
 public:

  /*in_stream 
    Constructor: we pass the type, audio or video 
  */
  in_stream(enum AVMediaType type);

  /*~in_stream 
    Destructor: must be virtual destructor
  */
  virtual ~in_stream();

  /* open
     open the filename and extracts the input stream
  */
  int open(std::string filename);

  /* close
     closes the input file and cleans the input stream
  */
  int close();

  /* hasFrame 
     tells if there we have a frame waiting to be processed
  */
  int hasFrame(){ return this->has_frame;};
  
  /* resetFrame 
     if needed does some cleanup on the input frame 
  */
  void resetFrame(){ this->has_frame = 0;};
  
  /* getFrame 
     returns a pointer to the last read frame
  */
  AVFrame * getFrame(){ return this->pFrame;};

  /* getCodecContext 
     returns a pointer to the current codec context - can be used to inizialize data processors
  */
  AVCodecContext * getCodecContext(){ return this->av_format_context->streams[this->stream_id]->codec;}

  /* getCodecContext 
     returns a pointer to the current codec context - can be used to inizialize data processors
  */
  AVFormatContext * getFormatContext(){ return this->av_format_context; }


  /* operator<<
     freand class for printing data about the current frame 
  */
  friend std::ostream &operator<<(std::ostream &stream, in_stream* o);

  /* readFrame 
     reads a frame from the input stream
  */
  virtual int readFrame(){return 1;};
  
  //A flag, used once in the init_stream
  static int initialized;

  //the error reporting class
  errorClass *error;

 protected:

  /* open_codec_context
     opens the input codec
  */
  int open_codec_context();

  // The type of the stream  (AVMEDIA_TYPE_VIDEO or AVMEDIA_TYPE_AUDIO)
  enum AVMediaType type;

  // A pointer to the input format context
  AVFormatContext *av_format_context;
  int stream_id;

  // A flag, tell us if the stream is currently open
  int is_open;

  // A flag  tell us if there is a frame waiting to be processed
  int has_frame;

  // The input packet, will hold the real input samples/data
  AVPacket   packet;

  // The last read  frame
  AVFrame    *pFrame;

  // Number of frames skipped, used in case of bad frames for keeping the sincronization 
  int frames_skipped;

 private:
  in_stream(){}; // Default constructor we don't want

  std::string buildErrorMessage(int_stream_error_code code, std::string payload);


};


#endif
