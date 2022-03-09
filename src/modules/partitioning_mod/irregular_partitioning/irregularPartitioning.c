/**
 * @file irregularPartitioning.c
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief
 * @version 2.2
 * @date 2019-02-06
 *
 * @copyright Compiii Thesis (c) 2019
 *
 */
#include "irregularPartitioning.h"
#include "clogger.h"

void initIrregPartMod()
{
    if (algorithmId == 1 || algorithmId == 2 || algorithmId == 4)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &maxProcess);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Get_processor_name(processorName, &processorNameSize);

        maxBlockDiag = f_IP(maxProcess);
        maxDiag = maxBlockDiag + maxFrag * (ceil(maxBlockDiag / 2.0) + 1);
        maxBlock = (maxBlockDiag + 1) * ((maxFrag * (maxBlockDiag + 2 * (maxBlockDiag % 2 != 0 ? 1 : 0)) + maxBlockDiag) / 2.0) + maxFrag * (int)ceil((maxBlockDiag / 2.0)) * ((1 - (int)ceil((maxBlockDiag / 2.0))) / 2.0);
        maxEvalBlock = getCount(rank);
        maxPixelatedBlock = maxEvalBlock;
        tabBlockData = (BlockData *)malloc(maxBlock * sizeof(BlockData));
        tabPixelatedBlockData = (PixelatedBlockData *)malloc(maxBlock * sizeof(PixelatedBlockData));
        Coord coord;
        if (maxFrag > 0 && (maxBlockDiag % 2 != 0))
        {
            coord.i = maxBlockDiag + 1;
        }
        else
        {
            coord.i = maxBlockDiag;
        }
        coord.j = maxDiag + 1;
        tabBlockDataCoord = (int *)malloc((getIdBlockDataByCoord(coord) + 1) * sizeof(int));
        tabBlock = (Block *)malloc(maxEvalBlock * sizeof(Block));
        tabPixelatedBlock = (PixelatedBlock *)malloc(maxEvalBlock * sizeof(PixelatedBlock));

        createBlockDataAsMPI_Datatype();

        constructAllBlockDataPerProcessor_IP(rank);
        constructAllBlockPerProcessor_IP(rank);
        MPI_Barrier(MPI_COMM_WORLD);
        constructAllPixaletedBlockDataPerProcessor_IP();
        constructAllPixelatedBlockPerProcessor_IP(rank);
    }
    else if (algorithmId == 5)
    {
        int currentFrag, tmpMaxFrag = maxFrag;
        tabs = (TabBlock *)malloc((tmpMaxFrag + 1) * sizeof(TabBlock));
        for (currentFrag = 1; currentFrag <= tmpMaxFrag; currentFrag++)
        {
            maxFrag = currentFrag;
            initIrregPart4S(&tabs[currentFrag]);
            constructAllBlockDataPerProcessor_IP(rank);
            tabs[currentFrag].tabBlockData = tabBlockData;
            constructAllBlockPerProcessor_IP(rank);
            tabs[currentFrag].tabBlock = tabBlock;
            MPI_Barrier(MPI_COMM_WORLD);
            constructAllPixaletedBlockDataPerProcessor_IP();
            tabs[currentFrag].tabPixelatedBlockData = tabPixelatedBlockData;
            constructAllPixelatedBlockPerProcessor_IP(rank);
            tabs[currentFrag].tabPixelatedBlock = tabPixelatedBlock;
            MPI_Barrier(MPI_COMM_WORLD);
        }

        maxFrag = tmpMaxFrag;

        int id = 0;
        currentFrag = 0;
        tabPixelatedBlockData = (PixelatedBlockData *)malloc(maxBlock * sizeof(PixelatedBlockData));
        while (id < maxBlock)
        {
            if (currentFrag < maxFrag - 1)
                currentFrag = tabs[currentFrag + 1].tabBlockData[id].fragLevel;
            tabPixelatedBlockData[id] = tabs[currentFrag + 1].tabPixelatedBlockData[id];
            id++;
        }

        int i = 0, currentID;
        id = rank;
        BlockDataList *path;
        tabPixelatedBlock = (PixelatedBlock *)malloc(maxEvalBlock * sizeof(PixelatedBlock));

        while (id < maxBlock)
        {
            currentFrag = tabs[maxFrag].tabBlock[i].blockData.fragLevel;
            if (currentFrag < maxFrag - 1)
            {
                tabPixelatedBlock[i] = tabs[currentFrag + 1].tabPixelatedBlock[i];
                tabPixelatedBlock[i].dependBlockDataListByLine = createBlockDataListByLineOrColumn(2);
                tabPixelatedBlock[i].dependBlockDataListByColumn = createBlockDataListByLineOrColumn(2);

                for (int j = 1; j <= tabs[maxFrag].tabPixelatedBlock[i].dependBlockDataListByLine.nbLineOrColumn; j++)
                {

                    if (!isEmptyBlockDataList(tabs[maxFrag].tabPixelatedBlock[i].dependBlockDataListByLine.blockDataList[j]))
                    {
                        for (path = tabs[maxFrag].tabPixelatedBlock[i].dependBlockDataListByLine.blockDataList[j]; path != NULL; path = path->next)
                        {
                            if (path->isLine == 1 && path->isColumn == 0)
                            {
                                currentID = (int)ceil(j / (float)pow(2, (maxFrag - currentFrag - 1)));
                                addBlockDataToBlockDataList(tabPixelatedBlock[i].dependBlockDataListByLine.blockDataList[currentID], path->blockData, 0, 0);
                            }
                            else
                            {
                                printf("\n\nError occuring in function for processor  %d \n\t\t a blockData id %d depend blockData id  = %d => isLine = %d isColumn = %d\n", rank, tabPixelatedBlock[i].pBlockData.idBlockData, path->blockData.coreData.id, path->isLine, path->isColumn);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }

                for (int j = 1; j <= tabs[maxFrag].tabPixelatedBlock[i].dependBlockDataListByColumn.nbLineOrColumn; j++)
                {
                    if (!isEmptyBlockDataList(tabs[maxFrag].tabPixelatedBlock[i].dependBlockDataListByColumn.blockDataList[j]))
                    {
                        for (path = tabs[maxFrag].tabPixelatedBlock[i].dependBlockDataListByColumn.blockDataList[j]; path != NULL; path = path->next)
                        {
                            if (path->isLine == 0 && path->isColumn == 1)
                            {
                                currentID = (int)ceil(j / (float)pow(2, (maxFrag - currentFrag - 1)));
                                addBlockDataToBlockDataList(tabPixelatedBlock[i].dependBlockDataListByColumn.blockDataList[currentID], path->blockData, 0, 1);
                            }
                            else
                            {
                                printf("\n\nError occuring in function for processor  %d \n\t\t a blockData id %d depend blockData id  = %d => isLine = %d isColumn = %d\n", rank, tabPixelatedBlock[i].pBlockData.idBlockData, path->blockData.coreData.id, path->isLine, path->isColumn);
                                exit(EXIT_FAILURE);
                            }
                        }
                    }
                }
            }
            else
                tabPixelatedBlock[i] = tabs[maxFrag].tabPixelatedBlock[i];
            i++;
            id += maxProcess;
        }
    }
}

void initIrregPart4S(TabBlock *tab)
{
    MPI_Comm_size(MPI_COMM_WORLD, &maxProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processorName, &processorNameSize);

    maxBlockDiag = f_IP(maxProcess);
    maxDiag = maxBlockDiag + maxFrag * (ceil(maxBlockDiag / 2.0) + 1);
    maxBlock = (maxBlockDiag + 1) * ((maxFrag * (maxBlockDiag + 2 * (maxBlockDiag % 2 != 0 ? 1 : 0)) + maxBlockDiag) / 2.0) + maxFrag * (int)ceil((maxBlockDiag / 2.0)) * ((1 - (int)ceil((maxBlockDiag / 2.0))) / 2.0);
    maxEvalBlock = getCount(rank);
    maxPixelatedBlock = maxEvalBlock;
    tabBlockData = (BlockData *)malloc(maxBlock * sizeof(BlockData));
    tabPixelatedBlockData = (PixelatedBlockData *)malloc(maxBlock * sizeof(PixelatedBlockData));
    tab->tabBlockData = (BlockData *)malloc(maxBlock * sizeof(BlockData));
    tab->tabPixelatedBlockData = (PixelatedBlockData *)malloc(maxBlock * sizeof(PixelatedBlockData));
    Coord coord;
    if (maxFrag > 0 && (maxBlockDiag % 2 != 0))
    {
        coord.i = maxBlockDiag + 1;
    }
    else
    {
        coord.i = maxBlockDiag;
    }
    coord.j = maxDiag + 1;
    tabBlockDataCoord = (int *)malloc((getIdBlockDataByCoord(coord) + 1) * sizeof(int));
    tabBlock = (Block *)malloc(maxEvalBlock * sizeof(Block));
    tabPixelatedBlock = (PixelatedBlock *)malloc(maxEvalBlock * sizeof(PixelatedBlock));
    tab->tabBlock = (Block *)malloc(maxEvalBlock * sizeof(Block));
    tab->tabPixelatedBlock = (PixelatedBlock *)malloc(maxEvalBlock * sizeof(PixelatedBlock));

    createBlockDataAsMPI_Datatype();
}

void constructAllBlockDataPerProcessor_IP(int rank)
{
    int id = rank;
    ConstBlockDataAttr constBlockDataAttr = initBlockDataAttrValue();
    BlockData blockData;
    while (id < maxBlock)
    {
        if (maxProcess == 3 && rank == 2)
        {
            ConstBlockDataAttr constBlockDataAttr = initBlockDataAttrValue();
            blockData = constructBlockDataById(id, &constBlockDataAttr);
        }
        else
        {
            blockData = constructBlockDataById(id, &constBlockDataAttr);
        }

        tabBlockData[id] = blockData;
        tabBlockDataCoord[getIdBlockDataByCoord(blockData.coreData.coord)] = blockData.coreData.id;
        id += maxProcess;
    }
    bcastBlockData(maxEvalBlock, 1, maxProcess);
    updateAddressBlockData(rank);
    updateBoundBlockData_IP(rank);
    bcastBlockData(maxEvalBlock, 1, maxProcess);
}

BlockData constructBlockDataById(int id, ConstBlockDataAttr *constBlockDataAttr)
{
    int step = constBlockDataAttr->step;
    int diag = constBlockDataAttr->diag;
    int nbBlock = constBlockDataAttr->nbBlock;
    int tmp = constBlockDataAttr->tmp;
    int fragLevel = constBlockDataAttr->fragLevel;
    int iter = constBlockDataAttr->iter;
    int initTmp = constBlockDataAttr->initTmp;
    int isOk = 0;
    int save = 0;
    BlockData blockData;

    while (iter <= step && isOk == 0 && fragLevel <= maxFrag)
    {
        diag++;
        if ((nbBlock + tmp) > id)
        {
            Coord coord;
            coord.i = id - nbBlock + 1;
            coord.j = diag + coord.i;
            BlockCoreData coreData;
            coreData.id = id;
            coreData.coord = coord;
            coreData.diag = diag;
            blockData.coreData = coreData;
            blockData.fragLevel = fragLevel;
            blockData.rank = rank;
            blockData.coreData.dim.nbLine = getNbLineBlock_IP(coord, fragLevel);
            blockData.coreData.dim.nbColumn = getNbColumnBlock_IP(coord, fragLevel);
            isOk = 1;
            save = 1;
        }
        if (isOk == 0 || save == 1)
        {
            nbBlock += tmp;
            if (initTmp == 1)
            {
                tmp = maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0);
                initTmp = 0;
            }
            else
                tmp--;
            if (iter == step)
            {
                fragLevel++;
                tmp = (int)ceil(maxBlockDiag / 2.0);
                initTmp = 1;
                if (fragLevel == maxFrag)
                {
                    step += maxBlockDiag + 1 + (maxBlockDiag % 2 != 0 ? 1 : 0);
                }
                else
                {
                    step += (maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1;
                }
            }
            iter++;
            if (save)
            {
                constBlockDataAttr->step = step;
                constBlockDataAttr->diag = diag;
                constBlockDataAttr->nbBlock = nbBlock;
                constBlockDataAttr->tmp = tmp;
                constBlockDataAttr->fragLevel = fragLevel;
                constBlockDataAttr->iter = iter;
                constBlockDataAttr->initTmp = initTmp;
            }
        }
    }

    return blockData;
}

ConstBlockDataAttr initBlockDataAttrValue()
{
    ConstBlockDataAttr constBlockDataAttr;
    constBlockDataAttr.step = (maxFrag != 0 ? (int)floor(maxBlockDiag / 2.0) : maxBlockDiag);
    constBlockDataAttr.diag = 0;
    constBlockDataAttr.nbBlock = 0;
    constBlockDataAttr.tmp = maxBlockDiag;
    constBlockDataAttr.fragLevel = 0;
    constBlockDataAttr.iter = 1;
    constBlockDataAttr.initTmp = 0;
    return constBlockDataAttr;
}

int f_IP(int p)
{
    return (partitioningTechniqueId == 0 ? f_IP_P(p) : f_IP_SQRT_2P(p)); //(p == 1 ? 1 : (int)ceil(sqrt(2 * p)));
}

void printConstBlockDataAttr(ConstBlockDataAttr constBlockDataAttr)
{
    printf("\n\tstep = %d", constBlockDataAttr.step);
    printf("\n\tdiag = %d", constBlockDataAttr.diag);
    printf("\n\tnbBlock = %d ", constBlockDataAttr.nbBlock);
    printf("\n\ttmp = %d", constBlockDataAttr.tmp);
    printf("\n\tfragLevel = %d", constBlockDataAttr.fragLevel);
    printf("\n\titer = %d", constBlockDataAttr.iter);
    printf("\n\tinitTmp = %d\n", constBlockDataAttr.initTmp);
}

int getNbLineBlock_IP(Coord coord, int fragLevel)
{
    int diag = coord.j - coord.i;
    if (fragLevel == 0)
        if (diag == 1 && coord.j == (maxBlockDiag + 1))
            return maxNumber - (maxBlockDiag - 1) * g_IP(maxNumber, maxProcess);
        else
            return g_IP(maxNumber, maxProcess);
    else
    {
        int limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel - 1) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
        if (diag > limit)
            coord.j = coord.i + limit;
        if (diag == (limit - 1))
        {
            coord.j--;
            return (int)ceil(getNbLineBlock_IP(coord, --fragLevel) / 2.0);
        }
        else
        {
            int a, b;
            if (coord.i % 2 != 0)
            {
                a = (coord.i + 1) / 2.0;
                b = (2 * coord.j - coord.i - 3) / 2.0;
                coord.i = a;
                coord.j = b;
                return (int)floor(getNbLineBlock_IP(coord, --fragLevel) / 2.0);
            }
            else
            {
                a = coord.i / 2.0;
                b = (2 * coord.j - coord.i - 4) / 2.0;
                coord.i = a;
                coord.j = b;
                return (int)ceil(getNbLineBlock_IP(coord, --fragLevel) / 2.0);
            }
        }
    }
}

int getNbColumnBlock_IP(Coord coord, int fragLevel)
{
    int diag = getDiagByCoord(coord);
    if (fragLevel == 0)
        if (coord.j == (maxBlockDiag + 1))
            return maxNumber - (maxBlockDiag - 1) * g_IP(maxNumber, maxProcess);
        else
            return g_IP(maxNumber, maxProcess);
    else
    {
        int limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel - 1) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
        if (diag > limit)
            coord.i = coord.j - limit;
        if (diag == (limit - 1))
        {
            coord.i++;
            return (int)floor(getNbColumnBlock_IP(coord, --fragLevel) / 2.0);
        }
        else
        {
            int a, b;
            if (coord.i % 2 != 0)
            {
                a = (coord.i + 3) / 2.0;
                b = (2 * coord.j - coord.i - 1) / 2.0;
                coord.i = a;
                coord.j = b;
                return (int)floor(getNbColumnBlock_IP(coord, --fragLevel) / 2.0);
            }
            else
            {
                a = (coord.i + 2) / 2.0;
                b = (2 * coord.j - coord.i - 2) / 2.0;
                coord.i = a;
                coord.j = b;
                return (int)ceil(getNbColumnBlock_IP(coord, --fragLevel) / 2.0);
            }
        }
    }
}

