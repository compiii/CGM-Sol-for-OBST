/**
 * @file compiii_pdpta.h
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-10
 * 
 * @copyright Compiii Thesis Copyright (c) 2021
 * 
 */
#ifndef COMPIII_DSEJ
#define COMPIII_DSEJ

#include "obst.h"

int compiiiDsej();
void communicateBlockDataDSEJ(int type, BlockData src, BlockData dest);
void receiveBlockDataDSEJ(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag, int option);
void sendBlockDataDSEJ(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag, int option);
void computeBlockDSEJ(Block block, int option);
#endif // !COMPIII_DSEJ