/**
 * @file compiii_pdpta.c
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 1.2
 * @date 2020-07-17
 * 
 * @copyright Compiii Thesis Copyright (c) 2020
 * 
 */

#include "compiii_dsej.h"
#include "clogger.h"

int compiiiDsej()
{
    long d = 0;
    int i = 0, last = 0;
    Block block;
    //Coord coord;
    for (d = 1; (d <= maxDiag) && (i != maxEvalBlock); d++)
    {
        block = tabBlock[i];
        while (block.blockData.coreData.diag == d && (i != maxEvalBlock))
        {
            if (d == maxDiag)
                last = 1;
            if (!isEmptyBlockDataList(block.needBlockDataList))
            {
                BlockDataList *path = block.needBlockDataList;
                while (path != NULL)
                {
                    BlockData src = path->blockData;
                    tmpDouble = MPI_Wtime();
                    communicateBlockDataDSEJ(0, src, block.blockData);
                    comTime += MPI_Wtime() - tmpDouble;
                    path = path->next;
                }
            }

            tmpDouble = MPI_Wtime();
            computeBlockDSEJ(block, (block.blockData.coreData.diag == 1 ? 0 : 1));
            calculTime += MPI_Wtime() - tmpDouble;

            if (!isEmptyBlockDataList(block.dependBlockDataList))
            {
                BlockDataList *path = block.dependBlockDataList;
                while (path != NULL)
                {
                    BlockData dest = path->blockData;
                    tmpDouble = MPI_Wtime();
                    communicateBlockDataDSEJ(1, block.blockData, dest);
                    comTime += MPI_Wtime() - tmpDouble;
                    path = path->next;
                }
            }

            i++;
            block = tabBlock[i];
        }
    }

    if (rank == 10000)
    {
        int i = 0;
        int j = 0;
        printf("**********************************************************************\n");
        printf("*                       Dynamic programming Table                    *\n");
        printf("**********************************************************************\n");
        for (i = 1; i <= maxNumber; i++)
        {
            for (j = 1; j <= maxNumber; j++)
            {
                if (i > j)
                    printf("\t");
                else
                    printf("%f\t", getTree(i, j));
            }
            printf("\n");
        }
    }

    return (last ? getTree(1, maxNumber) : 0);
}

void communicateBlockDataDSEJ(int type, BlockData src, BlockData dest)
{
    int count, blockLengths, stride, absc, ord; //, option = 0, idCurrentDestBlockData;

    count = src.coreData.dim.nbLine;
    blockLengths = src.coreData.dim.nbColumn;
    stride = maxNumber + 1;
    MPI_Type_vector(count, blockLengths, stride, MPI_FLOAT, &vector);
    MPI_Type_commit(&vector);
    MPI_Type_vector(count, blockLengths, stride, MPI_INT, &vectorBis);
    MPI_Type_commit(&vectorBis);
    absc = src.coreData.firstBound.i;
    ord = src.coreData.secondBound.i;

    if (rank == 1 && type == 100)
        logD("\ntag = %d => %d + %d * %d + %d == coord bloack data (%d, %d) of process %d\n", getIdBlockDataByCoord(dest.coreData.coord) + src.coreData.id * src.coreData.coord.i + src.coreData.coord.j, getIdBlockDataByCoord(dest.coreData.coord), src.coreData.id, src.coreData.coord.i, src.coreData.coord.j, dest.coreData.coord.i, dest.coreData.coord.j, dest.rank);

    if (type == 1)
    {
        sizeDataCom += blockLengths;
    }

    if (type == 0)
    {
        receiveBlockDataDSEJ(tabBlockData[src.coreData.id], vector, absc, ord, src.coreData.address, 0);
        receiveBlockDataDSEJ(tabBlockData[src.coreData.id], vectorBis, absc, ord, src.coreData.address, 1);
    }
    else
    {
        sendBlockDataDSEJ(dest, vector, absc, ord, src.coreData.address, 0);
        sendBlockDataDSEJ(dest, vectorBis, absc, ord, src.coreData.address, 1);
    }
    MPI_Type_free(&vector);
}

void receiveBlockDataDSEJ(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag, int option)
{
    if (option == 0)
        MPI_Recv(&tree[absc][ord], 1, vector, blockData.rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    else
        MPI_Recv(&cut[absc][ord], 1, vector, blockData.rank, tag + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void sendBlockDataDSEJ(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag, int option)
{
    if (option == 0)
        MPI_Isend(&tree[absc][ord], 1, vector, blockData.rank, tag, MPI_COMM_WORLD, &tabReq[idReq++]);
    else
        MPI_Isend(&cut[absc][ord], 1, vector, blockData.rank, tag + 1, MPI_COMM_WORLD, &tabReq[idReq++]);
}

void computeBlockDSEJ(Block block, int option)
{
    int d, i, k;
    float tmp;

    if (option == 0)
    {
        for (d = 1; d <= block.blockData.coreData.firstBound.j; d++)
        {
            for (i = block.blockData.coreData.secondBound.i; i <= (block.blockData.coreData.secondBound.j - d + 1); i++)
            {
                tree[i][i + d - 1] = (i == (i + d - 1)) ? w(i, i) : INT_MAX;
                cut[i][i + d - 1] = (i == (i + d - 1)) ? i : INT_MAX;
                if ((i != (i + d - 1)))
                    for (k = getCut(i, i + d - 2); k <= getCut(i + 1, i + d - 1) && k != i + d - 1; k++)
                    {
                        tmp = getTree(i, k) + getTree(k + 1, i + d - 1) + w(i, i + d - 1);
                        if (tmp < getTree(i, i + d - 1))
                        {
                            tree[i][i + d - 1] = tmp;
                            cut[i][i + d - 1] = k;
                        }
                    }
            }
        }
    }
    else if (option == 1)
    {
        for (d = block.blockData.coreData.firstBound.j; d >= block.blockData.coreData.firstBound.i; d--)
        {
            for (i = block.blockData.coreData.secondBound.i; i <= block.blockData.coreData.secondBound.j; i++)
            {
                tree[d][i] = INT_MAX;
                //for (k = d; k <= i - 1; k++)
                for (k = getCut(d, i - 1); k <= getCut(d + 1, i) && k != i; k++)
                {
                    tmp = getTree(d, k) + getTree(k + 1, i) + w(d, i);
                    if (tmp < tree[d][i])
                    {
                        tree[d][i] = tmp;
                        cut[d][i] = k;
                    }
                }
            }
        }
    }
}