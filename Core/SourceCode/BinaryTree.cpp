#include "BinaryTree.h"

BinaryTree::BinaryTree(){
	leftChild = NULL;
	rightChild = NULL;
	splitPoint = Config::UNKNOWNVALUE;
	feature = Config::NOTVALID;
	expectedValue = 0.0f;
	sumOfSQErrors = 0.0f;
	sumOfWeights = 0;
	numberOfRegions = 0;
	classLabel = 0;
	gamma = 0.0f;
	proportionOfInstances[0] = proportionOfInstances[1] = 0.0f;
}

BinaryTree::~BinaryTree() {
	deallocateTree();	
}

void BinaryTree::deallocateTree() {
	if( leftChild != NULL ) {
		leftChild->deallocateTree();
		delete leftChild;
		leftChild = NULL;
	}
	if( rightChild != NULL ) {
		rightChild->deallocateTree();
		delete rightChild;
		rightChild = NULL;
	}
}

void BinaryTree::generateTree(unsigned int classLabel, DataSet* data){
	leftChild = NULL;
	rightChild = NULL;
	gamma = 0.0f;
	this->splitPoint = Config::UNKNOWNVALUE;
	this->feature = Config::NOTVALID;
	this->expectedValue = 0.0f;
	sumOfSQErrors = 0.0f;
	sumOfWeights = (float) data->getNumberOfInstances();
	proportionOfInstances[0] = proportionOfInstances[1] = 0.0f;
	numberOfRegions = 0;
	this->classLabel = classLabel;

	//2. Calc. global var.
	float minVariance = 0.0f;

	for ( unsigned int i = 0; i < data->getNumberOfInstances(); i++ ){
		expectedValue += data->getResidualAt(i, classLabel) * data->getWeight();
		sumOfSQErrors += data->getResidualAt(i, classLabel) * data->getResidualAt(i, classLabel) * data->getWeight();
		//expectedValue += data->getOutputVariableAt(i) * data->getWeight();
		//sumOfSQErrors += data->getOutputVariableAt(i) * data->getOutputVariableAt(i) * data->getWeight();
	}

	//Minimum var. to know where to split.
	minVariance = Config::MINVARIANCE * computeVariance(expectedValue, sumOfSQErrors, sumOfWeights) / sumOfWeights;
	expectedValue /= sumOfWeights;

	numberOfRegions = 1 + buildRBinaryTree(0, expectedValue, sumOfWeights, minVariance, data, 0);
	
	//Minimize the gradient
	float fraction = Config::learningRate * (data->getNumberOfClasses() - 1) / ((float)data->getNumberOfClasses());
	setLeaves(fraction);
}

/**
* It sets the leaves (for gamma computation).
* @param fraction is the fraction to extract to the expected value.
*/
void BinaryTree::setLeaves(float fraction) {
	std::stack<BinaryTree*> stack;
	BinaryTree* top = this;
	stack.push(NULL);
	while (top) {
		if (top->feature == Config::NOTVALID) { //We're in a leaf node.
			top->gamma = fraction * top->expectedValue;
			leaves.push_back(top);
		}
		else { //We're in an intermediate node.
			if (top->leftChild != NULL) {
				stack.push(top->leftChild);
			}
			if(top->rightChild != NULL) {
				stack.push(top->rightChild);
			}
		}
		top = stack.top();
		stack.pop();
	}
}

