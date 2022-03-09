/**
 * @file compiii_dbyd.c
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 1.2
 * @date 2020-07-17
 * 
 * @copyright Compiii Thesis Copyright (c) 2020
 * 
 */
#include "compiii_dbyd.h"
#include "clogger.h"

int compiiiDbyd()
{
    long d = 0;
    int i = 0, last = 0, a, b, iter;
    Block block;
    PixelatedBlock pixelatedBlock;
    Coord coord;
    double buildTime = 0;
    for (d = 1; (d <= maxDiag) && (i != maxEvalBlock); d++)
    {
        block = tabBlock[i];
        pixelatedBlock = tabPixelatedBlock[i];
        while (block.blockData.coreData.diag == d && (i != maxEvalBlock))
        {
            if (d == maxDiag)
                last = 1;

            if (d == 1)
            {
                for (a = pixelatedBlock.pBlockData.nbEvaluatePixel; a <= pixelatedBlock.pBlockData.maxDiag; a++)
                {
                    if (a <= pixelatedBlock.pBlockData.nbEvaluatePixel)
                    {
                        for (b = 1; b <= a; b++)
                        {
                            coord.i = a - b + 1;
                            coord.j = b;
                            if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                            {
                                logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                            }
                            tmpDouble = MPI_Wtime();
                            computeOBST(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], 0, 0, 0, 0);
                            calculTime += MPI_Wtime() - tmpDouble;
                            if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                            {
                                logD("process %d finish the pixel %d Coord(%d,%d) d = %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                            }
                        }

                        for (b = 1; b <= a; b++)
                        {
                            coord.i = a - b + 1;
                            coord.j = b;
                            tmpDouble = MPI_Wtime();
                            BlockDataList *list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                            if (rank == 1000)
                                printf("BEGIN DELETE OCCUR\n");
                            list1 = deleteOccurBlockList(list1);
                            if (rank == 1000)
                            {
                                BlockDataList *affiche = list1;
                                while (affiche != NULL)
                                {
                                    printBlockData(affiche->blockData);
                                    affiche = affiche->next;
                                }
                            }
                            if (rank == 1000)
                                printf("END DELETE OCCUR\n");

                            BlockDataList *list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                            list2 = deleteOccurBlockList(list2);
                            if (rank == 1000)
                            {
                                printf("BEGIN DELETE BUILD\n");
                                BlockDataList *affiche = list2;
                                while (affiche != NULL)
                                {
                                    printBlockData(affiche->blockData);
                                    affiche = affiche->next;
                                }
                                printf("END DELETE BUILD\n");
                            }
                            BlockDataList *path = buildSendBlockDataList(list1, list2);
                            if (rank == 1000)
                            {
                                printf("BEGIN PRINT BUILD\n");
                                BlockDataList *affiche = path;
                                while (affiche != NULL)
                                {
                                    printBlockData(affiche->blockData);
                                    affiche = affiche->next;
                                }
                                printf("END PRINT BUILD\n");
                            }

                            if (!isEmptyBlockDataList(path))
                                if (d == 10000 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            buildTime += MPI_Wtime() - tmpDouble;
                            /*BlockDataList *path = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];

                            if (!isEmptyBlockDataList(path))
                                if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);

                            path = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];

                            if (!isEmptyBlockDataList(path))
                                if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            */

                            /*sendedList = createReceivedList();
                            if (!isEmptyBlockDataList(path))
                                while (path != NULL)
                                {
                                    if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                    {
                                        tmpDouble = MPI_Wtime();
                                        communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                        comTime += MPI_Wtime() - tmpDouble;
                                        sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                        if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                        {
                                            logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                        }
                                    }
                                    path = path->next;
                                }

                            free(sendedList);
                            */

                            /*
                            sendedList = createReceivedList();
                            if (!isEmptyBlockDataList(path))
                                while (path != NULL)
                                {
                                    if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                    {
                                        tmpDouble = MPI_Wtime();
                                        communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                        comTime += MPI_Wtime() - tmpDouble;
                                        sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                        if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                        {
                                            logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                        }
                                    }
                                    path = path->next;
                                }
                            free(sendedList);
                            */
                        }
                    }
                    else
                    {
                        iter = 0;
                        for (b = 2 + (a - pixelatedBlock.pBlockData.nbEvaluatePixel - 1); b <= pixelatedBlock.pBlockData.nbEvaluatePixel; b++)
                        {
                            coord.i = pixelatedBlock.pBlockData.nbEvaluatePixel - iter++;
                            coord.j = b;
                            if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                            {
                                logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                            }
                            tmpDouble = MPI_Wtime();
                            computeOBST(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], 0, 0, 0, 1);
                            calculTime += MPI_Wtime() - tmpDouble;
                            if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                            {
                                logD("process %d finish the pixel %d Coord(%d,%d) d = %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                            }
                        }

                        iter = 0;
                        for (b = 2 + (a - pixelatedBlock.pBlockData.nbEvaluatePixel - 1); b <= pixelatedBlock.pBlockData.nbEvaluatePixel; b++)
                        {
                            coord.i = pixelatedBlock.pBlockData.nbEvaluatePixel - iter++;
                            coord.j = b;

                            tmpDouble = MPI_Wtime();
                            BlockDataList *list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                            list1 = deleteOccurBlockList(list1);
                            BlockDataList *list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                            list2 = deleteOccurBlockList(list2);

                            BlockDataList *path = buildSendBlockDataList(list1, list2);
                            if (!isEmptyBlockDataList(path))
                                if (d == 1000 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            buildTime += MPI_Wtime() - tmpDouble;
                            /*BlockDataList *path = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                            if (!isEmptyBlockDataList(path))
                                if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            //printf("rank %d a = %d b = %d maxDiag = %d\n", rank, a, b, pixelatedBlock.pBlockData.maxDiag);
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);

                            path = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];

                            if (!isEmptyBlockDataList(path))
                                if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);*/

                            /*sendedList = createReceivedList();
                            if (!isEmptyBlockDataList(path))
                                while (path != NULL)
                                {
                                    if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                    {
                                        tmpDouble = MPI_Wtime();
                                        communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                        comTime += MPI_Wtime() - tmpDouble;
                                        sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                        if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                        {
                                            logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                        }
                                    }
                                    path = path->next;
                                }

                            free(sendedList);*/

                            /*
                            sendedList = createReceivedList();
                            if (!isEmptyBlockDataList(path))
                                while (path != NULL)
                                {
                                    if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                    {
                                        tmpDouble = MPI_Wtime();
                                        communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                        comTime += MPI_Wtime() - tmpDouble;
                                        sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                        if (d == 1 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                        {
                                            logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                        }
                                    }
                                    path = path->next;
                                }
                            free(sendedList);*/
                        }
                    }
                }
            }
            else if (d == 2)
            {
                for (a = 1; a <= pixelatedBlock.pBlockData.maxDiag; a++)
                {
                    if (a <= pixelatedBlock.pBlockData.nbEvaluatePixel)
                    {
                        for (b = 1; b <= a; b++)
                        {
                            coord.i = a - b + 1;
                            coord.j = b;

                            if (coord.i == 1)
                            {
                                PixelBlockDataList *path = pixelatedBlock.needPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                                if (!isEmptyPixelBlockDataList(path))
                                    while (path != NULL)
                                    {
                                        if (d == 2 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                        {
                                            logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                                        }
                                        /*tmpDouble = MPI_Wtime();
                                        communicateBlockData(0, path->pixelBlockData, block.blockData);
                                        comTime += MPI_Wtime() - tmpDouble;*/
                                        receivePixelBlockData(path->pixelBlockData, block.blockData);
                                        if (d == 2 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                        {
                                            logW("process %d receive the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                                        }
                                        path = path->next;
                                    }
                            }

                            if (coord.j == 1)
                            {
                                PixelBlockDataList *path = pixelatedBlock.needPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                                if (!isEmptyPixelBlockDataList(path))
                                    while (path != NULL)
                                    {
                                        if (d == 2 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                        {
                                            logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                                        }
                                        /*tmpDouble = MPI_Wtime();
                                        communicateBlockData(0, path->pixelBlockData, block.blockData);
                                        comTime += MPI_Wtime() - tmpDouble;*/
                                        receivePixelBlockData(path->pixelBlockData, block.blockData);
                                        if (d == 2 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                        {
                                            logW("process %d receive the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                                        }
                                        path = path->next;
                                    }
                            }

                            if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                            {
                                logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                            }
                            tmpDouble = MPI_Wtime();
                            computeOBST(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], 0, 0, 0, 0);
                            calculTime += MPI_Wtime() - tmpDouble;
                            if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                            {
                                logD("process %d finish the pixel %d Coord(%d,%d) d = %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                            }
                        }
                        for (b = 1; b <= a; b++)
                        {
                            coord.i = a - b + 1;
                            coord.j = b;

                            tmpDouble = MPI_Wtime();
                            BlockDataList *list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                            list1 = deleteOccurBlockList(list1);
                            BlockDataList *list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                            list2 = deleteOccurBlockList(list2);

                            BlockDataList *path = buildSendBlockDataList(list1, list2);
                            if (!isEmptyBlockDataList(path))
                                if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            buildTime += MPI_Wtime() - tmpDouble;
                            /*BlockDataList *path = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                            if (!isEmptyBlockDataList(path))
                                if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);

                            path = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                            if (!isEmptyBlockDataList(path))
                                if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);*/

                            /*sendedList = createReceivedList();
                            if (!isEmptyBlockDataList(path))
                                while (path != NULL)
                                {
                                    if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                    {
                                        tmpDouble = MPI_Wtime();
                                        communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                        comTime += MPI_Wtime() - tmpDouble;
                                        if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                        {
                                            logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                        }
                                        sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                    }
                                    path = path->next;
                                }

                            free(sendedList);*/

                            /*
                            sendedList = createReceivedList();                            
                            if (!isEmptyBlockDataList(path))
                                while (path != NULL)
                                {
                                    if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                    {
                                        tmpDouble = MPI_Wtime();
                                        communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                        comTime += MPI_Wtime() - tmpDouble;
                                        sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                        if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                        {
                                            logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                        }
                                    }
                                    path = path->next;
                                }
                            free(sendedList);*/
                        }
                    }
                    else
                    {
                        iter = 0;
                        for (b = 2 + (a - pixelatedBlock.pBlockData.nbEvaluatePixel - 1); b <= pixelatedBlock.pBlockData.nbEvaluatePixel; b++)
                        {
                            coord.i = pixelatedBlock.pBlockData.nbEvaluatePixel - iter++;
                            coord.j = b;

                            if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                            {
                                logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                            }
                            tmpDouble = MPI_Wtime();
                            computeOBST(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], 0, 0, 0, 1);
                            calculTime += MPI_Wtime() - tmpDouble;

                            if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                            {
                                logD("process %d finish the pixel %d Coord(%d,%d) d = %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                            }
                        }
                        iter = 0;
                        for (b = 2 + (a - pixelatedBlock.pBlockData.nbEvaluatePixel - 1); b <= pixelatedBlock.pBlockData.nbEvaluatePixel; b++)
                        {
                            coord.i = pixelatedBlock.pBlockData.nbEvaluatePixel - iter++;
                            coord.j = b;

                            tmpDouble = MPI_Wtime();
                            BlockDataList *list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                            list1 = deleteOccurBlockList(list1);
                            BlockDataList *list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                            list2 = deleteOccurBlockList(list2);

                            BlockDataList *path = buildSendBlockDataList(list1, list2);
                            if (!isEmptyBlockDataList(path))
                                if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            buildTime += MPI_Wtime() - tmpDouble;
                            /*BlockDataList *path = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                            if (!isEmptyBlockDataList(path))
                                if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);

                            path = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                            if (!isEmptyBlockDataList(path))
                                if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                {
                                    logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                }
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                            else
                                sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);*/

                            /*sendedList = createReceivedList();
                            if (!isEmptyBlockDataList(path))
                                while (path != NULL)
                                {
                                    if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                    {
                                        tmpDouble = MPI_Wtime();
                                        communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                        comTime += MPI_Wtime() - tmpDouble;
                                        sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                        if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                        {
                                            logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                        }
                                    }
                                    path = path->next;
                                }

                            free(sendedList);*/

                            /*
                            sendedList = createReceivedList();
                            if (!isEmptyBlockDataList(path))
                                while (path != NULL)
                                {
                                    if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                    {
                                        tmpDouble = MPI_Wtime();
                                        communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                        comTime += MPI_Wtime() - tmpDouble;
                                        sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                        if (d == 2 && verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                                        {
                                            logE("process %d send the pixel %d Coord(%d,%d) d = %d to process %d in %f s \t==> a = %d nbEvaluatePixel = %d diag = %d", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, path->blockData.rank, MPI_Wtime() - startTime, a, pixelatedBlock.pBlockData.nbEvaluatePixel, pixelatedBlock.pBlockData.maxDiag);
                                        }
                                    }
                                    path = path->next;
                                }
                            free(sendedList);*/
                        }
                    }
                }
            }
            else
            {
                int finalize = 0, stop = 0, id1P, id2P, gap = 0, tmpGap = 0, diagEval = 1, step = 0, begin, end, init;
                PixelBlockData pixel, pixel1, pixel2;
                PixelBlockDataList *pathPbList;
                BlockDataList *pathBList;
                while (stop != 1)
                {
                    for (a = 1 + step; a <= pixelatedBlock.pBlockData.maxDiag && a <= diagEval; a++)
                    {
                        if (a <= pixelatedBlock.pBlockData.nbEvaluatePixel)
                        {
                            for (b = 1; b <= a; b++)
                            {
                                coord.i = a - b + 1;
                                coord.j = b;
                                pixel = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];
                                id1P = id2P = ceil(pixel.coreData.id / 2.0);
                                if (pixel.coreData.id % 2 == 0)
                                {
                                    id1P += gap;
                                    id2P -= gap;
                                }
                                else if (pixel.coreData.id == 3)
                                {
                                    id2P -= 1;
                                }
                                else
                                {
                                    id1P += gap;
                                    id2P -= (gap + (gap != 0 ? 1 : 0));
                                }

                                if (id1P == pixel.coreData.id - 1)
                                    finalize = 1;

                                if (rank == 3000 && d == 3)
                                    logD("id1P = %d id2P = %d gap = %d\n", id1P, id2P, gap);

                                if (id1P <= pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j]->pixelBlockData.coreData.id)
                                {
                                    pathPbList = pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                                    while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id1P)
                                    {
                                        pathPbList = pathPbList->next;
                                    }
                                    if (pathPbList == NULL)
                                    {
                                        logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id1P %d", rank, d, gap, id1P);
                                        exit(EXIT_FAILURE);
                                    }
                                    pixel1 = pathPbList->pixelBlockData;

                                    if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pixel1.idBlockData].rank)
                                        if (pixel1.idBlockData != block.blockData.coreData.id && (coord.i == 1)) //|| coord.j == 1
                                        {
                                            if (!isEmptyBlockDataList(block.needBlockDataList))
                                            {
                                                pathBList = block.needBlockDataList;
                                                while (pathBList != NULL && pixel1.idBlockData != pathBList->blockData.coreData.id)
                                                {
                                                    pathBList = pathBList->next;
                                                }
                                                if (pathBList != NULL)
                                                {
                                                    if (rank == 1100 && d == 3)
                                                    {
                                                        logD("id1P = %d id2P = %d gap = %d\n", id1P, id2P, gap);
                                                        printPixelBlockData(pixel1);
                                                    }

                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                    //tmpDouble = MPI_Wtime();
                                                    //communicateBlockData(0, pixel1, block.blockData);
                                                    //comTime += MPI_Wtime() - tmpDouble;
                                                    receivePixelBlockData(pixel1, block.blockData);
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d receive the pixel %d Coord(%d, of process %d %d) for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pixel1.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel1, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel1, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (findTagToReceivedList(receivedList, pixel1.coreData.address) == 0)
                                                {
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                    /*tmpDouble = MPI_Wtime();
                                                    communicateBlockData(0, pixel1, block.blockData);
                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                    receivePixelBlockData(pixel1, block.blockData);
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d receive the pixel %d Coord(%d, of process %d %d) for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                }
                                            }
                                        }

                                    if (pixel.coreData.id % 2 != 0 && id1P == id2P)
                                    {
                                        pathPbList = pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                                        while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id1P - 1)
                                        {
                                            pathPbList = pathPbList->next;
                                        }
                                        if (pathPbList == NULL)
                                        {
                                            logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id1P - 1 = %d", rank, d, gap, id1P - 1);
                                            exit(EXIT_FAILURE);
                                        }

                                        if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pathPbList->pixelBlockData.idBlockData].rank)
                                            if (pathPbList->pixelBlockData.idBlockData != block.blockData.coreData.id && (coord.i == 1)) //|| coord.j == 1
                                            {
                                                if (!isEmptyBlockDataList(block.needBlockDataList))
                                                {
                                                    pathBList = block.needBlockDataList;
                                                    while (pathBList != NULL && pathPbList->pixelBlockData.idBlockData != pathBList->blockData.coreData.id)
                                                    {
                                                        pathBList = pathBList->next;
                                                    }
                                                    if (pathBList != NULL)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (findTagToReceivedList(receivedList, pathPbList->pixelBlockData.coreData.address) == 0)
                                                        {
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                            /*tmpDouble = MPI_Wtime();
                                                            communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                            comTime += MPI_Wtime() - tmpDouble;*/
                                                            receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d receive the pixel %d Coord(%d,%d) for  of process %d evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pathPbList->pixelBlockData.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                    }
                                }
                                else
                                {
                                    pixel1 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i - (pixel.coreData.id - id1P)][coord.j];
                                }

                                /*if (id1P != id2P)
                                {*/
                                if (id2P <= pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i]->pixelBlockData.coreData.id)
                                {
                                    pathPbList = pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                                    while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id2P)
                                    {
                                        if (d == 3000 && rank == 3)
                                        {
                                            printPixelBlockData(pathPbList->pixelBlockData);
                                        }
                                        pathPbList = pathPbList->next;
                                    }
                                    if (pathPbList == NULL)
                                    {
                                        logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id2P %d", rank, d, gap, id2P);
                                        exit(EXIT_FAILURE);
                                    }
                                    pixel2 = pathPbList->pixelBlockData;

                                    if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pixel2.idBlockData].rank)
                                        if (pixel2.idBlockData != block.blockData.coreData.id && (coord.j == 1)) //coord.i == 1 ||
                                        {
                                            if (!isEmptyBlockDataList(block.needBlockDataList))
                                            {
                                                pathBList = block.needBlockDataList;
                                                while (pathBList != NULL && pixel2.idBlockData != pathBList->blockData.coreData.id)
                                                {
                                                    if (rank == 200 && d == 4)
                                                    {
                                                        logD("id1P = %d id2P = %d pixel = %d pixel2 = %d gap = %d\n", id1P, id2P, pixel.coreData.id, pixel2.coreData.id, gap);
                                                        //printPixelBlockData(pathPbList->pixelBlockData);
                                                    }
                                                    if (rank == 200 && d == 4)
                                                        printBlockData(pathBList->blockData);
                                                    pathBList = pathBList->next;
                                                }
                                                if (pathBList != NULL)
                                                {
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                    /*tmpDouble = MPI_Wtime();
                                                    communicateBlockData(0, pixel2, block.blockData);
                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                    receivePixelBlockData(pixel2, block.blockData);
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d receive the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pixel2.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel2, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel2, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (findTagToReceivedList(receivedList, pixel2.coreData.address) == 0)
                                                {
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                    /*tmpDouble = MPI_Wtime();
                                                    communicateBlockData(0, pixel2, block.blockData);
                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                    receivePixelBlockData(pixel2, block.blockData);
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d receive the pixel %d Coord(%d, of process %d %d) for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                }
                                            }
                                        }

                                    if (pixel.coreData.id % 2 != 0 && id1P == id2P)
                                    {
                                        pathPbList = pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                                        while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id2P - 1)
                                        {
                                            pathPbList = pathPbList->next;
                                        }
                                        if (pathPbList == NULL)
                                        {
                                            logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id2P - 1 = %d", rank, d, gap, id2P - 1);
                                            exit(EXIT_FAILURE);
                                        }

                                        if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pathPbList->pixelBlockData.idBlockData].rank)
                                            if (pathPbList->pixelBlockData.idBlockData != block.blockData.coreData.id && (coord.j == 1)) //coord.i == 1 ||
                                            {
                                                if (!isEmptyBlockDataList(block.needBlockDataList))
                                                {
                                                    pathBList = block.needBlockDataList;
                                                    while (pathBList != NULL && pathPbList->pixelBlockData.idBlockData != pathBList->blockData.coreData.id)
                                                    {
                                                        pathBList = pathBList->next;
                                                    }
                                                    if (pathBList != NULL)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (findTagToReceivedList(receivedList, pathPbList->pixelBlockData.coreData.address) == 0)
                                                        {
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                            /*tmpDouble = MPI_Wtime();
                                                            communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                            comTime += MPI_Wtime() - tmpDouble;*/
                                                            receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d receive the pixel %d Coord(%d,%d) for  of process %d evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pathPbList->pixelBlockData.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                    }
                                }
                                else
                                {
                                    pixel2 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j - (pixel.coreData.id - id2P)];
                                }
                                /*}
                                else
                                {
                                    pixel2 = pixel1;
                                }*/

                                if (id1P == id2P)
                                {
                                    if (pixel.coreData.id % 2 == 0)
                                    {
                                        begin = pixel1.coreData.firstBound.i - 1;
                                        end = begin;
                                    }
                                    else
                                    {
                                        begin = pixel1.coreData.firstBound.i - 1;
                                        end = pixel1.coreData.firstBound.j;
                                    }
                                    init = 1;
                                    if (d == 50000)
                                    {
                                        printPixelBlockData(pixel1);
                                        printPixelBlockData(pixel2);
                                        printf("begin %d end %d\n", begin, end);
                                    }
                                }
                                else
                                {
                                    if (pixel.coreData.id == 3)
                                    {
                                        begin = pixel1.coreData.firstBound.i - 1;
                                        end = begin;
                                        init = 1;
                                        /*printPixelBlockData(pixel1);
                                        printPixelBlockData(pixel2);
                                        printf("begin %d end %d\n", begin, end);*/
                                    }
                                    else
                                    {
                                        begin = pixel1.coreData.firstBound.i - 1;
                                        end = pixel1.coreData.firstBound.j - 1;
                                        init = 0;
                                        if (d == 50000)
                                        {
                                            printPixelBlockData(pixel1);
                                            printPixelBlockData(pixel2);
                                            printf("begin %d end %d id1P = %d id2P = %d gap = %d\n", begin, end, id1P, id2P, gap);
                                        }
                                    }
                                }

                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                {
                                    logD("process %d start the first (+,min) of the pixel %d Coord(%d,%d) (id1=%d; id2=%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, pixel1.coreData.id, pixel2.coreData.id, d, MPI_Wtime() - startTime);
                                }
                                tmpDouble = MPI_Wtime();
                                computeOBST(pixel, begin, end, init, 2);
                                calculTime += MPI_Wtime() - tmpDouble;

                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                {
                                    logD("process %d finish the first (+,min) of the pixel %d Coord(%d,%d) (id1=%d; id2=%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, pixel1.coreData.id, pixel2.coreData.id, d, MPI_Wtime() - startTime);
                                }

                                if (id1P != id2P)
                                {
                                    if (id2P <= pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j]->pixelBlockData.coreData.id)
                                    {
                                        pathPbList = pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                                        while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id2P)
                                        {
                                            pathPbList = pathPbList->next;
                                        }
                                        if (pathPbList == NULL)
                                        {
                                            logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id2P %d", rank, d, gap, id2P);
                                            exit(EXIT_FAILURE);
                                        }
                                        pixel1 = pathPbList->pixelBlockData;

                                        if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pixel1.idBlockData].rank)
                                            if (pixel1.idBlockData != block.blockData.coreData.id && (coord.i == 1)) //|| coord.j == 1
                                            {
                                                if (!isEmptyBlockDataList(block.needBlockDataList))
                                                {
                                                    pathBList = block.needBlockDataList;
                                                    while (pathBList != NULL && pixel1.idBlockData != pathBList->blockData.coreData.id)
                                                    {
                                                        pathBList = pathBList->next;
                                                    }
                                                    if (pathBList != NULL)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel1, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel1, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (findTagToReceivedList(receivedList, pixel1.coreData.address) == 0)
                                                        {
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                            /*tmpDouble = MPI_Wtime();
                                                            communicateBlockData(0, pixel1, block.blockData);
                                                            comTime += MPI_Wtime() - tmpDouble;*/
                                                            receivePixelBlockData(pixel1, block.blockData);
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d receive the pixel %d Coord(%d,%d) for  of process %d evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pixel1.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel1, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel1, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                    }
                                    else
                                    {
                                        pixel1 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i - (pixel.coreData.id - id2P)][coord.j];
                                    }

                                    if (rank == 300 && d == 3)
                                    {
                                        logD("id1P = %d id2P = %d pixel = %d need pixel = %d gap = %d\n", id1P, id2P, pixel.coreData.id, pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i]->pixelBlockData.coreData.id, gap);
                                    }
                                    if (id1P <= pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i]->pixelBlockData.coreData.id)
                                    {
                                        pathPbList = pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                                        while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id1P)
                                        {
                                            pathPbList = pathPbList->next;
                                        }
                                        if (pathPbList == NULL)
                                        {
                                            logE("the list of need pixel of blocks must contains the id %d", id1P);
                                            exit(EXIT_FAILURE);
                                        }
                                        pixel2 = pathPbList->pixelBlockData;

                                        if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pixel2.idBlockData].rank)
                                            if (pixel2.idBlockData != block.blockData.coreData.id && (coord.j == 1)) // coord.i == 1 ||
                                            {
                                                if (!isEmptyBlockDataList(block.needBlockDataList))
                                                {
                                                    pathBList = block.needBlockDataList;
                                                    while (pathBList != NULL && pixel2.idBlockData != pathBList->blockData.coreData.id)
                                                    {
                                                        pathBList = pathBList->next;
                                                    }
                                                    if (pathBList != NULL)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel2, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel2, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (findTagToReceivedList(receivedList, pixel2.coreData.address) == 0)
                                                        {
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                            /*tmpDouble = MPI_Wtime();
                                                            communicateBlockData(0, pixel2, block.blockData);
                                                            comTime += MPI_Wtime() - tmpDouble;*/
                                                            receivePixelBlockData(pixel2, block.blockData);
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d receive the pixel %d Coord(%d,%d) for  of process %d evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pixel2.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel2, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel2, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                    }
                                    else
                                    {
                                        pixel2 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j - (pixel.coreData.id - id1P)];
                                    }

                                    begin = pixel2.coreData.secondBound.i;
                                    end = pixel2.coreData.secondBound.j;
                                    init = 0;
                                    if (d == 50000)
                                    {
                                        printPixelBlockData(pixel1);
                                        printPixelBlockData(pixel2);
                                        printf("begin %d end %d\n", begin, end);
                                    }
                                    /*printPixelBlockData(pixel1);
                                    printPixelBlockData(pixel2);
                                    printf("begin %d end %d\n", begin, end);*/

                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                    {
                                        logD("process %d start the second (+,min) of the pixel %d Coord(%d,%d) (id1=%d; id2=%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, pixel1.coreData.id, pixel2.coreData.id, d, MPI_Wtime() - startTime);
                                    }
                                    tmpDouble = MPI_Wtime();
                                    computeOBST(pixel, begin, end, init, 2);
                                    calculTime += MPI_Wtime() - tmpDouble;
                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                    {
                                        logD("process %d finish the second (+,min) of the pixel %d Coord(%d,%d) (id1=%d; id2=%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, pixel1.coreData.id, pixel2.coreData.id, d, MPI_Wtime() - startTime);
                                    }
                                }

                                if (finalize)
                                {
                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                    {
                                        logD("process %d start the finalisation of the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                    }

                                    tmpDouble = MPI_Wtime();
                                    computeOBST(pixel, 0, 0, 0, 3);
                                    calculTime += MPI_Wtime() - tmpDouble;

                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                    {
                                        logD("process %d finishs the finalisation of the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                    }
                                }
                            }

                            if (finalize)
                            {
                                for (b = 1; b <= a; b++)
                                {
                                    coord.i = a - b + 1;
                                    coord.j = b;

                                    if (d == 3 && rank == 200)
                                    {
                                        printBlockData(block.blockData);
                                        printPixelBlockData(pixel);
                                    }
                                    tmpDouble = MPI_Wtime();

                                    BlockDataList *list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                                    list1 = deleteOccurBlockList(list1);
                                    BlockDataList *list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                                    list2 = deleteOccurBlockList(list2);

                                    BlockDataList *path = buildSendBlockDataList(list1, list2);

                                    if (a == pixelatedBlock.pBlockData.maxDiag)
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                                    else
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                                    buildTime += MPI_Wtime() - tmpDouble;
                                    /*BlockDataList *path = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                                    if (a == pixelatedBlock.pBlockData.maxDiag)
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                                    else
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);

                                    path = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                                    if (a == pixelatedBlock.pBlockData.maxDiag)
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                                    else
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);*/
                                    /*sendedList = createReceivedList();
                                    if (!isEmptyBlockDataList(path))
                                        while (path != NULL)
                                        {
                                            if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                            {
                                                tmpDouble = MPI_Wtime();
                                                communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                                comTime += MPI_Wtime() - tmpDouble;
                                                sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                            }
                                            path = path->next;
                                        }

                                    free(sendedList);*/
                                    //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);

                                    /*
                                    sendedList = createReceivedList();
                                    if (!isEmptyBlockDataList(path))
                                        while (path != NULL)
                                        {
                                            if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                            {
                                                tmpDouble = MPI_Wtime();
                                                communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                                comTime += MPI_Wtime() - tmpDouble;
                                                sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                            }
                                            path = path->next;
                                        }
                                    free(sendedList);*/
                                    //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                                }
                            }
                        }
                        else
                        {
                            iter = 0;
                            for (b = 2 + (a - pixelatedBlock.pBlockData.nbEvaluatePixel - 1); b <= pixelatedBlock.pBlockData.nbEvaluatePixel; b++)
                            {
                                coord.i = pixelatedBlock.pBlockData.nbEvaluatePixel - iter++;
                                coord.j = b;

                                pixel = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];
                                id1P = id2P = ceil(pixel.coreData.id / 2.0);
                                if (pixel.coreData.id % 2 == 0)
                                {
                                    id1P += gap;
                                    id2P -= gap;
                                }
                                else
                                {
                                    id1P += gap;
                                    id2P -= (gap + (gap != 0 ? 1 : 0));
                                }

                                if (id1P == pixel.coreData.id - 1)
                                    finalize = 1;

                                if (id1P <= pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j]->pixelBlockData.coreData.id)
                                {
                                    pathPbList = pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                                    while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id1P)
                                    {
                                        pathPbList = pathPbList->next;
                                    }
                                    if (pathPbList == NULL)
                                    {
                                        logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id1P %d", rank, d, gap, id1P);
                                        exit(EXIT_FAILURE);
                                    }
                                    pixel1 = pathPbList->pixelBlockData;

                                    if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pixel1.idBlockData].rank)
                                        if (pixel1.idBlockData != block.blockData.coreData.id && (coord.i == 1)) //|| coord.j == 1
                                        {
                                            if (!isEmptyBlockDataList(block.needBlockDataList))
                                            {
                                                pathBList = block.needBlockDataList;
                                                while (pathBList != NULL && pixel1.idBlockData != pathBList->blockData.coreData.id)
                                                {
                                                    pathBList = pathBList->next;
                                                }
                                                if (pathBList != NULL)
                                                {
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                    /*tmpDouble = MPI_Wtime();
                                                    communicateBlockData(0, pixel1, block.blockData);
                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                    receivePixelBlockData(pixel1, block.blockData);
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d receive the pixel %d Coord(%d, of process %d %d) for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pixel1.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel1, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel1, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (findTagToReceivedList(receivedList, pixel1.coreData.address) == 0)
                                                {
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                    /*tmpDouble = MPI_Wtime();
                                                    communicateBlockData(0, pixel1, block.blockData);
                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                    receivePixelBlockData(pixel1, block.blockData);
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d receive the pixel %d Coord(%d, of process %d %d) for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                }
                                            }

                                            if (pixel.coreData.id % 2 != 0 && id1P == id2P)
                                            {
                                                pathPbList = pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                                                while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id1P - 1)
                                                {
                                                    pathPbList = pathPbList->next;
                                                }
                                                if (pathPbList == NULL)
                                                {
                                                    logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id1P - 1 = %d", rank, d, gap, id1P - 1);
                                                    exit(EXIT_FAILURE);
                                                }

                                                if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pathPbList->pixelBlockData.idBlockData].rank)
                                                    if (pathPbList->pixelBlockData.idBlockData != block.blockData.coreData.id && (coord.i == 1)) //|| coord.j == 1
                                                    {
                                                        if (!isEmptyBlockDataList(block.needBlockDataList))
                                                        {
                                                            pathBList = block.needBlockDataList;
                                                            while (pathBList != NULL && pathPbList->pixelBlockData.idBlockData != pathBList->blockData.coreData.id)
                                                            {
                                                                pathBList = pathBList->next;
                                                            }
                                                            if (pathBList != NULL)
                                                            {
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d wait the pixel %d Coord(%d,%d) for ev of process %d aluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                                /*tmpDouble = MPI_Wtime();
                                                                communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                                comTime += MPI_Wtime() - tmpDouble;*/
                                                                receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d receive the pixel %d Coord(%d,%d) for eval of process %d uate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                            }
                                                            else
                                                            {
                                                                if (findTagToReceivedList(receivedList, pathPbList->pixelBlockData.coreData.address) == 0)
                                                                {
                                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                    {
                                                                        logW("process %d wait the pixel %d Coord(%d,%d) for evaluate of process %d  the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                    }
                                                                    /*tmpDouble = MPI_Wtime();
                                                                    communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                                    receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                    {
                                                                        logW("process %d receive the pixel %d Coord(%d,%d) for evaluate of process %d  the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            if (findTagToReceivedList(receivedList, pathPbList->pixelBlockData.coreData.address) == 0)
                                                            {
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d wait the pixel %d Coord(%d,%d) for ev of process %d aluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                                /*tmpDouble = MPI_Wtime();
                                                                communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                                comTime += MPI_Wtime() - tmpDouble;*/
                                                                receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d receive the pixel %d Coord(%d,%d) for eval of process %d uate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                            }
                                                        }
                                                    }
                                            }
                                        }
                                }
                                else
                                {
                                    pixel1 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i - (pixel.coreData.id - id1P)][coord.j];
                                }

                                if (id2P <= pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i]->pixelBlockData.coreData.id)
                                {
                                    pathPbList = pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                                    while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id2P)
                                    {
                                        pathPbList = pathPbList->next;
                                    }
                                    if (pathPbList == NULL)
                                    {
                                        logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id2P %d", rank, d, gap, id2P);
                                        exit(EXIT_FAILURE);
                                    }
                                    pixel2 = pathPbList->pixelBlockData;

                                    if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pixel2.idBlockData].rank)
                                        if (pixel2.idBlockData != block.blockData.coreData.id && (coord.j == 1)) //coord.i == 1 ||
                                        {
                                            if (!isEmptyBlockDataList(block.needBlockDataList))
                                            {
                                                pathBList = block.needBlockDataList;
                                                while (pathBList != NULL && pixel2.idBlockData != pathBList->blockData.coreData.id)
                                                {
                                                    pathBList = pathBList->next;
                                                }
                                                if (pathBList != NULL)
                                                {
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                    /*tmpDouble = MPI_Wtime();
                                                    communicateBlockData(0, pixel2, block.blockData);
                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                    receivePixelBlockData(pixel2, block.blockData);
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d receive the pixel %d Coord(%d, of process %d %d) for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pixel2.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel2, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel2, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (findTagToReceivedList(receivedList, pixel2.coreData.address) == 0)
                                                {
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                    /*tmpDouble = MPI_Wtime();
                                                    communicateBlockData(0, pixel2, block.blockData);
                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                    receivePixelBlockData(pixel2, block.blockData);
                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                    {
                                                        logW("process %d receive the pixel %d Coord(%d, of process %d %d) for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                    }
                                                }
                                            }

                                            if (pixel.coreData.id % 2 != 0 && id1P == id2P)
                                            {
                                                pathPbList = pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                                                while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id2P - 1)
                                                {
                                                    pathPbList = pathPbList->next;
                                                }
                                                if (pathPbList == NULL)
                                                {
                                                    logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id2P - 1 = %d", rank, d, gap, id2P - 1);
                                                    exit(EXIT_FAILURE);
                                                }

                                                if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pathPbList->pixelBlockData.idBlockData].rank)
                                                    if (pathPbList->pixelBlockData.idBlockData != block.blockData.coreData.id && (coord.j == 1)) //coord.i == 1 ||
                                                    {
                                                        if (!isEmptyBlockDataList(block.needBlockDataList))
                                                        {
                                                            pathBList = block.needBlockDataList;
                                                            while (pathBList != NULL && pathPbList->pixelBlockData.idBlockData != pathBList->blockData.coreData.id)
                                                            {
                                                                pathBList = pathBList->next;
                                                            }
                                                            if (pathBList != NULL)
                                                            {
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d wait the pixel %d Coord(%d,%d) for ev of process %d aluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                                /*tmpDouble = MPI_Wtime();
                                                                communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                                comTime += MPI_Wtime() - tmpDouble;*/
                                                                receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d wait the pixel %d Coord(%d,%d) for ev of process %d aluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d receive the pixel %d Coord(%d,%d) for eval of process %d uate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                            }
                                                            else
                                                            {
                                                                if (findTagToReceivedList(receivedList, pathPbList->pixelBlockData.coreData.address) == 0)
                                                                {
                                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                    {
                                                                        logW("process %d wait the pixel %d Coord(%d,%d) for evaluate of process %d  the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                    }
                                                                    /*tmpDouble = MPI_Wtime();
                                                                    communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                                    comTime += MPI_Wtime() - tmpDouble;*/
                                                                    receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                    {
                                                                        logW("process %d receive the pixel %d Coord(%d,%d) for evaluate of process %d  the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        else
                                                        {
                                                            if (findTagToReceivedList(receivedList, pathPbList->pixelBlockData.coreData.address) == 0)
                                                            {
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d wait the pixel %d Coord(%d,%d) for ev of process %d aluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                                /*tmpDouble = MPI_Wtime();
                                                                communicateBlockData(0, pathPbList->pixelBlockData, block.blockData);
                                                                comTime += MPI_Wtime() - tmpDouble;*/
                                                                receivePixelBlockData(pathPbList->pixelBlockData, block.blockData);
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d wait the pixel %d Coord(%d,%d) for ev of process %d aluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                                {
                                                                    logW("process %d receive the pixel %d Coord(%d,%d) for eval of process %d uate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pathPbList->pixelBlockData.coreData.id, pathPbList->pixelBlockData.coreData.coord.i, pathPbList->pixelBlockData.coreData.coord.j, tabBlockData[pathPbList->pixelBlockData.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                                }
                                                            }
                                                        }
                                                    }
                                            }
                                        }
                                }
                                else
                                {
                                    pixel2 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j - (pixel.coreData.id - id2P)];
                                }

                                if (id1P == id2P)
                                {
                                    if (pixel.coreData.id % 2 == 0)
                                    {
                                        begin = pixel1.coreData.firstBound.i - 1;
                                        end = begin;
                                    }
                                    else
                                    {
                                        begin = pixel1.coreData.firstBound.i - 1;
                                        end = pixel1.coreData.firstBound.j;
                                    }
                                    init = 1;
                                    if (d == 50000)
                                    {
                                        printPixelBlockData(pixel1);
                                        printPixelBlockData(pixel2);
                                        printf("begin %d end %d\n", begin, end);
                                    }
                                }
                                else
                                {
                                    begin = pixel1.coreData.firstBound.i - 1;
                                    end = pixel1.coreData.firstBound.j - 1;
                                    init = 0;
                                    if (d == 50000)
                                    {
                                        printPixelBlockData(pixel1);
                                        printPixelBlockData(pixel2);
                                        printf("begin %d end %d id1P = %d id2P = %d gap = %d\n", begin, end, id1P, id2P, gap);
                                    }
                                }

                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                {
                                    logD("process %d start the first (+,min) of the pixel %d Coord(%d,%d) (id1=%d; id2=%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, pixel1.coreData.id, pixel2.coreData.id, d, MPI_Wtime() - startTime);
                                }
                                tmpDouble = MPI_Wtime();
                                computeOBST(pixel, begin, end, init, 2);
                                calculTime += MPI_Wtime() - tmpDouble;
                                if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                {
                                    logD("process %d finish the first (+,min) of the pixel %d Coord(%d,%d) (id1=%d; id2=%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, pixel1.coreData.id, pixel2.coreData.id, d, MPI_Wtime() - startTime);
                                }

                                if (id1P != id2P)
                                {
                                    if (id2P <= pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j]->pixelBlockData.coreData.id)
                                    {
                                        pathPbList = pixelatedBlock.needAllPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                                        while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id2P)
                                        {
                                            pathPbList = pathPbList->next;
                                        }
                                        if (pathPbList == NULL)
                                        {
                                            logE("process %d diagonal %d gap = %d => the list of need pixel of blocks must contains the id2P %d", rank, d, gap, id2P);
                                            exit(EXIT_FAILURE);
                                        }
                                        pixel1 = pathPbList->pixelBlockData;

                                        if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pixel1.idBlockData].rank)
                                            if (pixel1.idBlockData != block.blockData.coreData.id && (coord.i == 1)) //|| coord.j == 1
                                            {
                                                if (!isEmptyBlockDataList(block.needBlockDataList))
                                                {
                                                    pathBList = block.needBlockDataList;
                                                    while (pathBList != NULL && pixel1.idBlockData != pathBList->blockData.coreData.id)
                                                    {
                                                        pathBList = pathBList->next;
                                                    }
                                                    if (pathBList != NULL)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel1, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel1, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (findTagToReceivedList(receivedList, pixel1.coreData.address) == 0)
                                                        {
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                            /*tmpDouble = MPI_Wtime();
                                                            communicateBlockData(0, pixel1, block.blockData);
                                                            comTime += MPI_Wtime() - tmpDouble;*/
                                                            receivePixelBlockData(pixel1, block.blockData);
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d receive the pixel %d Coord(%d,%d) for  of process %d evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pixel1.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel1, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel1, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel1.coreData.id, pixel1.coreData.coord.i, pixel1.coreData.coord.j, tabBlockData[pixel1.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                    }
                                    else
                                    {
                                        pixel1 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i - (pixel.coreData.id - id2P)][coord.j];
                                    }

                                    if (id1P <= pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i]->pixelBlockData.coreData.id)
                                    {
                                        pathPbList = pixelatedBlock.needAllPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                                        while (pathPbList != NULL && pathPbList->pixelBlockData.coreData.id != id1P)
                                        {
                                            pathPbList = pathPbList->next;
                                        }
                                        if (pathPbList == NULL)
                                        {
                                            logE("the list of need pixel of blocks must contains the id %d", id1P);
                                            exit(EXIT_FAILURE);
                                        }
                                        pixel2 = pathPbList->pixelBlockData;

                                        if (tabBlockData[pixel.idBlockData].rank != tabBlockData[pixel2.idBlockData].rank)
                                            if (pixel2.idBlockData != block.blockData.coreData.id && (coord.j == 1)) //coord.i == 1 ||
                                            {
                                                if (!isEmptyBlockDataList(block.needBlockDataList))
                                                {
                                                    pathBList = block.needBlockDataList;
                                                    while (pathBList != NULL && pixel2.idBlockData != pathBList->blockData.coreData.id)
                                                    {
                                                        pathBList = pathBList->next;
                                                    }
                                                    if (pathBList != NULL)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d  for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel2, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel2, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                    else
                                                    {
                                                        if (findTagToReceivedList(receivedList, pixel2.coreData.address) == 0)
                                                        {
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                            /*tmpDouble = MPI_Wtime();
                                                            communicateBlockData(0, pixel2, block.blockData);
                                                            comTime += MPI_Wtime() - tmpDouble;*/
                                                            receivePixelBlockData(pixel2, block.blockData);
                                                            if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                            {
                                                                logW("process %d receive the pixel %d Coord(%d,%d) for  of process %d evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                            }
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    if (findTagToReceivedList(receivedList, pixel2.coreData.address) == 0)
                                                    {
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d wait the pixel %d Coord(%d,%d) of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                        /*tmpDouble = MPI_Wtime();
                                                        communicateBlockData(0, pixel2, block.blockData);
                                                        comTime += MPI_Wtime() - tmpDouble;*/
                                                        receivePixelBlockData(pixel2, block.blockData);
                                                        if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                                        {
                                                            logW("process %d receive the pixel %d Coord(%d,%d)  of process %d for evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel2.coreData.id, pixel2.coreData.coord.i, pixel2.coreData.coord.j, tabBlockData[pixel2.idBlockData].rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                                        }
                                                    }
                                                }
                                            }
                                    }
                                    else
                                    {
                                        pixel2 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j - (pixel.coreData.id - id1P)];
                                    }

                                    begin = pixel2.coreData.secondBound.i;
                                    end = pixel2.coreData.secondBound.j;
                                    init = 0;
                                    if (d == 50000)
                                    {
                                        printPixelBlockData(pixel1);
                                        printPixelBlockData(pixel2);
                                        printf("begin %d end %d\n", begin, end);
                                    }
                                    /*printPixelBlockData(pixel1);
                                    printPixelBlockData(pixel2);
                                    printf("begin %d end %d\n", begin, end);*/

                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                    {
                                        logD("process %d start the second (+,min) of the pixel %d Coord(%d,%d) (id1=%d; id2=%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, pixel1.coreData.id, pixel2.coreData.id, d, MPI_Wtime() - startTime);
                                    }
                                    tmpDouble = MPI_Wtime();
                                    computeOBST(pixel, begin, end, init, 2);
                                    calculTime += MPI_Wtime() - tmpDouble;
                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                    {
                                        logD("process %d finish the second (+,min) of the pixel %d Coord(%d,%d) (id1=%d; id2=%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, pixel1.coreData.id, pixel2.coreData.id, d, MPI_Wtime() - startTime);
                                    }
                                }

                                if (finalize)
                                {

                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                    {
                                        logD("process %d start the finalisation of the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                    }

                                    tmpDouble = MPI_Wtime();
                                    computeOBST(pixel, 0, 0, 0, 3);
                                    calculTime += MPI_Wtime() - tmpDouble;

                                    if (d == 3 && verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                                    {
                                        logD("process %d finish the finalisation of the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixel.coreData.id, pixel.coreData.coord.i, pixel.coreData.coord.j, d, MPI_Wtime() - startTime);
                                    }
                                }
                            }

                            if (finalize)
                            {
                                iter = 0;
                                for (b = 2 + (a - pixelatedBlock.pBlockData.nbEvaluatePixel - 1); b <= pixelatedBlock.pBlockData.nbEvaluatePixel; b++)
                                {
                                    coord.i = pixelatedBlock.pBlockData.nbEvaluatePixel - iter++;
                                    coord.j = b;

                                    tmpDouble = MPI_Wtime();

                                    BlockDataList *list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                                    list1 = deleteOccurBlockList(list1);
                                    BlockDataList *list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                                    list2 = deleteOccurBlockList(list2);

                                    BlockDataList *path = buildSendBlockDataList(list1, list2);

                                    if (a == pixelatedBlock.pBlockData.maxDiag)
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                                    else
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);
                                    buildTime += MPI_Wtime() - tmpDouble;
                                    /*BlockDataList *path = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                                    if (a == pixelatedBlock.pBlockData.maxDiag)
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                                    else
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);

                                    path = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                                    if (a == pixelatedBlock.pBlockData.maxDiag)
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 1);
                                    else
                                        sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);*/
                                    /*sendedList = createReceivedList();
                                    if (!isEmptyBlockDataList(path))
                                        while (path != NULL)
                                        {
                                            if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                            {
                                                tmpDouble = MPI_Wtime();
                                                communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                                comTime += MPI_Wtime() - tmpDouble;
                                                sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                            }
                                            path = path->next;
                                        }

                                    free(sendedList);*/
                                    //sendPixelBlockData(pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path, 0);

                                    /*
                                    sendedList = createReceivedList();
                                    if (!isEmptyBlockDataList(path))
                                        while (path != NULL)
                                        {
                                            if (!findTagToReceivedList(sendedList, path->blockData.rank))
                                            {
                                                tmpDouble = MPI_Wtime();
                                                communicateBlockData(1, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j], path->blockData);
                                                comTime += MPI_Wtime() - tmpDouble;
                                                sendedList = addTagToReceivedList(sendedList, path->blockData.rank);
                                            }
                                            path = path->next;
                                        }
                                    free(sendedList);*/
                                }
                            }
                        }

                        if (a % 2 != 0)
                            gap--;

                        if (finalize == 1)
                        {
                            if (a == pixelatedBlock.pBlockData.maxDiag)
                                stop = 1;
                            else
                            {
                                step++;
                                if (a % 2 != 0)
                                    tmpGap--;
                                finalize = 0;
                            }
                        }
                    }

                    if (stop != 1)
                    {
                        if (diagEval != pixelatedBlock.pBlockData.maxDiag)
                            diagEval += 2;
                        gap = tmpGap;
                        tmpGap = ++gap;
                    }
                }
                //printf("Finish diag %ld => rank %d\n", d, rank);
            }

            if ((d == 1 || d == 2 || d == 3) && block.blockData.rank == rank && block.blockData.coreData.diag == d)
            {
                logI("process %d finishs d = %d in %f s", rank, d, MPI_Wtime() - startTime);
            }

            i++;
            if (i != maxEvalBlock)
            {
                block = tabBlock[i];
                pixelatedBlock = tabPixelatedBlock[i];
            }
        }

        if ((rank == 4 || rank == 5 || rank == 6) && d == 200)
        {
            logD("process %d finishs d = %d in %f s", rank, d, MPI_Wtime() - startTime);
        }

        if ((rank == 5 || rank == 6 || rank == 7) && d == 500)
        {
            logD("process %d finishs d = %d in %f s", rank, d, MPI_Wtime() - startTime);
        }

        if ((rank == 0 || rank == 3) && d == 200)
        {
            logD("process %d finishs d = %d in %f s", rank, d, MPI_Wtime() - startTime);
        }

        if ((rank == 1 || rank == 2 || rank == 3) && d == 400)
        {
            logD("process %d finishs d = %d in %f s", rank, d, MPI_Wtime() - startTime);
        }

        /*if ((d == 2 && (rank == 6 || rank == 7 || rank == 8 || rank == 9 || rank == 10)) || ((d == 3 && (rank == 11 || rank == 12 || rank == 13 || rank == 14))))
        {
            logD("process %d finishs d = %d in %f s", rank, d, MPI_Wtime() - startTime);
        }*/
    }
    free(receivedList);

    if (last == 5000 || rank == 200000)
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
                    printf("%d\t", i); //getMCOP(i, j)
            }
            printf("\n");
        }
    }

    logD("rank %d : build time ==> %f s\n", rank, buildTime);
    return (last ? last : 0); //getMCOP(1, maxNumber)
}