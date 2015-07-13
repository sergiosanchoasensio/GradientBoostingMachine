#ifndef __BINARY_TREE__
#define __BINARY_TREE__

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <stack>
#include <queue>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Config.h"
#include "DataSet.h"


typedef struct {
	int nivell;
	int repetits;
} SAUXMap;

class BinaryTree {

public:

	/**
	* The object constructor.
	*/
	BinaryTree();

	/**
	* It builds the tree.
	* @param classLabel is the class for the tree.
	* @param data is the data to fit with a tree.
	*/
	void generateTree( unsigned int classLabel, DataSet* data );

	/**
	* The object destructor.
	*/
	~BinaryTree();

	/**
	* It computes the variance of a single instance.
	* @param value is the value for computing a sample mean.
	* @param sumSQValue is the sum of squared values.
	* @param weight is the normalization term.
	* @return the sample variance.
	*/
	float computeVariance( float value, float sumSQValue, float weight );

	/**
	* It returns the number of regions Ri in the tree.
	* @return the number of regions Ri of the tree.
	*/
	int getNumberOfRegions();

	/**
	* It makes a prediction of the expected value.
	* @param example is the given example to predict.
	* @return the expected value.
	*/
	float makeAPrediction( const float* example ) const;

	/** It writes the model to a file. */
	void saveToBinaryFile() const;

	/** It reads the model to a file. */
	void readFromBinaryFile();

	/**
	* It reads the file.
	* @param mFile is the model file to read.
	*/
	void readFromBinaryFile(std::ifstream& mFile);

	/**
	* It writes the model to a file.
	* @param oFile is the output file.
	*/
	void saveToBinaryFile(std::ofstream &oFile) const;

	/**
	* It prints by command line the model.
	* @param data is the data set.
	*/
	void print(const DataSet* data);

	/**
	* It copies to a string the model.
	* @param data is the data set.
	*/
	void printToFile(const DataSet* data);

	/**
	* It gets the gamma value for the given example.
	* @param example is the given example.
	*/
	float getGamma(const float* example) const;

private:

	/**
	* It sets the leaves (for gamma computation).
	* @param fraction is the fraction to extract to the expected value.
	*/
	void setLeaves(float fraction);

	/**
	* It builds the recursive binary tree.
	* @param depth
	* @param expectedValue
	* @param sumOfWeights
	* @param minVariance
	* @param data
	* @param splitsInTheTree
	*/
	int buildRBinaryTree( int depth, float expectedValue, float sumOfWeights, float minVariance, DataSet* data, int splitsInTheTree );

	/**
	* It splits the tree taking into account the selected feature and the data.
	* @param feature
	* @param data
	* @param gainValues
	* @param expectedValues
	* @param SSEs
	* @param totalSubSetWeights
	* @param proportions
	*/
	float variableSplit( int feature, DataSet* data, float* gainValues, float** expectedValues, float** SSEs, float** totalSubSetWeights, float** proportions );

	/** It deallocates the memory of the tree childs. */
	void deallocateTree();

	/**
	* It deallocates the memory of the tree.
	* @param data is the tree.
	*/
	void deallocateMemory( BinaryTree* data );

	/**
	* It deallocates the memory of the data set.
	* @param data is the data set.
	*/
	void deallocateMemory( DataSet* data );

	/**
	* It deallocates the memory of a pointer.
	* @param data is pointer.
	*/
	void deallocateMemory( float* data );

	/**
	* It deallocates the memory of a dynamic structure.
	* @param data is pointer array.
	* @param lengthOfFeatures is the length of the previous array.
	*/
	void deallocateMemory( float** data, int lengthOfFeatures );

	/**
	* It prints the model.
	* @parm depth is the actual depth of the tree.
	* @param data is the data set.
	*/
	void print(int depth, const DataSet* data);

	/**
	* It copies to a string the model.
	* @parm depth is the actual depth of the tree.
	* @param data is the data set.
	* @param pGraphvizData is the string to fill.
	*/
	void printToFile(int depth, const DataSet* data, std::string& pGraphvizData);

	/** It indicates the number of regions of the tree. */
	int numberOfRegions;

	/** It indicates the expected value of the node. */
	float expectedValue;

	/** It indicates the sum of squared errors of the node. */
	float sumOfSQErrors;

	/** It indicates the sum of weights of a node. */
	float sumOfWeights;

	/** It indicates the node's feature. */
	int feature;

	/** It indicates the node's split point. */
	float splitPoint;

	/** It indicates the class label for the tree. */
	int classLabel;

	/** 
	It indicates the proportion of instances. 
	proportionOfInstances[0] is the leftChild.
	proportionOfInstances[1] is the rightChild.
	*/
	float proportionOfInstances[2];

	/** It is a left child of the three. */
	BinaryTree *leftChild;

	/** It is a right child of the three. */
	BinaryTree *rightChild;

	/** Pointer to the leaf nodes of the tree .*/
	std::vector <BinaryTree*> leaves;

	/** What the tree returns. */
	float gamma;
};

#endif