int BinaryTree::buildRBinaryTree( int depth, float expectedValue, float sumOfWeights, float minVariance, DataSet* data, int splitsInTheTree ){
	//3. Calc. Var. example by example.
	if ( data->getNumberOfInstances() <= 0 ) {
		this->feature = Config::NOTVALID;
		return splitsInTheTree;
	}
	
	float priorVariance = 0;
	float totValueSum = 0;
	float totSumOfSQErrors = 0;
	float totSumOfWeights = (float) data->getNumberOfInstances();

	for ( unsigned int i = 0; i < data->getNumberOfInstances(); i++ ){
		totValueSum += data->getResidualAt(i, classLabel) * data->getWeight();
		totSumOfSQErrors += data->getResidualAt(i, classLabel) * data->getResidualAt(i, classLabel) * data->getWeight();
		//totValueSum += data->getOutputVariableAt(i) * data->getWeight();
		//totSumOfSQErrors += data->getOutputVariableAt(i) * data->getOutputVariableAt(i) * data->getWeight();
	}

	priorVariance = computeVariance(totValueSum, totSumOfSQErrors, totSumOfWeights);
	
	this->expectedValue = expectedValue;
	
	//Finish
	if ((sumOfWeights < (float)( Config::MINIMNUMBEROFINSTANCESPERLEAF)) || //Not enough instances.
		( ( priorVariance / sumOfWeights ) < minVariance ) || //Not enough variance.
		( Config::MAXIMUMDEPTH > 0 && depth >= Config::MAXIMUMDEPTH) ||  //Maximum depth reached.
		( Config::MAXIMUMNUMBEROFLEAVES > 0 && ( ( splitsInTheTree + 1 ) >= Config::MAXIMUMNUMBEROFLEAVES ) ) //Maximum number of leaves reached.
		) {
		this->feature = Config::NOTVALID; // Leaf node
		this->sumOfSQErrors = priorVariance;
		this->sumOfWeights = sumOfWeights;
		return splitsInTheTree;
	}

	unsigned int lengthOfFeatures = data->getNumberOfFeatures() - 1; //We do not want the last feature (the class)!
	
	float* splits = NULL;
	splits = new float[ lengthOfFeatures ];
	
	float* gainValues = NULL;
	gainValues = new float[ lengthOfFeatures ];
	
	float** expectedValues;
	expectedValues = new float*[ lengthOfFeatures ];
	
	float** SSEs = NULL;
	SSEs = new float*[ lengthOfFeatures ];
	
	float** totalSubSetWeights;
	totalSubSetWeights = new float*[ lengthOfFeatures ];
	
	float** proportions;
	proportions = new float*[ lengthOfFeatures ];

	for ( unsigned int i = 0; i < lengthOfFeatures; i++ ){
		splits[i] = -Config::UNKNOWNVALUE;
		gainValues[i] = -Config::UNKNOWNVALUE;
		SSEs[i] = new float[2];
		expectedValues[i] = new float[2];
		totalSubSetWeights[i] = new float[2];
		proportions[i] = new float[2];
	}

	for ( unsigned int i = 0; i < lengthOfFeatures; i++ ){
		splits[i] = variableSplit( i, data, gainValues, expectedValues, SSEs, totalSubSetWeights, proportions );
	}

	float higherGainVariance = -Config::UNKNOWNVALUE;
	for ( unsigned int i = 0; i < lengthOfFeatures; i++ ){
		if ( ( i == 0 ) || gainValues[i] > higherGainVariance ){
			higherGainVariance = gainValues[i];
			this->feature = i;
		}
	}
	//Assure that there are two subsets with the minimum number of instances per leaf.
	bool minimumNumberOfRequiredInstances = false;
	if( (totalSubSetWeights[feature][0] * data->getNumberOfInstances()) >= Config::MINIMNUMBEROFINSTANCESPERLEAF && 
		(totalSubSetWeights[feature][1] * data->getNumberOfInstances()) >= Config::MINIMNUMBEROFINSTANCESPERLEAF ) {
		minimumNumberOfRequiredInstances = true;
	}

	if ( gainValues[feature] > 0.0f && minimumNumberOfRequiredInstances == true ) {
		proportionOfInstances[0] = proportions[feature][0];
		proportionOfInstances[1] = proportions[feature][1];
		splitPoint = splits[feature];
		std::string symbol = "<=";
		DataSet *leftDataset = NULL;
		leftDataset = new DataSet( ( unsigned int ) feature, symbol, splitPoint, data );
		leftChild = new BinaryTree();
		splitsInTheTree = leftChild->buildRBinaryTree( depth + 1, expectedValues[feature][0], totalSubSetWeights[feature][0], minVariance, leftDataset, splitsInTheTree + 1 );

		symbol = ">";
		DataSet *rightDataset = NULL;
		rightDataset = new DataSet( ( unsigned int ) feature, symbol, splitPoint, data );
		rightChild = new BinaryTree();
		splitsInTheTree = rightChild->buildRBinaryTree( depth + 1, expectedValues[feature][1], totalSubSetWeights[feature][1], minVariance, rightDataset, splitsInTheTree );

		deallocateMemory( leftDataset );
		deallocateMemory( rightDataset );
	}
	else {
		feature = Config::NOTVALID;
	}
	this->sumOfSQErrors = priorVariance;
	this->sumOfWeights = sumOfWeights;

	//Deallocate the memory.
	deallocateMemory( proportions, lengthOfFeatures );
	deallocateMemory( totalSubSetWeights, lengthOfFeatures );
	deallocateMemory( expectedValues, lengthOfFeatures );
	deallocateMemory( SSEs, lengthOfFeatures );
	deallocateMemory( gainValues );
	deallocateMemory( splits );

	return splitsInTheTree;
}

