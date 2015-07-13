#include "DataSet.h"

DataSet::DataSet( const std::string &fileName ) {
	data = NULL;
	residuals = NULL;
	numberOfInstances = 0;
	numberOfClasses = 0;
	numberOfFeatures = 0;
	classPosition = 0;
	loadData( fileName );
}

DataSet::DataSet( const DataSet* dataset ) {
	data = NULL;
	residuals = NULL;
	numberOfInstances = dataset->numberOfInstances;
	numberOfClasses = dataset->numberOfClasses;
	numberOfFeatures = dataset->numberOfFeatures;
	classPosition = dataset->classPosition;
	//Allocate the matrix.
	data = new float*[numberOfInstances];
	residuals = new float*[numberOfInstances];
	for ( unsigned int i = 0; i < numberOfInstances; i++ ) {
		data[i] = new float[numberOfFeatures];
		for ( unsigned int j = 0; j < numberOfFeatures; j++ ) {
			data[i][j] = dataset->data[i][j];
		}
		residuals[i] = new float[numberOfClasses];
		for ( unsigned int j = 0; j < numberOfClasses; j++ ) {
			residuals[i][j] = dataset->residuals[i][j];
		}
	}
	for ( unsigned int j = 0; j < dataset->featureType.size(); j++ ) {
		featureType.push_back( dataset->featureType[j] );
	}
	for ( unsigned int j = 0; j < dataset->features.size(); j++ ) {
		features.push_back( dataset->features[j] );
	}
}

DataSet::DataSet( unsigned int feature, const std::string& symbol, float threshold, const  DataSet* dataset ) {
	data = NULL;
	residuals = NULL;
	numberOfInstances = dataset->getNumberOfInstancesWithThresholdAt( feature, symbol, threshold );
	numberOfClasses = dataset->numberOfClasses;
	numberOfFeatures = dataset->numberOfFeatures;
	classPosition = dataset->classPosition;
	//Allocate the matrix.
	data = new float*[numberOfInstances];
	residuals = new float*[numberOfInstances];
	for (unsigned int i = 0; i < numberOfInstances; i++) {
		data[i] = new float[numberOfFeatures];
		residuals[i] = new float[numberOfClasses];
	}
	//Copy the data.
	unsigned int smallSet = 0, largeSet = 0;
	for ( unsigned int i = 0; i < dataset->numberOfInstances; i++ ) {
		if ( symbol.compare("<=") == 0 )  {
			if ( dataset->data[i][ feature ] <= threshold ) {
				for ( unsigned int j = 0; j < numberOfFeatures; j++ ) {
					data[smallSet][j] = dataset->data[i][j];
				}
				for ( unsigned int j = 0; j < dataset->numberOfClasses; j++ ) {
					residuals[smallSet][j] = dataset->residuals[i][j];
				}
				smallSet++;
			}
		} else {
			if ( dataset->data[i][ feature ] > threshold ) {
				for ( unsigned int j = 0; j < numberOfFeatures; j++ ) {
					data[largeSet][j] = dataset->data[i][j];
				}
				for ( unsigned int j = 0; j < dataset->numberOfClasses; j++ ) {
					residuals[largeSet][j] = dataset->residuals[i][j];
				}
				largeSet++;
			}
		}
	}	
	for ( unsigned int j = 0; j < dataset->featureType.size(); j++ ) {
		featureType.push_back(dataset->featureType[j]);
	}
	for ( unsigned int j = 0; j < dataset->features.size(); j++ ) {
		features.push_back(dataset->features[j]);
	}
}

DataSet::~DataSet() {
	if ( data != NULL ) {
		for ( unsigned int i = 0; i < numberOfInstances; i++ ) {
			delete[] data[i];
		}
		delete[] data;
		data = NULL;
	}
	if ( residuals != NULL )  {
		for ( unsigned int i = 0; i < numberOfInstances; i++ ) {
			delete[] residuals[i];
		}
		delete[] residuals;
		residuals = NULL;
	}
}

