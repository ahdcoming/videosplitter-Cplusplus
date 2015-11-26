#ifndef IN_AUDIO_STREAM_H
#define IN_AUDIO_STREAM_H

#include <string>
#include <iostream>

#include "in_stream.h"

extern "C" {
#include <libavformat/avformat.h>
}

class in_audio: public in_stream{
 public:
  in_audio():in_stream(AVMEDIA_TYPE_AUDIO){}
    friend std::ostream &operator<<(std::ostream &stream, in_audio* o);
    
 private:

};


#endif
