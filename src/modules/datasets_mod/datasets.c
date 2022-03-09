/**
 * @file datasets.c
 * @author Jerry Lacmou (jerrylacmou@gmail.com)
 * @brief 
 * @version 1.1
 * @date 2019-11-05
 * 
 * @copyright Compiii Thesis Copyright (c) 2019
 * 
 */
#include "datasets.h"
#include "config.h"
#include "clogger.h"

void initDataSetsMod()
{
    tabP = (float *)malloc(maxNumber * sizeof *tabP);
    tabQ = (float *)malloc((maxNumber + 1) * sizeof *tabQ);
}

void createDatasets()
{
    int i;
    srand(time(0));
    file = fopen(getFilenameCreateDatasets(), "w");
    if (file == NULL)
    {
        logE("error: file %s not exist\n", getFilenameCreateDatasets());
        exit(EXIT_FAILURE);
    }
    printf("\n");

    float varp, s = 0, varq;
    for (i = 0; i < MAX_NUMBER_KEYS; i++)
    {
        tabP[i] = 1 + (long)99 * (rand() / (double)RAND_MAX);
        tabQ[i] = 1 + (long)99 * (rand() / (double)RAND_MAX);
        s = s + (tabP[i] + tabQ[i]);
    }
    tabQ[MAX_NUMBER_KEYS] = 1 + (long)99 * (rand() / (double)RAND_MAX);
    s = s + tabQ[MAX_NUMBER_KEYS];
    for (i = 0; i < MAX_NUMBER_KEYS; i++)
    {
        varp = tabP[i] / (float)(s);
        tabP[i] = varp;
        fprintf(file, "%.10f\t", varp);
    }
    for (i = 0; i < MAX_NUMBER_KEYS; i++)
    {
        varq = tabQ[i] / (float)(s);
        tabQ[i] = varq;
        fprintf(file, "%.10f\t", varq);
    }
    varq = tabQ[MAX_NUMBER_KEYS] / (float)(s);
    tabQ[MAX_NUMBER_KEYS] = varq;
    fprintf(file, "%.10f\t", varq);
    fclose(file);
}

void readFileData(float *tabP, float *tabQ)
{
    int i;
    file = fopen(getFilenameInputDatasets(), "r");
    if (file == NULL)
    {
        logE("error: file %s not exist\n", getFilenameInputDatasets());
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < maxNumber; i++)
        fscanf(file, "%f", &tabP[i]);
    for (i = 0; i <= maxNumber; i++)
        fscanf(file, "%f", &tabQ[i]);
    fclose(file);

    /*for (i = 0; i < 10; i++)
        printf("%.10f \t", tabP[i]);
    for (i = 0; i <= 10; i++)
        printf("%.10f \t", tabQ[i]);*/
}