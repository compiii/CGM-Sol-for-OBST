/**
 * @file regularPartitioning.c
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-04
 * 
 * @copyright Compiii Thesis (c) 2019 
 * 
 */
#include "regularPartitioning.h"
#include "clogger.h"

void initRegPartMod()
{
    MPI_Comm_size(MPI_COMM_WORLD, &maxProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processorName, &processorNameSize);
    maxFrag = 0;

    maxBlockDiag = f_RP(maxProcess);
    maxDiag = maxBlockDiag;
    maxBlock = ((maxBlockDiag + 1) * maxBlockDiag) / 2.0;
    maxEvalBlock = 0;
    maxPixelatedBlock = maxEvalBlock;
    tabBlockData = (BlockData *)malloc(maxBlock * sizeof(BlockData));
    tabPixelatedBlockData = (PixelatedBlockData *)malloc(maxBlock * sizeof(PixelatedBlockData));

    Coord coord;
    coord.i = maxBlockDiag;
    coord.j = maxDiag + 1;
    tabBlockDataCoord = (int *)malloc((getIdBlockDataByCoord(coord) + 1) * sizeof(int));

    constructAllBlockDataPerProcessor_RP(rank);
    constructAllBlockPerProcessor_RP(rank);
    //MPI_Barrier(MPI_COMM_WORLD);
    constructAllPixaletedBlockDataPerProcessor_RP();
    constructAllPixelatedBlockPerProcessor_RP(rank);
}

void constructAllBlockDataPerProcessor_RP(int rank)
{
    int id = 0, diag = 1, nbBlock = maxBlockDiag, step = 1, tmp = 0;
    BlockData blockData;
    while (id < maxBlock)
    {

        Coord coord;
        coord.i = step++;
        coord.j = diag + coord.i;
        BlockCoreData coreData;
        coreData.id = id;
        coreData.coord = coord;
        coreData.diag = diag;
        coreData.dim.nbLine = getNbLineBlock_RP(coord, 0);
        coreData.dim.nbColumn = getNbColumnBlock_RP(coord, 0);
        coreData.address = tmp + (id == 0 ? 0 : getSizeBlock(tabBlockData[id - 1].coreData.dim));
        tmp = coreData.address;
        blockData.coreData = coreData;
        blockData.fragLevel = 0;
        blockData.rank = (diag % 2 != 0 ? blockData.coreData.coord.i - 1 : blockData.coreData.coord.i - 1 + diag - 1);
        maxPixelatedBlock = maxEvalBlock += (rank == blockData.rank ? 1 : 0);

        tabBlockData[id] = blockData;
        tabBlockDataCoord[getIdBlockDataByCoord(blockData.coreData.coord)] = blockData.coreData.id;
        id++;

        if (id == nbBlock)
        {
            nbBlock += maxBlockDiag - diag;
            diag++;
            step = 1;
        }
    }
    updateBoundBlockData_RP(rank);
    tabBlock = (Block *)malloc(maxEvalBlock * sizeof(Block));
    tabPixelatedBlock = (PixelatedBlock *)malloc(maxEvalBlock * sizeof(PixelatedBlock));
}

int f_RP(int p)
{
    return p;
}

int getNbLineBlock_RP(Coord coord, int fragLevel)
{
    int diag = coord.j - coord.i;

    if (diag == 1 && coord.j == (maxBlockDiag + 1))
        return maxNumber - (maxBlockDiag - 1) * g_RP(maxNumber, maxProcess);
    else
        return g_RP(maxNumber, maxProcess);
}

int getNbColumnBlock_RP(Coord coord, int fragLevel)
{
    if (coord.j == (maxBlockDiag + 1))
        return maxNumber - (maxBlockDiag - 1) * g_RP(maxNumber, maxProcess);
    else
        return g_RP(maxNumber, maxProcess);
}

int g_RP(int n, int p)
{
    int size = (int)ceil(n / (float)f_RP(p));
    if (size * (f_RP(p) - 1) >= n)
        return (int)floor(n / (float)f_RP(p));
    else
        return size;
}

void updateBoundBlockData_RP(int rank)
{
    int id = 0, offset = 0;
    while (id < maxBlock)
    {
        offset = getOffsetFirstBound_RP(tabBlockData[id], tabBlockData[id]);
        tabBlockData[id].coreData.firstBound.i = 1 + offset;
        tabBlockData[id].coreData.firstBound.j = tabBlockData[id].coreData.dim.nbLine + offset;

        offset = getOffsetSecondBound_RP(tabBlockData[id], tabBlockData[id]);
        tabBlockData[id].coreData.secondBound.i = 1 + offset;
        tabBlockData[id].coreData.secondBound.j = tabBlockData[id].coreData.dim.nbColumn + offset;

        id++;
    }
}

