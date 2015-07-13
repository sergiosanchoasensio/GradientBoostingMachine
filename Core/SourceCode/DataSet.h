#ifndef __DATA_SET__
#define __DATA_SET__

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
#include "Config.h"
#include "NominalFeature.h"

class DataSet {

public:

	/**
	* The data set constructor.
	* @param fileName is the name of the file to open.
	*/
	DataSet( const std::string &fileName );

	/**
	* Copy constructor.
	* @param dataset is the data set to clone.
	*/
	DataSet( const DataSet* dataset );

	/**
	* It copies the desired number of instances with threshold.
	* @param feature is the desired feature to copy data from.
	* @param symbol is the desired symbol [<= | > ].
	* @param threshold is the threshold value.
	* @param dataset is the data set to clone.
	*/
	DataSet( unsigned int feature, const std::string& symbol, float threshold, const  DataSet* dataset );

	/**
	* The data set destructor.
	*/
	~DataSet();

	/**
	* It sorts the data according to the given feature index by using iterative quicksort algorithm.
	* @param feature is the feature to sort.
	*/
	void sort( unsigned int feature );

	/**
	* It prints the data set.
	*/
	void print() const;

	/*
	* Get the number of instances
	*/
	unsigned int getNumberOfInstances() const;

	/*
	* Get the number of classes 
	*/
	unsigned int getNumberOfClasses() const;

	/*
	* Get the number of features
	*/
	unsigned int getNumberOfFeatures() const;

	/*
	* Get the class position
	*/
	unsigned int getClassPosition() const;

	float getValueAt(unsigned int index, unsigned int feature) const;

	/**
	* It gets the example at the given position and example.
	*/
	float getExampleAt( unsigned int index, float *example ) const;

	/**
	* Weight per example.
	*/
	inline float getWeight() const;

	/**
	* It gets the residual at the given position and label.
	* @param index is the index of the data matrix.
	* @param label is the index of the desired label.
	* @return the desired residual.
	*/
	float getResidualAt( unsigned int index, unsigned int label ) const;

	/**
	* It sets the residual at the given position and label.
	* @param index is the index of the data matrix.
	* @param label is the index of the desired label.
	* @param value is the new value for the residual.
	*/
	void setResidualAt( unsigned int index, unsigned int label, float value );

	/**
	* It gets the output variable (class) at the given position.
	* @param index is the index of the data matrix.
	* @return the desired output variable.
	*/
	inline float getOutputVariableAt( unsigned int index ) const;

	/**
	* It counts the number of instances which follow a given threshold.
	* @param feature is the feature index.
	* @param symbol is the symbol [<= | > ].
	* @param threshold is the threshold value.
	*/
	int getNumberOfInstancesWithThresholdAt( unsigned int feature, const std::string& symbol, float threshold ) const;
	
	std::string getFeatureName( unsigned int index ) const;

	/**
	* It computes the residual for the given class.
	* @param exampleIndex is the index of the desired example.
	* @param classIndex is the index of the desired class.
	* @param value is the value to substract to the residual.
	*/
	void computeResidual(unsigned int exampleIndex, unsigned int classIndex, float value );

	/*
	* It returns the number of instances with a predefined class label.
	* @param classLabel is the desired class label.
	* @return the number of instances with that class label.
	*/
	int getNumberOfInstancesOfClass(int classLabel);

private:

	/**
	* It reads the header information and sets
	* the variable types and also the number of
	* instances in the file.
	*/
	void readHeader( const std::string &fileName );

	/**
	* It reads the data information.
	*/
	void readData( const std::string &fileName );

	/**
	* It loads the file into memory.
	* @param fileName is the file to open.
	*/
	void loadData( const std::string &fileName );
	
	/**
	* It checks if the given line is a comment or not (starts with '%', '#' or '@').
	* @param line is the line being read.
	* @return true if it is a comment. False otherwise.
	*/
	bool isAComment( const std::string& line ) const;

	/**
	* It trims the given line.
	* @param sLine the line being read.
	* @return a new trimmed string.
	*/
	std::string trim( const std::string& line ) const;

	/**
	* It reads a line.
	* @param myFile is the file to read.
	* @param line is the line which contains the data.
	* @return true if end of file. False otherwise.
	*/
	bool readLine( std::ifstream& myFile, std::string& line ) const;

	/**
	* It process a nominal feature.
	* @param line is the line with the information.
	*/
	void processNominalFeature( const std::string& line );

	/**
	* It process a numeric feature. It only stores the name of the
	* feature!
	* @param line is the line with the information.
	*/
	void processNumericFeature( const std::string& line );

	/**
	* It extracts the value of the feature and stores it in the feature value array.
	* @param line the is line with the information.
	* @param i is the offset to read from.
	* @param output is the storage buffer.
	*/
	void extractValue( const std::string line, unsigned int &i, char* output ) const;

	/** The data itselft, allocated as [#instances][#features]. */
	float **data;
	/** The residual matrix, allocated as [#instances][#numberOfClasses]. */
	float **residuals;
	/** The number of examples in the data matrix. */
	unsigned int numberOfInstances;
	/** The number of classes in the data matrix. */
	unsigned int numberOfClasses;
	/** The number of features. */
	unsigned int numberOfFeatures;
	/** It tells where is the class in the features. */
	unsigned int classPosition;
	/** It is used for sorting. */
	std::stack<int>stack;
	/** The names of the types for the features. It stores {nominal, numeric}.*/
	std::vector<std::string> featureType;
	/** Information about the features, specially with nominal ones. */
	std::vector<NominalFeature> features;
};

/** It gets the weight of each example. */
inline float DataSet::getWeight() const {
	return 1.0f;
}

/** It gets the output variable. */
inline float DataSet::getOutputVariableAt( unsigned int index ) const {
	if ( index < numberOfInstances ) {
		return data[index][numberOfFeatures - 1];
	}
	else {
		std::cout << "Error: Index out of bounds when accessing to the output variable!" << std::endl;
	}
	return Config::UNKNOWNVALUE;
}

#endif
