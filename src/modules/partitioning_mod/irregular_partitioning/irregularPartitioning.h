/**
 * @file irregularPartitioning.h
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief
 * @version 2.1
 * @date 2019-02-05
 *
 * @copyright Compiii Thesis (c) 2019
 *
 */
#ifndef IRREGULARPARTITIONING_H_
#define IRREGULARPARTITIONING_H_

#include "partitioning.h"
#include "irregularPartitioningWithP.h"
#include "irregularPartitioningWithSqrt2P.h"

typedef struct ConstructBlockDataAttribute_T ConstBlockDataAttr;
struct ConstructBlockDataAttribute_T
{
    int step;
    int diag;
    int nbBlock; // number of blocks before the diagonal block id (it is not easy to define)
    int tmp;
    int fragLevel;
    int iter; // iterator
    int initTmp;
    int evalLower; // default value is 1 because the evaluation begins to the lower triangular matrix
};

MPI_Datatype CoordMpiDt, DimMpiDt, BlockCoreDataMpiDt, BlockDataMpiDt;

void initIrregPartMod();
void initIrregPart4S(TabBlock *tab);

void constructAllBlockDataPerProcessor_IP(int rank);
void constructAllBlockPerProcessor_IP(int rank);
BlockData constructBlockDataById(int id, ConstBlockDataAttr *constBlockDataAttr);
ConstBlockDataAttr initBlockDataAttrValue();
int f_IP(int p);

void printConstBlockDataAttr(ConstBlockDataAttr constBlockDataAttr);

int getNbLineBlock_IP(Coord coord, int fragLevel);
int getNbColumnBlock_IP(Coord coord, int fragLevel);
int g_IP(int n, int p);
void createBlockDataAsMPI_Datatype();
void freeMemory_IP();
int getCount(int rank);

void updateAddressBlockData(int rank);
void bcastBlockData(int count, int blockLengths, int stride);

BlockDataList *getNeedBlockDataList_IP(BlockData root, BlockDataList **allNeedBlockData);
void getNeedBlockDataListLine_IP(BlockData root, BlockDataList *allNeedBlockData, BlockData blockData, BlockDataList *blockDataList, int canAdd, int option, int level, int type);
void getNeedBlockDataListColumn_IP(BlockData root, BlockDataList *allNeedBlockData, BlockData blockData, BlockDataList *blockDataList, int canAdd, int option, int level, int type);

BlockDataList *getDependBlockDataList_IP(BlockData root);
void getDependBlockDataListLine_IP(BlockData root, BlockDataList *blockDataList, int option, int level);
void getDependBlockDataListColumn_IP(BlockData root, BlockDataList *blockDataList, int option, int level);

void updateBoundBlockData_IP(int rank);
int getOffsetFirstBound_IP(BlockData root, BlockData blockData);
int getOffsetSecondBound_IP(BlockData root, BlockData blockData);

void constructAllPixaletedBlockDataPerProcessor_IP();
void constructAllPixelatedBlockPerProcessor_IP(int rank);

PixelBlockData constructPixelBlockDataByCoordAndBlockData_IP(Coord coord, BlockData blockData, PixelBlockData **tabPixelBlockData, int address);

#endif