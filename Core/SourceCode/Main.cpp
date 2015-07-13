#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "Config.h"
#include "DataSet.h"
#include "BinaryTree.h"
#include "GradientBoosting.h"

const std::string version = "2015.06.22-9:07";

std::string getAtribute(const std::string& line) {
	char value[Config::MAXBUFFER];
	unsigned int pos = 0, j = 0;
	//Jump blank spaces.
	while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) { pos++; }
	//Jump --
	while (pos < line.size() && line[pos] == '-'){ pos++; }
	//Get the value.
	while (pos < line.size() && line[pos] != '=' && line[pos] != ' ' && line[pos] != '\t' && j < Config::MAXBUFFER) { value[j++] = tolower( line[pos++] ); }
	value[j] = '\0';
	std::string newValue(value);
	return newValue;
}

std::string getValue( const std::string& line ) {
	char value[Config::MAXBUFFER];
	unsigned int pos = 0, j = 0;
	while (pos < line.size() && line[pos] != '=') { pos++; }
	pos++;
	//Jump blank spaces.
	while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) { pos++; }
	//Get the value.
	while (pos < line.size() && line[pos] != ' ' && line[pos] != '\t' && j < Config::MAXBUFFER) { value[j++] = ( line[pos++] ); }
	value[j] = '\0';
	std::string newValue(value);
	return newValue;
}

//True = training, false = test
bool parseArgs( int argc, char **argv ){
	std::string attribute;
	bool isTraining = false;
	//argv[0] is the name of the executable
	for (int i = 1; i < argc; i++){
		attribute = getAtribute(argv[i]);
		//It is found?
		if (attribute.find("train") != std::string::npos){
			isTraining = true;
			Config::inputFile = getValue(argv[i]);
		}
		else if (attribute.find("test") != std::string::npos){
			if (isTraining) {
				std::cout << "Error: Train and test at the same time! Set as testing" << std::endl;
			}
			else {
				isTraining = false;
				Config::inputFile = getValue(argv[i]);
			}
		}
	}
	for (int i = 1; i < argc; i++){
		attribute = getAtribute(argv[i]);
		if (attribute.find("minimumnumberofinstancesperleaf") != std::string::npos){
			Config::MINIMNUMBEROFINSTANCESPERLEAF = atoi(getValue(argv[i]).c_str());
		}
		else if (attribute.find("maximumdepth") != std::string::npos){
			Config::MAXIMUMDEPTH = atoi(getValue(argv[i]).c_str());
		}
		else if (attribute.find("maximumnumberofleaves") != std::string::npos){
			Config::MAXIMUMNUMBEROFLEAVES = atoi(getValue(argv[i]).c_str());
		}
		else if (attribute.find("minimumvariance") != std::string::npos){
			Config::MINVARIANCE = (float)atof(getValue(argv[i]).c_str());
		}
		else if (attribute.find("numberoftrees") != std::string::npos){
			Config::numberOfTrees = atoi(getValue(argv[i]).c_str());
		}
		else if (attribute.find("learningrate") != std::string::npos){
			Config::learningRate = (float)atof(getValue(argv[i]).c_str());
		}
		else if (attribute.find("applyweight") != std::string::npos){
			if (getValue(argv[i])[0] == 't' || getValue(argv[i])[0] == 'T') {
				Config::applyWeight = true;
			}
		}
		else if (attribute.find("verbose") != std::string::npos){
			if (getValue(argv[i])[0] == 't' || getValue(argv[i])[0] == 'T') {
				Config::verbose = true;
			}
		}
		else if (attribute.find("multipleexperiments") != std::string::npos){
			if (getValue(argv[i])[0] == 't' || getValue(argv[i])[0] == 'T') {
				Config::multipleExperiments = true;
			}
		}
	}
	return isTraining;
}

int main(int argc, char **argv){
	std::cout << "> Sergi Sancho Asensio <Binary> Regression tree version " << version << std::endl;
	if (argc < 2) {
		std::cout << "Error: I need arguments!" << std::endl;
		std::cout << "Usage: " << std::endl;
		std::cout << "--attribute=value" << std::endl;
		std::cout << "Example: " << std::endl;
		std::cout << "--train=../GradientBoosting/DataSet/iris.arff --minimumnumberofinstancesperleaf=4 --maximumnumberofleaves=0 --maximumdepth=0 --minimumvariance=0.003 --numberoftrees=100 --learningrate=0.1 --verbose=F --multipleexperiments=F" << std::endl;
	}
	else {

		GradientBoosting gb;
		if (parseArgs(argc, argv)) { //Is training
			Config::print();
			gb.train();
		}
		else { //Test
			Config::print();
			gb.test();
		}
	}
	if (Config::verbose && !Config::multipleExperiments) {
		std::cout << "[Press enter to exit]" << std::endl;
		fflush(NULL);
		getchar();
	}
	return 0;
}
