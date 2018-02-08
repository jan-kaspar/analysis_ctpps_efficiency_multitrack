#include "TFile.h"

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"

#include "DataFormats/Common/interface/DetSetVector.h"

#include "DataFormats/CTPPSDetId/interface/TotemRPDetId.h"

#include "DataFormats/CTPPSReco/interface/TotemRPRecHit.h"
#include "DataFormats/CTPPSReco/interface/TotemRPUVPattern.h"
#include "DataFormats/CTPPSReco/interface/TotemRPLocalTrack.h"

#include "DataFormats/CTPPSReco/interface/CTPPSPixelRecHit.h"
#include "DataFormats/CTPPSReco/interface/CTPPSPixelLocalTrack.h"

#include "config.h"
#include "data_formats.h"

#include <vector>
#include <string>

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

int main()
{
	// start of code that may throw exceptions
	try {

	// load config
	if (cfg.LoadFrom("config.py") != 0)
	{
		printf("ERROR: cannot load config.\n");
		return 1;
	}

	printf("-------------------- config ----------------------\n");
	cfg.Print(true);
	printf("--------------------------------------------------\n");

	// initialise input collection
	fwlite::ChainEvent ev(cfg.input_files);
	printf("* events in input chain: %llu\n", ev.size());

	// book output
	map<unsigned int, map<RunLS, StripData> > stripDataCollection;
	map<unsigned int, map<RunLS, PixelData> > pixelDataCollection;

	// loop over the chain entries
	unsigned int ev_count = 0;
	for (ev.toBegin(); ! ev.atEnd(); ++ev)
	{
		RunLS runLS(ev.id().run(), ev.id().luminosityBlock());
		unsigned int timestamp = ev.time().unixTime();

		ev_count++;

		// TODO
		//if (ev_count > 1000)
		//	break;

		// get strip input data
		fwlite::Handle< DetSetVector<TotemRPRecHit> > hStripRecHits;
		hStripRecHits.getByLabel(ev, "totemRPRecHitProducer");

		fwlite::Handle< DetSetVector<TotemRPUVPattern> > hStripPatterns;
		hStripPatterns.getByLabel(ev, "totemRPUVPatternFinder");

		fwlite::Handle< DetSetVector<TotemRPLocalTrack> > hStripTracks;
		hStripTracks.getByLabel(ev, "totemRPLocalTrackFitter");

		// process strip input
		struct StripInfo
		{
			set<unsigned int> u_planes;
			set<unsigned int> v_planes;

			map<unsigned int, unsigned int> clustersPerPlane;

			bool u_tooFull = false;
			bool v_tooFull = false;

			unsigned int u_patterns = 0;
			unsigned int v_patterns = 0;

			bool hasTrack = false;
		};

		map<unsigned int, StripInfo> stripInfo;

		for (const auto &dsRecHits : *hStripRecHits)
		{
			TotemRPDetId planeId(dsRecHits.detId());
			unsigned int rpDecId = planeId.arm()*100 + planeId.station()*10 + planeId.rp();
			unsigned int planeIdx = planeId.plane();

			stripInfo[rpDecId].clustersPerPlane[planeIdx] = dsRecHits.size();

			if (dsRecHits.size() == 0)
				continue;

			if ((planeIdx % 2) == 0)
				stripInfo[rpDecId].v_planes.insert(planeIdx);
			else
				stripInfo[rpDecId].u_planes.insert(planeIdx);
		}

		for (const auto &dsPatterns : *hStripPatterns)
		{
			TotemRPDetId rpId(dsPatterns.detId());
			unsigned int rpDecId = rpId.arm()*100 + rpId.station()*10 + rpId.rp();

			for (const auto &pat : dsPatterns)
			{
				if (! pat.getFittable())
					continue;

				if (pat.getProjection() == TotemRPUVPattern::projU)
					stripInfo[rpDecId].u_patterns++;
				if (pat.getProjection() == TotemRPUVPattern::projV)
					stripInfo[rpDecId].v_patterns++;
			}
		}

		for (const auto &dsTracks : *hStripTracks)
		{
			TotemRPDetId rpId(dsTracks.detId());
			unsigned int rpDecId = rpId.arm()*100 + rpId.station()*10 + rpId.rp();

			for (const auto &tr : dsTracks)
			{
				if (tr.isValid())
					stripInfo[rpDecId].hasTrack = true;
			}
		}

		for (auto &p : stripInfo)
		{
			auto &rpInfo = p.second;

			unsigned int n_too_full_u = 0, n_too_full_v = 0;

			for (const auto &clP : rpInfo.clustersPerPlane)
			{
				if (clP.second <= 5)
					continue;

				if ((clP.first % 2) == 1)
					n_too_full_u++;
				else
					n_too_full_v++;
			}

			rpInfo.u_tooFull = (n_too_full_u >= 3);
			rpInfo.v_tooFull = (n_too_full_v >= 3);
		}

		/*
		printf("--------------------------\n");
		printf("%u:%llu\n", ev.id().run(), ev.id().event());
		printf("%u\n", ev.time().unixTime());
		for (const auto &p : stripInfo)
		{
			printf("RP %u\n", p.first);
			printf("    u_planes: %lu, v_planes: %lu\n", p.second.u_planes.size(), p.second.v_planes.size());
			printf("    u_tooFull: %u, v_tooFull: %u\n", p.second.u_tooFull, p.second.v_tooFull);
			printf("    u_patterns: %u, v_patterns: %u\n", p.second.u_patterns, p.second.v_patterns);
			printf("    hasTrack: %u\n", p.second.hasTrack);
		}
		*/

		// increment strip counters
		for (const auto &p : stripInfo)
		{
			const unsigned int &rpDecId = p.first;
			const StripInfo &rpInfo = p.second;

			bool pl_suff = (rpInfo.u_planes.size() >= 3 && rpInfo.v_planes.size() >= 3);
			bool pat_suff = (rpInfo.u_patterns >= 1 && rpInfo.v_patterns >= 1);
			bool pat_suff_or_tooFull = (rpInfo.u_patterns >= 1 || rpInfo.u_tooFull) && (rpInfo.v_patterns >= 1 || rpInfo.v_tooFull);

			bool destr_suff = (rpInfo.u_patterns >= 1 || rpInfo.u_tooFull) || (rpInfo.v_patterns >= 1 || rpInfo.v_tooFull);

			StripData &data = stripDataCollection[rpDecId][runLS];

			data.timestamp_min = min(data.timestamp_min, timestamp);
			data.timestamp_max = max(data.timestamp_max, timestamp);
			
			data.n_any++;

			if (pl_suff) data.n_pl_suff++;
			if (pl_suff && rpInfo.hasTrack) data.n_pl_suff_and_track++;

			if (pat_suff) data.n_pat_suff++;
			if (pat_suff && rpInfo.hasTrack) data.n_pat_suff_and_track++;

			if (pat_suff_or_tooFull) data.n_pat_suff_or_tooFull++;
			if (pat_suff_or_tooFull && rpInfo.hasTrack) data.n_pat_suff_or_tooFull_and_track++;

			if (destr_suff) data.n_destr_suff++;
		}

		// stop if pixels are not available
		if (! cfg.pixelsAvailable)
			continue;

		// get pixel input data
		fwlite::Handle< DetSetVector<CTPPSPixelRecHit> > hPixelRecHits;
		hPixelRecHits.getByLabel(ev, "ctppsPixelRecHits");

		fwlite::Handle< DetSetVector<CTPPSPixelLocalTrack> > hPixelTracks;
		hPixelTracks.getByLabel(ev, "ctppsPixelLocalTracks");
		
		
		// process strip input
		struct PixelInfo
		{
			unsigned int n_recHits = 0;
			unsigned int n_tracks = 0;
		};

		map<unsigned int, PixelInfo> pixelInfo;

		for (const auto &dsHits : *hPixelRecHits)
		{
			CTPPSPixelDetId planeId(dsHits.detId());
			unsigned int rpDecId = planeId.arm()*100 + planeId.station()*10 + planeId.rp();

			pixelInfo[rpDecId].n_recHits += dsHits.size();
		}

		// process strip input
		for (const auto &dsTracks : *hPixelTracks)
		{
			CTPPSPixelDetId rpId(dsTracks.detId());
			unsigned int rpDecId = rpId.arm()*100 + rpId.station()*10 + rpId.rp();

			pixelInfo[rpDecId].n_tracks = dsTracks.size();
		}

		// increase pixel counters
		for (const auto &p : pixelInfo)
		{
			const unsigned int &rpDecId = p.first;
			const auto &info = p.second;

			PixelData &data = pixelDataCollection[rpDecId][runLS];

			data.timestamp_min = min(data.timestamp_min, timestamp);
			data.timestamp_max = max(data.timestamp_max, timestamp);
			
			data.n_any++;

			if (info.n_tracks == 1) data.n_1_track++;
			if (info.n_recHits > 20 || info.n_tracks >= 1) data.n_at_least_1_track++;
		}
	}

	printf("* events processed: %u\n", ev_count);

	// save strip data
	for (const auto &rpp : stripDataCollection)
	{
		char buf[100];
		sprintf(buf, "%u.out", rpp.first);

		FILE *f = fopen(buf, "w");

		for (const auto &p : rpp.second)
		{
			p.first.Write(f);
			fprintf(f, ",");
			p.second.Write(f);
			fprintf(f, "\n");
		}

		fclose(f);
	}

	// save pixel data
	for (const auto &rpp : pixelDataCollection)
	{
		char buf[100];
		sprintf(buf, "%u.out", rpp.first);

		FILE *f = fopen(buf, "w");

		for (const auto &p : rpp.second)
		{
			p.first.Write(f);
			fprintf(f, ",");
			p.second.Write(f);
			fprintf(f, "\n");
		}

		fclose(f);
	}

	// end of code that may throw exceptions
	}

	catch (const exception &e)
	{
		printf("ERROR: there was an exception:\n");
		printf("%s\n", e.what());
		return 123;
	}

	// normal termination
	printf("* finished correctly\n");
	return 0;
}