/**
* It extracts the value of the feature and stores it in the feature value array.
* @param line the is line with the information.
* @param i is the offset to read from.
* @param output is the storage buffer.
*/
void DataSet::extractValue( const std::string line, unsigned int &i, char* output ) const {
	unsigned int j = 0;
	while ( i < line.size() && ( line[i] == ' ' || line[i] == '\t' ) ) { //Jump possible spaces.
		i++; 
	}
	if ( line[i] == '\'' || line[i] == '\"' ) {
		output[j++] = line[i++];
		while ( i < line.size() && line[i] != '\'' && line[i] != '\"' && line[i] != ',' && line[i] != '}' ) { //Get the name of the feature.
			output[j++] = line[i++];
		}
		output[j++] = line[i++];
	}
	else {
		while ( i < line.size() && line[i] != ' ' && line[i] != '\t' && line[i] != ',' && line[i] != '}' ) { //Get the name of the feature.
			output[j++] = line[i++];
		}
	}
	output[j] = '\0';
}

/**
* It process a nominal feature.
* @param line is the line with the information.
*/
void DataSet::processNominalFeature( const std::string& line ) {
	bool isTheClass = false;
	char value[Config::MAXBUFFER] = {0};
	unsigned int i = 10; //The name always starts 10 chars before the '@attribute' label.
	extractValue( line, i, value );
	if ( strcmp( value, "class" ) == 0 || strcmp( value, "\"class\"" ) == 0 || strcmp( value, "\'class\'" ) == 0 ) {
		isTheClass = true;
		classPosition = numberOfFeatures;
	}
	NominalFeature nominalFeature( value );
	i = line.find( '{', 0 ) + 1; //It finds the index of the first '{' character, starting from 0.
	do { //Do while the string is not empty...
		extractValue( line, i, value );
		i++; //Jump the ',' symbol.
		if ( strlen( value ) > 0 ) {
			nominalFeature.add( value );
			if ( isTheClass ) {
				numberOfClasses++;
			}
		}
	} while ( i < line.size() && line[i] != '}' );
	features.push_back( nominalFeature );
}

/**
* It process a numeric feature. It only stores the name of the
* feature!
* @param line is the line with the information.
*/
void DataSet::processNumericFeature( const std::string& line ) {
	char value[Config::MAXBUFFER] = { 0 };
	unsigned int i = 10; //The name always starts 10 chars before the '@attribute' label.
	extractValue( line, i, value );
	NominalFeature nominalFeature( value );
	features.push_back( nominalFeature );
}

/**
* It reads the header information and sets
* the variable types and also the number of
* instances in the file.
*/
void DataSet::readHeader( const std::string &fileName ) {
	std::ifstream inputFile( fileName, std::ios::in );
	if ( inputFile.is_open() ) {
		std::string line;
		bool dataStarted = false, isEOF = readLine( inputFile, line );
		numberOfFeatures = 0;
		numberOfInstances = 0;
		while ( !isEOF && !dataStarted ) {
			line = trim( line );
			std::transform( line.begin(), line.end(), line.begin(), ::tolower );
			if ( line.find( "@attribute", 0 ) != std::string::npos ) { //We have found a feature.
				if ( line.find( '{', 10 ) != std::string::npos ) { //A nominal feature.
					featureType.push_back( "nominal" );
					processNominalFeature( line );
				}
				else { //A numeric feature.
					featureType.push_back( "numeric" );
					processNumericFeature( line );
				}
				numberOfFeatures++;
			} else if ( line.find( "@data", 0 ) != std::string::npos ) {
				dataStarted = true;
			}
			if ( !dataStarted ) {
				isEOF = readLine( inputFile, line );
			}
		}
		if ( dataStarted ) {
			isEOF = readLine( inputFile, line );
			while ( !isEOF ) {
				numberOfInstances++;
				isEOF = readLine( inputFile, line );
			}
		}
		std::cout << "Number of features read = " << numberOfFeatures << std::endl;
		std::cout << "Number of instances read = " << numberOfInstances << std::endl;
		//Al final, tanquem el fitxer...
		inputFile.close();
	}
	else {
		std::cout << "Error: I could not open the file \'" << fileName << "\'!" << std::endl;
		exit( -1 );
	}
}

