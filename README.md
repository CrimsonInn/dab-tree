# Distributed Asynchronous Boosting Tree (DAB Tree)
Team: Qin Lyu, Bingfeng Xia, MingLong Wu, Hengte Lin

<img src="/ReadMeImages/in.png" width="200">

## Introduction
<p>The goal of this project is to apply parallel computation techniques taught in Harvard CS205 course to a data science or a computational science problem. In general, by massive parallel computation, an algorithm is expected to accommodate larger datasets or to perform large scale computation efficiently.</p>
<p>Gradient boosting decision tree (GBDT) is a machine learning technique that can be used for regression and classification problem [1]. GBDT was proposed by Jerome H. Friedman [2,3] that constructs additive model using decision tree as a weak learner. Advantages of GBDT include its capability to model feature interaction and to perform inherent feature selection.</p>
<p>GBDT has been applied to wide varities of applications including physics [4], computer vision [5], and web-search ranking [6, 7]. To cope with increased scale of datasets, a distributed and parallel version of GBDT will be implemented in this project. The scaling property of the implemented system will be tested based on a real world application.</p>

## What is a Tree

<img src="/ReadMeImages/tree.png" width="200">

Tree the a kind of prediction model based on thresholds. According to WikiPedia, "A decision tree is a decision support tool that uses a tree-like graph or model of decisions and their possible consequences, including chance event outcomes, resource costs, and utility. It is one way to display an algorithm."

Here is an example:

<img src="/ReadMeImages/tree2.png" width="400">

## What is a Gradient Boosting Tree
First One need to know what is Ensamble. Ensamble is a technology to group some models and make them vote for the final result. There are two main kinds of ensamble, bagging and boosting. In bagging, models are indipendent of each other. In Gradient Boosting, models interatively learn what's left from the previous model.

<img src="/ReadMeImages/ensamble.png" width="600">

## Goal
A distributed asynchronous stochastic gradient boosting system will be implemented and be tested on a HPC cluster. In addition, parallel computing will be used to accelerate training models on each node.

## Design
Data can be first divided into packs and saved as google protobuf files. Then protobuf files are read by trainers in batches. Trainers will iteratively grow nodes on a tree until reach limit. Then trainer will read next data batch and train the next tree.

<img src="/ReadMeImages/Design.png" width="600">

## Pipeline of training one tree

<img src="/ReadMeImages/train_one_tree.png" width="600">

## Local Training Process

Both data preparation and tree node growing are parallelized using c++ threading.

<img src="/ReadMeImages/pipeline.png" width="600">

## MPI Method
The algorithm can operate on an OpenMPI cluster. The cluster was designed to have one Master and several Workers. Workers are the nodes that actually read data and create trees, while Master act as a trading center to asynchronously exchange trees among Workers.

### Batch Delivery Mode
To reduce the communication overhead while trading trees with Master, DABTREE supports the option to deliever trees in batches. Delivering trees in batches will significantly reduce the cost of communication, with the cost of reducing accuracy.

<img src="/ReadMeImages/batchMPI.png" width="600">

## Performance
Scalability and efficiency of algorithm will be tested. Training speed and prediction accuracy can be compared with an existing Gradient Boosting Tree library.

## Instruction

Pull the repo in master branch. 

cd into "/build" folder. 

run "cmake.sh". 

cd back to root folder. 

type "mpirun -n #numnodes ./cluster --round=#numTreePerNode --threads=#threadPerWorker --batch=#samplingSize --bundle=#(true/false)batchFunctionality " 

PS: If you used batch delivery functionality, numTreePerNode must be multiple of 10.

## Preliminary results
A Gradient boosting decision tree algorithm is implemented with C++ using serial computation approach. A preiliminary version of parallel algorithm using MPI is also implemented. Initial tests are performed by comparing training time to existing software packages including GBoost in sklearn and XGBoost. Computation time is listed below. All algorithms achieve similar accuracies.

#### Training size (88896, 17)

<img src="/ReadMeImages/single.jpg" width="600">

<img src="/ReadMeImages/efficiency.jpg" width="600">

<img src="/ReadMeImages/AWS.png" width="600">

## Reference
1. Wikipedia, https://en.wikipedia.org/wiki/Gradient_boosting
2. Friedman, Jerome H. "Greedy function approximation: a gradient boosting machine." Annals of statistics (2001): 1189-1232.
3. Friedman, Jerome H. "Stochastic gradient boosting." Computational Statistics & Data Analysis 38.4 (2002): 367-378.
4. Roe, Byron P., et al. "Boosted decision trees as an alternative to artificial neural networks for particle identification." Nuclear Instruments and Methods in Physics Research Section A: Accelerators, Spectrometers, Detectors and Associated Equipment 543.2 (2005): 577-584.
5. Bissacco, Alessandro, Ming-Hsuan Yang, and Stefano Soatto. "Fast human pose estimation using appearance and motion via multi-dimensional boosting regression." Computer Vision and Pattern Recognition, 2007. CVPR'07. IEEE Conference on. IEEE, 2007.
6. Mohan, Ananth, Zheng Chen, and Kilian Q. Weinberger. "Web-search ranking with initialized gradient boosted regression trees." Yahoo! Learning to Rank Challenge. 2011.
7. Chapelle, Olivier, and Yi Chang. "Yahoo! learning to rank challenge overview." Yahoo! Learning to Rank Challenge. 2011.