float BinaryTree::variableSplit( int feature, DataSet* data, float* gainValues, float** expectedValues, float** SSEs, float** totalSubSetWeights, float** proportions ) {
	//Initialize variables
	//First to the right side of the tree, then to the left one.
	float totValueSum = 0;
	float totSumOfSQErrors = 0;
	float totSumOfWeights = 0;
	float splitPoint = Config::UNKNOWNVALUE; //not valid
	int indexOfFirstMissingValue = Config::NOTVALID;
	float sumValue[2], sumOfSQErrors[2], sumOfWeights[2], currSums[2], currSumSquared[2], currSumOfWeights[2];
	sumValue[0] = sumOfSQErrors[0] = sumOfWeights[0] = currSums[0] = currSumSquared[0] = currSumOfWeights[0] = 0;
	sumValue[1] = sumOfSQErrors[1] = sumOfWeights[1] = currSums[1] = currSumSquared[1] = currSumOfWeights[1] = 0;
	
	//Sort the data
	data->sort( feature );

	//Right side
	for ( unsigned int i = 0; i < data->getNumberOfInstances(); i++ ) {
		//Missing value check
		if ( data->getValueAt( i, feature ) == Config::UNKNOWNVALUE ){
			break;
		}
		currSums[1] += data->getResidualAt(i, classLabel) * data->getWeight();
		currSumSquared[1] += data->getResidualAt(i, classLabel) * data->getResidualAt(i, classLabel) * data->getWeight();
		//currSums[1] += data->getOutputVariableAt(i) * data->getWeight();
		//currSumSquared[1] += data->getOutputVariableAt(i) * data->getOutputVariableAt(i) * data->getWeight();
		currSumOfWeights[1] += data->getWeight();
	}
	totValueSum = sumValue[1] = currSums[1];
	totSumOfSQErrors = sumOfSQErrors[1] = currSumSquared[1];
	totSumOfWeights = sumOfWeights[1] = currSumOfWeights[1];

	//Test possible split points
	float currSplit = data->getValueAt( 0, feature );
	float currVal = 0, bestVal = Config::UNKNOWNVALUE;

	for ( unsigned int i = 0; i < data->getNumberOfInstances(); i++ ) {
		//Missing value check
		if ( data->getValueAt(i, feature) == Config::UNKNOWNVALUE ) { //We do not allow missing values in this stage.
			indexOfFirstMissingValue = i;
			break;
		}
		if ( data->getValueAt( i, feature ) > currSplit ) {
			//Compute the variance in right and left part of the tree
			currVal = computeVariance( currSums[0], currSumSquared[0], currSumOfWeights[0]) + computeVariance(currSums[1], currSumSquared[1], currSumOfWeights[1] );

			if ( currVal < bestVal ) {
				bestVal = currVal;
				splitPoint = (data->getValueAt(i, feature) + currSplit) / 2.0f;
				//Left
				sumValue[0] = currSums[0];
				sumOfSQErrors[0] = currSumSquared[0];
				sumOfWeights[0] = currSumOfWeights[0];
				//Right
				sumValue[1] = currSums[1];
				sumOfSQErrors[1] = currSumSquared[1];
				sumOfWeights[1] = currSumOfWeights[1];
			}
		}
		//Extract of the right part of the tree and place it to the left.
		currSplit = data->getValueAt( i, feature );
		currSums[0] += data->getResidualAt( i, classLabel ) * data->getWeight();
		currSumSquared[0] += data->getResidualAt( i, classLabel ) * data->getResidualAt( i, classLabel ) * data->getWeight();
		//currSums[0] += data->getOutputVariableAt( i ) * data->getWeight();
		//currSumSquared[0] += data->getOutputVariableAt( i ) * data->getOutputVariableAt( i ) * data->getWeight();
		currSumOfWeights[0] += data->getWeight();

		currSums[1] -= data->getResidualAt( i, classLabel ) * data->getWeight();
		currSumSquared[1] -= data->getResidualAt( i, classLabel ) * data->getResidualAt( i, classLabel ) * data->getWeight();
		//currSums[1] -= data->getOutputVariableAt( i ) * data->getWeight();
		//currSumSquared[1] -= data->getOutputVariableAt( i ) * data->getOutputVariableAt( i ) * data->getWeight();
		currSumOfWeights[1] -= data->getWeight();

	}
	if( sumOfWeights[0] <= 0.0f || sumOfWeights[1] <= 0.0f ) { //Not worth the effort!
		gainValues[feature] = -Config::UNKNOWNVALUE;
		return -Config::UNKNOWNVALUE;
	}
	
	//Compute weights for each split, normalizing these.
	proportions[feature][0] = sumOfWeights[0];
	proportions[feature][1] = sumOfWeights[1];

	float sum = proportions[feature][0] + proportions[feature][1];
	if (sum != 0.0f) {
		proportions[feature][0] /= sum;
		proportions[feature][1] /= sum;
	}

	//Handle missing values.
	if ( indexOfFirstMissingValue > -1 ) {
		for ( unsigned int i = indexOfFirstMissingValue; i < data->getNumberOfInstances(); ++i ) {
			//Left part of the tree
			sumValue[0] += data->getResidualAt( i, classLabel ) * data->getWeight() * proportions[feature][0];
			sumOfSQErrors[0] += data->getResidualAt( i, classLabel ) * data->getResidualAt( i, classLabel ) * data->getWeight() * proportions[feature][0];
			sumOfWeights[0] += data->getWeight() * proportions[feature][0];
			//Right part of the tree
			sumValue[1] += data->getResidualAt( i, classLabel ) * data->getWeight() * proportions[feature][1];
			sumOfSQErrors[1] += data->getResidualAt( i, classLabel ) * data->getResidualAt( i, classLabel ) * data->getWeight() * proportions[feature][1];
			sumOfWeights[1] += data->getWeight() * proportions[feature][1];

			totValueSum += data->getResidualAt( i, classLabel ) * data->getWeight();
			totSumOfSQErrors += data->getResidualAt( i, classLabel ) * data->getResidualAt( i, classLabel ) * data->getWeight();
			totSumOfWeights += data->getWeight();
		}
	}

	//Normalize
	if ( sumOfWeights[0] > 0.0f ) { 
		expectedValues[feature][0] = sumValue[0] / sumOfWeights[0]; 
		SSEs[feature][0] = sumOfSQErrors[0] / sumOfWeights[0];
	}
	else { 
		expectedValues[feature][0] = totValueSum / totSumOfWeights;
		SSEs[feature][0] = totSumOfSQErrors / totSumOfWeights;
	}

	if ( sumOfWeights[1] > 0.0f ) { 
		expectedValues[feature][1] = sumValue[1] / sumOfWeights[1]; 
		SSEs[feature][1] = sumOfSQErrors[1] / sumOfWeights[1];
	}
	else { 
		expectedValues[feature][1] = totValueSum / totSumOfWeights; 
		SSEs[feature][1] = totSumOfSQErrors / totSumOfWeights;
	}
	
	totalSubSetWeights[feature][0] = sumOfWeights[0];
	totalSubSetWeights[feature][1] = sumOfWeights[1];

	gainValues[feature] = computeVariance( totValueSum, totSumOfSQErrors, totSumOfWeights ) - ( computeVariance( sumValue[0], sumOfSQErrors[0], sumOfWeights[0] ) + computeVariance( sumValue[1], sumOfSQErrors[1], sumOfWeights[1] ) );

	return splitPoint;
}

