#ifndef IN_VIDEO_STREAM_H
#define IN_VIDEO_STREAM_H

#include "in_stream.h"

class in_video: public in_stream{
 public:
  /*in_video
    constructor, calls the parent class
  */
  in_video():in_stream(AVMEDIA_TYPE_VIDEO){}

    friend std::ostream &operator<<(std::ostream &stream, in_video* o);
    
    /*readFrame
      virtual: read a frame from the input stream 
    */

    int readFrame();
    
 private:

};


#endif