int g_IP(int n, int p)
{
    int size = (int)ceil(n / (float)f_IP(p));
    if (size * (f_IP(p) - 1) >= n)
        return (int)floor(n / (float)f_IP(p));
    else
        return size;
}

void createBlockDataAsMPI_Datatype()
{
    /**
     * @brief create a type for struct Coord_T
     *
     */
    MPI_Type_contiguous(2, MPI_INT, &CoordMpiDt);
    MPI_Type_commit(&CoordMpiDt);

    /**
     * @brief create a type for struct Dimension_T
     *
     */
    MPI_Type_contiguous(2, MPI_INT, &DimMpiDt);
    MPI_Type_commit(&DimMpiDt);

    /**
     * @brief create a type for struct BlockCoreData_T
     *
     */
    int count = 7;
    int blocklengths[7] = {1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype types[7] = {MPI_INT, CoordMpiDt, MPI_INT, DimMpiDt, MPI_INT, CoordMpiDt, CoordMpiDt};
    MPI_Aint offsets[7];
    offsets[0] = offsetof(BlockCoreData, id);
    offsets[1] = offsetof(BlockCoreData, coord);
    offsets[2] = offsetof(BlockCoreData, address);
    offsets[3] = offsetof(BlockCoreData, dim);
    offsets[4] = offsetof(BlockCoreData, diag);
    offsets[5] = offsetof(BlockCoreData, firstBound);
    offsets[6] = offsetof(BlockCoreData, secondBound);
    MPI_Type_create_struct(count, blocklengths, offsets, types, &BlockCoreDataMpiDt);
    MPI_Type_commit(&BlockCoreDataMpiDt);

    /**
     * @brief create a type for struct BlockData_T
     *
     */
    count = 3;
    int blocklengths2[3] = {1, 1, 1};
    MPI_Datatype types2[3] = {BlockCoreDataMpiDt, MPI_INT, MPI_INT};
    MPI_Aint offsets2[3];
    offsets2[0] = offsetof(BlockData, coreData);
    offsets2[1] = offsetof(BlockData, fragLevel);
    offsets2[2] = offsetof(BlockData, rank);
    MPI_Type_create_struct(count, blocklengths2, offsets2, types2, &BlockDataMpiDt);
    MPI_Type_commit(&BlockDataMpiDt);
}

void freeMemory_IP()
{
    free(tabBlockData);
    free(tabBlockDataCoord);
    free(tabBlock);
    free(tabPixelatedBlockData);
    free(tabPixelatedBlock);
    MPI_Type_free(&CoordMpiDt);
    MPI_Type_free(&DimMpiDt);
    MPI_Type_free(&BlockCoreDataMpiDt);
    MPI_Type_free(&BlockDataMpiDt);
}

int getCount(int id)
{
    int count = (int)floor(maxBlock / (float)maxProcess);
    if (maxBlock % maxProcess == 0 || ((count * maxProcess + id + 1) > maxBlock))
        return count;
    else
        return count + 1;
}

void updateAddressBlockData(int rank)
{
    int id = rank, iter = 0, i = 0, prevAddress = 0;
    while (id < maxBlock)
    {
        tabBlockData[id].coreData.address = prevAddress;
        for (iter = i; iter < id; iter++)
        {
            tabBlockData[id].coreData.address += getSizeBlock(tabBlockData[iter].coreData.dim);
            tabBlockDataCoord[getIdBlockDataByCoord(tabBlockData[iter].coreData.coord)] = tabBlockData[iter].coreData.id;
        }
        prevAddress = tabBlockData[id].coreData.address + getSizeBlock(tabBlockData[id].coreData.dim);
        i = id + 1;
        id += maxProcess;
    }
    while (i < maxBlock)
    {
        tabBlockDataCoord[getIdBlockDataByCoord(tabBlockData[i].coreData.coord)] = tabBlockData[i].coreData.id;
        i++;
    }
}

void updateBoundBlockData_IP(int rank)
{
    int id = rank, offset = 0;
    while (id < maxBlock)
    {
        offset = getOffsetFirstBound_IP(tabBlockData[id], tabBlockData[id]);
        tabBlockData[id].coreData.firstBound.i = 1 + offset;
        tabBlockData[id].coreData.firstBound.j = tabBlockData[id].coreData.dim.nbLine + offset;

        offset = getOffsetSecondBound_IP(tabBlockData[id], tabBlockData[id]);
        tabBlockData[id].coreData.secondBound.i = 1 + offset;
        tabBlockData[id].coreData.secondBound.j = tabBlockData[id].coreData.dim.nbColumn + offset;

        id += maxProcess;
    }
}
int getOffsetFirstBound_IP(BlockData root, BlockData blockData)
{
    Coord coord = blockData.coreData.coord, c;
    int fragLevel = blockData.fragLevel, diag = getDiagByCoord(coord), limit, exit;

    if (fragLevel == 0 && coord.i == 1)
        return 0;
    else
    {
        limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel - 1) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
        if (diag != (limit - 1) && coord.i == 1)
            return 0;
    }

    c = coord;
    if (maxFrag == 0)
    {
        c.i--;
    }
    else
    {
        exit = 0;
        if (diag == (limit - 1))
        {
            c.i = 2 * coord.i - 1;
            c.j = coord.i + coord.j;
            exit = 1;
        }
        if (diag >= limit && maxFrag != fragLevel && exit == 0)
        {
            limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
            if (diag == (limit - 2))
            {
                c.i--;
                exit = 1;
            }
        }
        if (exit == 0)
        {
            c.i--;
        }
    }
    blockData = getBlockDataByCoord(c);
    return (blockData.coreData.id == root.coreData.id ? 0 : blockData.coreData.dim.nbLine) + getOffsetFirstBound_IP(root, blockData);
}
int getOffsetSecondBound_IP(BlockData root, BlockData blockData)
{
    int diag = getDiagByCoord(blockData.coreData.coord);
    int fragLevel = blockData.fragLevel;
    Coord coord = blockData.coreData.coord;

    if (diag == 1)
    {
        return (coord.i - 1) * g_IP(maxNumber, maxProcess);
    }
    else
    {
        if (fragLevel == 0)
        {
            coord.j--;
        }
        else
        {
            int limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel - 1) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
            if (diag == (limit - 1) || diag > limit)
            {
                coord.j--;
            }
            else
            {
                int a, b;
                if (coord.i % 2 != 0)
                {
                    a = (coord.i + 1) / 2.0;
                    b = (2 * coord.j - coord.i - 3) / 2.0;
                    coord.i = a;
                    coord.j = b;
                }
                else
                {
                    a = coord.i / 2.0;
                    b = 1 + ((2 * coord.j - coord.i - 4) / 2.0);
                    coord.i = a;
                    coord.j = b;
                }
            }
        }
    }
    blockData = getBlockDataByCoord(coord);
    return (blockData.coreData.id == root.coreData.id ? 0 : blockData.coreData.dim.nbColumn) + getOffsetSecondBound_IP(root, blockData);
}

