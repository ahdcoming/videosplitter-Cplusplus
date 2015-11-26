#include "main.h"

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

  std::cout << outputContext;

#if DEBUG == 1
 av_log_set_level(AV_LOG_DEBUG);
 av_log_set_callback(my_log_callback);
#endif

  
  // We are done.
  std::cout << "End" << std::endl;

  return 0;
}