int getOffsetFirstBound_RP(BlockData root, BlockData blockData)
{
    Coord coord = blockData.coreData.coord, c;

    if (coord.i == 1)
        return 0;
    c = coord;
    c.i--;
    blockData = getBlockDataByCoord(c);
    return (blockData.coreData.id == root.coreData.id ? 0 : blockData.coreData.dim.nbLine) + getOffsetFirstBound_RP(root, blockData);
}

int getOffsetSecondBound_RP(BlockData root, BlockData blockData)
{
    int diag = getDiagByCoord(blockData.coreData.coord);
    Coord coord = blockData.coreData.coord;

    if (diag == 1)
    {
        return (coord.i - 1) * g_RP(maxNumber, maxProcess);
    }
    else
    {
        coord.j--;
    }
    blockData = getBlockDataByCoord(coord);
    return (blockData.coreData.id == root.coreData.id ? 0 : blockData.coreData.dim.nbColumn) + getOffsetSecondBound_RP(root, blockData);
}

void freeMemory_RP()
{
    free(tabBlockData);
    free(tabBlockDataCoord);
    free(tabBlock);
    free(tabPixelatedBlockData);
    free(tabPixelatedBlock);
}

BlockDataList *getNeedBlockDataList_RP(BlockData root, BlockDataList **allNeedBlockData)
{
    BlockDataList *needBlockDataList = createBlockDataList();
    getNeedBlockDataListLine_RP(root, *allNeedBlockData, root, needBlockDataList, 0, 1, 0, 0);
    getNeedBlockDataListColumn_RP(root, *allNeedBlockData, root, needBlockDataList, 0, 1, 1, 0);
    return needBlockDataList;
}

void getNeedBlockDataListLine_RP(BlockData root, BlockDataList *allNeedBlockData, BlockData blockData, BlockDataList *blockDataList, int canAdd, int option, int level, int type)
{

    if (type == 0)
    {
        if (root.rank == blockData.rank && canAdd != 0)
            return;
        if (canAdd == 1)
        {
            if (findBlockDataToBlockDataList(allNeedBlockData, blockData) == 0)
            {
                addBlockDataToBlockDataList(blockDataList, blockData, option, level);
            }
        }
        else
        {
            canAdd = 1;
        }
    }

    if (type == 1)
    {
        if (canAdd != 0)
            addBlockDataToBlockDataList(blockDataList, blockData, option, level);
        if (blockData.coreData.diag == 1)
            return;
        if (canAdd == 0)
            canAdd = 1;
    }

    int diag = getDiagByCoord(blockData.coreData.coord);
    Coord coord = blockData.coreData.coord;
    if (diag == 1)
        return;
    else
    {
        coord.j--;
    }
    return getNeedBlockDataListLine_RP(root, allNeedBlockData, getBlockDataByCoord(coord), blockDataList, canAdd, option, level, type);
}

void getNeedBlockDataListColumn_RP(BlockData root, BlockDataList *allNeedBlockData, BlockData blockData, BlockDataList *blockDataList, int canAdd, int option, int level, int type)
{
    if (type == 0)
    {
        if (root.rank == blockData.rank && canAdd != 0)
            return;
        if (canAdd == 1)
        {
            if (findBlockDataToBlockDataList(allNeedBlockData, blockData) == 0)
            {
                addBlockDataToBlockDataList(blockDataList, blockData, option, level);
            }
        }
        else
        {
            canAdd = 1;
        }
    }

    if (type == 1)
    {
        if (canAdd != 0)
            addBlockDataToBlockDataList(blockDataList, blockData, option, level);
        if (blockData.coreData.diag == 1)
            return;
        if (canAdd == 0)
            canAdd = 1;
    }

    int diag = getDiagByCoord(blockData.coreData.coord);
    Coord coord = blockData.coreData.coord;
    if (diag == 1)
    {
        return;
    }
    else
    {
        coord.i++;
    }
    return getNeedBlockDataListColumn_RP(root, allNeedBlockData, getBlockDataByCoord(coord), blockDataList, canAdd, option, level, type);
}