void bcastBlockData(int c, int b, int s)
{

    MPI_Barrier(MPI_COMM_WORLD);
    int id;
    int count = c;
    int blockLengths = b;
    int stride = s;
    MPI_Type_vector(count, blockLengths, stride, BlockDataMpiDt, &vector);
    MPI_Type_commit(&vector);
    for (id = 0; id < maxProcess; id++)
    {
        if (rank != id)
        {
            MPI_Send(tabBlockData + rank, 1, vector, id, 0, MPI_COMM_WORLD);
        }
    }
    for (id = 0; id < maxProcess; id++)
    {
        if (rank != id)
        {
            count = getCount(id);
            MPI_Type_vector(count, blockLengths, stride, BlockDataMpiDt, &vector);
            MPI_Type_commit(&vector);
            MPI_Recv(tabBlockData + id, 1, vector, id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    MPI_Type_free(&vector);
}

BlockDataList *getNeedBlockDataList_IP(BlockData root, BlockDataList **allNeedBlockData)
{
    BlockDataList *needBlockDataList = createBlockDataList();
    getNeedBlockDataListLine_IP(root, *allNeedBlockData, root, needBlockDataList, 0, 1, 0, 0);
    getNeedBlockDataListColumn_IP(root, *allNeedBlockData, root, needBlockDataList, 0, 1, 1, 0);
    return needBlockDataList;
}

void getNeedBlockDataListLine_IP(BlockData root, BlockDataList *allNeedBlockData, BlockData blockData, BlockDataList *blockDataList, int canAdd, int option, int level, int type)
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
    int fragLevel = blockData.fragLevel;
    Coord coord = blockData.coreData.coord;
    if (diag == 1)
        return;
    else
    {
        if (fragLevel == 0)
        {
            coord.j--;
        }
        else
        {
            int limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel - 1) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
            if (diag == (limit - 1) || diag > limit)
            {
                coord.j--;
            }
            else
            {
                int a, b;
                if (coord.i % 2 != 0)
                {
                    a = (coord.i + 1) / 2.0;
                    b = (2 * coord.j - coord.i - 3) / 2.0;
                    coord.i = a;
                    coord.j = b;
                }
                else
                {
                    a = coord.i / 2.0;
                    b = 1 + ((2 * coord.j - coord.i - 4) / 2.0);
                    coord.i = a;
                    coord.j = b;
                }
            }
        }
    }
    return getNeedBlockDataListLine_IP(root, allNeedBlockData, getBlockDataByCoord(coord), blockDataList, canAdd, option, level, type);
}

void getNeedBlockDataListColumn_IP(BlockData root, BlockDataList *allNeedBlockData, BlockData blockData, BlockDataList *blockDataList, int canAdd, int option, int level, int type)
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
    int fragLevel = blockData.fragLevel;
    Coord coord = blockData.coreData.coord;
    if (diag == 1)
    {
        return;
    }
    else
    {
        if (fragLevel == 0)
        {
            coord.i++;
        }
        else
        {
            int limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel - 1) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
            if (diag == (limit - 1) || diag > limit)
            {
                coord.i++;
            }
            else
            {
                int a, b;
                if (coord.i % 2 != 0)
                {
                    a = (coord.i + 1) / 2.0;
                    b = (2 * coord.j - coord.i - 1) / 2.0;
                    coord.i = a;
                    coord.j = b;
                }
                else
                {
                    a = (coord.i + 2) / 2.0;
                    b = (2 * coord.j - coord.i - 2) / 2.0;
                    coord.i = a;
                    coord.j = b;
                }
            }
        }
    }
    return getNeedBlockDataListColumn_IP(root, allNeedBlockData, getBlockDataByCoord(coord), blockDataList, canAdd, option, level, type);
}