float BinaryTree::getGamma(const float* example) const{
	float currentGamma = 0;
	if (feature < 0){
		currentGamma = this->gamma;
	}
	else {
		if (example[feature] != Config::UNKNOWNVALUE) {
			if (example[feature] <= splitPoint){
				currentGamma = leftChild->getGamma(example);
			}
			else {
				currentGamma = rightChild->getGamma(example);
			}
		}
		//If it is an uknown value the best we can do is to compute an average of the two possible cases (left & right)
		else {
			float tmp = 0.0f;
			tmp = leftChild->getGamma(example);
			if (tmp > 0) {
				currentGamma += proportionOfInstances[0] * tmp;
			}
			tmp = rightChild->getGamma(example);
			if (tmp > 0) {
				currentGamma += proportionOfInstances[1] * tmp;
			}
		}
	}
	return currentGamma;
}


float BinaryTree::makeAPrediction( const float* example ) const{
	float prediction = 0;
	//Leaf node
	if ( feature < 0 ){
		prediction = expectedValue;
	}
	//Internal node
	else {
		if ( example[feature] != Config::UNKNOWNVALUE ) {
			if ( example[feature] <= splitPoint ){
				prediction = leftChild->makeAPrediction( example );
			}
			else {
				prediction = rightChild->makeAPrediction( example );
			}
		}
		//Uknown values
		else {
			float tmp = 0.0f;
			tmp = leftChild->makeAPrediction( example );
			if (tmp > 0) {
				prediction += proportionOfInstances[0] * tmp;
			}
			tmp = rightChild->makeAPrediction( example );
			if (tmp > 0) {
				prediction += proportionOfInstances[1] * tmp;
			}
		}
	}
	return prediction;
}


