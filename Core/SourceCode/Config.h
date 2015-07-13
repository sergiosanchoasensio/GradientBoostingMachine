#ifndef __CONFIGURATION__
#define __CONFIGURATION__

#include <iostream>
#include <string>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>

class Config {

public:
	/** The maximum amount of characters in a char* string. */
	static const unsigned int MAXBUFFER = 3000;

	/** It indicates a non valid feature value. */
	static const int NOTVALID;

	/** An unknown value. */
	static const float UNKNOWNVALUE;

	/** It prints the configuration. */
	static void print();

	/** It indicates the minimum variance required for split. */
	static float MINVARIANCE;

	/** It indicates the minimum number of instances that a leaf node can have. */
	static int MINIMNUMBEROFINSTANCESPERLEAF;

	/** It indicates the maximum depth of the tree. */
	static int MAXIMUMDEPTH;

	/** It indicates the maximum number of leaves in the tree. */
	static int MAXIMUMNUMBEROFLEAVES;

	/** Data set file name. */
	static std::string inputFile;
	
	/** The model file. */
	static std::string modelFile;

	/** The learning rate of the GBM. */
	static float learningRate;

	/** The number of trees (rounds). */
	static int numberOfTrees;

	/** It prints the tree. */
	static bool verbose;

	/** Prepares te algorithm to launch multiple experiments. */
	static bool multipleExperiments;

	/** It weights. */
	static float* classWeight;

	/** Apply weight. */
	static bool applyWeight;
};

#endif