BlockDataList *getDependBlockDataList_IP(BlockData root)
{
    BlockDataList *dependBlockDataList = createBlockDataList();
    getDependBlockDataListLine_IP(root, dependBlockDataList, 0, 0);
    getDependBlockDataListColumn_IP(root, dependBlockDataList, 0, 1);
    return dependBlockDataList;
}

void getDependBlockDataListLine_IP(BlockData root, BlockDataList *blockDataList, int option, int level)
{
    BlockData blockData;
    Coord coord = root.coreData.coord, c;
    int fragLevel = root.fragLevel, diag = getDiagByCoord(coord), limit = 0, exit;

    c.i = coord.i;
    c.j = coord.j + (maxFrag - fragLevel) * (maxBlockDiag - (int)floor(maxBlockDiag / 2.0) + 1);
    if (c.j == (maxDiag + 1))
        return;

    if (fragLevel == 0 && coord.i == 1)
        ;
    else
    {
        limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel - 1) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
    }

    c = coord;
    if (maxFrag == 0)
    {
        c.j++;
        blockData = getBlockDataByCoord(c);
        if (rank != blockData.rank)
            addBlockDataToBlockDataList(blockDataList, blockData, option, level);
        getDependBlockDataListLine_IP(blockData, blockDataList, option, level);
        return;
    }
    else
    {
        c = coord;
        exit = 0;
        if (diag == (limit - 1))
        {
            c.i *= 2;
            c.j += coord.i + 1;
            exit = 1;
            blockData = getBlockDataByCoord(c);
            if (rank != blockData.rank)
                addBlockDataToBlockDataList(blockDataList, blockData, option, level);
            getDependBlockDataListLine_IP(blockData, blockDataList, option, level);
            return;
        }
        if (diag >= limit && maxFrag != fragLevel && exit == 0)
        {
            limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
            if (diag == (limit - 2))
            {
                exit = 1;
                c = coord;
                c.j++;
                blockData = getBlockDataByCoord(c);
                if (rank != blockData.rank)
                    addBlockDataToBlockDataList(blockDataList, blockData, option, level);
                getDependBlockDataListLine_IP(blockData, blockDataList, option, level);
                c.i = 2 * coord.i - 1;
                c.j = coord.j + coord.i + 1;
                blockData = getBlockDataByCoord(c);
                if (root.rank != blockData.rank)
                    addBlockDataToBlockDataList(blockDataList, blockData, option, level);
                getDependBlockDataListLine_IP(blockData, blockDataList, option, level);
                return;
            }
        }
        if (exit == 0)
        {
            c = coord;
            c.j++;
            blockData = getBlockDataByCoord(c);
            if (rank != blockData.rank)
                addBlockDataToBlockDataList(blockDataList, blockData, option, level);
            getDependBlockDataListLine_IP(blockData, blockDataList, option, level);
            return;
        }
    }
}

