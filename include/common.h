#ifndef COMMON_H
#define COMMON_H

#define DEBUG 1
#define AUDIO_VIDEO_MAX_DELAY 10
#define INTERLEAVED   1 

#include <string>
#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libswresample/swresample.h> 
#include <libswscale/swscale.h> 
}



//Helper class
class common{

 public:
  //Useful procedure for ffmpeg error handling and reporting
  static const char *get_error_text(const int error){
    static char error_buffer[255];
    av_strerror(error, error_buffer, sizeof(error_buffer));
    return error_buffer;
  }

  static void YUV2RGB(int Y, int Cr, int Cb, char *colors){
    int R,G,B;
    
    R = Y + 1.370705*(Cr-128);
    G = Y - 0.337633 * (Cb-128)-0.698001*(Cr-128);
    B = Y + 1.732446 * (Cb-128);
    
    if(R>255){R=255;  }
    if(G>255){G=255;  }
    if(B>255){B=255;  }
    
    if(R<0){R=0;  }
    if(G<0){G=0;  }
    if(B<0){B=0;  }
    
    R = R*220/256;
    G = G*220/256;
    B = B*220/256;
    
    colors[0] = R;
    colors[1] = G;
    colors[2] = B;
  }

};

#define IF_VAL_REPORT_ERROR_AND_RETURN(VAL, ERRORSTRING)  if(VAL){ this->error->setMessage(ERRORSTRING); return 1; }
#define IF_VAL_NEGATIVE_REPORT_ERROR_AND_RETURN(VAL, ERRORSTRING)  if(VAL<0){ this->error->setMessage(ERRORSTRING); return 1; }

class errorClass{
 public:

  errorClass(){ hasErrorFlag = 0;}

  void setMessage(std::string error){ this->errorMessage = error ; this->hasErrorFlag=1;}
  int  hasError(){ return this->hasErrorFlag;}
  
  /* get and set error messages for external use */
  std::string getMessage(){ return this->errorMessage;};
  std::string getLastMessage(){ return this->errorMessage;};
  
  void printLastMessage(){ std::cout << "Last error was: " << this->errorMessage << std::endl; };
  
  std::string errorMessage;
  int hasErrorFlag;
};



#endif
