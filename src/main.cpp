#include "main.h"
#include "common.h"

#include <iostream>
#include <string>
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

#include "in_context.h"
#include "out_context.h"

#if DEBUG == 1
#include <stdlib.h>
void my_log_callback(void *ptr, int level, const char *fmt, va_list vargs) { printf("----> ");printf (fmt,vargs);}
#endif

int main(int argc, char *argv[]){

  std::string inputFile;
  std::string outputPrefix;
  std::string outputFile;
  int outputCounter = 0;

  //Let's parse the commandline
 
  //Print Usage if the argument's count is wrong
  if (argc < 3) {
    std::cerr << "\nusage: " << argv[0] << " inputFile outputPrefix\n" << std::endl;
    return 1;
  }

  inputFile = argv[1];
  outputPrefix   = argv[2];
  std::cout << "\nTarget file: \"" << inputFile << "\" using outputPrefix: \"" << outputPrefix << "\" " <<  std::endl;

  av_register_all();

#if DEBUG == 1
 av_log_set_level(AV_LOG_DEBUG);
 av_log_set_callback(my_log_callback);
#endif


  in_context   inputContext;
  //open input streams, audio and video
  int err;
  err = inputContext.open(inputFile);
  if(err){
    std::cout << "Error: " << err << " - " << inputContext.getLastErrorMessage() << std::endl;
    return 1;
  }

  std::cout << inputContext;

  out_context   outputContext;
  outputFile = outputPrefix;
  outputFile.append(outputCounter + ".wmv");

  //open input streams, audio and video
  err = outputContext.open(outputFile);
  if(err){
    std::cout << "Error: " << err << " - " << outputContext.getLastErrorMessage() << std::endl;
    return 1;
  }

  //open input streams, audio and video
  err = outputContext.initVideoProcessor(inputContext.getVideoCodecContext());
  if(err){
    std::cout << "Error: could not init video processor" << std::endl;
    return 1;
  }

  //open input streams, audio and video
  err = outputContext.initAudioProcessor(inputContext.getAudioCodecContext());
  if(err){
    std::cout << "Error: could not init audio processor" << std::endl;
    std::cout << outputContext.getErrorMessage() << std::endl;
    return 1;
  }

  

  std::cout << outputContext;


 int keepGoing = 1;
 int result;


 while(keepGoing){
   std::cout << "********Cycle*********** " <<  std::endl;

   std::cout << "Video Dts " << outputContext.cur_video_dts() << " - Audio Dts " << outputContext.cur_audio_dts() << std::endl;

   if((outputContext.cur_video_dts() - outputContext.cur_audio_dts()) >= AUDIO_VIDEO_MAX_DELAY){
     result = inputContext.readAudioFrame();
     std::cout << "Read audio frame!" << std::endl;

     if(!result){
       std::cout << "Audio bailed out!" << std::endl;
       keepGoing = 0;
     }

     if(inputContext.hasAudioFrame()){
       
       std::cout << "Has audio frame! " << std::endl;
	    
      
       //TODO Is Silent!!!
       //TODO is black

       result = outputContext.saveAudioFrame(inputContext.getAudioFrame());
       inputContext.resetAudioFrame();

       if(result){
	 return 1;
       }
       
     }
     
   }else{
     result = inputContext.readVideoFrame();

     std::cout << "Read video frame!" << std::endl;

      if(!result){
	std::cout << "Video bailed out!" << std::endl;
	keepGoing = 0;
      }
      
      if(inputContext.hasVideoFrame()){
	std::cout << "Has video frame! " << std::endl;
	result = outputContext.saveVideoFrame(inputContext.getVideoFrame());      
	inputContext.resetVideoFrame();	

	if(result){
	  return 1;
	}
      }
   }
 }

 outputContext.close();
  
  // We are done.
  std::cout << "End" << std::endl;

  return 0;
}
