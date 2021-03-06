#include "config.h"
#include "data_formats.h"

#include "TFile.h"
#include "TGraphErrors.h"
#include "TProfile.h"
#include "TH2D.h"

#include <vector>
#include <string>
#include <climits>

using namespace std;

//----------------------------------------------------------------------------------------------------

void PrintUsage(const char *pName)
{
	printf("USAGE: %s <config> <output dir> <input dir> <input dir> ... <input dir>\n", pName);
}

//----------------------------------------------------------------------------------------------------

int main(int argc, const char **argv)
{
	// parse command line input
	if (argc < 4)
	{
		printf("ERROR: too few input arguments.\n");
		PrintUsage(argv[0]);
	}
	
	const string configFile = argv[1];
	const string outputDir = argv[2];
	vector<string> inputDirs;
	for (int i = 3; i < argc; i++)
		inputDirs.push_back(argv[i]);

	// load config
	if (cfg.LoadFrom(configFile) != 0)
	{
		printf("ERROR: cannot load config.\n");
		return 1;
	}

	//printf("-------------------- config ----------------------\n");
	//cfg.Print(true);
	//printf("--------------------------------------------------\n");

	printf("* inputDirs: %lu\n", inputDirs.size());

	// settings
	unsigned int period_length = 30 * 60;	// s

	unsigned int timestamp_min_eff = cfg.timestamp_min;
	unsigned int timestamp_bins = ceil((cfg.timestamp_max - cfg.timestamp_min) / period_length);
	unsigned int timestamp_max_eff = cfg.timestamp_min + timestamp_bins * period_length;

	// book data structures
	map<unsigned int, map<RunLS, StripData>> stripDataCollection;
	map<unsigned int, map<RunLS, PixelData>> pixelDataCollection;

	// load and merge input
	for (const auto &inputDir : inputDirs)
	{
		//printf("* loading from %s\n", inputDir.c_str());

		for (const auto &rpId : cfg.stripRPIds)
		{
			char buf[200];
			sprintf(buf, "%s/%u.out", inputDir.c_str(), rpId);
			const auto &data = LoadStripData(buf);
			Add(stripDataCollection[rpId], data);
		}

		for (const auto &rpId : cfg.pixelRPIds)
		{
			char buf[200];
			sprintf(buf, "%s/%u.out", inputDir.c_str(), rpId);
			const auto &data = LoadPixelData(buf);
			Add(pixelDataCollection[rpId], data);
		}
	}

	// get pileup data
	TFile *f_in_pu = TFile::Open("/afs/cern.ch/work/j/jkaspar/analyses/ctpps/efficiency_multitrack/pile_up/pileup.root");
	TGraph *g_pileup_vs_time = (TGraph *) f_in_pu->Get("g_pileup_vs_time");

	// prepare output file
	TFile *f_out = TFile::Open((outputDir + "/make_ratios.root").c_str(), "recreate");

	// process pileup data
	TProfile *p_pileup_vs_time = new TProfile("p_pileup_vs_time", ";timestamp", timestamp_bins, timestamp_min_eff, timestamp_max_eff);
	for (int i = 0; i < g_pileup_vs_time->GetN(); ++i)
		p_pileup_vs_time->Fill(g_pileup_vs_time->GetX()[i], g_pileup_vs_time->GetY()[i]);

	for (int bi = 1; bi <= p_pileup_vs_time->GetNbinsX(); ++bi)
	{
		if (p_pileup_vs_time->GetBinError(bi) > 0.1)
		{
			p_pileup_vs_time->SetBinContent(bi, 0.);
			p_pileup_vs_time->SetBinError(bi, 0.);
		}
	}

	p_pileup_vs_time->Write();

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

		TGraphErrors *g_eff_pat_suff_or_tooFull_vs_run = new TGraphErrors();
		g_eff_pat_suff_or_tooFull_vs_run->SetName("g_eff_pat_suff_or_tooFull_vs_run");

		TProfile *p_eff_pat_suff_or_tooFull_vs_time = new TProfile("p_eff_pat_suff_or_tooFull_vs_time", ";timestamp",
				timestamp_bins, timestamp_min_eff, timestamp_max_eff);

		sprintf(buf, "%s/efficiency_%u.csv", outputDir.c_str(), rpp.first);
		FILE *f_csv = fopen(buf, "w");

		for (const auto &p : rpp.second)
		{
			const auto &data = p.second;

			// cut off low statistics bins
			if (data.n_pat_suff_or_tooFull < 100)
				continue;

			double time = (data.timestamp_max + data.timestamp_min) / 2.;
			double time_unc = (data.timestamp_max - data.timestamp_min) / 2.;

			double x_run = double(p.first.run) + double(p.first.ls) / 10000;

			double eff_pat_suff_or_tooFull = double(data.n_pat_suff_or_tooFull_and_track) / data.n_pat_suff_or_tooFull;
			double eff_pat_suff_or_tooFull_unc = eff_pat_suff_or_tooFull / sqrt(double(data.n_pat_suff_or_tooFull_and_track));

			int idx = g_eff_pat_suff_or_tooFull_vs_time->GetN();
			g_eff_pat_suff_or_tooFull_vs_time->SetPoint(idx, time, eff_pat_suff_or_tooFull);
			g_eff_pat_suff_or_tooFull_vs_time->SetPointError(idx, time_unc, eff_pat_suff_or_tooFull_unc);

			g_eff_pat_suff_or_tooFull_vs_run->SetPoint(idx, x_run, eff_pat_suff_or_tooFull);
			g_eff_pat_suff_or_tooFull_vs_run->SetPointError(idx, 0., eff_pat_suff_or_tooFull_unc);

			p_eff_pat_suff_or_tooFull_vs_time->Fill(time, eff_pat_suff_or_tooFull);

			fprintf(f_csv, "%u,%u,%u,%u,%.4f,%.4f\n", p.first.run, p.first.ls,
				data.timestamp_min, data.timestamp_max, eff_pat_suff_or_tooFull, eff_pat_suff_or_tooFull_unc);
		}

		TH2D *h2_eff_vs_pileup = new TH2D("h2_eff_vs_pileup", ";pileup;efficiency", 100, 10., 60., 100, 0.3, 0.9);

		for (int bi = 1; bi <= p_eff_pat_suff_or_tooFull_vs_time->GetNbinsX(); ++bi)
		{
			double eff = p_eff_pat_suff_or_tooFull_vs_time->GetBinContent(bi);
			double pileup = p_pileup_vs_time->GetBinContent(bi);

			if (eff > 0. && pileup > 0.)
				h2_eff_vs_pileup->Fill(pileup, eff);
		}

		g_eff_pat_suff_or_tooFull_vs_time->Write();
		g_eff_pat_suff_or_tooFull_vs_run->Write();
		p_eff_pat_suff_or_tooFull_vs_time->Write();

		h2_eff_vs_pileup->Write();

		fclose(f_csv);
	}

	// process pixel data
	printf("* processing pixel data\n");
	for (const auto &rpp : pixelDataCollection)
	{
		char buf[100];
		sprintf(buf, "%u", rpp.first);
		TDirectory *d_rp = f_out->mkdir(buf);

		gDirectory = d_rp;

		TGraphErrors *g_single_track_ratio_vs_time = new TGraphErrors();
		g_single_track_ratio_vs_time->SetName("g_single_track_ratio_vs_time");

		TGraphErrors *g_single_track_ratio_vs_run = new TGraphErrors();
		g_single_track_ratio_vs_run->SetName("g_single_track_ratio_vs_run");

		unsigned int timestamp_min_eff = cfg.timestamp_min;
		unsigned int timestamp_bins = ceil((cfg.timestamp_max - cfg.timestamp_min) / period_length);
		unsigned int timestamp_max_eff = cfg.timestamp_min + timestamp_bins * period_length;

		TProfile *p_single_track_ratio_vs_time = new TProfile("p_single_track_ratio_vs_time", ";timestamp",
				timestamp_bins, timestamp_min_eff, timestamp_max_eff);

		for (const auto &p : rpp.second)
		{
			const auto &data = p.second;

			// cut off low statistics bins
			if (data.n_at_least_1_track < 100)
				continue;

			double time = (data.timestamp_max + data.timestamp_min) / 2.;
			double time_unc = (data.timestamp_max - data.timestamp_min) / 2.;

			double x_run = double(p.first.run) + double(p.first.ls) / 10000;

			double single_track_ratio = double(data.n_1_track) / data.n_at_least_1_track;
			double single_track_ratio_unc = single_track_ratio / sqrt(double(data.n_at_least_1_track));

			int idx = g_single_track_ratio_vs_time->GetN();
			g_single_track_ratio_vs_time->SetPoint(idx, time, single_track_ratio);
			g_single_track_ratio_vs_time->SetPointError(idx, time_unc, single_track_ratio_unc);

			g_single_track_ratio_vs_run->SetPoint(idx, x_run, single_track_ratio);
			g_single_track_ratio_vs_run->SetPointError(idx, 0., single_track_ratio_unc);

			p_single_track_ratio_vs_time->Fill(time, single_track_ratio);
		}

		g_single_track_ratio_vs_time->Write();
		g_single_track_ratio_vs_run->Write();

		p_single_track_ratio_vs_time->Write();
	}

	// clean up
	delete f_out;
	delete f_in_pu;

	return 0;
}