void getDependBlockDataListColumn_IP(BlockData root, BlockDataList *blockDataList, int option, int level)
{
    BlockData blockData;
    Coord coord = root.coreData.coord, c;
    int fragLevel = root.fragLevel, diag = getDiagByCoord(coord), limit, exit;

    if (fragLevel == 0 && coord.i == 1)
        return;
    else
    {
        limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel - 1) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
        if (diag != (limit - 1) && coord.i == 1)
            return;
    }

    c = coord;
    if (maxFrag == 0)
    {
        c.i--;
        blockData = getBlockDataByCoord(c);
        if (rank != blockData.rank)
            addBlockDataToBlockDataList(blockDataList, blockData, option, level);
        getDependBlockDataListColumn_IP(blockData, blockDataList, option, level);
        return;
    }
    else
    {
        c = coord;
        exit = 0;
        if (diag == (limit - 1))
        {
            c.i = 2 * coord.i - 1;
            c.j = coord.i + coord.j;
            exit = 1;
            blockData = getBlockDataByCoord(c);
            if (rank != blockData.rank)
                addBlockDataToBlockDataList(blockDataList, blockData, option, level);
            getDependBlockDataListColumn_IP(blockData, blockDataList, option, level);
            return;
        }
        if (diag >= limit && maxFrag != fragLevel && exit == 0)
        {
            limit = (int)floor(maxBlockDiag / 2.0) + 2 + (fragLevel) * ((maxBlockDiag + (maxBlockDiag % 2 != 0 ? 1 : 0)) / 2 + 1);
            if (diag == (limit - 2))
            {
                exit = 1;
                c = coord;
                c.i--;
                blockData = getBlockDataByCoord(c);
                if (rank != blockData.rank)
                    addBlockDataToBlockDataList(blockDataList, blockData, option, level);
                getDependBlockDataListColumn_IP(blockData, blockDataList, option, level);
                c.i = 2 * coord.i - 2;
                c.j = coord.j + coord.i;
                blockData = getBlockDataByCoord(c);
                if (rank != blockData.rank)
                    addBlockDataToBlockDataList(blockDataList, blockData, option, level);
                getDependBlockDataListColumn_IP(blockData, blockDataList, option, level);
                return;
            }
        }
        if (exit == 0)
        {
            c = coord;
            c.i--;
            blockData = getBlockDataByCoord(c);
            if (rank != blockData.rank)
                addBlockDataToBlockDataList(blockDataList, blockData, option, level);
            getDependBlockDataListColumn_IP(blockData, blockDataList, option, level);
            return;
        }
    }
}

