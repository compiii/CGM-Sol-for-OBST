/**
 * @file compiii_4s.c
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-01-14
 *
 * @copyright Compiii Thesis Copyright (c) 2022
 *
 */
#include "compiii_4s.h"
#include "clogger.h"

int compiii4s()
{
    long d = 0;
    int i = 0, last = 0;
    Block block;
    PixelBlockData pixel1, pixel2, pixel3, pixel4;
    PixelatedBlock pixelatedBlock;
    Coord coord, coordBis;
    BlockDataList *pathP1, *pathP2, *pathP3, *pathP4;
    PixelBlockDataList *path;
    BlockDataList *list1, *list2;

    receivedList4s = createReceivedList4s();
    for (d = 1; (d <= maxDiag) && (i != maxEvalBlock); d++)
    {
        block = tabBlock[i];
        pixelatedBlock = tabPixelatedBlock[i];
        while (tabBlockData[pixelatedBlock.pBlockData.idBlockData].coreData.diag == d && (i != maxEvalBlock))
        {
            if (d == maxDiag)
                last = 1;

            /*if (d != 1)
            {
                coord.i = 1;
                coord.j = 1;
                pixel1 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];

                path = pixelatedBlock.needPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                if (!isEmptyPixelBlockDataList(path))
                    while (path != NULL)
                    {
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        if (tabBlockData[path->pixelBlockData.idBlockData].fragLevel < maxFrag - 1 && tabBlockData[path->pixelBlockData.idBlockData].fragLevel != tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel)
                        {
                            coordBis.i = (int)ceil(path->pixelBlockData.coreData.coord.i / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            coordBis.j = (int)ceil(path->pixelBlockData.coreData.coord.j / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            PixelBlockData kBlock = tabPixelatedBlockData[path->pixelBlockData.idBlockData].pixelBlockDataTab[coordBis.i][coordBis.j];
                            if (findTagToReceivedList4s(receivedList4s, kBlock.coreData.id, kBlock.coreData.address, kBlock.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(kBlock, block.blockData);
                            }
                        }
                        else
                        {
                            if (findTagToReceivedList4s(receivedList4s, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.address, path->pixelBlockData.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(path->pixelBlockData, block.blockData);
                            }
                        }
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d received the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        path = path->next;
                    }

                path = pixelatedBlock.needPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                if (!isEmptyPixelBlockDataList(path))
                    while (path != NULL)
                    {
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        if (tabBlockData[path->pixelBlockData.idBlockData].fragLevel < maxFrag - 1 && tabBlockData[path->pixelBlockData.idBlockData].fragLevel != tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel)
                        {
                            if (rank == 7 && d == 5 && path->pixelBlockData.coreData.id == 9000)
                            {
                                printPixelBlockData(path->pixelBlockData);
                            }
                            coordBis.i = (int)ceil(path->pixelBlockData.coreData.coord.i / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            coordBis.j = (int)ceil(path->pixelBlockData.coreData.coord.j / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            PixelBlockData kBlock = tabPixelatedBlockData[path->pixelBlockData.idBlockData].pixelBlockDataTab[coordBis.i][coordBis.j];
                            if (rank == 7 && d == 5 && path->pixelBlockData.coreData.id == 9000)
                            {
                                printPixelBlockData(kBlock);
                            }
                            if (findTagToReceivedList4s(receivedList4s, kBlock.coreData.id, kBlock.coreData.address, kBlock.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(kBlock, block.blockData);
                            }
                        }
                        else
                        {
                            if (findTagToReceivedList4s(receivedList4s, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.address, path->pixelBlockData.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(path->pixelBlockData, block.blockData);
                            }
                        }
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d recieved the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        path = path->next;
                    }

                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }
                tmpDouble = MPI_Wtime();
                computePixelBlock4s(pixel1, (block.blockData.coreData.diag == 1 ? 0 : 1));
                calculTime += MPI_Wtime() - tmpDouble;
                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d finishes the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }

                tmpDouble = MPI_Wtime();

            }

            coord.i = 2;
            coord.j = 1;
            pixel2 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];

            path = pixelatedBlock.needPixelBlockDataListByLine.pixelBlockDataList[coord.i];
            if (!isEmptyPixelBlockDataList(path))
                while (path != NULL)
                {
                    if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                    {
                        logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                    }
                    if (tabBlockData[path->pixelBlockData.idBlockData].fragLevel < maxFrag - 1 && tabBlockData[path->pixelBlockData.idBlockData].fragLevel != tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel)
                    {
                        coordBis.i = (int)ceil(path->pixelBlockData.coreData.coord.i / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                        coordBis.j = (int)ceil(path->pixelBlockData.coreData.coord.j / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                        PixelBlockData kBlock = tabPixelatedBlockData[path->pixelBlockData.idBlockData].pixelBlockDataTab[coordBis.i][coordBis.j];
                        if (findTagToReceivedList4s(receivedList4s, kBlock.coreData.id, kBlock.coreData.address, kBlock.coreData.coord) == 0)
                        {
                            receivePixelBlockData4s(kBlock, block.blockData);
                        }
                    }
                    else
                    {
                        if (findTagToReceivedList4s(receivedList4s, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.address, path->pixelBlockData.coreData.coord) == 0)
                        {
                            receivePixelBlockData4s(path->pixelBlockData, block.blockData);
                        }
                    }
                    if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                    {
                        logW("process %d received the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                    }
                    path = path->next;
                }

            if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
            {
                logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
            }
            tmpDouble = MPI_Wtime();
            computePixelBlock4s(pixel2, (block.blockData.coreData.diag == 1 ? 0 : 1));
            calculTime += MPI_Wtime() - tmpDouble;
            if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
            {
                logD("process %d finishes the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
            }

            if (d != 1)
            {
                coord = pixel1.coreData.coord;
                list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                list2 = deleteOccurBlockList(list2);
                sendPixelBlockData(pixel1, list2, 1);
            }

            coord = pixel2.coreData.coord;
            list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
            list2 = deleteOccurBlockList(list2);
            sendPixelBlockData(pixel2, list2, 1);

            coord.i = 1;
            coord.j = 2;
            pixel3 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];

            path = pixelatedBlock.needPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
            if (!isEmptyPixelBlockDataList(path))
                while (path != NULL)
                {
                    if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                    {
                        logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                    }
                    if (tabBlockData[path->pixelBlockData.idBlockData].fragLevel < maxFrag - 1 && tabBlockData[path->pixelBlockData.idBlockData].fragLevel != tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel)
                    {
                        coordBis.i = (int)ceil(path->pixelBlockData.coreData.coord.i / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                        coordBis.j = (int)ceil(path->pixelBlockData.coreData.coord.j / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                        PixelBlockData kBlock = tabPixelatedBlockData[path->pixelBlockData.idBlockData].pixelBlockDataTab[coordBis.i][coordBis.j];
                        if (findTagToReceivedList4s(receivedList4s, kBlock.coreData.id, kBlock.coreData.address, kBlock.coreData.coord) == 0)
                        {
                            receivePixelBlockData4s(kBlock, block.blockData);
                        }
                    }
                    else
                    {
                        if (findTagToReceivedList4s(receivedList4s, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.address, path->pixelBlockData.coreData.coord) == 0)
                        {
                            receivePixelBlockData4s(path->pixelBlockData, block.blockData);
                        }
                    }
                    if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                    {
                        logW("process %d recieved the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                    }
                    path = path->next;
                }

            if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
            {
                logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
            }
            tmpDouble = MPI_Wtime();
            computePixelBlock4s(pixel3, (block.blockData.coreData.diag == 1 ? 0 : 1));
            calculTime += MPI_Wtime() - tmpDouble;
            if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
            {
                logD("process %d finished the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
            }

            if (d != 1)
            {
                coord = pixel1.coreData.coord;
                list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                list1 = deleteOccurBlockList(list1);
                sendPixelBlockData(pixel1, list1, 1);
            }

            coord = pixel3.coreData.coord;
            list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
            list1 = deleteOccurBlockList(list1);
            sendPixelBlockData(pixel3, list1, 1);

            coord.i = 2;
            coord.j = 2;
            pixel4 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];

            if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
            {
                logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
            }
            tmpDouble = MPI_Wtime();
            computePixelBlock4s(pixel4, 1);
            calculTime += MPI_Wtime() - tmpDouble;
            if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
            {
                logD("process %d finishes the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
            }

            tmpDouble = MPI_Wtime();

            coord = pixel3.coreData.coord;
            list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
            list2 = deleteOccurBlockList(list2);
            sendPixelBlockData(pixel3, list2, 1);

            coord = pixel4.coreData.coord;
            list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
            list2 = deleteOccurBlockList(list2);
            sendPixelBlockData(pixel4, list2, 1);

            coord = pixel2.coreData.coord;
            list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
            list1 = deleteOccurBlockList(list1);
            if (d == 2 && rank == 5)
            {
                BlockDataList *p = list1;
                while (p != NULL)
                {
                    printBlockData(p->blockData);
                    p = p->next;
                }
            }
            sendPixelBlockData(pixel2, list1, 1);

            coord = pixel4.coreData.coord;
            list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
            list1 = deleteOccurBlockList(list1);
            sendPixelBlockData(pixel4, list1, 1);*/

            if (d != 1)
            {
                coord.i = 1;
                coord.j = 1;
                pixel1 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];

                path = pixelatedBlock.needPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                if (!isEmptyPixelBlockDataList(path))
                {
                    path = reverse(path);
                    while (path != NULL)
                    {
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        if (tabBlockData[path->pixelBlockData.idBlockData].fragLevel < maxFrag - 1 && tabBlockData[path->pixelBlockData.idBlockData].fragLevel != tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel)
                        {
                            coordBis.i = (int)ceil(path->pixelBlockData.coreData.coord.i / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            coordBis.j = (int)ceil(path->pixelBlockData.coreData.coord.j / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            PixelBlockData kBlock = tabPixelatedBlockData[path->pixelBlockData.idBlockData].pixelBlockDataTab[coordBis.i][coordBis.j];
                            if (findTagToReceivedList4s(receivedList4s, kBlock.coreData.id, kBlock.coreData.address, kBlock.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(kBlock, block.blockData);
                            }
                        }
                        else
                        {
                            if (findTagToReceivedList4s(receivedList4s, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.address, path->pixelBlockData.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(path->pixelBlockData, block.blockData);
                            }
                        }
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d received the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        path = path->next;
                    }
                }

                path = pixelatedBlock.needPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                if (!isEmptyPixelBlockDataList(path))
                {
                    path = reverse(path);
                    while (path != NULL)
                    {
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        if (tabBlockData[path->pixelBlockData.idBlockData].fragLevel < maxFrag - 1 && tabBlockData[path->pixelBlockData.idBlockData].fragLevel != tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel)
                        {
                            if (rank == 7 && d == 5 && path->pixelBlockData.coreData.id == 9000)
                            {
                                printPixelBlockData(path->pixelBlockData);
                            }
                            coordBis.i = (int)ceil(path->pixelBlockData.coreData.coord.i / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            coordBis.j = (int)ceil(path->pixelBlockData.coreData.coord.j / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            PixelBlockData kBlock = tabPixelatedBlockData[path->pixelBlockData.idBlockData].pixelBlockDataTab[coordBis.i][coordBis.j];
                            if (rank == 7 && d == 5 && path->pixelBlockData.coreData.id == 9000)
                            {
                                printPixelBlockData(kBlock);
                            }
                            if (findTagToReceivedList4s(receivedList4s, kBlock.coreData.id, kBlock.coreData.address, kBlock.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(kBlock, block.blockData);
                            }
                        }
                        else
                        {
                            if (findTagToReceivedList4s(receivedList4s, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.address, path->pixelBlockData.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(path->pixelBlockData, block.blockData);
                            }
                        }
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d recieved the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        path = path->next;
                    }
                }

                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }
                tmpDouble = MPI_Wtime();
                computePixelBlock4s(pixel1, (block.blockData.coreData.diag == 1 ? 0 : 1));
                calculTime += MPI_Wtime() - tmpDouble;
                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d finishes the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }

                list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                list1 = deleteOccurBlockList(list1);
                list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                list2 = deleteOccurBlockList(list2);

                pathP1 = buildSendBlockDataList(list1, list2);

                sendPixelBlockData(pixel1, pathP1, 1);
            }

            if (block.blockData.fragLevel < maxFrag)
            {
                coord.i = 2;
                coord.j = 1;
                pixel2 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];

                path = pixelatedBlock.needPixelBlockDataListByLine.pixelBlockDataList[coord.i];
                if (!isEmptyPixelBlockDataList(path))
                {
                    path = reverse(path);
                    while (path != NULL)
                    {
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        if (tabBlockData[path->pixelBlockData.idBlockData].fragLevel < maxFrag - 1 && tabBlockData[path->pixelBlockData.idBlockData].fragLevel != tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel)
                        {
                            coordBis.i = (int)ceil(path->pixelBlockData.coreData.coord.i / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            coordBis.j = (int)ceil(path->pixelBlockData.coreData.coord.j / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            PixelBlockData kBlock = tabPixelatedBlockData[path->pixelBlockData.idBlockData].pixelBlockDataTab[coordBis.i][coordBis.j];
                            if (findTagToReceivedList4s(receivedList4s, kBlock.coreData.id, kBlock.coreData.address, kBlock.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(kBlock, block.blockData);
                            }
                        }
                        else
                        {
                            if (findTagToReceivedList4s(receivedList4s, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.address, path->pixelBlockData.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(path->pixelBlockData, block.blockData);
                            }
                        }
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d received the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        path = path->next;
                    }
                }

                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }
                tmpDouble = MPI_Wtime();
                computePixelBlock4s(pixel2, (block.blockData.coreData.diag == 1 ? 0 : 1));
                calculTime += MPI_Wtime() - tmpDouble;
                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d finishes the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }

                list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                list1 = deleteOccurBlockList(list1);
                list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                list2 = deleteOccurBlockList(list2);

                pathP2 = buildSendBlockDataList(list1, list2);

                sendPixelBlockData(pixel2, pathP2, 1);

                coord.i = 1;
                coord.j = 2;
                pixel3 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];

                path = pixelatedBlock.needPixelBlockDataListByColumn.pixelBlockDataList[coord.j];
                if (!isEmptyPixelBlockDataList(path))
                {
                    path = reverse(path);
                    while (path != NULL)
                    {
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d wait the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        if (tabBlockData[path->pixelBlockData.idBlockData].fragLevel < maxFrag - 1 && tabBlockData[path->pixelBlockData.idBlockData].fragLevel != tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel)
                        {
                            coordBis.i = (int)ceil(path->pixelBlockData.coreData.coord.i / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            coordBis.j = (int)ceil(path->pixelBlockData.coreData.coord.j / (float)pow(2, (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel - tabBlockData[path->pixelBlockData.idBlockData].fragLevel - (tabBlockData[pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].idBlockData].fragLevel == maxFrag ? 1 : 0))));
                            PixelBlockData kBlock = tabPixelatedBlockData[path->pixelBlockData.idBlockData].pixelBlockDataTab[coordBis.i][coordBis.j];
                            if (findTagToReceivedList4s(receivedList4s, kBlock.coreData.id, kBlock.coreData.address, kBlock.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(kBlock, block.blockData);
                            }
                        }
                        else
                        {
                            if (findTagToReceivedList4s(receivedList4s, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.address, path->pixelBlockData.coreData.coord) == 0)
                            {
                                receivePixelBlockData4s(path->pixelBlockData, block.blockData);
                            }
                        }
                        if (d == 2000 || verboseDebug) // && (pixel.coreData.id == 4 || pixel.coreData.id == 6))
                        {
                            logW("process %d recieved the pixel %d Coord(%d,%d) fo of process %d r evaluate the pixel %d Coord(%d,%d) d = %d in %f s", rank, path->pixelBlockData.coreData.id, path->pixelBlockData.coreData.coord.i, path->pixelBlockData.coreData.coord.j, tabBlockData[path->pixelBlockData.idBlockData].rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.i, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.coord.j, d, MPI_Wtime() - startTime);
                        }
                        path = path->next;
                    }
                }

                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }
                tmpDouble = MPI_Wtime();
                computePixelBlock4s(pixel3, (block.blockData.coreData.diag == 1 ? 0 : 1));
                calculTime += MPI_Wtime() - tmpDouble;
                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d finished the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }

                list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                list1 = deleteOccurBlockList(list1);
                list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                list2 = deleteOccurBlockList(list2);

                pathP3 = buildSendBlockDataList(list1, list2);

                sendPixelBlockData(pixel3, pathP3, 1);

                coord.i = 2;
                coord.j = 2;
                pixel4 = pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j];

                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d start the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }
                tmpDouble = MPI_Wtime();
                computePixelBlock4s(pixel4, 1);
                calculTime += MPI_Wtime() - tmpDouble;
                if (d == 1000 || verboseDebug) // && (pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 2 || pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id == 3))
                {
                    logD("process %d finishes the pixel %d Coord(%d,%d) d = %d in %f s", rank, pixelatedBlock.pBlockData.pixelBlockDataTab[coord.i][coord.j].coreData.id, coord.i, coord.j, d, MPI_Wtime() - startTime);
                }

                tmpDouble = MPI_Wtime();

                list1 = pixelatedBlock.dependBlockDataListByLine.blockDataList[coord.i];
                list1 = deleteOccurBlockList(list1);
                list2 = pixelatedBlock.dependBlockDataListByColumn.blockDataList[coord.j];
                list2 = deleteOccurBlockList(list2);

                pathP4 = buildSendBlockDataList(list1, list2);

                sendPixelBlockData(pixel4, pathP4, 1);
            }

            // logE("process %d finishs d = %d in %f s", rank, d, MPI_Wtime() - startTime);

            i++;
            block = tabBlock[i];
            pixelatedBlock = tabPixelatedBlock[i];
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

/*void communicateBlockData4s(int type, BlockData src, BlockData dest)
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
        receiveBlockData4s(tabBlockData[src.coreData.id], vector, absc, ord, src.coreData.address, 0);
        receiveBlockData4s(tabBlockData[src.coreData.id], vectorBis, absc, ord, src.coreData.address, 1);
    }
    else
    {
        sendBlockData4s(dest, vector, absc, ord, src.coreData.address, 0);
        sendBlockData4s(dest, vectorBis, absc, ord, src.coreData.address, 1);
    }
    MPI_Type_free(&vector);
}

void receiveBlockData4s(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag, int option)
{
    if (option == 0)
        MPI_Recv(&tree[absc][ord], 1, vector, blockData.rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    else
        MPI_Recv(&cut[absc][ord], 1, vector, blockData.rank, tag + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void sendBlockData4s(BlockData blockData, MPI_Datatype vector, unsigned long absc, unsigned long ord, int tag, int option)
{
    if (option == 0)
        MPI_Isend(&tree[absc][ord], 1, vector, blockData.rank, tag, MPI_COMM_WORLD, &tabReq[idReq++]);
    else
        MPI_Isend(&cut[absc][ord], 1, vector, blockData.rank, tag + 1, MPI_COMM_WORLD, &tabReq[idReq++]);
}*/

void receivePixelBlockData4s(PixelBlockData src, BlockData dest)
{
    int count, blockLengths, stride, absc, ord;

    count = src.coreData.dim.nbLine;
    blockLengths = src.coreData.dim.nbColumn;
    stride = maxNumber + 1;
    MPI_Type_vector(count, blockLengths, stride, MPI_FLOAT, &vector);
    MPI_Type_commit(&vector);
    MPI_Type_vector(count, blockLengths, stride, MPI_INT, &vectorBis);
    MPI_Type_commit(&vectorBis);
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
        MPI_Recv(&cut[absc][ord], 1, vectorBis, source, tag + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        comTime += MPI_Wtime() - tmpDouble;
    }
    else
    {
        // MPI_Request request;
        tmpDouble = MPI_Wtime();
        MPI_Send(&tag, 1, MPI_INT, source, tag, MPI_COMM_WORLD); //, &request);

        MPI_Recv(&tree[absc][ord], 1, vector, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&cut[absc][ord], 1, vectorBis, source, tag + 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        comTime += MPI_Wtime() - tmpDouble;
    }

    if (rank == 10000 && tag == 960000)
    {
        printf("COMPIIIIIIIIIIIIIII\n");
        printPixelBlockData(src);
    }
    receivedList4s = addTagToReceivedList4s(receivedList4s, src.coreData.id, tag, src.coreData.coord);
    MPI_Type_free(&vector);
    MPI_Type_free(&vectorBis);
}

ReceivedList4s *createReceivedList4s()
{
    ReceivedList4s *receivedList4s = (ReceivedList4s *)malloc(sizeof *receivedList4s);
    receivedList4s->id = -1;
    receivedList4s->tag = -1;
    receivedList4s->coord.i = -1;
    receivedList4s->coord.j = -1;
    receivedList4s->next = NULL;
    return receivedList4s;
}
ReceivedList4s *addTagToReceivedList4s(ReceivedList4s *receivedList4s, int id, int tag, Coord coord)
{
    if (isEmptyReceivedList4s(receivedList4s))
    {
        receivedList4s->id = id;
        receivedList4s->tag = tag;
        receivedList4s->coord = coord;
        return receivedList4s;
    }
    else
    {
        ReceivedList4s *new = createReceivedList4s();
        new->id = id;
        new->tag = tag;
        new->coord = coord;
        new->next = receivedList4s;
        return new;
    }
}
int isEmptyReceivedList4s(ReceivedList4s *receivedList4s) { return receivedList4s->tag == -1 && receivedList4s->id == -1; }
int findTagToReceivedList4s(ReceivedList4s *receivedList4s, int id, int tag, Coord coord)
{
    if (isEmptyReceivedList4s(receivedList4s))
        return 0;
    ReceivedList4s *path = receivedList4s;
    while (path != NULL && !(path->id == id && path->tag == tag && path->coord.i == coord.i && path->coord.j == coord.j))
        path = path->next;
    return (path != NULL ? 1 : 0);
}

void computeBlock4s(Block block, int option)
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
                // for (k = d; k <= i - 1; k++)
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

void computePixelBlock4s(PixelBlockData pixel, int option)
{
    int d, i, k;
    float tmp;

    if (option == 0)
    {
        for (d = 1; d <= pixel.coreData.firstBound.j; d++)
        {
            for (i = pixel.coreData.secondBound.i; i <= (pixel.coreData.secondBound.j - d + 1); i++)
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
        for (d = pixel.coreData.firstBound.j; d >= pixel.coreData.firstBound.i; d--)
        {
            for (i = pixel.coreData.secondBound.i; i <= pixel.coreData.secondBound.j; i++)
            {
                tree[d][i] = INT_MAX;
                // for (k = d; k <= i - 1; k++)
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

PixelBlockDataList *reverse(PixelBlockDataList *list)
{
    if (isEmptyPixelBlockDataList(list))
    {
        return list;
    }
    else
    {
        PixelBlockDataList *head = createPixelBlockDataList();

        PixelBlockDataList *path = list;

        head->pixelBlockData = path->pixelBlockData;

        path = path->next;

        while (path != NULL)
        {
            PixelBlockDataList *new = createPixelBlockDataList();
            new->pixelBlockData = path->pixelBlockData;
            new->next = head;
            head = new;
            path = path->next;
        }

        return head;
    }
}