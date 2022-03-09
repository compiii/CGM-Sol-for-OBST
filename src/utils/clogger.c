/**
Copyright 2015 Christopher Schneider

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**/



#include <string.h>
#include "clogger.h"

void initLogger(){
//    CCS_LOG_LEVEL = CCS_LOGS_LEVEL_DEFAULT;
}

void makeMessage(FILE *output, const char *tag, const char *func, const int line, const char *format, ...) {
    char arr[CCS_CLogger_BUFFER_SIZE];
    char functionAndLine[strlen(func)+11]; //10 max length of an int, +1;
    memset(functionAndLine, 0,sizeof(functionAndLine));
    sprintf(functionAndLine, "%s:%d",func, line);
    va_list args;

	time_t temps;

    struct tm date;
    time(&temps);
    date=*localtime(&temps);

	va_start(args, format);
	snprintf(arr, CCS_CLogger_BUFFER_SIZE, CCS_CLogger_LOGGER_FORMAT, tag, functionAndLine, asctime(&date), format);
	logFile = fopen(CCS_LOGS_FILENAME,"a+");
	vfprintf(logFile, arr, args);
	fclose(logFile);
	va_end(args);
    
	va_start(args, format);
	if(!strcmp(tag,"ERROR"))
	    snprintf(arr, CCS_CLogger_BUFFER_SIZE, CCS_CLogger_LOGGER_FORMAT_RED, tag, functionAndLine, asctime(&date), format);
	else if (!strcmp(tag,"WARN"))
	    snprintf(arr, CCS_CLogger_BUFFER_SIZE, CCS_CLogger_LOGGER_FORMAT_YEL, tag, functionAndLine, asctime(&date), format);
	else if (!strcmp(tag,"DEBUG"))
	    snprintf(arr, CCS_CLogger_BUFFER_SIZE, CCS_CLogger_LOGGER_FORMAT_GRN, tag, functionAndLine, asctime(&date), format);
	else if (!strcmp(tag,"INFO"))
	    snprintf(arr, CCS_CLogger_BUFFER_SIZE, CCS_CLogger_LOGGER_FORMAT_NRM, tag, functionAndLine, asctime(&date), format);
	else if (!strcmp(tag,"VERBOSE"))
	    snprintf(arr, CCS_CLogger_BUFFER_SIZE, CCS_CLogger_LOGGER_FORMAT_BLU, tag, functionAndLine, asctime(&date), format);
	else if (!strcmp(tag,"MORE"))
	    snprintf(arr, CCS_CLogger_BUFFER_SIZE, CCS_CLogger_LOGGER_FORMAT_MAG, tag, functionAndLine, asctime(&date), format);
	vfprintf(output, arr, args);
    va_end(args);
    fflush(output);
}

void setLogLevel(int logLevel){
 //   CCS_LOG_LEVEL = logLevel;
}
