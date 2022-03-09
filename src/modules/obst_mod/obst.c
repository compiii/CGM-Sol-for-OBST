/**
 * @file obst.c
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-10
 *
 * @copyright Compiii Thesis Copyright (c) 2021
 *
 */
#include "obst.h"
#include "clogger.h"

void initOBSTMod()
{
    int i;

    if (tree != NULL)
    {
        free(tree[0]);
        free(tree);
    }

    if (cut != NULL)
    {
        free(cut[0]);
        free(cut);
    }

    tree = (float **)malloc((maxNumber + 1) * sizeof *tree);
    if (tree == NULL)
    {
        logE("insuffisant memory for allocate tree\n");
        exit(EXIT_FAILURE);
    }

    tree[0] = (float *)malloc((maxNumber + 1) * (maxNumber + 1) * sizeof *tree[0]);
    if (tree[0] == NULL)
    {
        logE("insuffisant memory for allocate tree[0]\n");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < (maxNumber + 1); i++)
    {
        tree[i] = tree[0] + i * (maxNumber + 1);
    }

    cut = (int **)malloc((maxNumber + 1) * sizeof *cut);
    if (cut == NULL)
    {
        logE("insuffisant memory for allocate cut\n");
        exit(EXIT_FAILURE);
    }

    cut[0] = (int *)malloc((maxNumber + 1) * (maxNumber + 1) * sizeof *cut[0]);
    if (cut[0] == NULL)
    {
        logE("insuffisant memory for allocate cut[0]\n");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < (maxNumber + 1); i++)
    {
        cut[i] = cut[0] + i * (maxNumber + 1);
    }

    receivedList = createReceivedList();

    idReq = 0, flag = 0, readyToRecv = 0;
    tabReq = malloc(maxBlock * maxDiag * sizeof *tabReq);
}

