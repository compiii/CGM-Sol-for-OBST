/**
 * @file compiii_4s.h
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-14
 *
 * @copyright Compiii Thesis Copyright (c) 2022
 *
 */
#ifndef COMPIII_4S
#define COMPIII_4S

#include "obst.h"

typedef struct ReceivedList4s_T ReceivedList4s;
struct ReceivedList4s_T
{
    int id;
    int tag;
    Coord coord;
    ReceivedList4s *next;
};

ReceivedList4s *receivedList4s;

int compiii4s();
void computeBlock4s(Block block, int option);
void computePixelBlock4s(PixelBlockData pixel, int option);

void receivePixelBlockData4s(PixelBlockData src, BlockData dest);

ReceivedList4s *createReceivedList4s();
ReceivedList4s *addTagToReceivedList4s(ReceivedList4s *receivedList, int id, int tag, Coord coord);
int isEmptyReceivedList4s(ReceivedList4s *receivedList);
int findTagToReceivedList4s(ReceivedList4s *receivedList, int id, int tag, Coord coord);
PixelBlockDataList *reverse(PixelBlockDataList *list);

#endif // !COMPIII_4s