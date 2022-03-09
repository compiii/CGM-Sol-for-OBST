/**
 * @file partitioning.h
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief
 * @version 1.1
 * @date 2019-11-05
 *
 * @copyright Compiii Thesis Copyright (c) 2019
 *
 */
#ifndef PARTITIONING_H_
#define PARTITIONING_H_

#include "main.h"

void printBlock(Block block);
// void printBlockData(BlockData blockData);
void printTabBlock(Block *tabBlock);
void printTabBlockData(BlockData *tabBlockData);

// int g(int n, int p);

int getSizeBlock(Dim dim);
int getDiagByCoord(Coord coord);

BlockData getBlockDataByCoord(Coord coord);

BlockDataList *createBlockDataList();
int isEmptyBlockDataList(BlockDataList *blockDataList);
void addBlockDataToBlockDataList(BlockDataList *blockDataList, BlockData blockData, int option, int level);
int findBlockDataToBlockDataList(BlockDataList *blockDataList, BlockData blockData);
void mergeBlockDataList(BlockDataList **list1, BlockDataList *list2);
void printBlockDataList(BlockDataList *blockDataList);

// https://stackoverflow.com/questions/22222156/copying-linked-list-in-c
BlockDataList *copyBlockDataList(BlockDataList *list);

// void constructAllPixaletedBlockDataPerProcessor();
int getFirstIdPixelBlock(BlockData blockData);
int determineNbLinePixelBlockRecv(int line, int fragLevel, int nbLineBlock);
int determineNbColumnPixelBlockRecv(int column, int fragLevel, int nbColumnBlock);
// PixelBlockData constructPixelBlockDataByCoordAndBlockData(Coord coord, BlockData blockData, PixelBlockData **tabPixelBlockData, int address);
// void printPixelBlockData(PixelBlockData pBlockData);
void printPixelatedBlockData(PixelatedBlockData pBlockData);
int determineGapLineBetweenTwoBlockData(BlockData root, BlockData blockData);
int determineGapColumnBetweenTwoBlockData(BlockData root, BlockData blockData);
// void constructAllPixelatedBlockPerProcessor(int rank);
void getNeedPixelBlockDataListLine(BlockData root, BlockData blockData, PixelBlockDataListByLineOrColumn *pixelBlockDataList);
void getNeedPixelBlockDataListColumn(BlockData root, BlockData blockData, PixelBlockDataListByLineOrColumn *pixelBlockDataList);
void getDependPixelBlockDataListLine(BlockData root, BlockData blockData, BlockDataListByLineOrColumn *blockDataList);
void getDependPixelBlockDataListColumn(BlockData root, BlockData blockData, BlockDataListByLineOrColumn *blockDataList);

PixelBlockDataListByLineOrColumn createPixelBlockDataListByLineOrColumn(int nbLineOrColumn);
// PixelBlockDataList *createPixelBlockDataList();
// int isEmptyPixelBlockDataList(PixelBlockDataList *blockDataList);
void addBlockDataToPixelBlockDataList(PixelBlockDataList *blockDataList, PixelBlockData blockData);
BlockDataListByLineOrColumn createBlockDataListByLineOrColumn(int nbLineOrColumn);

#endif