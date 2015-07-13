#include "GradientBoosting.h"

GradientBoosting::GradientBoosting(){
	genericDataset = NULL;
	boostingTrees = NULL;
}

GradientBoosting::~GradientBoosting(){
	if (genericDataset) {
		if (boostingTrees) {
			for (unsigned int i = 0; i < (unsigned int)Config::numberOfTrees; i++) {
				for (unsigned int j = 0; j < genericDataset->getNumberOfClasses(); j++) {
					delete boostingTrees[i][j];
				}
				delete[] boostingTrees[i];
			}
			delete[] boostingTrees;
			boostingTrees = NULL;
		}
		delete genericDataset;
		genericDataset = NULL;
	}
}

void GradientBoosting::computeSoftMax(float* labelPredictions, float* labelProbabilities) const {
	float value, sumexp = 0;
	for (unsigned int k = 0; k < (unsigned int) genericDataset->getNumberOfClasses(); k++) {
		value = (float)std::exp(labelPredictions[k]);
		labelProbabilities[k] = value;
		sumexp += value;
	}
	for (unsigned int k = 0; k < (unsigned int) genericDataset->getNumberOfClasses(); k++) {
		if (sumexp > 0.0f) {
			labelProbabilities[k] /= sumexp;
		}
		else {
			labelProbabilities[k] = 0.0f;
		}
	}
}

int GradientBoosting::computePrediction(float* example, float* labelPredictions, float* labelProbabilities) const {
	float* w = new float[genericDataset->getNumberOfClasses()];
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		labelPredictions[i] = 0;
	}
	if (Config::applyWeight){
		for (unsigned int i = 0; i < genericDataset->getNumberOfClasses(); i++) {
			w[i] = genericDataset->getNumberOfInstancesOfClass(i) / (float)genericDataset->getNumberOfInstances();
		}
		for (unsigned int i = 0; i < (unsigned int)Config::numberOfTrees; i++) {
			for (unsigned int j = 0; j < (unsigned int)genericDataset->getNumberOfClasses(); j++) {
				labelPredictions[j] += w[j] * boostingTrees[i][j]->getGamma(example);
			}
		}
	}
	else {
		for (unsigned int i = 0; i < (unsigned int)Config::numberOfTrees; i++) {
			for (unsigned int j = 0; j < (unsigned int)genericDataset->getNumberOfClasses(); j++) {
				labelPredictions[j] += boostingTrees[i][j]->getGamma(example);
			}
		}
	}

	computeSoftMax(labelPredictions, labelProbabilities);

	//Get the maximum probability value for obtaining the most probable label
	float maxValue = labelProbabilities[0];
	int label = 0;
	for (unsigned int i = 1; i < (unsigned int) genericDataset->getNumberOfClasses(); i++) {
		if (maxValue < labelProbabilities[i]) {
			maxValue = labelProbabilities[i];
			label = (int)i;
		}
	}
	if (w) {
		delete[] w;
	}
	return label;
}


/**
* It computes the proportion of true positives. It is heavily based upon
* the Weka implementation.
* @param classIndex the desired class index.
* @return the proportion of true positives.
*/
float GradientBoosting::truePositives(int classIndex, int** confusionMatrix) const {
	float correct = 0, total = 0;
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		if (i == (unsigned int)classIndex) {
			correct += confusionMatrix[classIndex][i];
		}
		total += confusionMatrix[classIndex][i];
	}
	if (total <= 0.0f) {
		return 0.0f;
	}
	return correct / total;
}

/**
* It computes the true positive rate. It is heavily based upon
* the Weka implementation.
* @return the recall.
*/
float GradientBoosting::recall(int** confusionMatrix) const {
	float* classCounts = new float[genericDataset->getNumberOfClasses()];
	float classCountSum = 0, truePositiveTotal = 0, temp = 0;
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		classCounts[i] = 0;
	}
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		for (unsigned int j = 0; j < (unsigned int)genericDataset->getNumberOfClasses(); j++) {
			classCounts[i] += confusionMatrix[i][j];
		}
		classCountSum += classCounts[i];
	}
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		temp = truePositives(i, confusionMatrix);
		truePositiveTotal += (temp * classCounts[i]);
	}
	if (classCounts) {
		delete[] classCounts;
	}
	return truePositiveTotal / classCountSum;
}

