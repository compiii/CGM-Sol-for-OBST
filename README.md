# CGM-based Parallel Solutions For The OBST Problem

This repository stores coarse-grained multicomputer parallel algorithms to solve the optimal binary search tree (OBST) problem using : 

1.  the regular partitioning techniques : 
    - Kechid and Myoupo [doi:10.1109/ITNG.2008.158](https://doi.org/10.1109/ITNG.2008.158)
    - Myoupo and Kengne [doi:10.10.1504/ijhpcn.2014.062729](https://doi.org/10.1504/ijhpcn.2014.062729) 

2.  the irregular partitioning technique : Kengne and Lacmou [doi:10.1007/s41019-019-0093-9](https://doi.org/10.1007/s41019-019-0093-9)

3.  the four-splitting technique.

## Build projects

Just execute in each root folder:

````
make
````

## Run projects

#### A. Configuration

The config file is located at the path :

````
{project}/resources/config/config.json
````

You can set the data set, the data size, the algorithm be used, etc.

````
...
...
"datasets": {
    "datasets-dir": "resources/datasets/",
    "datasets-type": "frequencies/",
    "datasets-dirname": "1023/",
    "datasets-filename": "1023",
    "datasets-extension": ".data",
    "datasets-max-number-keys": "1023",
    "datasets-input": {
        "datasets-input-id": "_2",
        "datasets-input-max-fragmentation": "2",
        "datasets-input-max-number-keys": "4095",
        "datasets-input-algorithm": "5",
        "datasets-input-partitioning-technique": "1"
    },
    "datasets-create": {
        "datasets-create-id": "_1"
    }
},
...
...
````

The algorithms be used are identifyied by a number :

0.  Knuth's sequential algorithm
4.  irregular partitioning technique (Kengne and Lacmou)
5.  four-splitting technique (Lacmou et *al.*)

The partitioning techniques (for the parallel solutions) are also identifyied by a number :

1.  the DAG is divided by *sqrt(p)*

#### B. Parallel algorithms

Run :

````
mpirun -np {number of processors} -hostfile hosts ./bin/CGM-OBST.run {data size} {number of fragmentations} {algorithm be used} {partitioning technique}
````

For instance :

````
mpirun -np 8 -hostfile hosts ./bin/CGM-OBST.run 8191 1 5 1
````

## License
[MIT](LICENSE)