BlockDataList *getDependBlockDataList_RP(BlockData root)
{
    BlockDataList *dependBlockDataList = createBlockDataList();
    getDependBlockDataListLine_RP(root, dependBlockDataList, 0, 0);
    getDependBlockDataListColumn_RP(root, dependBlockDataList, 0, 1);
    return dependBlockDataList;
}

void getDependBlockDataListLine_RP(BlockData root, BlockDataList *blockDataList, int option, int level)
{
    BlockData blockData;
    Coord coord = root.coreData.coord, c;

    c.i = coord.i;
    c.j = coord.j;
    if (c.j == (maxDiag + 1))
        return;

    c = coord;
    c.j++;
    blockData = getBlockDataByCoord(c);
    if (rank != blockData.rank)
        addBlockDataToBlockDataList(blockDataList, blockData, option, level);
    getDependBlockDataListLine_RP(blockData, blockDataList, option, level);
    return;
}

void getDependBlockDataListColumn_RP(BlockData root, BlockDataList *blockDataList, int option, int level)
{
    BlockData blockData;
    Coord coord = root.coreData.coord, c;

    if (coord.i == 1)
        return;

    c = coord;
    c.i--;
    blockData = getBlockDataByCoord(c);
    if (rank != blockData.rank)
        addBlockDataToBlockDataList(blockDataList, blockData, option, level);
    getDependBlockDataListColumn_RP(blockData, blockDataList, option, level);
    return;
}

void constructAllBlockPerProcessor_RP(int rank)
{
    int id = rank;
    int i = 0;
    BlockDataList *allNeedBlockDataList = createBlockDataList();
    while (id < maxBlock)
    {
        if (rank == tabBlockData[id].rank)
        {
            tabBlock[i].blockData = tabBlockData[id];
            tabBlock[i].needBlockDataList = createBlockDataList();
            tabBlock[i].needBlockDataList = getNeedBlockDataList_RP(tabBlockData[id], &allNeedBlockDataList);
            if (!isEmptyBlockDataList(tabBlock[i].needBlockDataList))
                mergeBlockDataList(&allNeedBlockDataList, copyBlockDataList(tabBlock[i].needBlockDataList));
            tabBlock[i].dependBlockDataList = createBlockDataList();
            tabBlock[i].dependBlockDataList = getDependBlockDataList_RP(tabBlockData[id]);
            i++;
        }
        id++;
    }
    free(allNeedBlockDataList);
}

void constructAllPixaletedBlockDataPerProcessor_RP()
{
    int a, b, i, iter, address, idpBlock = 0, id = 0;
    BlockData blockData;
    PixelatedBlockData pBlockData;
    PixelBlockData pixel;
    Coord coord;
    while (id < maxBlock)
    {
        blockData = tabBlockData[id];
        pBlockData.idBlockData = blockData.coreData.id;
        pBlockData.nbEvaluatePixel = pow(2, maxFrag - blockData.fragLevel);
        pBlockData.maxDiag = 2 * pBlockData.nbEvaluatePixel - 1;
        pBlockData.pixelBlockDataTab = (PixelBlockData **)malloc((pBlockData.nbEvaluatePixel + 1) * sizeof *pBlockData.pixelBlockDataTab);
        pBlockData.pixelBlockDataTab[0] = (PixelBlockData *)malloc((pBlockData.nbEvaluatePixel + 1) * (pBlockData.nbEvaluatePixel + 1) * sizeof *pBlockData.pixelBlockDataTab[0]);
        for (i = 1; i <= pBlockData.nbEvaluatePixel; i++)
        {
            pBlockData.pixelBlockDataTab[i] = pBlockData.pixelBlockDataTab[0] + i * (pBlockData.nbEvaluatePixel + 1);
        }

        idpBlock = getFirstIdPixelBlock(tabBlockData[blockData.coreData.id]);
        address = blockData.coreData.address;
        for (a = 1; a <= pBlockData.maxDiag; a++)
        {
            if (a <= pBlockData.nbEvaluatePixel)
                for (b = 1; b <= a; b++)
                {
                    coord.i = a - b + 1;
                    coord.j = b;
                    pixel = constructPixelBlockDataByCoordAndBlockData_RP(coord, blockData, pBlockData.pixelBlockDataTab, address);
                    pixel.coreData.id = idpBlock;
                    address += getSizeBlock(pixel.coreData.dim);
                    pBlockData.pixelBlockDataTab[coord.i][coord.j] = pixel;
                }
            else
            {
                iter = 0;
                for (b = 2 + (a - pBlockData.nbEvaluatePixel - 1); b <= pBlockData.nbEvaluatePixel; b++)
                {
                    coord.i = pBlockData.nbEvaluatePixel - iter++;
                    coord.j = b;
                    pixel = constructPixelBlockDataByCoordAndBlockData_RP(coord, blockData, pBlockData.pixelBlockDataTab, address);
                    pixel.coreData.id = idpBlock;
                    address += getSizeBlock(pixel.coreData.dim);

                    pBlockData.pixelBlockDataTab[coord.i][coord.j] = pixel;
                }
            }

            idpBlock++;
        }
        tabPixelatedBlockData[blockData.coreData.id] = pBlockData;
        id++;
    }
}

