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
#include "irregularPartitioningWithSqrt2P.h"

int f_IP_SQRT_2P(int p){
    return (p == 1 ? 1 : (int) ceil(sqrt(2 * p)));
}