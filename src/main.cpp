#include "main.h"
#include "common.h"

#include <iostream>
#include <iomanip>
#include <string>

#include "in_context.h"
#include "out_context.h"

#if DEBUG == 2
#include <stdlib.h>
void my_log_callback(void *ptr, int level, const char *fmt, va_list vargs) { printf("----> ");printf (fmt,vargs);}
#endif


int main(int argc, char *argv[]){

  std::string inputFile;
  std::string outputPrefix;
  std::string outputFile;

  /*--------------- PARSE THE COMMANDLINE -------------*/
 
  //Print Usage if the argument's count is wrong
  if (argc < 3) {
    std::cerr << "\nusage: " << argv[0] << " inputFile outputPrefix\n" << std::endl;
    return 1;
  }

  inputFile = argv[1];
  outputPrefix   = argv[2];
  std::cout << "\nTarget file: \"" << inputFile << "\" using outputPrefix: \"" << outputPrefix << "\" " <<  std::endl;


  /*--------------- INITIALIZE FFMPEG ENVIRONMENT AND BOOTSTRAP INPUT AND OUTPUT CONTEXTS --------------*/

  /* Register al codecs */
  av_register_all();
  
#if DEBUG == 2
  av_log_set_level(AV_LOG_DEBUG);
  av_log_set_callback(my_log_callback);
#endif

  /* Input context, conveys audio and video stream from the input file  */
  in_context   inputContext;

  //open input audio and video stream 
  int err;
  err = inputContext.open(inputFile);

  if(err){
    std::cout << "Error: " << err << " - " << inputContext.error->getLastMessage() << std::endl;
    return 1;
  }

  // Let's print some information about the input file and context
  std::cout << inputContext;

  /* The output context, push audio and video stream information to the output file */
  out_context  outputContext(outputPrefix);

  //open output file (based on the outputprefix) and setup streams, audio and video
  err = outputContext.open();
  if(err){
    std::cout << "Error: " << err << " - " << outputContext.error->getLastMessage() << std::endl;
    return 1;
  }

  //open output video processor (needs some info from the input video stream)
  err = outputContext.initVideoProcessor(inputContext.getVideoCodecContext());
  if(err){
    std::cout << "Error: could not init video processor" << std::endl;
    std::cout << outputContext.error->getMessage() << std::endl;
    return 1;
  }
  
  //open output audio processor (need some info from the input audio stream)
  err = outputContext.initAudioProcessor(inputContext.getAudioCodecContext());
  if(err){
    std::cout << "Error: could not init audio processor" << std::endl;
    std::cout << outputContext.error->getMessage() << std::endl;
    return 1;
  }
  
  //Let's print some info about the output context
  std::cout << outputContext;
  
  //A flag
  int keepGoing = 1;
  
  //And the result holder
  int result;

  //DTS the frame number expressed in the output stream codec timebase, tipically 1/1000 th of a second
  int64_t audio_dts, video_dts;

  int isFirstVideoFrame = 1;
  int blackFramesFlag   = 0;
  int output_close_file = 0;

  float minVolume, currentVolume;

  while(keepGoing){

    audio_dts = outputContext.cur_audio_dts();
    video_dts = outputContext.cur_video_dts();
    //Let's print some data
    std::cout.fill('0');
    std::cout << "Time (mm:ss)  " << std::setw(2)<< video_dts/60000 << ":" << std::setw(2) << (video_dts/1000)%60 << " - "  ;

#if DEBUG == 1
    std::cout << "Video Dts " << video_dts << " - Audio Dts " << audio_dts ;
#endif

    /* We have two input streams and two output streams. 
       We need to get them sorted frame by frame.
       The reason is that we are going to split the input file at the blank frames, but we need also to keep
       audio and video in sync */

    /* So we read frames from the audio OR the video stream using AUDIO_VIDEO_MAX_DELAY as a guide */
    
    if((video_dts - audio_dts) >= AUDIO_VIDEO_MAX_DELAY){

      // We read a frame from the input audio stream 
      result = inputContext.readAudioFrame();
      // In this case we bail out
      if(!result){
	std::cout << std::endl << "No Audio frame read." << std::endl;
	keepGoing = 0;
      }

      //We have a frame to pass to the output audio stream (it will resampled before saving)
      if(inputContext.hasAudioFrame()){
	result = outputContext.saveAudioFrame(inputContext.getAudioFrame(),inputContext.skippedAudioFrames());      

	//We do some cleanup, if needed
	inputContext.resetAudioFrame();
	
	if(result){
	  std::cout << outputContext.error->getLastMessage() << std::endl;
	  return 1;
	}

	if(blackFramesFlag && video_dts > 1000){
	  if(outputContext.isSilentFrame()){
	    output_close_file = 1;
	  }else{
	    currentVolume = outputContext.getAudioVolume();
	    std::cout << std::endl << "AUDIO Volume: "  << currentVolume << std::endl;
	    if(currentVolume < minVolume){
	      minVolume = currentVolume;
	    }else{
	      //The volume is raising again, it's the same as a silent frame
	      output_close_file = 1;
	    }
	  }
	}
      }

   }else{

      //We read a video frame from the input stream
      result = inputContext.readVideoFrame();

      //In this case we bail out
      if(!result){
	std::cout << std::endl <<  "No Video frame read." << std::endl;
	keepGoing = 0;
      }
      
      //We have a frame to pass wto the output video stream (it be resampled before saving)
      if(inputContext.hasVideoFrame()){
	result = outputContext.saveVideoFrame(inputContext.getVideoFrame(),inputContext.skippedVideoFrames());      
	//we do some cleanup, if needed
	inputContext.resetVideoFrame();	

	if(result){
	  std::cout << outputContext.error->getLastMessage() << std::endl;
	  return 1;
	}

	if(outputContext.isBlackFrame()){
	  if(!isFirstVideoFrame){
	    std::cout << std::endl <<  " - BLACK FRAME " << std::endl;
	    blackFramesFlag++ ; 
	    
	    minVolume = outputContext.getAudioVolume();
	    
	  }
	}else{
	  blackFramesFlag = 0 ; 
	  isFirstVideoFrame = 0;
	}

      }
    }

    if(output_close_file){
      //Reset the flags
      output_close_file  = 0;
      blackFramesFlag = 0 ; 
      isFirstVideoFrame = 1;
      
      outputContext.close();
      outputContext.open();

    }

    
#if DEBUG == 2
   std::cout << std::endl;
#else
   std::cout << "\r";
#endif
   
  }
  
  //We close the output file
  outputContext.close();
  
  // We are done.
  std::cout << std::endl << "End" << std::endl;

  return 0;
}