/**
* It computes the precision at the given class. It is heavily based upon
* the Weka implementation.
* @param classIndex the desired class index.
* @return the precision.
*/
float GradientBoosting::precisionAt(int classIndex, int** confusionMatrix) const {
	float correct = 0, total = 0;
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		if (i == (unsigned int)classIndex) {
			correct += confusionMatrix[i][classIndex];
		}
		total += confusionMatrix[i][classIndex];
	}
	if (total <= 0.0f) {
		return 0.0f;
	}
	return correct / total;
}

/**
* It computes the precision. It is heavily based upon
* the Weka implementation.
* @return the precision.
*/
float GradientBoosting::precision(int** confusionMatrix) const {
	float* classCounts = new float[genericDataset->getNumberOfClasses()];
	float classCountSum = 0, precisionTotal = 0, temp = 0;
	for (unsigned int i = 0; i <(unsigned int)genericDataset->getNumberOfClasses(); i++) {
		classCounts[i] = 0;
	}
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		for (unsigned int j = 0; j < (unsigned int)genericDataset->getNumberOfClasses(); j++) {
			classCounts[i] += confusionMatrix[i][j];
		}
		classCountSum += classCounts[i];
	}
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		temp = precisionAt(i,confusionMatrix);
		precisionTotal += (temp * classCounts[i]);
	}
	if (classCounts) {
		delete[] classCounts;
	}
	return precisionTotal / classCountSum;
}

/**
* It computes the F-Measure at the given class. It is heavily based upon
* the Weka implementation.
* @param classIndex the desired class index.
* @return the F-Measure.
*/
float GradientBoosting::FMeasureAt(int classIndex, int** confusionMatrix) const {
	float pre = precisionAt(classIndex, confusionMatrix);
	float rec = truePositives(classIndex, confusionMatrix);
	if ((pre + rec) <= 0.0f) {
		return 0.0f;
	}
	return (2 * pre * rec) / (pre + rec);
}

/**
* It computes the F-Measure. It is heavily based upon
* the Weka implementation.
* @return the precision.
*/
float GradientBoosting::FMeasure(int** confusionMatrix) const {
	float* classCounts = new float[genericDataset->getNumberOfClasses()];
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); ++i) {
		classCounts[i] = 0;
	}
	float classCountSum = 0, FMeasureTotal = 0, temp = 0;
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); ++i) {
		for (unsigned int j = 0; j < (unsigned int)genericDataset->getNumberOfClasses(); ++j) {
			classCounts[i] += confusionMatrix[i][j];
		}
		classCountSum += classCounts[i];
	}
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); ++i) {
		temp = FMeasureAt(i, confusionMatrix);
		FMeasureTotal += (temp * classCounts[i]);
	}
	if (classCounts) {
		delete[] classCounts;
	}
	return FMeasureTotal / classCountSum;
}


void GradientBoosting::test() {
	genericDataset = new DataSet(Config::inputFile);
	readModelFromFile();

	int** confusionMatrix = new int*[genericDataset->getNumberOfClasses()];
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++){
		confusionMatrix[i] = new int[genericDataset->getNumberOfClasses()];
		for (unsigned int j = 0; j < (unsigned int)genericDataset->getNumberOfClasses(); j++){
			confusionMatrix[i][j] = 0;
		}
	}

	float* labelPredictions = new float[genericDataset->getNumberOfClasses()];
	float* labelProbabilities = new float[genericDataset->getNumberOfClasses()];

	float* example = new float[genericDataset->getNumberOfFeatures() - 1];

	unsigned int predictedLabel = 0, missedPredictions = 0;
	float realOutput = 0;
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfInstances(); i++){
		realOutput = genericDataset->getExampleAt(i, example);
		predictedLabel = computePrediction(example, labelPredictions, labelProbabilities);
		confusionMatrix[predictedLabel][(int)realOutput]++;
		if ((int)predictedLabel != (int)realOutput){
			missedPredictions++;
		}
	}

	std::cout << "Confusion matrix:" << std::endl;
	for (unsigned int i = 0; i < (unsigned int)genericDataset->getNumberOfClasses(); i++) {
		for (unsigned int j = 0; j < (unsigned int)genericDataset->getNumberOfClasses(); j++) {
			std::cout << confusionMatrix[i][j] << "\t";
		}
		std::cout << std::endl;
	}
	float error = missedPredictions / (float)genericDataset->getNumberOfInstances();
	std::cout << "Test accuracy: " << (1 - error) << " (error: " << error << ")" << std::endl;
	std::cout << "F-Measure: " << FMeasure(confusionMatrix) << std::endl;

	if (confusionMatrix) {
		for (unsigned int i = 0; i < (unsigned int) genericDataset->getNumberOfClasses(); i++){
			delete[] confusionMatrix[i];
		}
		delete[] confusionMatrix;
		confusionMatrix = NULL;
	}
	if (labelPredictions) {
		delete[] labelPredictions;
		labelPredictions = NULL;
	}
	if (labelProbabilities) {
		delete[] labelProbabilities;
		labelProbabilities = NULL;
	}
	if (example) {
		delete[] example;
		example = NULL;
	}
}