void constructAllBlockPerProcessor_IP(int rank)
{
    int id = rank;
    int i = 0;
    BlockDataList *allNeedBlockDataList = createBlockDataList();
    while (id < maxBlock)
    {
        tabBlock[i].blockData = tabBlockData[id];
        tabBlock[i].needBlockDataList = createBlockDataList();

        tabBlock[i].needBlockDataList = getNeedBlockDataList_IP(tabBlockData[id], &allNeedBlockDataList);
        if (!isEmptyBlockDataList(tabBlock[i].needBlockDataList))
            mergeBlockDataList(&allNeedBlockDataList, copyBlockDataList(tabBlock[i].needBlockDataList));
        tabBlock[i].dependBlockDataList = createBlockDataList();
        tabBlock[i].dependBlockDataList = getDependBlockDataList_IP(tabBlockData[id]);
        i++;
        id += maxProcess;
    }
    free(allNeedBlockDataList);
}

void constructAllPixaletedBlockDataPerProcessor_IP()
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
                    pixel = constructPixelBlockDataByCoordAndBlockData_IP(coord, blockData, pBlockData.pixelBlockDataTab, address);
                    if (blockData.coreData.diag == 1 && (coord.i + coord.j) > pBlockData.nbEvaluatePixel)
                    {
                        pixel.coreData.id = (coord.i + coord.j) - pBlockData.nbEvaluatePixel;
                    }
                    else
                    {
                        pixel.coreData.id = idpBlock;
                    }
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
                    pixel = constructPixelBlockDataByCoordAndBlockData_IP(coord, blockData, pBlockData.pixelBlockDataTab, address);
                    if (blockData.coreData.diag == 1 && (coord.i + coord.j) > pBlockData.nbEvaluatePixel)
                    {
                        pixel.coreData.id = (coord.i + coord.j) - pBlockData.nbEvaluatePixel;
                    }
                    else
                    {
                        pixel.coreData.id = idpBlock;
                    }
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

void constructAllPixelatedBlockPerProcessor_IP(int rank)
{
    int id = rank;
    int i = 0;
    Block block;
    BlockDataList *path, *dependBlockDataList;
    while (id < maxBlock)
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
        getNeedBlockDataListLine_IP(block.blockData, NULL, block.blockData, needBlockDataList, 0, 1, 0, 1);
        getNeedBlockDataListColumn_IP(block.blockData, NULL, block.blockData, needBlockDataList, 0, 1, 1, 1);
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
        getDependBlockDataListLine_IP(block.blockData, dependBlockDataList, 1, 0);
        getDependBlockDataListColumn_IP(block.blockData, dependBlockDataList, 1, 1);
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
        id += maxProcess;
        free(dependBlockDataList);
    }
}

PixelBlockData constructPixelBlockDataByCoordAndBlockData_IP(Coord coord, BlockData blockData, PixelBlockData **tabPixelBlockData, int address)
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