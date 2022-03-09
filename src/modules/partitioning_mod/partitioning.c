/**
 * @file partitioning.c
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-04
 * 
 * @copyright Compiii Thesis (c) 2019 
 * 
 */
#include "partitioning.h"
#include "clogger.h"

void printBlock(Block block)
{
    printf("\\**********************************************\\\n");
    printBlockData(block.blockData);
    printf("\nneed block data list : \n");
    printBlockDataList(block.needBlockDataList);
    printf("\ndepend block data list : \n");
    printBlockDataList(block.dependBlockDataList);
    printf("\\**********************************************\\\n");
}

void printBlockData(BlockData blockData)
{
    printf("\nid block : %d", blockData.coreData.id);
    printf("\n\tcoordinates : ");
    printf("\n\t\ti -> %d", blockData.coreData.coord.i);
    printf("\n\t\tj -> %d", blockData.coreData.coord.j);
    printf("\n\tdimensions : ");
    printf("\n\t\tnbLine -> %d", blockData.coreData.dim.nbLine);
    printf("\n\t\tnbColumn -> %d", blockData.coreData.dim.nbColumn);
    printf("\n\taddress : %d", blockData.coreData.address);
    printf("\n\tdiagonale : %d", blockData.coreData.diag);
    printf("\n\tfirst bound : ");
    printf("\n\t\tbegin -> %d", blockData.coreData.firstBound.i);
    printf("\n\t\tend -> %d", blockData.coreData.firstBound.j);
    printf("\n\tsecond bound : ");
    printf("\n\t\tbegin -> %d", blockData.coreData.secondBound.i);
    printf("\n\t\tend -> %d", blockData.coreData.secondBound.j);
    printf("\n\tfragLevel : %d", blockData.fragLevel);
    printf("\n\trank : %d\n", blockData.rank);
}

void printTabBlockData(BlockData *tabBlockData)
{
    int i = 0;
    while (i < maxBlock)
    {
        printBlockData(tabBlockData[i]);
        i++;
    }
}

void printTabBlock(Block *tabBlock)
{
    int i = 0;
    while (i < maxEvalBlock)
    {
        printBlock(tabBlock[i]);
        i++;
    }
}

int getSizeBlock(Dim dim)
{
    return dim.nbLine * dim.nbColumn;
}

int getDiagByCoord(Coord coord)
{
    return coord.j - coord.i;
}

int getIdBlockDataByCoord(Coord coord)
{
    int i = coord.i;
    int j = coord.j;
    return (maxDiag - i + 1) * (i - 1) + i * (i - 1) / 2.0 + (j - i - 1);
}

BlockData getBlockDataByCoord(Coord coord)
{
    return tabBlockData[tabBlockDataCoord[getIdBlockDataByCoord(coord)]];
}

BlockDataList *createBlockDataList()
{
    BlockDataList *list;
    list = (BlockDataList *)malloc(sizeof(BlockDataList));
    list->blockData.rank = -1;
    list->blockData.fragLevel = -1;
    list->isLine = 0;
    list->isColumn = 0;
    list->next = NULL;
    return list;
}

int isEmptyBlockDataList(BlockDataList *blockDataList)
{
    return (blockDataList == NULL) || (blockDataList != NULL && (blockDataList->blockData.fragLevel == -1 || blockDataList->blockData.rank == -1));
}

void addBlockDataToBlockDataList(BlockDataList *blockDataList, BlockData blockData, int option, int level)
{
    if (isEmptyBlockDataList(blockDataList))
    {
        blockDataList->blockData = blockData;
        if (level == 0)
        {
            blockDataList->isLine = 1;
            blockDataList->isColumn = 0;
        }
        else
        {
            blockDataList->isColumn = 1;
            blockDataList->isLine = 0;
        }
    }
    else
    {
        int isPresent = 0;
        BlockDataList *path = blockDataList;
        while (path->next != NULL && isPresent == 0)
        {
            if (option == 0 && path->blockData.rank == blockData.rank)
            {
                isPresent = 1;
                if (level == 0)
                {
                    path->isLine = 1;
                    path->isColumn = 0;
                }
                else
                {
                    path->isColumn = 1;
                    path->isLine = 0;
                }
                if (path->blockData.coreData.diag > blockData.coreData.diag)
                    path->blockData = blockData;
            }
            path = path->next;
        }
        if (option == 0 && path->next == NULL && isPresent == 0)
        {
            if (path->blockData.rank == blockData.rank)
            {
                isPresent = 1;
                if (level == 0)
                {
                    path->isLine = 1;
                    path->isColumn = 0;
                }
                else
                {
                    path->isColumn = 1;
                    path->isLine = 0;
                }
                if (path->blockData.coreData.diag > blockData.coreData.diag)
                    path->blockData = blockData;
            }
        }
        if (isPresent == 0)
        {
            BlockDataList *l = createBlockDataList();
            l->blockData = blockData;
            l->next = NULL;
            path->next = l;
            if (level == 0)
            {
                l->isLine = 1;
                l->isColumn = 0;
            }
            else
            {
                l->isColumn = 1;
                l->isLine = 0;
            }
        }
    }
    return;
}

