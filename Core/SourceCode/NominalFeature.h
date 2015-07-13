#ifndef __NOMINAL_FEATURE__
#define __NOMINAL_FEATURE__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

class NominalFeature {

public:

	/**
	* It sets the object to an empty state.
	*/
	NominalFeature() {
		numberOfvalues = 0;
	}

	/**
	* It constructs the nominal feature object.
	* @param name is the name of the feature.
	*/
	NominalFeature( const std::string& featureName )  {
		name = featureName;
		numberOfvalues = 0;
	}

	/** The nominal feature destructor. */
	~NominalFeature() {}

	/** It sets the name. */
	void setName( std::string name ) {
		this->name = name;
	}

	/** It gets the name. */
	std::string getName() const {
		return name;
	}

	/**
	* It adds a new value to the object.
	* @param featureValue is the string containing the feature value.
	*/
	void add( const std::string& featureValue ) {
		fromStringToIntMap.insert( std::pair<std::string, int>( featureValue, (int) numberOfvalues ) );
		fromIntToStringMap.insert( std::pair<int, std::string>( (int) numberOfvalues, featureValue ) );
		numberOfvalues++;
	}

	/**
	* It returns the index value of the feature.
	* @return the index value.
	*/
	int getIntValue( std::string value ) const {
		return fromStringToIntMap.at( value );
	}

	/**
	* It returns the string value of the feature.
	* @return the value.
	*/
	std::string getStringValue( int index ) const {
		return fromIntToStringMap.at( index );
	}

	/**
	* It returns the number of values in the feature.
	* @return the number of values.
	*/
	unsigned int getNumberOfValues() const {
		return numberOfvalues;
	}

private:

	/** Name of the feature. */
	std::string name;

	/** The number of values. */
	unsigned int numberOfvalues;

	/** The mapping from string to int. */
	std::map<std::string, int> fromStringToIntMap;

	/** The mapping from int to string. */
	std::map<int, std::string> fromIntToStringMap;
};

#endif