float BinaryTree::computeVariance( float value, float sumSQValue, float weight ){
	float variance = 0;
	if ( weight > 0 ){
		variance = sumSQValue - ( ( value*value ) / weight );
	}
	else{
		std::cout << "ERROR: Weight value is <= 0!" << std::endl;
	}
	return variance;
}

void BinaryTree::print( const DataSet* data ) {
	print(0, data);
} 

void BinaryTree::print(int depth, const DataSet* data) {
	if (feature < 0) { //A leaf node.
		std::cout << " : " << expectedValue;
		float avgError = 0;
		if (sumOfWeights > 0) {
			avgError = sumOfSQErrors / sumOfWeights;
		}
		std::cout << " (w: " << sumOfWeights << " /e: " << avgError << ")";
		return;
	}
	//Left successor.
	std::cout << std::endl;
	for (int i = 0; i < depth; ++i) {
		std::cout << "|  ";
	}
	std::cout << data->getFeatureName(feature) << " <= " << splitPoint;
	if (leftChild != NULL) {
		leftChild->print(depth + 1, data);
	}
	//Right successor.
	std::cout << std::endl;
	for (int i = 0; i < depth; i++) {
		std::cout << "|  ";
	}
	std::cout << data->getFeatureName(feature) << " > " << splitPoint;
	if (rightChild != NULL) {
		rightChild->print(depth + 1, data);
	}
	if (depth == 0) {
		std::cout << std::endl << std::endl;
	}
}