int findBlockDataToBlockDataList(BlockDataList *blockDataList, BlockData blockData)
{
    if (isEmptyBlockDataList(blockDataList))
        return 0;
    BlockDataList *path = blockDataList;
    while (path != NULL && path->blockData.coreData.id != blockData.coreData.id)
        path = path->next;
    return (path != NULL ? 1 : 0);
}

void mergeBlockDataList(BlockDataList **list1, BlockDataList *list2)
{
    if (isEmptyBlockDataList(list2))
    {
        return;
    }
    else
    {
        if (isEmptyBlockDataList(*list1))
        {
            *list1 = list2;
            return;
        }
        BlockDataList *path = *list1;
        while (path->next != NULL)
            path = path->next;
        path->next = list2;
        return;
    }
}

void printBlockDataList(BlockDataList *blockDataList)
{
    if (isEmptyBlockDataList(blockDataList))
    {
        printf("\nBlockDataList is empty\n");
        return;
    }
    else
    {
        BlockDataList *path = blockDataList;
        while (path != NULL)
        {
            printBlockData(path->blockData);
            printf("\n\tisLine : %d", path->isLine);
            printf("\n\tisColumn : %d\n", path->isColumn);
            path = path->next;
        }
        return;
    }
}

//https://stackoverflow.com/questions/22222156/copying-linked-list-in-c
BlockDataList *copyBlockDataList(BlockDataList *list)
{
    BlockDataList *newList = NULL;
    BlockDataList **newIt = &newList;
    while (list != NULL)
    {
        *newIt = malloc(sizeof(BlockDataList));
        memcpy(*newIt, list, sizeof(BlockDataList));
        list = list->next;
        newIt = &((*newIt)->next);
    }
    return newList;
}

int getFirstIdPixelBlock(BlockData blockData)
{
    if (blockData.fragLevel == 0)
    {
        if (blockData.coreData.diag == 1)
            return 1;
        else
            return (blockData.coreData.diag - 2) * pow(2, maxFrag) + 2;
    }
    else
    {
        int i, id, tmp, dist;
        tmp = floor(maxBlockDiag / 2.0);
        id = (floor(maxBlockDiag / 2.0) - 1) * pow(2, maxFrag) + 2;
        for (i = 1; i < blockData.fragLevel; i++)
        {
            tmp += (ceil(maxBlockDiag / 2.0) + 1);
            id += (ceil(maxBlockDiag / 2.0) + 1) * pow(2, maxFrag - i);
        }
        dist = blockData.coreData.diag - tmp - 1;
        id += dist * pow(2, maxFrag - blockData.fragLevel);
        return id;
    }
}

int determineNbLinePixelBlockRecv(int line, int fragLevel, int nbLineBlock)
{
    if (fragLevel == 0)
    {
        return nbLineBlock;
    }
    else
    {
        if (line % 2 == 0)
        {
            return (int)floor(determineNbLinePixelBlockRecv((int)ceil(line / 2.0), --fragLevel, nbLineBlock) / 2.0);
        }
        else
        {
            return (int)ceil(determineNbLinePixelBlockRecv((int)ceil(line / 2.0), --fragLevel, nbLineBlock) / 2.0);
        }
    }
}

