#ifndef _data_formats_h_
#define _data_formats_h_

#include <map>
#include <string>
#include <climits>

using namespace std;

//----------------------------------------------------------------------------------------------------

struct RunLS
{
	unsigned int run ;
	unsigned int ls;

	RunLS(unsigned int _r=0, unsigned int _l=0) : run(_r), ls(_l) {}

	bool operator< (const RunLS &other) const
	{
		if (run < other.run)
			return true;
		if (run > other.run)
			return false;

		return (ls < other.ls);
	}

	void Write(FILE *f = stdout) const
	{
		fprintf(f, "%u,%u", run, ls);
	}
};

//----------------------------------------------------------------------------------------------------

struct StripData
{
	unsigned int timestamp_min = UINT_MAX;
	unsigned int timestamp_max = 0;

	unsigned int n_any = 0;
	unsigned int n_pl_suff = 0;
	unsigned int n_pl_suff_and_track = 0;
	unsigned int n_pat_suff = 0;
	unsigned int n_pat_suff_and_track = 0;
	unsigned int n_pat_suff_or_tooFull = 0;
	unsigned int n_pat_suff_or_tooFull_and_track = 0;
	unsigned int n_destr_suff = 0;

	void Write(FILE *f = stdout) const
	{
		fprintf(f, "%u,", timestamp_min);
		fprintf(f, "%u,", timestamp_max);
		fprintf(f, "%u,", n_any);
		fprintf(f, "%u,", n_pl_suff);
		fprintf(f, "%u,", n_pl_suff_and_track);
		fprintf(f, "%u,", n_pat_suff);
		fprintf(f, "%u,", n_pat_suff_and_track);
		fprintf(f, "%u,", n_pat_suff_or_tooFull);
		fprintf(f, "%u,", n_pat_suff_or_tooFull_and_track);
		fprintf(f, "%u", n_destr_suff);
	}

	void Add(const StripData &other)
	{
		timestamp_min = min(timestamp_min, other.timestamp_min);
		timestamp_max = max(timestamp_max, other.timestamp_max);

		n_any += other.n_any;
		n_pl_suff += other.n_pl_suff;
		n_pl_suff_and_track += other.n_pl_suff_and_track;
		n_pat_suff += other.n_pat_suff;
		n_pat_suff_and_track += other.n_pat_suff_and_track;
		n_pat_suff_or_tooFull += other.n_pat_suff_or_tooFull;
		n_pat_suff_or_tooFull_and_track += other.n_pat_suff_or_tooFull_and_track;
		n_destr_suff += other.n_destr_suff;
	}
};

//----------------------------------------------------------------------------------------------------

map<RunLS, StripData> LoadStripData(const string &fileName)
{
	map<RunLS, StripData> output;

	FILE *f = fopen(fileName.c_str(), "r");
	if (f == NULL)
		return output;
	
	while (!feof(f))
	{
		RunLS runLS;
		StripData d;

		int ret = fscanf(f, "%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u",
			&runLS.run, &runLS.ls,
			&d.timestamp_min,
			&d.timestamp_max,
			&d.n_any,
			&d.n_pl_suff,
			&d.n_pl_suff_and_track,
			&d.n_pat_suff,
			&d.n_pat_suff_and_track,
			&d.n_pat_suff_or_tooFull,
			&d.n_pat_suff_or_tooFull_and_track,
			&d.n_destr_suff
		);

		if (ret == 12)
			output[runLS] = d;
	}
	
	fclose(f);
	
	return output;
}

//----------------------------------------------------------------------------------------------------

struct PixelData
{
	unsigned int timestamp_min = UINT_MAX;
	unsigned int timestamp_max = 0;

	unsigned int n_any = 0;
	unsigned int n_1_track = 0;
	unsigned int n_at_least_1_track = 0;

	void Write(FILE *f = stdout) const
	{
		fprintf(f, "%u,", timestamp_min);
		fprintf(f, "%u,", timestamp_max);
		fprintf(f, "%u,", n_any);
		fprintf(f, "%u,", n_1_track);
		fprintf(f, "%u", n_at_least_1_track);
	}

	void Add(const PixelData &other)
	{
		timestamp_min = min(timestamp_min, other.timestamp_min);
		timestamp_max = max(timestamp_max, other.timestamp_max);

		n_any += other.n_any;
		n_1_track += other.n_1_track;
		n_at_least_1_track += other.n_at_least_1_track;
	}
};

//----------------------------------------------------------------------------------------------------

map<RunLS, PixelData> LoadPixelData(const string &fileName)
{
	map<RunLS, PixelData> output;

	FILE *f = fopen(fileName.c_str(), "r");
	if (f == NULL)
		return output;
	
	while (!feof(f))
	{
		RunLS runLS;
		PixelData d;

		int ret = fscanf(f, "%u,%u,%u,%u,%u,%u,%u",
			&runLS.run, &runLS.ls,
			&d.timestamp_min,
			&d.timestamp_max,
			&d.n_any,
			&d.n_1_track,
			&d.n_at_least_1_track
		);

		if (ret == 7)
			output[runLS] = d;
	}
	
	fclose(f);
	
	return output;
}

//----------------------------------------------------------------------------------------------------

template <class T>
void Add(T &dest, const T &add)
{
	for (const auto p : add)
	{
		dest[p.first].Add(p.second);
	}
}

//----------------------------------------------------------------------------------------------------

#endif
