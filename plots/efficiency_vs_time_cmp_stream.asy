import root;
import pad_layout;

string topDir = "../";

int timestamp0 = 1451602800;

string periods[];
real p_x_mins[], p_x_maxs[];
//periods.push("2016_preTS2"); p_x_mins.push(0); p_x_maxs.push(0);
//periods.push("2016_postTS2"); p_x_mins.push(0); p_x_maxs.push(0);
//periods.push("2017_preTS2"); p_x_mins.push(0); p_x_maxs.push(0);
periods.push("2017_postTS2"); p_x_mins.push(640); p_x_maxs.push(645);

string rps[], rp_labels[];
//rps.push("3"); rp_labels.push("45-210-fr-hr");
//rps.push("2"); rp_labels.push("45-210-nr-hr");
//rps.push("102"); rp_labels.push("56-210-nr-hr");
rps.push("103"); rp_labels.push("56-210-fr-hr");

string streams[];
pen s_pens[];
streams.push("ZeroBias"); s_pens.push(red+0.8pt);
streams.push("DoubleEG"); s_pens.push(blue);
streams.push("SingleMuon"); s_pens.push(heavygreen);

string quantity = "p_eff_pat_suff_or_tooFull_vs_time";

xSizeDef = 12cm;

//----------------------------------------------------------------------------------------------------

for (int pi : periods.keys)
{
	NewRow();

	for (int rpi : rps.keys)
	{
		NewPad("days from 1 Jan 2016", "efficiency");

		AddToLegend("(" + replace(periods[pi], "_", "\_") + ")");
		AddToLegend("(" + rp_labels[rpi] + ")");

		TGraph_x_min = timestamp0 + 24*3600*p_x_mins[pi];
		TGraph_x_max = timestamp0 + 24*3600*p_x_maxs[pi];

		for (int sti : streams.keys)
		{
			string f = topDir + periods[pi] + "/" + streams[sti] + "/make_ratios.root";

			RootObject obj = RootGetObject(f, rps[rpi] + "/" + quantity, error=false);
			if (obj.valid)
				draw(scale(1./24/3600, 1.) * shift(-timestamp0), obj, "d0,vl,eb", s_pens[sti], streams[sti]);
		}
		
		limits((p_x_mins[pi], 0.3), (p_x_maxs[pi], 0.7), Crop);

		AttachLegend(NW, NE);
	}
}

//----------------------------------------------------------------------------------------------------

GShipout(hSkip=2mm, vSkip=0mm);