/**
* It reads the data information.
*/
void DataSet::readData( const std::string &fileName ) {
	std::ifstream inputFile( fileName, std::ios::in );
	if ( inputFile.is_open() ) {
		std::string line;
		unsigned int index = 0, numberOfUnknowns = 0;
		char value[Config::MAXBUFFER] = { 0 };
		bool headerEnded = false, isEOF = readLine( inputFile, line );
		while ( !isEOF && !headerEnded ) {
			line = trim( line );
			std::transform( line.begin(), line.end(), line.begin(), ::tolower );
			if ( line.find( "@data", 0 ) != std::string::npos ) {
				headerEnded = true;
			}
			else {
				isEOF = readLine( inputFile, line );
			}
		}
		for ( unsigned int i = 0; i < numberOfInstances; i++ ) {
			readLine( inputFile, line );
			line = trim( line );
			std::transform( line.begin(), line.end(), line.begin(), ::tolower );
			//Extract the features.
 			index = 0;
			for ( unsigned int feat = 0; feat < numberOfFeatures; feat++ ) {
				extractValue(line, index, value);
				index++; //Jump the ',' character.
				if ( featureType[feat].compare( "nominal" ) == 0 ) {
					if ( strcmp( value, "?" ) == 0 || strcmp( value, "na" ) == 0 || strcmp( value, "null" ) == 0 ) {
						data[i][feat] = Config::UNKNOWNVALUE;
						numberOfUnknowns++;
					}
					else {
						data[i][feat] = (float) features[feat].getIntValue( value );
					}
				}
				else {
					if ( strcmp( value, "?" ) == 0 || strcmp( value, "na" ) == 0 ) {
						data[i][feat] = Config::UNKNOWNVALUE;
					}
					else {
						data[i][feat] = (float) atof( value );
					}
				}
				//We need to fill in the residual matrix.
				if ( feat == numberOfFeatures - 1 ) {
					residuals[i][(int) data[i][feat]] = 1; //1 - 0 strategy: "(int)data[i][feat]" tells us the class to set to 1.
				}
			}
		}
		inputFile.close();
		std::cout << "I have read " << numberOfUnknowns << " unkown values." << std::endl;
	}
	else {
		std::cout << "Error: I could not open the file \'" << fileName << "\'!" << std::endl;
		exit(-1);
	}
}

void DataSet::loadData( const std::string &fileName ) {
	std::cout << "Trying to open the file \'" << fileName << "\'..." << std::endl;
	readHeader( fileName );
	//Allocate the matrix.
	data = new float*[numberOfInstances];
	residuals = new float*[numberOfInstances];
	for ( unsigned int i = 0; i < numberOfInstances; i++ ) {
		data[i] = new float[numberOfFeatures];
		residuals[i] = new float[numberOfClasses];
		for ( unsigned int j = 0; j < numberOfClasses; j++ ) {
			residuals[i][j] = 0;
		}
	}
	//Finally read the data.
	readData( fileName );
}

/**
* It checks if the given line is a comment or not (starts with '%', '#' or '@').
* @param line is the line being read.
* @return true if it is a comment. False otherwise.
*/
bool DataSet::isAComment( const std::string& line ) const {
	bool isAComment = false;
	unsigned int i = 0;
	while ( i < line.size() ) {
		if ( line.at(i) == '%' || line.at(i) == '#' ) {
			isAComment = true;
			break;
		}
		if ( line.at(i) != ' ' || line.at(i) != '\t' ) {
			break;
			i++;
		}
	}
	return isAComment;
}

/**
* It trims the given line.
* @param sLine the line being read.
* @return a new trimmed string.
*/
std::string DataSet::trim( const std::string& line ) const {
	unsigned int i = 0, e = line.size() - 1, uiSize = line.size();
	//Search for the initial spaces, tabs, and other non-useful stuff.
	while ( i < line.size() && ( line[i] == ' ' ||
		line[i] == '\n' || line[i] == '\t' || line[i] == '\r' ||
		line[i] == (char) 13 ) ) {
		i++;
		uiSize++;
	}
	//Search for the final spaces, tabs, and others.
	while ( e >= 1 && ( line[e] == ' ' ||
		line[e] == '\n' || line[e] == '\t' || line[e] == '\r' ||
		line[e] == (char) 13 ) ) {
		e--;
		uiSize--;
	}
	return line.substr(i, uiSize);
}