void BinaryTree::printToFile(const DataSet* data) {
	std::string pGraphvizData;
	printToFile(0, data, pGraphvizData);
	//save to a file
	std::cout << "Saving..." << std::endl;
	std::cout << pGraphvizData << std::endl;
	std::ofstream f("BinaryTree.txt");
	if (!f.fail()){
		f << pGraphvizData;
		f.close();
	}
	std::cout << "... To /BinaryTree.txt" << std::endl;
}

void BinaryTree::printToFile(int depth, const DataSet* data, std::string& pGraphvizData) {
	char aux[50];
	memset(aux, '\0', sizeof(aux));
	if (feature < 0) { //A leaf node.
		pGraphvizData += " : ";
		#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
			sprintf(aux, "%.2f", expectedValue);
		#else
			sprintf_s(aux, "%.2f", expectedValue);
		#endif
		pGraphvizData += aux;
		float avgError = 0;
		if (sumOfWeights > 0) {
			avgError = sumOfSQErrors / sumOfWeights;
		}
		pGraphvizData += " (";
		pGraphvizData += "w:";
		#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
			sprintf(aux, "%.2f", sumOfWeights);
		#else
			sprintf_s(aux, "%.2f", sumOfWeights);
		#endif
		pGraphvizData += aux;
		pGraphvizData += " /";
		#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
			sprintf(aux, "%.2f", avgError);
		#else
			sprintf_s(aux, "%.2f", avgError);
		#endif
		pGraphvizData += " e:";
		pGraphvizData += aux;
		pGraphvizData += ")";
		return;
	}
	//Left successor.
	if (pGraphvizData != "")
	pGraphvizData += '\n';
	for (int i = 0; i < depth; ++i) {
		pGraphvizData += "|  ";
	}
	pGraphvizData += data->getFeatureName(feature) + " <= ";
	#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
		sprintf(aux, "%.2f", splitPoint);
	#else
		sprintf_s(aux, "%.2f", splitPoint);
	#endif
	pGraphvizData += aux;
	if (leftChild != NULL) {
		leftChild->printToFile(depth + 1, data, pGraphvizData);
	}
	//Right successor.
	pGraphvizData += '\n';
	for (int i = 0; i < depth; i++) {
		pGraphvizData += "|  ";
	}
	pGraphvizData += data->getFeatureName(feature) + " > ";
	#if defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
		sprintf(aux, "%.2f", splitPoint);
	#else
		sprintf_s(aux, "%.2f", splitPoint);
	#endif
	pGraphvizData += aux;
	if (rightChild != NULL) {
		rightChild->printToFile(depth + 1, data, pGraphvizData);
	}
	if (depth == 0) {
		std::cout << std::endl << std::endl;
	}
}

int BinaryTree::getNumberOfRegions() {
	return numberOfRegions;
}

void BinaryTree::readFromBinaryFile() {
	std::ifstream mFile;
	mFile.open( Config::modelFile.c_str(), std::ios::in | std::ios::binary );
	if ( mFile.is_open() ){
		readFromBinaryFile(mFile);
		//setLeaves()
		mFile.close();
	} 
	else {
		std::cout << "> Error: I couldn't read the model file! " << std::endl;
	}

}

