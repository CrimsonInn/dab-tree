# Distributed Asynchronous Boosting Tree (DAB Tree)
Team: Qin Lyu, Bingfeng Xia, MingLong Wu, Hengte Lin

## Introduction
<p>The goal of this project is to apply parallel computation techniques taught in Harvard CS205 course to a data science or a computational science problem. In general, by massive parallel computation, an algorithm is expected to accommodate larger datasets or to perform large scale computation efficiently.</p>
<p>Gradient boosting decision tree (GBDT) is a machine learning technique that can be used for regression and classification problem [1]. GBDT was proposed by Jerome H. Friedman [2,3] that constructs additive model using decision tree as a weak learner. Advantages of GBDT include its capability to model feature interaction and to perform inherent feature selection.</p>
<p>GBDT has been applied to wide varities of applications including physics [4], computer vision [5], and web-search ranking [6, 7]. To cope with increased scale of datasets, a distributed and parallel version of GBDT will be implemented in this project. The scaling property of the implemented system will be tested based on a real world application.</p>

## Goal
A distributed asynchronous stochastic gradient boosting system will be implemented and be tested on a HPC cluster. In addition, parallel computing will be used to accelerate training models on each node.

## Performance
Scalability and efficiency of algorithm will be tested. Training speed and prediction accuracy can be compared with an existing Gradient Boosting Tree library.

## Preliminary results
Gradient boosting decision tree algorithm was implemented with C++ using serial computation approach. A preiliminary version of parall algorithm using MPI was also implemented. Initial tests was performed by comparing training time to existing software packages including GBoost in sklearn and XGBoost. Computation time is listed below. All algorithms achieved similar accuracy.
#### Training size (88896, 17)
1. GBoost (sklearn)   31.33 sec
2. XGBoost            14.72 sec
3. DabTree(MPI)       7.21 sec


## Reference
1. Wikipedia, https://en.wikipedia.org/wiki/Gradient_boosting
2. Friedman, Jerome H. "Greedy function approximation: a gradient boosting machine." Annals of statistics (2001): 1189-1232.
3. Friedman, Jerome H. "Stochastic gradient boosting." Computational Statistics & Data Analysis 38.4 (2002): 367-378.
4. Roe, Byron P., et al. "Boosted decision trees as an alternative to artificial neural networks for particle identification." Nuclear Instruments and Methods in Physics Research Section A: Accelerators, Spectrometers, Detectors and Associated Equipment 543.2 (2005): 577-584.
5. Bissacco, Alessandro, Ming-Hsuan Yang, and Stefano Soatto. "Fast human pose estimation using appearance and motion via multi-dimensional boosting regression." Computer Vision and Pattern Recognition, 2007. CVPR'07. IEEE Conference on. IEEE, 2007.
6. Mohan, Ananth, Zheng Chen, and Kilian Q. Weinberger. "Web-search ranking with initialized gradient boosted regression trees." Yahoo! Learning to Rank Challenge. 2011.
7. Chapelle, Olivier, and Yi Chang. "Yahoo! learning to rank challenge overview." Yahoo! Learning to Rank Challenge. 2011.
