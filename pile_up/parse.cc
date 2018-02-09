#include "TH1D.h"
#include "TFile.h"
#include "TGraph.h"

#include <time.h>
#include <string>
#include <map>

using namespace std;

//----------------------------------------------------------------------------------------------------

int main()
{
	// open files
	FILE *f_in = fopen("brilcalc_2016.csv", "r");

	TFile *f_out = TFile::Open("pileup.root", "recreate");

	// parse file
	TGraph *g_pileup_vs_hour = new TGraph(); g_pileup_vs_hour->SetName("g_pileup_vs_hour");

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

		unsigned int timestamp0 = 1451602800; // 1 Jan 2016 in UNIX time
		unsigned int timestamp = t - timestamp0;
		double hour = double(timestamp) / 3600.;

		//printf("%s --> %s\n", s_date.c_str(), s_pu.c_str());
		//printf("    %u\n", (unsigned int)t);

		int g_idx = g_pileup_vs_hour->GetN();
		g_pileup_vs_hour->SetPoint(g_idx, hour, pu);
	}

	g_pileup_vs_hour->Write();

	// clean up
	fclose(f_in);
	delete f_out;

	return 0;
}