#!/bin/sh
mkdir -p $1/src/main/cpp/fdk-aac/libSBRdec/include/log/
echo "void android_errorWriteLog(int i, const char *string){}" > $1/src/main/cpp/fdk-aac/libSBRdec/include/log/log.h