int determineNbColumnPixelBlockRecv(int column, int fragLevel, int nbColumnBlock)
{
    if (fragLevel == 0)
    {
        return nbColumnBlock;
    }
    else
    {
        if (column % 2 == 0)
        {
            return (int)ceil(determineNbColumnPixelBlockRecv((int)ceil(column / 2.0), --fragLevel, nbColumnBlock) / 2.0);
        }
        else
        {
            return (int)floor(determineNbColumnPixelBlockRecv((int)ceil(column / 2.0), --fragLevel, nbColumnBlock) / 2.0);
        }
    }
}

void printPixelBlockData(PixelBlockData pixel)
{
    printf("\nid pixel block : %d", pixel.coreData.id);
    printf("\n\tcoordinates : ");
    printf("\n\t\ti -> %d", pixel.coreData.coord.i);
    printf("\n\t\tj -> %d", pixel.coreData.coord.j);
    printf("\n\tdimensions : ");
    printf("\n\t\tnbLine -> %d", pixel.coreData.dim.nbLine);
    printf("\n\t\tnbColumn -> %d", pixel.coreData.dim.nbColumn);
    printf("\n\tfirst bound : ");
    printf("\n\t\tbegin -> %d", pixel.coreData.firstBound.i);
    printf("\n\t\tend -> %d", pixel.coreData.firstBound.j);
    printf("\n\tsecond bound : ");
    printf("\n\t\tbegin -> %d", pixel.coreData.secondBound.i);
    printf("\n\t\tend -> %d", pixel.coreData.secondBound.j);
    printf("\n\taddress : %d", pixel.coreData.address);
    printf("\n\tdiagonale : %d", pixel.coreData.diag);
    printf("\n\tid block data : %d\n", pixel.idBlockData);
}

void printPixelatedBlockData(PixelatedBlockData pBlockData)
{
    printf("\nid block data : %d", pBlockData.idBlockData);
    printf("\n\tid max diag : %d", pBlockData.maxDiag);
    printf("\n\tid number evaluated pixel : %d\n", pBlockData.nbEvaluatePixel + 1);
}

