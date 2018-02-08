#include "command_line_tools.h"
#include "data_formats.h"

#include "TFile.h"
#include "TGraphErrors.h"

#include <vector>
#include <string>
#include <climits>

using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage(const char *pName)
{
	printf("USAGE: %s <output dir> <input dir> <input dir> ... <input dir>\n", pName);
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// parse command line input
	if (argc < 3)
	{
		printf("ERROR: too few input arguments.\n");
		PrintUsage(argv[0]);
	}
	
	const string outputDir = argv[1];
	vector<string> inputDirs;
	for (unsigned int i = 2; i < argc; i++)
		inputDirs.push_back(argv[i]);

	// configuration
	vector<unsigned int> stripRPs = { 3, 103 };
	vector<unsigned int> pixelRPs = { 23, 123 };

	// book data structures
	map<unsigned int, map<RunLS, StripData>> stripDataCollection;
	map<unsigned int, map<RunLS, PixelData>> pixelDataCollection;

	// load and merge input
	for (const auto &inputDir : inputDirs)
	{
		printf("* loading from %s\n", inputDir.c_str());

		for (const auto &rpId : stripRPs)
		{
			char buf[200];
			sprintf(buf, "%s/%u.out", inputDir.c_str(), rpId);
			const auto &data = LoadStripData(buf);
			Add(stripDataCollection[rpId], data);
		}

		for (const auto &rpId : pixelRPs)
		{
			char buf[200];
			sprintf(buf, "%s/%u.out", inputDir.c_str(), rpId);
			const auto &data = LoadPixelData(buf);
			Add(pixelDataCollection[rpId], data);
		}
	}

	// prepare output file
	TFile *f_out = TFile::Open((outputDir + "/make_ratios.root").c_str(), "recreate");

	// process strip data
	printf("* processing strip data\n");
	for (const auto &rpp : stripDataCollection)
	{
		char buf[100];
		sprintf(buf, "%u", rpp.first);
		TDirectory *d_rp = f_out->mkdir(buf);

		gDirectory = d_rp;

		TGraphErrors *g_eff_pat_suff_or_tooFull_vs_time = new TGraphErrors();
		g_eff_pat_suff_or_tooFull_vs_time->SetName("g_eff_pat_suff_or_tooFull_vs_time");

		for (const auto &p : rpp.second)
		{
			const auto &data = p.second;

			// cut off low statistics bins
			if (data.n_any < 1000)
				continue;

			double time = (data.timestamp_max + data.timestamp_min) / 2.;
			double time_unc = (data.timestamp_max - data.timestamp_min) / 2.;

			double eff_pat_suff_or_tooFull = double(data.n_pat_suff_or_tooFull_and_track) / data.n_pat_suff_or_tooFull;
			double eff_pat_suff_or_tooFull_unc = eff_pat_suff_or_tooFull / sqrt(double(data.n_pat_suff_or_tooFull_and_track));

			int idx = g_eff_pat_suff_or_tooFull_vs_time->GetN();
			g_eff_pat_suff_or_tooFull_vs_time->SetPoint(idx, time, eff_pat_suff_or_tooFull);
			g_eff_pat_suff_or_tooFull_vs_time->SetPointError(idx, time_unc, eff_pat_suff_or_tooFull_unc);
		}

		g_eff_pat_suff_or_tooFull_vs_time->Write();
	}

	// clean up
	delete f_out;

	return 0;
}