/**
* It reads a line.
* @param myFile the file to read.
* @param line the line which contains the data.
* @return true if end of file. False otherwise.
*/
bool DataSet::readLine( std::ifstream& myFile, std::string& line ) const {
	std::getline( myFile, line, '\n' );
	while ( !myFile.eof() && ( line.size() == 0 || isAComment( line ) ) ) {
		std::getline( myFile, line, '\n' );
	}
	return myFile.eof();
}

/**
* It prints the data set.
*/
void DataSet::print() const {
	std::cout << " Header information: " << std::endl;
	std::cout << "   Number of examples = " << numberOfInstances << std::endl;
	std::cout << "   Number of features = " << numberOfFeatures << std::endl;
	for ( unsigned int i = 0; i < features.size(); i++ ) {
		if ( featureType[i].compare( "nominal" ) == 0 ) {
			std::cout << "Nominal feature " << i << " = " << features[i].getName() << std::endl;
			for ( unsigned int j = 0; j < features[i].getNumberOfValues(); j++ ) {
				std::cout << "\tValue " << j << " = " << features[i].getStringValue(j) << std::endl;
			}
		}
		else {
			std::cout << "Numeric feature " << i << " = " << features[i].getName() << std::endl;
		}
	}
	for ( unsigned int i = 0; i < numberOfInstances; i++ ) {
		for ( unsigned int j = 0; j < numberOfFeatures; j++ ) {
			std::cout << data[i][j];
			if ( j < numberOfFeatures - 1 ) {
				std::cout << ", ";
			}
		}
		std::cout << "  residuals = [ ";
		for ( unsigned int j = 0; j < numberOfClasses; j++ ) {
			std::cout << residuals[i][j];
			if ( j < numberOfClasses - 1 ) {
				std::cout << ", ";
			}
		}
		std::cout << "]" << std::endl;
	}
}

void DataSet::sort( unsigned int feature ) {
	int right, left, L2, R2;
	float pivot, dataTmp;
	if ( feature < numberOfFeatures ) {
		stack.push( 0 ); //Stack the initial left value.
		stack.push( numberOfInstances - 1 ); //Stack the initial right value.
		while ( !stack.empty() ) {
			right = stack.top();
			stack.pop();
			left = stack.top();
			stack.pop();
			do {
				pivot = data[ (left + right) >> 1][feature]; //Take the half of the array as the pivot.
				L2 = left;
				R2 = right;
				do {
					while ( data[L2][feature] < pivot ) { L2++; } //Scan the left part of the array (left to right).
					while ( data[R2][feature] > pivot ) { R2--; } //Scan the right part of the array (right to left).
					if (L2 <= R2) {
						if (L2 != R2) { //Swap L2 and R2.
							for ( unsigned int feat = 0; feat < numberOfFeatures; feat++ ) { //Swap features.
								dataTmp = data[L2][feat];
								data[L2][feat] = data[R2][feat];
								data[R2][feat] = dataTmp;
							}
							for ( unsigned int label = 0; label < numberOfClasses; label++ ) { //Swap residuals.
								dataTmp = residuals[L2][label];
								residuals[L2][label] = residuals[R2][label];
								residuals[R2][label] = dataTmp;
							}
						}
						L2++;
						R2--;
					}
				} while ( L2 < R2 );
				if ( R2 - left > right - L2 ) {
					if ( left < R2 ) {
						stack.push(left);
						stack.push(R2);
					}
					left = L2;
				}
				else {
					if ( L2 < right ) {
						stack.push(L2);
						stack.push(right);
					}
					right = R2;
				}
			} while ( left < right );
		}
	}
	else {
		std::cout << "Error: index out of bounds when sorting the data." << std::endl;
	}
}

unsigned int DataSet::getNumberOfInstances() const {
	return numberOfInstances;
}