int knuthAlgorithm()
{
    int d, i, k;
    float tmp;
    for (d = 1; d <= maxNumber; d++)
    {
        for (i = 1; i <= (maxNumber - d + 1); i++)
        {
            tree[i][i + d - 1] = (i == (i + d - 1)) ? w(i, i) : INT_MAX;
            cut[i][i + d - 1] = (i == (i + d - 1)) ? i : INT_MAX;
            if ((i != (i + d - 1)))
            {
                for (k = getCut(i, i + d - 2); k <= getCut(i + 1, i + d - 1) && k != i + d - 1; k++)
                // for (k = i; k < i + d - 1; k++)
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
    return getTree(1, maxNumber);
}

float getTree(int i, int j)
{
    return tree[i][j];
}

int getCut(int i, int j)
{
    return cut[i][j];
}

float w(long i, long j)
{
    int a;
    float r = 0;
    r = tabQ[j];
    for (a = i - 1; a < j; a++)
        r += (tabP[a] + tabQ[a]);
    return r;
}

int max(int m, int n)
{
    return (m >= n ? m : n);
}

int min(int m, int n)
{
    return (m <= n ? m : n);
}

void communicateBlockData(int type, PixelBlockData src, BlockData dest)
{
    int count, blockLengths, stride, absc, ord; //, option = 0, idCurrentDestBlockData;

    count = src.coreData.dim.nbLine;
    blockLengths = src.coreData.dim.nbColumn;
    stride = maxNumber + 1;
    MPI_Type_vector(count, blockLengths, stride, MPI_INT, &vector);
    MPI_Type_commit(&vector);
    absc = src.coreData.firstBound.i;
    ord = src.coreData.secondBound.i;

    if (rank == 1 && type == 100)
        logD("\ntag = %d => %d + %d * %d + %d == coord bloack data (%d, %d) of process %d\n", getIdBlockDataByCoord(dest.coreData.coord) + src.coreData.id * src.coreData.coord.i + src.coreData.coord.j, getIdBlockDataByCoord(dest.coreData.coord), src.coreData.id, src.coreData.coord.i, src.coreData.coord.j, dest.coreData.coord.i, dest.coreData.coord.j, dest.rank);

    if (type == 1)
    {
        sizeDataCom += blockLengths;
    }

    if (type == 0)
        receiveBlockData(tabBlockData[src.idBlockData], vector, absc, ord, src.coreData.address);
    else
        sendBlockData(dest, vector, absc, ord, src.coreData.address);
    MPI_Type_free(&vector);
}

void receiveBlockData(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag)
{
    MPI_Recv(&tree[absc][ord], 1, vector, blockData.rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    receivedList = addTagToReceivedList(receivedList, tag);
}

void sendBlockData(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag)
{
    MPI_Isend(&tree[absc][ord], 1, vector, blockData.rank, tag, MPI_COMM_WORLD, &tabReq[idReq++]);
}

void computeOBST(PixelBlockData pixel, int begin, int end, int init, int option)
{
    int d, i, k;
    float tmp;
    if (option == 0)
    {
        if (rank == 1000)
            logD("first bound i = %d j = %d ----- second bound i = %d j = %d\n", pixel.coreData.firstBound.i, pixel.coreData.firstBound.j, pixel.coreData.secondBound.i, pixel.coreData.secondBound.j);
        for (d = 1; d <= pixel.coreData.secondBound.j; d++)
        {
            for (i = pixel.coreData.firstBound.i; i <= (pixel.coreData.secondBound.j - d + 1); i++)
            {
                tree[i][i + d - 1] = (i == (i + d - 1)) ? 0 : INT_MAX;
                for (k = i; k <= i + d - 2; k++)
                {
                    tmp = getTree(i, k) + getTree(k + 1, i + d - 1) + w(i, i + d - 1);
                    if (tmp < tree[i][i + d - 1])
                    {
                        tree[i][i + d - 1] = tmp;
                    }
                }
            }
        }
    }
    else if (option == 1)
    {
        if (rank == 10000)
            logD("first bound i = %d j = %d ----- second bound i = %d j = %d\n", pixel.coreData.firstBound.i, pixel.coreData.firstBound.j, pixel.coreData.secondBound.i, pixel.coreData.secondBound.j);
        for (d = pixel.coreData.firstBound.j; d >= pixel.coreData.firstBound.i; d--)
        {
            for (i = pixel.coreData.secondBound.i; i <= pixel.coreData.secondBound.j; i++)
            {
                tree[d][i] = INT_MAX;
                for (k = d; k <= i - 1; k++)
                {
                    tmp = getTree(d, k) + getTree(k + 1, i) + w(d, i);
                    if (tmp < tree[d][i])
                    {
                        tree[d][i] = tmp;
                    }
                }
            }
        }
    }
    else if (option == 2)
    {
        if (rank == 2000)
            logD("first bound i = %d j = %d ----- second bound i = %d j = %d\n", pixel.coreData.firstBound.i, pixel.coreData.firstBound.j, pixel.coreData.secondBound.i, pixel.coreData.secondBound.j);
        for (d = pixel.coreData.firstBound.j; d >= pixel.coreData.firstBound.i; d--)
        {
            for (i = pixel.coreData.secondBound.i; i <= pixel.coreData.secondBound.j; i++)
            {
                if (init == 1)
                    tree[d][i] = INT_MAX;
                for (k = begin; k <= end; k++)
                {
                    tmp = getTree(d, k) + getTree(k + 1, i) + w(d, i);
                    if (tmp < tree[d][i])
                    {
                        tree[d][i] = tmp;
                    }
                }
            }
        }
    }
    else if (option == 3)
    {
        if (rank == 10000)
            logD("first bound i = %d j = %d ----- second bound i = %d j = %d\n", pixel.coreData.firstBound.i, pixel.coreData.firstBound.j, pixel.coreData.secondBound.i, pixel.coreData.secondBound.j);
        for (d = pixel.coreData.firstBound.j; d >= pixel.coreData.firstBound.i; d--)
        {
            for (i = pixel.coreData.secondBound.i; i <= pixel.coreData.secondBound.j; i++)
            {
                for (k = pixel.coreData.secondBound.i; k <= i - 1; k++)
                {
                    tmp = getTree(d, k) + getTree(k + 1, i) + w(d, i);
                    if (tmp < tree[d][i])
                    {
                        tree[d][i] = tmp;
                    }
                }

                for (k = d; k <= pixel.coreData.firstBound.j - 1; k++)
                {
                    tmp = getTree(d, k) + getTree(k + 1, i) + w(d, i);
                    if (tmp < tree[d][i])
                    {
                        tree[d][i] = tmp;
                    }
                }
            }
        }
    }
}

ReceivedList *createReceivedList()
{
    ReceivedList *receivedList = (ReceivedList *)malloc(sizeof *receivedList);
    receivedList->tag = -1;
    receivedList->next = NULL;
    return receivedList;
}

ReceivedList *addTagToReceivedList(ReceivedList *receivedList, int tag)
{
    if (isEmptyReceivedList(receivedList))
    {
        receivedList->tag = tag;
        return receivedList;
    }
    else
    {
        ReceivedList *new = createReceivedList();
        new->tag = tag;
        new->next = receivedList;
        if (rank == 1000)
        {
            ReceivedList *path = new;
            while (path != NULL)
            {
                printf("\t %d => ", path->tag);
                path = path->next;
            }
            printf("NULL\n");
        }
        return new;
    }
}

int isEmptyReceivedList(ReceivedList *receivedList)
{
    return receivedList->tag == -1;
}

int findTagToReceivedList(ReceivedList *receivedList, int tag)
{
    if (isEmptyReceivedList(receivedList))
        return 0;
    ReceivedList *path = receivedList;
    while (path != NULL && path->tag != tag)
        path = path->next;
    return (path != NULL ? 1 : 0);
}

SentList *createSentList()
{
    SentList *list = (SentList *)malloc(sizeof *list);
    list->src.idBlockData = -1;
    list->dest.coreData.id = -1;
    list->next = NULL;
    return list;
}

int isEmptySentList(SentList *list)
{
    return list == NULL || (list->src.idBlockData == -1 &&
                            list->dest.coreData.id == -1);
}

SentList *addDataToSentList(SentList *list, PixelBlockData src, BlockData dest)
{
    if (isEmptySentList(list))
    {
        list = createSentList();
        list->src = src;
        list->dest = dest;
        return list;
    }
    else
    {
        SentList *new = createSentList();
        new->src = src;
        new->dest = dest;
        new->next = list;
        /*if (rank == 1000)
        {
            SentList *path = new;
            while (path != NULL)
            {
                printf("\t %d => ", path->tag);
                path = path->next;
            }
            printf("NULL\n");
        }*/
        return new;
    }
}

BlockDataList *buildSendBlockDataList(BlockDataList *list1, BlockDataList *list2)
{
    if (!isEmptyBlockDataList(list1) && isEmptyBlockDataList(list2))
    {
        return list1;
    }
    else if (isEmptyBlockDataList(list1) && !isEmptyBlockDataList(list2))
    {
        return list2;
    }
    else
    {
        BlockDataList *path = list1;
        BlockDataList *check = NULL;
        BlockDataList *updateList = list2;
        BlockDataList *newList = createBlockDataList();
        int find = 0;
        BlockData b;
        int end = 0;
        while (path != NULL && end != 1)
        {
            BlockData b1 = path->blockData;
            find = 0;
            for (check = updateList; check != NULL && find == 0; check = check->next)
            {
                BlockData b2 = check->blockData;
                if (b1.rank == b2.rank)
                {
                    b = (b1.coreData.id < b2.coreData.id ? b1 : b2);
                    find = 1;
                }
            }
            /*while (check != NULL && find == 0)
            {
                BlockData b2 = check->blockData;
                if (b1.rank == b2.rank)
                {
                    b = (b1.coreData.id < b2.coreData.id ? b1 : b2);
                    find = 1;
                }
                else
                {
                    check = check->next;
                }
            }*/
            if (find)
            {
                addBlockDataToBlockDataList(newList, b, 1, 0);
                updateList = removeBlockDataToBlockDataList(updateList, b);
            }
            else
            {
                addBlockDataToBlockDataList(newList, b1, 1, 0);
            }

            if (isEmptyBlockDataList(updateList))
            {
                end = 1;
            }
            path = path->next;
        }

        if (end)
        {
            while (path != NULL)
            {
                addBlockDataToBlockDataList(newList, path->blockData, 1, 0);
                path = path->next;
            }
        }
        else
        {
            while (updateList != NULL)
            {
                addBlockDataToBlockDataList(newList, updateList->blockData, 1, 0);
                updateList = updateList->next;
            }
        }

        return newList;
    }
}

BlockDataList *deleteOccurBlockList(BlockDataList *list)
{
    if (isEmptyBlockDataList(list))
    {
        return list;
    }
    else
    {
        BlockDataList *newList = createBlockDataList();
        addBlockDataToBlockDataList(newList, list->blockData, 1, 0);
        BlockDataList *path = list->next;
        int find = 0;

        while (path != NULL)
        {
            BlockDataList *check;
            find = 0;
            for (check = newList; check != NULL && find == 0; check = check->next)
            {
                BlockData *b = &check->blockData;
                /*if (rank == 0)
                {
                    printf("Testsss rank %d id %d vs rank %d id %d\n", b->rank, b->coreData.id, path->blockData.rank, path->blockData.coreData.id);
                }*/
                if (b->rank == path->blockData.rank)
                {
                    find = 1;
                    if (b->coreData.id > path->blockData.coreData.id)
                    {
                        b = &path->blockData;
                    }
                }
            }
            if (find == 0)
            {
                /*if (rank == 0)
                {
                    printf("Herrrrrrre %d\n", path->blockData.rank);
                }*/
                addBlockDataToBlockDataList(newList, path->blockData, 1, 0);
                /*if (rank == 0)
                {
                    printf("\t\tBegin new List Herrrrrrre \n\n\n");
                    BlockDataList *affiche = newList;
                    while (affiche != NULL)
                    {
                        printBlockData(affiche->blockData);
                        affiche = affiche->next;
                    }

                    printf("\t\tEnd new List Herrrrrrre \n\n\n");
                }*/
            }
            path = path->next;
        }

        /*path = list;
        while (path != NULL)
        {
            newList = deleteOccur(newList, path->blockData);
            path = path->next;
        }*/

        return newList;
    }
}

BlockDataList *removeBlockDataToBlockDataList(BlockDataList *list, BlockData blockData)
{
    int first = 1, find = 0;
    if (isEmptyBlockDataList(list))
        return NULL;
    else
    {
        BlockDataList *path = list;
        BlockDataList *del = path;
        BlockDataList *prec = path;
        while (path != NULL && find == 0)
        {
            if (path->blockData.rank == blockData.rank)
            {
                find = 1;
            }
            else
            {
                first = 0;
                prec = path;
                path = path->next;
            }
        }

        if (first == 1)
        {
            del = list;
            list = list->next;
            free(del);
            path = list;
            prec = list;
        }
        else
        {
            del = path;
            prec->next = path->next;
            free(del);
            path = prec->next;
        }
        return list;
    }
}

void sendPixelBlockData(PixelBlockData src, BlockDataList *dependBlockList, int finalized)
{
    int count, blockLengths, stride, absc, ord;

    int flag;
    int tag = 0;
    int source = 0;

    // int maxTry = 1000000;
    // int compte = 0;

    if (finalized == 0)
    {
        if (!isEmptySentList(sentList)) //&& compte <= maxTry
        {
            SentList *tmp = createSentList();
            while (sentList != NULL)
            {
                tag = sentList->src.coreData.address;
                source = sentList->dest.rank;

                MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
                MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);

                if (flag)
                {
                    count = sentList->src.coreData.dim.nbLine;
                    blockLengths = sentList->src.coreData.dim.nbColumn;
                    stride = maxNumber + 1;
                    MPI_Type_vector(count, blockLengths, stride, MPI_FLOAT, &vector);
                    MPI_Type_commit(&vector);
                    MPI_Type_vector(count, blockLengths, stride, MPI_INT, &vectorBis);
                    MPI_Type_commit(&vectorBis);
                    absc = sentList->src.coreData.firstBound.i;
                    ord = sentList->src.coreData.secondBound.i;

                    sizeDataCom += (blockLengths * count);
                    nbMessageExchange++;

                    tmpDouble = MPI_Wtime();
                    MPI_Send(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD);
                    MPI_Send(&cut[absc][ord], 1, vectorBis, source, tag + 1, MPI_COMM_WORLD);
                    comTime += MPI_Wtime() - tmpDouble;
                    MPI_Type_free(&vector);
                    MPI_Type_free(&vectorBis);
                }
                else
                {
                    // printf("YESSSSSSSSSS rank %d\n", rank);
                    tmp = addDataToSentList(tmp, sentList->src, sentList->dest);
                }

                sentList = sentList->next;
            }
            sentList = tmp;
            // compte++;
        }

        if (!isEmptyBlockDataList(dependBlockList))
        {
            count = src.coreData.dim.nbLine;
            blockLengths = src.coreData.dim.nbColumn;
            stride = maxNumber + 1;
            MPI_Type_vector(count, blockLengths, stride, MPI_FLOAT, &vector);
            MPI_Type_commit(&vector);
            MPI_Type_vector(count, blockLengths, stride, MPI_INT, &vectorBis);
            MPI_Type_commit(&vectorBis);
            absc = src.coreData.firstBound.i;
            ord = src.coreData.secondBound.i;

            tag = src.coreData.address;

            sendedList = createReceivedList();
            while (dependBlockList != NULL)
            {
                source = dependBlockList->blockData.rank;

                if (!findTagToReceivedList(sendedList, dependBlockList->blockData.rank))
                {
                    if (rank == 1000)
                        printBlockData(dependBlockList->blockData);
                    MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
                    MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);

                    if (flag)
                    {
                        if (rank == 10000)
                            printf("\tSTART YEEEEEEEEEEE rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                        sizeDataCom += (blockLengths * count);
                        nbMessageExchange++;
                        tmpDouble = MPI_Wtime();
                        MPI_Send(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD);
                        MPI_Send(&cut[absc][ord], 1, vectorBis, source, tag + 1, MPI_COMM_WORLD);
                        comTime += MPI_Wtime() - tmpDouble;
                        if (rank == 10000)
                            printf("\tENDYEEEEEEEEEEE rank %d source %d tag %d\n", rank, source, tag);
                    }
                    else
                    {
                        if (rank == 10000)
                            printf("\tSTART YOOOOOOOOOO rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                        sentList = addDataToSentList(sentList, src, dependBlockList->blockData);
                        if (rank == 10000)
                            printf("\tEND YOOOOOOOOOO rank %d source %d tag %d\n", rank, source, tag);
                    }
                    sendedList = addTagToReceivedList(sendedList, dependBlockList->blockData.rank);
                }
                dependBlockList = dependBlockList->next;
            }
            free(sendedList);
            MPI_Type_free(&vector);
            MPI_Type_free(&vectorBis);
        }
    }
    else
    {
        if (!isEmptySentList(sentList))
        {
            while (sentList != NULL)
            {
                tag = sentList->src.coreData.address;
                source = sentList->dest.rank;

                MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
                MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);

                count = sentList->src.coreData.dim.nbLine;
                blockLengths = sentList->src.coreData.dim.nbColumn;
                stride = maxNumber + 1;
                MPI_Type_vector(count, blockLengths, stride, MPI_FLOAT, &vector);
                MPI_Type_commit(&vector);
                MPI_Type_vector(count, blockLengths, stride, MPI_INT, &vectorBis);
                MPI_Type_commit(&vectorBis);
                absc = sentList->src.coreData.firstBound.i;
                ord = sentList->src.coreData.secondBound.i;
                sizeDataCom += (blockLengths * count);
                nbMessageExchange++;

                if (flag)
                {
                    tmpDouble = MPI_Wtime();
                    if (rank == 10000)
                        printf("\tSTART YEEEEEEEEEEEEEEEEEEEEEEEEE rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                    MPI_Send(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD);
                    MPI_Send(&cut[absc][ord], 1, vectorBis, source, tag + 1, MPI_COMM_WORLD);
                    if (rank == 10000)
                        printf("\tEND YEEEEEEEEEEEEEEEEEEEEEEEEE rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                    comTime += MPI_Wtime() - tmpDouble;
                }
                else
                {
                    tmpDouble = MPI_Wtime();
                    MPI_Request request;
                    if (rank == 10000)
                        printf("\tSTART YUUUUUUUUUUUUUUUU rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                    MPI_Isend(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD, &request);
                    MPI_Isend(&cut[absc][ord], 1, vectorBis, source, tag + 1, MPI_COMM_WORLD, &request);
                    if (rank == 10000)
                        printf("\tEND YUUUUUUUUUUUUUUUU rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                    comTime += MPI_Wtime() - tmpDouble;
                }
                MPI_Type_free(&vector);
                MPI_Type_free(&vectorBis);

                sentList = sentList->next;
            }
        }

        if (!isEmptyBlockDataList(dependBlockList))
        {
            count = src.coreData.dim.nbLine;
            blockLengths = src.coreData.dim.nbColumn;
            stride = maxNumber + 1;
            MPI_Type_vector(count, blockLengths, stride, MPI_FLOAT, &vector);
            MPI_Type_commit(&vector);
            MPI_Type_vector(count, blockLengths, stride, MPI_INT, &vectorBis);
            MPI_Type_commit(&vectorBis);
            absc = src.coreData.firstBound.i;
            ord = src.coreData.secondBound.i;

            sizeDataCom += (blockLengths * count);
            nbMessageExchange++;

            tag = src.coreData.address;

            sendedList = createReceivedList();
            while (dependBlockList != NULL)
            {
                source = dependBlockList->blockData.rank;

                if (!findTagToReceivedList(sendedList, dependBlockList->blockData.rank))
                {
                    MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
                    MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
                    if (flag)
                    {
                        tmpDouble = MPI_Wtime();
                        if (rank == 10000)
                            printf("\tSTART YRRRRRRRRRRRR rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                        MPI_Send(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD);
                        MPI_Send(&cut[absc][ord], 1, vectorBis, source, tag + 1, MPI_COMM_WORLD);
                        if (rank == 10000)
                            printf("\tEND YRRRRRRRRRRRR rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                        comTime += MPI_Wtime() - tmpDouble;
                    }
                    else
                    {
                        tmpDouble = MPI_Wtime();
                        MPI_Request request;
                        if (rank == 10000)
                            printf("\tSTART YWWWWWWWWWWWW rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                        MPI_Isend(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD, &request);
                        MPI_Isend(&cut[absc][ord], 1, vectorBis, source, tag + 1, MPI_COMM_WORLD, &request);
                        if (rank == 100000)
                            printf("\tEND YWWWWWWWWWWW rank %d source %d tag %d flag = %d\n", rank, source, tag, flag);
                        comTime += MPI_Wtime() - tmpDouble;
                    }
                    sendedList = addTagToReceivedList(sendedList, dependBlockList->blockData.rank);
                }
                dependBlockList = dependBlockList->next;
            }
            free(sendedList);
            MPI_Type_free(&vector);
            MPI_Type_free(&vectorBis);
        }
        free(sentList);
    }
}

void receivePixelBlockData(PixelBlockData src, BlockData dest)
{
    int count, blockLengths, stride, absc, ord;

    count = src.coreData.dim.nbLine;
    blockLengths = src.coreData.dim.nbColumn;
    stride = maxNumber + 1;
    MPI_Type_vector(count, blockLengths, stride, MPI_INT, &vector);
    MPI_Type_commit(&vector);
    absc = src.coreData.firstBound.i;
    ord = src.coreData.secondBound.i;

    int flag;
    int tag = src.coreData.address;
    int source = tabBlockData[src.idBlockData].rank;

    MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);

    if (flag)
    {
        tmpDouble = MPI_Wtime();
        MPI_Recv(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        comTime += MPI_Wtime() - tmpDouble;
    }
    else
    {
        // MPI_Request request;
        tmpDouble = MPI_Wtime();
        MPI_Send(&tag, 1, MPI_INT, source, tag, MPI_COMM_WORLD); //, &request);

        MPI_Recv(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        comTime += MPI_Wtime() - tmpDouble;
    }

    receivedList = addTagToReceivedList(receivedList, tag);
    MPI_Type_free(&vector);
}