void BinaryTree::saveToBinaryFile() const {
	std::ofstream oFile;
	oFile.open( Config::modelFile.c_str(), std::ios::out | std::ios::binary );
	if ( oFile.is_open() ){
		saveToBinaryFile( oFile );
		oFile.close();
	}
	else {
		std::cout << "> Error: I couldn't create the model file! " << std::endl;
	}
}

void BinaryTree::saveToBinaryFile( std::ofstream &oFile ) const{
	oFile.write( reinterpret_cast<const char*> ( &classLabel ), sizeof( classLabel ) );
	oFile.write( reinterpret_cast<const char*> ( &feature ), sizeof( feature ) );
	oFile.write( reinterpret_cast<const char*> ( &splitPoint ), sizeof( splitPoint ) );
	oFile.write( reinterpret_cast<const char*> ( &expectedValue ), sizeof( expectedValue ) );
	oFile.write( reinterpret_cast<const char*> ( &sumOfSQErrors ), sizeof( sumOfSQErrors ) );
	oFile.write( reinterpret_cast<const char*> ( &sumOfWeights ), sizeof( sumOfWeights ) );
	oFile.write(reinterpret_cast<const char*> (&gamma), sizeof(gamma));
	//A leaf node
	if ( feature < 0 ) {
		return;
	}
	//Left & Right
	oFile.write( reinterpret_cast<const char*> ( &proportionOfInstances[0] ), sizeof( proportionOfInstances[0] ) );
	oFile.write( reinterpret_cast<const char*> ( &proportionOfInstances[1] ), sizeof( proportionOfInstances[1] ) );
	if ( leftChild != NULL ) {
		leftChild->saveToBinaryFile( oFile );
	}
	if (rightChild != NULL) {
		rightChild->saveToBinaryFile( oFile );
	}
}

void BinaryTree::readFromBinaryFile(std::ifstream& mFile) {
	leftChild = rightChild = NULL;
	proportionOfInstances[0] = proportionOfInstances[1] = 0;
	mFile.read( reinterpret_cast<char*> ( &classLabel ), sizeof( classLabel ) );
	mFile.read( reinterpret_cast<char*> ( &feature ), sizeof( feature ) );
	mFile.read( reinterpret_cast<char*> ( &splitPoint ), sizeof( splitPoint ) );
	mFile.read( reinterpret_cast<char*> ( &expectedValue ), sizeof( expectedValue ) );
	mFile.read( reinterpret_cast<char*> ( &sumOfSQErrors ), sizeof( sumOfSQErrors ) );
	mFile.read( reinterpret_cast<char*> ( &sumOfWeights ), sizeof( sumOfWeights ) );
	mFile.read(reinterpret_cast<char*> (&gamma), sizeof(gamma));
	if ( feature < 0 ) {
		return;
	}
	mFile.read( reinterpret_cast<char*> ( &proportionOfInstances[0] ), sizeof( proportionOfInstances[0] ) );
	mFile.read( reinterpret_cast<char*> ( &proportionOfInstances[1] ), sizeof( proportionOfInstances[1] ) );
	leftChild = new BinaryTree();
	rightChild = new BinaryTree();
	if (leftChild != NULL) {
		leftChild->readFromBinaryFile( mFile );
	}
	if (rightChild != NULL) {
		rightChild->readFromBinaryFile( mFile );
	}
}

void BinaryTree::deallocateMemory( DataSet* data ){
	if( data != NULL ) {
		delete data;
		data = NULL;
	}
}

void BinaryTree::deallocateMemory( BinaryTree* data ){
	if (data != NULL) {
		delete data;
		data = NULL;
	}
}

void BinaryTree::deallocateMemory( float* data ){
	if (data != NULL) {
		delete[] data;
		data = NULL;
	}
}

void BinaryTree::deallocateMemory( float** data, int lengthOfFeatures ){
	if (data != NULL) {
		for (int i = 0; i < lengthOfFeatures; i++) {
			delete[] data[i];
		}
		delete[] data;
		data = NULL;
	}
}
