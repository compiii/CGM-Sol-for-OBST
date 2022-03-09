/**
 * @file obst.h
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-10
 * 
 * @copyright Compiii Thesis Copyright (c) 2021
 * 
 */
#ifndef OBST_H_
#define OBST_H_

#include "main.h"

typedef struct SentList_T SentList;
struct SentList_T
{
    PixelBlockData src;
    BlockData dest;
    SentList *next;
};

SentList *sentList;

SentList *createSentList();
int isEmptySentList(SentList *list);
SentList *addDataToSentList(SentList *list, PixelBlockData src, BlockData dest);
BlockDataList *removeBlockDataToBlockDataList(BlockDataList *list, BlockData blockData);
BlockDataList *deleteOccurBlockList(BlockDataList *list);
BlockDataList *buildSendBlockDataList(BlockDataList *list1, BlockDataList *list2);

void sendPixelBlockData(PixelBlockData src, BlockDataList *dependBlockList, int finalized);
void receivePixelBlockData(PixelBlockData src, BlockData dest);

typedef struct ReceivedList_T ReceivedList;
struct ReceivedList_T
{
    int tag;
    ReceivedList *next;
};

ReceivedList *receivedList, *sendedList;

float **tree;
int **cut;

void initOBSTMod();
int knuthAlgorithm();
float getTree(int i, int j);
int getCut(int i, int j);
float w(long i, long j);
int max(int n, int m);
int min(int m, int n);

void computeOBST(PixelBlockData pixel, int begin, int end, int init, int option);
void communicateBlockData(int type, PixelBlockData src, BlockData blockData);
void receiveBlockData(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag);
void sendBlockData(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag);

ReceivedList *createReceivedList();
ReceivedList *addTagToReceivedList(ReceivedList *receivedList, int tag);
int isEmptyReceivedList(ReceivedList *receivedList);
int findTagToReceivedList(ReceivedList *receivedList, int tag);

#endif
