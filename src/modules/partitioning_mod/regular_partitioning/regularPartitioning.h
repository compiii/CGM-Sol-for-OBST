/**
 * @file regularPartitioning.h
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-04
 * 
 * @copyright Compiii Thesis (c) 2019 
 * 
 */
#ifndef REGULARPARTITIONING_H_
#define REGULARPARTITIONING_H_

#include "partitioning.h"

void initRegPartMod();

void constructAllBlockDataPerProcessor_RP(int rank);
void constructAllBlockPerProcessor_RP(int rank);
int f_RP(int p);

int getNbLineBlock_RP(Coord coord, int fragLevel);
int getNbColumnBlock_RP(Coord coord, int fragLevel);
int g_RP(int n, int p);
void freeMemory_RP();

BlockDataList *getNeedBlockDataList_RP(BlockData root, BlockDataList **allNeedBlockData);
void getNeedBlockDataListLine_RP(BlockData root, BlockDataList *allNeedBlockData, BlockData blockData, BlockDataList *blockDataList, int canAdd, int option, int level, int type);
void getNeedBlockDataListColumn_RP(BlockData root, BlockDataList *allNeedBlockData, BlockData blockData, BlockDataList *blockDataList, int canAdd, int option, int level, int type);

BlockDataList *getDependBlockDataList_RP(BlockData root);
void getDependBlockDataListLine_RP(BlockData root, BlockDataList *blockDataList, int option, int level);
void getDependBlockDataListColumn_RP(BlockData root, BlockDataList *blockDataList, int option, int level);

void updateBoundBlockData_RP(int rank);
int getOffsetFirstBound_RP(BlockData root, BlockData blockData);
int getOffsetSecondBound_RP(BlockData root, BlockData blockData);

void constructAllPixaletedBlockDataPerProcessor_RP();
void constructAllPixelatedBlockPerProcessor_RP(int rank);

PixelBlockData constructPixelBlockDataByCoordAndBlockData_RP(Coord coord, BlockData blockData, PixelBlockData **tabPixelBlockData, int address);

#endif