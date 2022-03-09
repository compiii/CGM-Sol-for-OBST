#ifndef MAIN_H_
#define MAIN_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "mpi.h"

int algorithmId;
int partitioningTechniqueId;
char algorithmName[100], partitioningName[100];

double startTime, tmpDouble, endTime, calculTime, comTime, comRate, calculRate, totalTime, idlenessTime, idlenessRate;
int sizeDataCom, nbMessageExchange;
float obst;

int maxProcess,
    rank, processorNameSize;
char processorName[MPI_MAX_PROCESSOR_NAME];

/**
 * @brief Definition of the Coord_T data structure that refers to the coordinates of a block
 *
 */
typedef struct Coord_T Coord;
struct Coord_T
{
    int i;
    int j;
};

/**
 * @brief Definition of the Dimension_T data structure that refers to the dimensions of a block
 *
 */
typedef struct Dimension_T Dim;
struct Dimension_T
{
    int nbLine;
    int nbColumn;
};

/**
 * @brief Definition of the BlockCoreData_T data structure that refers to the core data of
 * a block
 *
 */
typedef struct BlockCoreData_T BlockCoreData;
struct BlockCoreData_T
{
    int id;
    Coord coord;
    int address;
    Dim dim;
    int diag;
    Coord firstBound;
    Coord secondBound;
};

/**
 * @brief Definition of the BlockData_T data structure that refers to the data of a block
 *
 */
typedef struct BlockData_T BlockData;
struct BlockData_T
{
    BlockCoreData coreData;
    int fragLevel;
    int rank;
};

/**
 * @brief Definition of the PixelBlockData_T data structure that refers to the data of
 * a pixel block. It define to a PixelBlockCoreData type which is a struct BlockCoreData_T.
 *
 */
typedef struct BlockCoreData_T PixelBlockCoreData;
typedef struct PixelBlockData_T PixelBlockData;
struct PixelBlockData_T
{
    PixelBlockCoreData coreData;
    int idBlockData;
};

/**
 * @brief Definition of the BlockDataList_T data structure that refers to the list of BlockData
 *
 */
typedef struct BlockDataList_T BlockDataList;
struct BlockDataList_T
{
    BlockData blockData;
    int isLine;
    int isColumn;
    BlockDataList *next;
};

/**
 * @brief Definition of the Block_T data structure
 *
 */
typedef struct Block_T Block;
struct Block_T
{
    BlockData blockData;
    BlockDataList *needBlockDataList;
    BlockDataList *dependBlockDataList;
};

/**
 * @brief Definition of the PixelBlockDataList_T data structure that refers to the list of
 * PixelBlockData
 *
 */
typedef struct PixelBlockDataList_T PixelBlockDataList;
struct PixelBlockDataList_T
{
    PixelBlockData pixelBlockData;
    PixelBlockDataList *next;
};

/**
 * @brief Definition of the PixelBlockDataListByLineOrColumn_T data structure that refers to the list of
 * PixelBlockData by line or column
 *
 */
typedef struct PixelBlockDataListByLineOrColumn_T PixelBlockDataListByLineOrColumn;
struct PixelBlockDataListByLineOrColumn_T
{
    int nbLineOrColumn;
    PixelBlockDataList **pixelBlockDataList;
};

/**
 * @brief Definition of the BlockDataListByLineOrColumn_T data structure that refers to the list of
 * BlockData by line or column
 */
typedef struct BlockDataListByLineOrColumn_T BlockDataListByLineOrColumn;
struct BlockDataListByLineOrColumn_T
{
    int nbLineOrColumn;
    BlockDataList **blockDataList;
};

/**
 * @brief Definition of the PixelatedBlockData_T data structure
 *
 */
typedef struct PixelatedBlockData_T PixelatedBlockData;
struct PixelatedBlockData_T
{
    int idBlockData;
    int nbEvaluatePixel;
    int maxDiag;
    PixelBlockData **pixelBlockDataTab;
};

/**
 * @brief Definition of the PixelatedBlock_T data structure
 *
 */
typedef struct PixelatedBlock_T PixelatedBlock;
struct PixelatedBlock_T
{
    PixelatedBlockData pBlockData;
    PixelBlockDataListByLineOrColumn needPixelBlockDataListByLine;
    PixelBlockDataListByLineOrColumn needPixelBlockDataListByColumn;
    PixelBlockDataListByLineOrColumn needAllPixelBlockDataListByLine;
    PixelBlockDataListByLineOrColumn needAllPixelBlockDataListByColumn;
    BlockDataListByLineOrColumn dependBlockDataListByLine;
    BlockDataListByLineOrColumn dependBlockDataListByColumn;
};

typedef struct TabBlock_T TabBlock;
struct TabBlock_T
{
    BlockData *tabBlockData;
    Block *tabBlock;
    PixelatedBlockData *tabPixelatedBlockData;
    PixelatedBlock *tabPixelatedBlock;
};

TabBlock *tabs;

MPI_Datatype vector, vectorBis;
int maxNumber, maxFrag, maxDiag, maxBlock, maxBlockDiag, maxEvalBlock, maxSizeBlock;
int *tabBlockDataCoord;
float *tabP, *tabQ;
BlockData *tabBlockData;
Block *tabBlock;
int isEmptyBlockDataList(BlockDataList *blockDataList);
int isEmptyPixelBlockDataList(PixelBlockDataList *blockDataList);
int getIdBlockDataByCoord(Coord coord);
BlockDataList *createBlockDataList();
void addBlockDataToBlockDataList(BlockDataList *blockDataList, BlockData blockData, int option, int level);

int maxPixelatedBlock;
PixelatedBlockData *tabPixelatedBlockData;
PixelatedBlock *tabPixelatedBlock;
void printBlockData(BlockData blockData);
void printPixelBlockData(PixelBlockData pBlockData);

int bufsize, idReq, flag, readyToRecv;
char *buf;
MPI_Request *tabReq;

int verboseDebug;

PixelBlockDataList *createPixelBlockDataList();
#endif