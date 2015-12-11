#ifndef IN_AUDIO_STREAM_H
#define IN_AUDIO_STREAM_H

#include "in_stream.h"

class in_audio: public in_stream{
 public:
  /*in_audio
    constructor, calls the parent class
   */
  
  in_audio():in_stream(AVMEDIA_TYPE_AUDIO){}
    friend std::ostream &operator<<(std::ostream &stream, in_audio* o);

    /*readFrame
      virtual: read a frame from the input stream 
    */
    int readFrame();

 private:

};


#endif
