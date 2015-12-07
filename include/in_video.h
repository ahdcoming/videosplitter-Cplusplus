#ifndef IN_VIDEO_STREAM_H
#define IN_VIDEO_STREAM_H

#include "in_stream.h"

class in_video: public in_stream{
 public:
  in_video():in_stream(AVMEDIA_TYPE_VIDEO){}
    friend std::ostream &operator<<(std::ostream &stream, in_video* o);

    //virtuals
    int readFrame();
    
 private:

};


#endif