unsigned int DataSet::getNumberOfClasses() const {
	return numberOfClasses;
}

unsigned int DataSet::getNumberOfFeatures() const {
	return numberOfFeatures;
}

unsigned int DataSet::getClassPosition() const {
	return classPosition;
}

//NEW
float DataSet::getValueAt( unsigned int index, unsigned int feature ) const {
	if ( index < numberOfInstances ) {
		if (feature < numberOfFeatures) {
			return data[index][feature];
		}
		else {
			std::cout << "Error: Example array is null" << std::endl;
			return Config::UNKNOWNVALUE;
		}
	}
	else {
		std::cout << "Error: Index out of bounds when get in the example" << std::endl;
		return Config::UNKNOWNVALUE;
	}
}

float DataSet::getExampleAt( unsigned int index, float* example ) const {
	if ( index < numberOfInstances ) {
		if ( example != NULL ) {
			// numberOfFeatures - 1 because we don't want to copy the class
			for ( unsigned int i = 0; i < numberOfFeatures - 1; i++ ) {
				example[i] = data[index][i];
			}
			return data[index][numberOfFeatures - 1];
		}
		else {
			std::cout << "Error: Example array is null" << std::endl;
			return Config::UNKNOWNVALUE;
		}
	}
	else {
		std::cout << "Error: Index out of bounds when get in the example" << std::endl;
		return Config::UNKNOWNVALUE;
	}
}

float DataSet::getResidualAt( unsigned int index, unsigned int label ) const {
	if ( index < numberOfInstances ) {
		if ( label < numberOfClasses ) {
			return residuals[index][label];
		}
		else {
			std::cout << "Error: The label index is out of bounds when accessing to the residuals!" << std::endl;
		}
	}
	else {
		std::cout << "Error: Index out of bounds when accessing to the residuals!" << std::endl;
	}
	return Config::UNKNOWNVALUE;
}

void DataSet::setResidualAt( unsigned int index, unsigned int label, float value ) {
	if ( index < numberOfInstances ) {
		if ( label < numberOfClasses ) {
			residuals[index][label] = value;
		}
		else {
			std::cout << "Error: The label index is out of bounds when accessing to the residuals!" << std::endl;
		}
	}
	else {
		std::cout << "Error: Index out of bounds when accessing to the residuals!" << std::endl;
	}
}

int DataSet::getNumberOfInstancesWithThresholdAt( unsigned int feature, const std::string& symbol, float threshold ) const {
	if ( feature < numberOfFeatures ) {
		int count = 0;
		for ( unsigned int example = 0; example < numberOfInstances; example++ ) {
			if ( symbol.compare("<=") == 0 ) {
				if ( data[example][feature] <= threshold ) {
					count++;
				}
			}
			else { //'>' case.
				if ( data[example][feature] > threshold ) {
					count++;
				}
			}
		}
		return count;
	}
	else {
		std::cout << "Error: Index out of bounds when counting the number of instances with threshold!" << std::endl;
	}
	return 0;
}

std::string DataSet::getFeatureName( unsigned int index ) const{
	if ( index < numberOfFeatures ) {
		return features[index].getName();
	}
	return "NULL";
}

/*
* It returns the number of instances with a predefined class label.
* @param classLabel is the desired class label.
* @return the number of instances with that class label.
*/
int DataSet::getNumberOfInstancesOfClass(int classLabel) {
	int count = 0;
	for (unsigned int i = 0; i < numberOfInstances; i++) {
		if ((int)(data[i][numberOfFeatures - 1]) == classLabel) {
			count++;
		}
	}
	return count;
}

void DataSet::computeResidual(unsigned int exampleIndex, unsigned int classIndex, float value){
	if (exampleIndex < numberOfInstances) {
		if (classIndex < numberOfClasses) {
			//value = predicció
			residuals[exampleIndex][classIndex] -= value;
		}
		else {
			std::cout << "Error: Index out of bounds in label when computing residuals!" << std::endl;
		}
	}
	else {
		std::cout << "Error: Index out of bounds when computing residuals!" << std::endl;
	}
}

