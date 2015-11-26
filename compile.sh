#!/bin/sh

c++ src/main.cpp src/in_context.cpp src/in_stream.cpp  src/in_video.cpp src/in_audio.cpp   -o build/videosplitter -I include  -lavcodec -lavformat -lavutil -lswscale -lswresample