void constructAllPixelatedBlockPerProcessor_RP(int rank)
{
    int id = rank;
    int i = 0;
    Block block;
    BlockDataList *path, *dependBlockDataList;
    while (id < maxBlock)
    {
        if (rank == tabBlockData[tabPixelatedBlockData[id].idBlockData].rank)
        {
            tabPixelatedBlock[i].pBlockData = tabPixelatedBlockData[id];
            block = tabBlock[i];

            tabPixelatedBlock[i].needPixelBlockDataListByLine = createPixelBlockDataListByLineOrColumn(pow(2, maxFrag - block.blockData.fragLevel));
            tabPixelatedBlock[i].needPixelBlockDataListByColumn = createPixelBlockDataListByLineOrColumn(pow(2, maxFrag - block.blockData.fragLevel));
            if (!isEmptyBlockDataList(block.needBlockDataList))
                for (path = block.needBlockDataList; path != NULL; path = path->next)
                {
                    if (path->isLine == 1 && path->isColumn == 0)
                    {
                        getNeedPixelBlockDataListLine(block.blockData, path->blockData, &tabPixelatedBlock[i].needPixelBlockDataListByLine);
                    }
                    else if (path->isLine == 0 && path->isColumn == 1)
                    {
                        getNeedPixelBlockDataListColumn(block.blockData, path->blockData, &tabPixelatedBlock[i].needPixelBlockDataListByColumn);
                    }
                    else
                    {
                        printf("\n\nError occuring in function constructAllPixelatedBlockPerProcessor for processor  %d \n\t\t a blockData id %d need blockData id  = %d => isLine = %d isColumn = %d\n", rank, block.blockData.coreData.id, path->blockData.coreData.id, path->isLine, path->isColumn);
                        exit(EXIT_FAILURE);
                    }
                }

            tabPixelatedBlock[i].needAllPixelBlockDataListByLine = createPixelBlockDataListByLineOrColumn(pow(2, maxFrag - block.blockData.fragLevel));
            tabPixelatedBlock[i].needAllPixelBlockDataListByColumn = createPixelBlockDataListByLineOrColumn(pow(2, maxFrag - block.blockData.fragLevel));

            BlockDataList *needBlockDataList = createBlockDataList();
            getNeedBlockDataListLine_RP(block.blockData, NULL, block.blockData, needBlockDataList, 0, 1, 0, 1);
            getNeedBlockDataListColumn_RP(block.blockData, NULL, block.blockData, needBlockDataList, 0, 1, 1, 1);
            if (!isEmptyBlockDataList(needBlockDataList))
                for (path = needBlockDataList; path != NULL; path = path->next)
                {
                    if (path->isLine == 1 && path->isColumn == 0)
                    {
                        getNeedPixelBlockDataListLine(block.blockData, path->blockData, &tabPixelatedBlock[i].needAllPixelBlockDataListByLine);
                    }
                    else if (path->isLine == 0 && path->isColumn == 1)
                    {
                        getNeedPixelBlockDataListColumn(block.blockData, path->blockData, &tabPixelatedBlock[i].needAllPixelBlockDataListByColumn);
                    }
                    else
                    {
                        printf("\n\nError occuring in function constructAllPixelatedBlockPerProcessor for processor  %d \n\t\t a blockData id %d need blockData id  = %d => isLine = %d isColumn = %d\n", rank, block.blockData.coreData.id, path->blockData.coreData.id, path->isLine, path->isColumn);
                        exit(EXIT_FAILURE);
                    }
                }

            tabPixelatedBlock[i].dependBlockDataListByLine = createBlockDataListByLineOrColumn(pow(2, maxFrag - block.blockData.fragLevel));
            tabPixelatedBlock[i].dependBlockDataListByColumn = createBlockDataListByLineOrColumn(pow(2, maxFrag - block.blockData.fragLevel));

            dependBlockDataList = createBlockDataList();
            getDependBlockDataListLine_RP(block.blockData, dependBlockDataList, 1, 0);
            getDependBlockDataListColumn_RP(block.blockData, dependBlockDataList, 1, 1);
            if (!isEmptyBlockDataList(dependBlockDataList))
                for (path = dependBlockDataList; path != NULL; path = path->next)
                {
                    if (path->isLine == 1 && path->isColumn == 0)
                    {
                        getDependPixelBlockDataListLine(block.blockData, path->blockData, &tabPixelatedBlock[i].dependBlockDataListByLine);
                    }
                    else if (path->isLine == 0 && path->isColumn == 1)
                    {
                        getDependPixelBlockDataListColumn(block.blockData, path->blockData, &tabPixelatedBlock[i].dependBlockDataListByColumn);
                    }
                    else
                    {
                        printf("\n\nError occuring in function constructAllPixelatedBlockPerProcessor for processor  %d \n\t\t a blockData id %d depend blockData id  = %d => isLine = %d isColumn = %d\n", rank, block.blockData.coreData.id, path->blockData.coreData.id, path->isLine, path->isColumn);
                        exit(EXIT_FAILURE);
                    }
                }
            i++;
            free(dependBlockDataList);
        }
        id++;
    }
}

