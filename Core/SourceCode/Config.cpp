#include "Config.h"

const int Config::NOTVALID = std::numeric_limits<int>::min();
const float Config::UNKNOWNVALUE = std::numeric_limits<float>::max();

float Config::MINVARIANCE = 0.000001f;
int Config::MINIMNUMBEROFINSTANCESPERLEAF = 4;
int Config::MAXIMUMDEPTH = 1;
int Config::MAXIMUMNUMBEROFLEAVES = 0;
float Config::learningRate = 0.1f;
int Config::numberOfTrees = 10;
bool Config::verbose = false;
bool Config::multipleExperiments = false;

bool Config::applyWeight = false;
float* Config::classWeight = NULL;

std::string Config::inputFile = "../GradientBoosting/DataSet/notas/notas-10-1tst.dat";
std::string Config::modelFile = "model.bin";

void Config::print(){
	std::cout << "Configuration:" << std::endl;
	std::cout << "\t Input file: " << inputFile << std::endl;
	std::cout << "\t Model file: " << modelFile << std::endl;
	std::cout << "\t Min variance: " << MINVARIANCE << std::endl;
	std::cout << "\t Min number of instances per leaf: " << MINIMNUMBEROFINSTANCESPERLEAF << std::endl;
	std::cout << "\t Maximum depth: " << MAXIMUMDEPTH << std::endl;
	std::cout << "\t Maximum number of leaves: " << MAXIMUMNUMBEROFLEAVES << std::endl;
	std::cout << "\t Learning rate: " << learningRate << std::endl;
	std::cout << "\t Number of trees: " << numberOfTrees << std::endl;
	std::cout << "\t Apply weight: " << applyWeight << std::endl;
	std::cout << "\t Verbose: " << verbose << std::endl;
	std::cout << "\t Multiple experiments: " << multipleExperiments << std::endl;
}