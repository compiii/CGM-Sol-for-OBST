/**
 * @file output.h
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 1.1
 * @date 2019-11-05
 * 
 * @copyright Compiii Thesis Copyright (c) 2019
 * 
 */
#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "main.h"

#define MAX_SIZE_STRING 1024
#define CONVERT_TO_MEGABYTE 1048576.0

typedef struct GeneralInfo_T GeneralInfo;
struct GeneralInfo_T
{
    char softName[MAX_SIZE_STRING];
    char softVersion[MAX_SIZE_STRING];
    char dataset[MAX_SIZE_STRING];
    char algorithm[MAX_SIZE_STRING];
    char partitioningTechnique[MAX_SIZE_STRING];
    char processorName[MAX_SIZE_STRING];
    int algorithmId;
    int partitioningTechniqueId;
    int maxProcess;
    int rank;
};

typedef struct InputInfo_T InputInfo;
struct InputInfo_T
{
    int maxNumberOfKeys;
    int maxFragmentation;
};

typedef struct OutputInfo_T OutputInfo;
struct OutputInfo_T
{
    float obst;
    double totalTime;
    double calculTime;
    double comTime;
    double idlenessTime;
    int nbMessageExchange;
    int sizeDataCom;
};

void saveStatistic(char *filename, GeneralInfo genInfo, InputInfo inInfo, OutputInfo outInfo);

#endif