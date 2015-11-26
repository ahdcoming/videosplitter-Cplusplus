#include "main.h"
#include "in_context.h"

#include <iostream>
#include <string>

#if DEBUG == 1
#include <stdlib.h>
void my_log_callback(void *ptr, int level, const char *fmt, va_list vargs) { printf("----> ");printf (fmt,vargs);}
#endif

int main(int argc, char *argv[]){

  std::string filename;
  std::string prefix;

  //Let's parse the commandline
 
  //Print Usage if the argument's count is wrong
  if (argc < 3) {
    std::cerr << "\nusage: " << argv[0] << " inputFile outputPrefix\n" << std::endl;
    return 1;
  }

  filename = argv[1];
  prefix   = argv[2];
  std::cout << "\nTarget file: \"" << filename << "\" using prefix: \"" << prefix << "\" " <<  std::endl;

  av_register_all();

  in_context   inputStream;
  //open input streams, audio and video
  int err;
  if(err = inputStream.open(filename)){
    std::cout << "Error: " << err << " - " << inputStream.getLastErrorMessage() << std::endl;
    return 1;
  }

 
  std::cout << inputStream;

#if DEBUG == 1
 av_log_set_level(AV_LOG_DEBUG);
 av_log_set_callback(my_log_callback);
#endif

  
  // We are done.
  std::cout << "End" << std::endl;

  return 0;
}

