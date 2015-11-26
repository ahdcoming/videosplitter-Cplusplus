#ifndef IN_VIDEO_STREAM_H
#define IN_VIDEO_STREAM_H

#include <string>
#include <iostream>

#include "in_stream.h"

extern "C" {
#include <libavformat/avformat.h>
}

class in_video: public in_stream{
 public:
  in_video():in_stream(AVMEDIA_TYPE_VIDEO){}
    friend std::ostream &operator<<(std::ostream &stream, in_video* o);
    
 private:

};


#endif
