# Asynchronous Boosting Tree (ABTree)
Team: Qin Lyu, Bingfeng Xia, MingLong Wu, Hengte Lin

## Motivation:
Gradient boosting decision tree (GBDT) is widely used in industry. However, people sometimes find it too slow to train a GBDT model on one server, especially on large-scale and high-dimensional dataset. In this project, we will build a distributed and parallel version of GBDT and test its scaling property on real world applications.
## Goal:
We will implement distributed asynchronous stochastic gradient boosting on HPC cluster, and use parallel computing to accelerate training on each node.
## Background
Gradient boosting decision tree(GBDT) is a powerful machine learning technique for regression and classification problems, which is first proposed in the paper “Gradient Boosting Decision Trees Algorithm and its modern offsprings” by Jerome H. Friedman.
GBDT constructs additive regression model utilizing decision trees as the weak learner. GBDT is able to model feature interactions and inherently perform feature selection. It is adaptable 

## Performance
Scalability and efficiency of algorithm will be tested. It is also possible to compare accuracy and training speed with existing Gradient Boosting Tree libraries.

## Specs
Language: C++11 (multithreading), Python (interface), MPI (distributed computation)