void GradientBoosting::readModelFromFile() {
	std::ifstream modelFile;
	modelFile.open(Config::modelFile.c_str(), std::ios::in | std::ios::binary);
	if (modelFile.is_open()) {
		unsigned int sizeOfModel = 0;
		modelFile.read(reinterpret_cast<char*>(&sizeOfModel), sizeof(sizeOfModel));
		if (sizeOfModel != (unsigned int) Config::numberOfTrees) {
			Config::numberOfTrees = sizeOfModel;
			std::cout << "WARNING: The number of trees is different" << std::endl;
		}
		std::cout << "Number of trees read from file = " << Config::numberOfTrees << std::endl;
		if (sizeOfModel > 0) {
			boostingTrees = new BinaryTree**[Config::numberOfTrees];
			for (unsigned int i = 0; i < (unsigned int)Config::numberOfTrees; i++) {
				boostingTrees[i] = new BinaryTree*[genericDataset->getNumberOfClasses()];
				for (unsigned int j = 0; j < genericDataset->getNumberOfClasses(); j++) {
					boostingTrees[i][j] = new BinaryTree();
					boostingTrees[i][j]->readFromBinaryFile(modelFile);
					if (Config::verbose) {
						std::cout << " > Tree from round = " << i << " and class label = " << j << ": " << std::endl;
						boostingTrees[i][j]->print(genericDataset);
					}
				}
			}
		}
		else {
			modelFile.close();
			std::cout << "ERROR: Invalid number of trees" << std::endl;
			exit(-1);
		}
		modelFile.close();
	}
	else {
		std::cout << "ERROR: Can't load the model file" << std::endl;
		exit(-1);
	}
}

void GradientBoosting::train() {
	genericDataset = new DataSet(Config::inputFile);
	boostingTrees = new BinaryTree**[Config::numberOfTrees];
	for (unsigned int i = 0; i < (unsigned int)Config::numberOfTrees; i++) {
		boostingTrees[i] = new BinaryTree*[genericDataset->getNumberOfClasses()];
		for (unsigned int j = 0; j < genericDataset->getNumberOfClasses(); j++) {
			boostingTrees[i][j] = new BinaryTree();
		}
	}
	float* example = new float[genericDataset->getNumberOfFeatures()-1];

	std::ofstream modelFile;
	modelFile.open(Config::modelFile.c_str(), std::ios::out | std::ios::binary);
	if (modelFile.is_open()){
		unsigned int sizeOfModel = Config::numberOfTrees;
		modelFile.write(reinterpret_cast<const char*>(&sizeOfModel), sizeof(sizeOfModel));
		for (unsigned int i = 0; i < (unsigned int)Config::numberOfTrees; i++) {
			for (unsigned int j = 0; j < genericDataset->getNumberOfClasses(); j++) {
				boostingTrees[i][j]->generateTree(j, genericDataset);
				boostingTrees[i][j]->saveToBinaryFile(modelFile);
				if (Config::verbose) {
					std::cout << " > Tree from round = " << i << " and class label = " << j << ": " << std::endl;
					boostingTrees[i][j]->print(genericDataset);
				}
				//Compute residuals
				for (unsigned int ex = 0; ex < genericDataset->getNumberOfInstances(); ex++) {
					genericDataset->getExampleAt(ex, example);
					genericDataset->computeResidual(ex, j, boostingTrees[i][j]->getGamma(example));
				}
			}
		}
		modelFile.close();

		if (Config::multipleExperiments) test();
	}
	else {
		std::cout << "ERROR: Failed to write the model file" << std::endl;
	}
	if (example) {
		delete[] example;
		example = NULL;
	}
}
