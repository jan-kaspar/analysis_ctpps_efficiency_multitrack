#include "TH1D.h"
#include "TFile.h"
#include "TGraph.h"

#include <time.h>
#include <string>
#include <map>

using namespace std;

//----------------------------------------------------------------------------------------------------

void ParseOne(const string &fileName, TGraph *g_pileup_vs_time)
{
	// open files
	FILE *f_in = fopen(fileName.c_str(), "r");

	while (!feof(f_in))
	{
		char line[500];
		char *res = fgets(line, 499, f_in);

		if (res == NULL)
			break;

		// remove end-line char
		if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = 0;

		//printf("%s\n", line);

		if (line[0] == '#')
			continue;

		char *pch = strtok(line, ",");
		int idx = 0;
		string s_date;
		string s_pu;
		while (pch != NULL)
		{
			if (idx == 2)
				s_date = pch;

			if (idx == 7)
				s_pu = pch;

			pch = strtok(NULL, ",");
			idx++;
		}

		double pu = atof(s_pu.c_str());

		if (pu <= 0. || pu > 70)
			continue;

		struct tm tm;
		strptime(s_date.c_str(), "%m/%d/%y %H:%M:%S", &tm);
		time_t t = mktime(&tm);

		int g_idx = g_pileup_vs_time->GetN();
		g_pileup_vs_time->SetPoint(g_idx, t, pu);
	}

	// clean up
	fclose(f_in);
}

//----------------------------------------------------------------------------------------------------

int main()
{
	// prepare output
	TFile *f_out = TFile::Open("pileup.root", "recreate");

	TGraph *g_pileup_vs_time = new TGraph(); g_pileup_vs_time->SetName("g_pileup_vs_time");

	// parse input
	ParseOne("brilcalc_2016.csv", g_pileup_vs_time);
	ParseOne("brilcalc_2017.csv", g_pileup_vs_time);

	// save output
	g_pileup_vs_time->Write();

	// clean up
	delete f_out;

	return 0;
}
