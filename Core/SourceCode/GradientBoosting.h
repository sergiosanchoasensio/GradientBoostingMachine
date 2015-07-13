#ifndef __GRADIENT_BOOSTING__
#define __GRADIENT_BOOSTING__

#define _CRT_SECURE_NO_WARNINGS 1

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <stack>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include "Config.h"
#include "DataSet.h"
#include "BinaryTree.h"

class GradientBoosting {

public:
	/**
	*  It constructs the gradient boosting object.
	*/
	GradientBoosting();

	/**
	*  It destructs the gradient boosting object.
	*/
	~GradientBoosting();

	/** It trains. */
	void train();

	/** It predicts. */
	void test();

private:
	/**
	* It computes the true positives. 
	* @param classIndex the desired class index.
	* @param confusionMatrix is the confusion matrix.
	* @return the proportion of true positives.
	*/
	float truePositives(int classIndex, int** confusionMatrix) const;

	/**
	* It computes the true positive rate.
	* @param confusionMatrix is the confusion matrix.
	* @return the recall.
	*/
	float recall(int** confusionMatrix) const;

	/**
	* It computes the precision at the given class.
	* @param classIndex is the class index.
	* @param confusionMatrix is the confusion matrix.
	* @return the precision.
	*/
	float precisionAt(int classIndex, int** confusionMatrix) const;

	/**
	* It computes the precision.
	* @param confusionMatrix is the confusion matrix.
	* @return the precision.
	*/
	float precision(int** confusionMatrix) const;

	/**
	* It computes the F-Measure at the given class.
	* @param classIndex is the class index.
	* @param confusionMatrix is the confusion matrix.
	* @return the F-Measure.
	*/
	float FMeasureAt(int classIndex, int** confusionMatrix) const;

	/**
	* It computes the F-Measure.
	* @param confusionMatrix is the confusion matrix.
	* @return the precision.
	*/
	float FMeasure(int** confusionMatrix) const;
	
	/**
	* It computes the softmax.
	* @param labelPredictions are the class predictions.
	* @param labelProbabilities are the class probabilities.
	* @return the precision.
	*/
	void computeSoftMax(float* labelPredictions, float* labelProbabilities) const;

	/**
	* It computes the class probabilities.
	* @param example is the example.
	* @param labelPredictions are the class predictions.
	* @param labelProbabilities are the class probabilities.
	*/
	int computePrediction(float* example, float* labelPredictions, float* labelProbabilities) const;

	/** It reads the model from a file. */
	void readModelFromFile();

	/** Generic data set. */
	DataSet* genericDataset;

	/** The boosting trees [numberOfTrees][numberOfClasses]. */
	BinaryTree*** boostingTrees;

};

#endif