int determineGapLineBetweenTwoBlockData(BlockData root, BlockData blockData)
{
    int diag = getDiagByCoord(blockData.coreData.coord);
    int fragLevel = blockData.fragLevel;
    Coord coord = blockData.coreData.coord;

    if (root.coreData.id == blockData.coreData.id)
    {
        if (diag != 1)
        {
            return 0;
        }
        else
        {
            return -pow(2, maxFrag);
        }
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
    return pow(2, maxFrag - blockData.fragLevel) + determineGapLineBetweenTwoBlockData(root, blockData);
}
int determineGapColumnBetweenTwoBlockData(BlockData root, BlockData blockData)
{
    int diag = getDiagByCoord(blockData.coreData.coord);
    int fragLevel = blockData.fragLevel;
    Coord coord = blockData.coreData.coord;

    if (root.coreData.id == blockData.coreData.id)
    {
        if (diag != 1)
        {
            return 0;
        }
        else
        {
            return -pow(2, maxFrag);
        }
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
    blockData = getBlockDataByCoord(coord);
    return pow(2, maxFrag - blockData.fragLevel) + determineGapColumnBetweenTwoBlockData(root, blockData);
}

void getNeedPixelBlockDataListLine(BlockData root, BlockData blockData, PixelBlockDataListByLineOrColumn *pixelBlockDataList)
{
    int idFirstPBlock = getFirstIdPixelBlock(root);
    int idSecondPBlock = (blockData.coreData.diag == 1 ? 2 : getFirstIdPixelBlock(blockData));
    int gap = determineGapLineBetweenTwoBlockData(blockData, root);
    int stride = idFirstPBlock - gap - idSecondPBlock;

    for (int i = 1; i <= pow(2, maxFrag - root.fragLevel); i++)
    {
        for (int j = pow(2, maxFrag - blockData.fragLevel);
             j >= (blockData.coreData.diag != 1 ? 1 : pow(2, maxFrag) - (i + stride) + 1); j--)
        {
            addBlockDataToPixelBlockDataList(pixelBlockDataList->pixelBlockDataList[i], tabPixelatedBlockData[blockData.coreData.id].pixelBlockDataTab[i + stride][j]);
        }
    }
}

void getNeedPixelBlockDataListColumn(BlockData root, BlockData blockData, PixelBlockDataListByLineOrColumn *pixelBlockDataList)
{
    int idFirstPBlock = getFirstIdPixelBlock(root);
    int idSecondPBlock = (blockData.coreData.diag == 1 ? 2 : getFirstIdPixelBlock(blockData));
    int gap = determineGapColumnBetweenTwoBlockData(blockData, root);
    int stride = idFirstPBlock - gap - idSecondPBlock;

    for (int j = 1; j <= pow(2, maxFrag - root.fragLevel); j++)
    {
        for (int i = pow(2, maxFrag - blockData.fragLevel); i >= (blockData.coreData.diag != 1 ? 1 : pow(2, maxFrag) - (j + stride) + 1); i--)
        {
            addBlockDataToPixelBlockDataList(pixelBlockDataList->pixelBlockDataList[j], tabPixelatedBlockData[blockData.coreData.id].pixelBlockDataTab[i][j + stride]);
        }
    }
}

void getDependPixelBlockDataListLine(BlockData root, BlockData blockData, BlockDataListByLineOrColumn *blockDataList)
{
    int idFirstPBlock = (root.coreData.diag == 1 ? 2 : getFirstIdPixelBlock(root));
    int idSecondPBlock = getFirstIdPixelBlock(blockData);
    int gap = determineGapLineBetweenTwoBlockData(root, blockData);
    int stride = idSecondPBlock - gap - idFirstPBlock;

    for (int i = 1; i <= pow(2, maxFrag - blockData.fragLevel); i++)
    {
        addBlockDataToBlockDataList(blockDataList->blockDataList[i + stride], blockData, 1, 0);
    }
}
void getDependPixelBlockDataListColumn(BlockData root, BlockData blockData, BlockDataListByLineOrColumn *blockDataList)
{
    int idFirstPBlock = (root.coreData.diag == 1 ? 2 : getFirstIdPixelBlock(root));
    int idSecondPBlock = getFirstIdPixelBlock(blockData);
    int gap = determineGapColumnBetweenTwoBlockData(root, blockData);
    int stride = idSecondPBlock - gap - idFirstPBlock;

    for (int i = 1; i <= pow(2, maxFrag - blockData.fragLevel); i++)
    {
        addBlockDataToBlockDataList(blockDataList->blockDataList[i + stride], blockData, 1, 1);
    }
}

PixelBlockDataListByLineOrColumn createPixelBlockDataListByLineOrColumn(int nbLineOrColumn)
{
    PixelBlockDataListByLineOrColumn list;
    list.nbLineOrColumn = nbLineOrColumn;
    list.pixelBlockDataList = (PixelBlockDataList **)malloc((nbLineOrColumn + 1) * sizeof *list.pixelBlockDataList);
    for (int i = 1; i <= nbLineOrColumn; i++)
    {
        list.pixelBlockDataList[i] = createPixelBlockDataList();
    }
    return list;
}

PixelBlockDataList *createPixelBlockDataList()
{
    PixelBlockDataList *list;
    list = (PixelBlockDataList *)malloc(sizeof *list);
    list->pixelBlockData.idBlockData = -1;
    list->next = NULL;
    return list;
}
int isEmptyPixelBlockDataList(PixelBlockDataList *blockDataList)
{
    return (blockDataList->pixelBlockData.idBlockData == -1);
}
void addBlockDataToPixelBlockDataList(PixelBlockDataList *blockDataList, PixelBlockData blockData)
{
    if (isEmptyPixelBlockDataList(blockDataList))
    {
        blockDataList->pixelBlockData = blockData;
    }
    else
    {
        PixelBlockDataList *path = blockDataList;
        while (path->next != NULL)
        {
            path = path->next;
        }

        PixelBlockDataList *l = createPixelBlockDataList();
        l->pixelBlockData = blockData;
        l->next = NULL;
        path->next = l;
    }
    return;
}
BlockDataListByLineOrColumn createBlockDataListByLineOrColumn(int nbLineOrColumn)
{
    BlockDataListByLineOrColumn list;
    list.nbLineOrColumn = nbLineOrColumn;
    list.blockDataList = (BlockDataList **)malloc((nbLineOrColumn + 1) * sizeof *list.blockDataList);
    for (int i = 1; i <= nbLineOrColumn; i++)
    {
        list.blockDataList[i] = createBlockDataList();
    }
    return list;
}