PixelBlockData constructPixelBlockDataByCoordAndBlockData_RP(Coord coord, BlockData blockData, PixelBlockData **tabPixelBlockData, int address)
{
    PixelBlockData pixel;
    pixel.coreData.coord = coord;
    pixel.coreData.diag = coord.i + coord.j - 1;
    pixel.idBlockData = blockData.coreData.id;

    if (pixel.coreData.coord.j == 1)
    {
        pixel.coreData.dim.nbLine = determineNbLinePixelBlockRecv(pixel.coreData.coord.i, maxFrag - blockData.fragLevel, blockData.coreData.dim.nbLine);
        if (pixel.coreData.coord.i == 1)
        {
            pixel.coreData.firstBound.i = blockData.coreData.firstBound.j - pixel.coreData.dim.nbLine + 1;
            pixel.coreData.firstBound.j = blockData.coreData.firstBound.j;
        }
        else
        {
            pixel.coreData.firstBound.j = tabPixelBlockData[pixel.coreData.coord.i - 1][pixel.coreData.coord.j].coreData.firstBound.i - 1;
            pixel.coreData.firstBound.i = pixel.coreData.firstBound.j - pixel.coreData.dim.nbLine + 1;
        }
    }
    else
    {
        pixel.coreData.dim.nbLine = tabPixelBlockData[pixel.coreData.coord.i][pixel.coreData.coord.j - 1].coreData.dim.nbLine;
        pixel.coreData.firstBound.i = tabPixelBlockData[pixel.coreData.coord.i][pixel.coreData.coord.j - 1].coreData.firstBound.i;
        pixel.coreData.firstBound.j = tabPixelBlockData[pixel.coreData.coord.i][pixel.coreData.coord.j - 1].coreData.firstBound.j;
    }

    if (pixel.coreData.coord.i == 1)
    {
        pixel.coreData.dim.nbColumn = determineNbColumnPixelBlockRecv(pixel.coreData.coord.j, maxFrag - blockData.fragLevel, blockData.coreData.dim.nbColumn);
        if (pixel.coreData.coord.j == 1)
        {
            pixel.coreData.secondBound.i = blockData.coreData.secondBound.i;
            pixel.coreData.secondBound.j = blockData.coreData.secondBound.i + pixel.coreData.dim.nbColumn - 1;
        }
        else
        {
            pixel.coreData.secondBound.i = tabPixelBlockData[pixel.coreData.coord.i][pixel.coreData.coord.j - 1].coreData.secondBound.j + 1;
            pixel.coreData.secondBound.j = pixel.coreData.secondBound.i + pixel.coreData.dim.nbColumn - 1;
        }
    }
    else
    {
        pixel.coreData.dim.nbColumn = tabPixelBlockData[pixel.coreData.coord.i - 1][pixel.coreData.coord.j].coreData.dim.nbColumn;
        pixel.coreData.secondBound.i = tabPixelBlockData[pixel.coreData.coord.i - 1][pixel.coreData.coord.j].coreData.secondBound.i;
        pixel.coreData.secondBound.j = tabPixelBlockData[pixel.coreData.coord.i - 1][pixel.coreData.coord.j].coreData.secondBound.j;
    }

    pixel.coreData.address = address;

    return pixel;
}