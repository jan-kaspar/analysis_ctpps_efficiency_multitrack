#ifndef _config_h_
#define _config_h_

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"

#include <vector>
#include <string>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct Config
{
	vector<string> input_files;

	unsigned int timestamp_min, timestamp_max;

	bool pixelsAvailable;

	vector<unsigned int> stripRPIds;
	vector<unsigned int> pixelRPIds;

	int LoadFrom(const string &f);

	void Print(bool print_input_files=false) const;
};

//----------------------------------------------------------------------------------------------------

int Config::LoadFrom(const string &f_in)
{
	const edm::ParameterSet& config = edm::readPSetsFrom(f_in)->getParameter<edm::ParameterSet>("config");

	input_files = config.getParameter<vector<string>>("input_files");

	timestamp_min = config.getParameter<unsigned int>("timestamp_min");
	timestamp_max = config.getParameter<unsigned int>("timestamp_max");

	pixelsAvailable = config.getParameter<bool>("pixelsAvailable");

	stripRPIds = config.getParameter<vector<unsigned int>>("stripRPIds");
	pixelRPIds = config.getParameter<vector<unsigned int>>("pixelRPIds");

	return 0;
}

//----------------------------------------------------------------------------------------------------

void Config::Print(bool print_input_files) const
{
	if (print_input_files)
	{
		printf("* input files\n");
		for (const auto &f : input_files)
			printf("    %s\n", f.c_str());
		printf("\n");
	}

	printf("* pixelsAvailable: %u\n", pixelsAvailable);

	printf("\n");
	printf("* RP ids\n");
	printf("    strips: \n");
	for (const auto &id : stripRPIds)
		printf("%u, ", id);
	printf("\n");
	printf("    pixels: \n");
	for (const auto &id : pixelRPIds)
		printf("%u, ", id);
	printf("\n");
}

//----------------------------------------------------------------------------------------------------

Config cfg;

#endif
