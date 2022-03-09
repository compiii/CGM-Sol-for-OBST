#include "clogger.h"
#include "config.h"
#include "partitioning.h"
#include "regularPartitioning.h"
#include "irregularPartitioning.h"
#include "datasets.h"
#include "obst.h"
#include "compiii_dbyd.h"
#include "compiii_pbyp.h"
#include "compiii_dsej.h"
#include "compiii_4s.h"
#include "kechid_dpba.h"
#include "output.h"

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &maxProcess);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processorName, &processorNameSize);
    if (argc > 1)
    {
        maxNumber = atol(argv[1]);
        maxFrag = atol(argv[2]);
        algorithmId = atoi(argv[3]);
        partitioningTechniqueId = atoi(argv[4]);
    }
    else
    {
        maxNumber = getMaxNumberOfKeys();
        maxFrag = getMaxNumberFragmentation();
        algorithmId = getAlgorithmInputDatasets();
        partitioningTechniqueId = getPartitioningTechnique();
    }

    char *print = malloc(sizeof(char));
    char tmp[1024];
    int verbose = 1;
    verboseDebug = 0;
    sprintf(tmp, "%s", "");
    GeneralInfo generalInfo;
    InputInfo inputInfo;
    OutputInfo outputInfo;
    char fileName[MAX_SIZE_STRING];

    if (algorithmId == 0)
        sprintf(algorithmName, "%s", "Knuth's sequential algorithm");
    else if (algorithmId == 1)
        sprintf(algorithmName, "%s", "OBST_A (diagonal by diagonal)");
    else if (algorithmId == 2)
        sprintf(algorithmName, "%s", "OBST_B (pixel by pixel)");
    else if (algorithmId == 3)
        sprintf(algorithmName, "%s", "OBST_C (Kechid et Myoupo'09)");
    else if (algorithmId == 4)
        sprintf(algorithmName, "%s", "OBST_D (Kengne et Lacmou'19)");
    else if (algorithmId == 5)
        sprintf(algorithmName, "%s", "OBST_E (four splitting'22)");
    else
    {
        logE("check the value of algorithm use in config.json file");
        exit(EXIT_FAILURE);
    }

    print = concat(2, algorithmName, " input datas\n------------------------------\n");

    if (partitioningTechniqueId == 0)
        sprintf(partitioningName, "%s", "the DAG is divided by p\n------------------------------\n");
    else if (partitioningTechniqueId == 1)
        sprintf(partitioningName, "%s", "the DAG is divided by sqrt(2p)\n------------------------------\n");
    else
    {
        logE("check the value of partitioning technique use in config.json file");
        exit(EXIT_FAILURE);
    }

    print = concat(2, print, partitioningName);

    if (algorithmId == 1 || algorithmId == 2 || algorithmId == 4 || algorithmId == 5)
        initIrregPartMod();
    else if (algorithmId == 3)
        initRegPartMod();

    strcpy(generalInfo.softName, getSoftName());
    strcpy(generalInfo.softVersion, getSoftVersion());
    strcpy(generalInfo.algorithm, algorithmName);
    strcpy(generalInfo.processorName, processorName);
    strcpy(generalInfo.dataset, getInputDataSetId());
    generalInfo.algorithmId = algorithmId;
    generalInfo.partitioningTechniqueId = partitioningTechniqueId;
    generalInfo.rank = rank;
    generalInfo.maxProcess = maxProcess;

    inputInfo.maxNumberOfKeys = maxNumber;
    inputInfo.maxFragmentation = maxFrag;

    if (rank == 0)
    {
        sprintf(tmp, "max number of keys: %d\n", maxNumber);
        print = concat(2, print, tmp);
        if (algorithmId != 0)
        {
            sprintf(tmp, "max number of process: %d\n", maxProcess);
            print = concat(2, print, tmp);
            sprintf(tmp, "max number of fragmentation: %d\n", maxFrag);
            print = concat(2, print, tmp);
            sprintf(tmp, "max number of step: %d\n", maxDiag);
            print = concat(2, print, tmp);
            sprintf(tmp, "max number of blocks: %d\n", maxBlock);
            print = concat(2, print, tmp);
        }
        logI(print);
        print = concat(1, "");
    }

    if ((algorithmId == 0 && rank == 0) || algorithmId != 0)
    {
        initDataSetsMod();
        // createDatasets();
        readFileData(tabP, tabQ);
        initOBSTMod();
        startTime = tmpDouble = endTime = calculTime = comTime = comRate = calculRate = totalTime = idlenessTime = idlenessRate = sizeDataCom = nbMessageExchange = 0;
    }

    if (verbose && rank == 0)
    {
        print = concat(3, processorName, " starts computation of ", algorithmName);
        logI(print);
        print = concat(1, "");
    }

    if (algorithmId == 0)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0)
        {
            startTime = MPI_Wtime();
            obst = knuthAlgorithm();
            endTime = MPI_Wtime();
            totalTime = endTime - startTime;
            calculTime = totalTime;
            calculRate = 100;

            outputInfo.obst = obst;
            outputInfo.totalTime = totalTime;
            outputInfo.calculTime = calculTime;
            outputInfo.comTime = 0;
            outputInfo.idlenessTime = 0;
            outputInfo.nbMessageExchange = 0;
            outputInfo.sizeDataCom = 0;

            strcpy(fileName, getFileNamePrimarySurvey(rank));
            saveStatistic(fileName, generalInfo, inputInfo, outputInfo);

            sprintf(tmp, " of %s ", algorithmName);
            print = concat(3, processorName, " ends computation", tmp);
            logI(print);
            print = concat(1, "");
            sprintf(tmp, "Output datas of %s (process %d) for %s", processorName, rank, algorithmName);
            print = concat(2, tmp, "\n------------------------------\n");
            sprintf(tmp, "obst: %f\n", obst);
            print = concat(2, print, tmp);
            sprintf(tmp, "total time: %f\n------------------------------\n", totalTime);
            print = concat(2, print, tmp);
            logI(print);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    else if (algorithmId == 1 || algorithmId == 2 || algorithmId == 3 || algorithmId == 4 || algorithmId == 5)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        startTime = MPI_Wtime();
        if (algorithmId == 1)
            obst = compiiiDbyd();
        else if (algorithmId == 2)
            obst = compiiiPbyp();
        else if (algorithmId == 3)
            obst = kechidDpba();
        else if (algorithmId == 4)
            obst = compiiiDsej();
        else if (algorithmId == 5)
            obst = compiii4s();
        // MPI_Barrier(MPI_COMM_WORLD);
        endTime = MPI_Wtime();

        idlenessTime = endTime - startTime - (calculTime + comTime);
        totalTime = calculTime + comTime + idlenessTime;

        calculRate = calculTime / totalTime * 100;
        comRate = comTime / totalTime * 100;
        idlenessRate = idlenessTime / totalTime * 100;

        outputInfo.obst = obst;
        outputInfo.totalTime = totalTime;
        outputInfo.calculTime = calculTime;
        outputInfo.comTime = comTime;
        outputInfo.idlenessTime = idlenessTime;
        outputInfo.nbMessageExchange = nbMessageExchange;
        outputInfo.sizeDataCom = sizeDataCom * sizeof(int);

        if (rank == tabBlockData[maxBlock - 1].rank)
        {
            strcpy(fileName, getFileNamePrimarySurvey(rank));
            saveStatistic(fileName, generalInfo, inputInfo, outputInfo);
        }
        else
        {
            strcpy(fileName, getFileNameSecondarySurvey(rank));
            saveStatistic(fileName, generalInfo, inputInfo, outputInfo);
        }

        if (verbose)
        {
            if (algorithmId == 1 || algorithmId == 2 || algorithmId == 4 || algorithmId == 5)
            {
                if (rank == tabBlockData[maxBlock - 1].rank || (rank == tabBlockData[maxBlock - maxProcess].rank))
                {
                    sprintf(tmp, " of %s ", algorithmName);
                    print = concat(3, processorName, " ends computation", tmp);
                    logI(print);
                    print = concat(1, "");

                    sprintf(tmp, "Output datas of %s (process %d) for %s", processorName, rank, algorithmName);
                    print = concat(2, tmp, "\n------------------------------\n");
                    sprintf(tmp, "obst: %f\n", obst);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "total time: %f\n", totalTime);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "calcul time: %f\n", calculTime);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "comunication time: %f\n", comTime);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "idleness time: %f\n", idlenessTime);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "calcul rate: %f\n", calculRate);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "comunication rate: %f\n", comRate);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "idleness rate: %f\n", idlenessRate);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "number of message exchange: %d\n", nbMessageExchange);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "size of data communicate: %f MB\n", sizeDataCom * sizeof(unsigned short) / 1048576.0);
                    print = concat(2, print, tmp);
                    logI(print);
                }
            }
            else if (algorithmId == 3)
            {
                int i = 0, iter = 0, max = INT_MIN, process;
                while (iter < maxProcess)
                {
                    while (iter != tabBlockData[maxBlock - 1 - i].rank)
                    {
                        i++;
                    }
                    if (i > max)
                    {
                        max = i;
                        process = iter;
                    }
                    i = 0;
                    iter++;
                }
                if (rank == tabBlockData[maxBlock - 1].rank || rank == process)
                {
                    sprintf(tmp, " of %s ", algorithmName);
                    print = concat(3, processorName, " ends computation", tmp);
                    logI(print);
                    print = concat(1, "");

                    sprintf(tmp, "Output datas of %s (process %d) for %s", processorName, rank, algorithmName);
                    print = concat(2, tmp, "\n------------------------------\n");
                    sprintf(tmp, "obst: %f\n", obst);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "total time: %f\n", totalTime);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "calcul time: %f\n", calculTime);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "comunication time: %f\n", comTime);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "idleness time: %f\n", idlenessTime);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "calcul rate: %f\n", calculRate);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "comunication rate: %f\n", comRate);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "idleness rate: %f\n", idlenessRate);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "number of message exchange: %d\n", nbMessageExchange);
                    print = concat(2, print, tmp);
                    sprintf(tmp, "size of data communicate: %f MB\n", sizeDataCom * sizeof(unsigned short) / 1048576.0);
                    print = concat(2, print, tmp);
                    logI(print);
                }
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    // free(tabTree[0]);
    // free(tabTree);

    return 0;
}
