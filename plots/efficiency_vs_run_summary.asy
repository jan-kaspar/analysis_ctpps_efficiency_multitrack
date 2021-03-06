import root;
import pad_layout;

string topDir = "../";

int timestamp0 = 1451602800;

string periods[];
real p_x_mins[], p_x_maxs[];
periods.push("2016_preTS2"); p_x_mins.push(0); p_x_maxs.push(0);
periods.push("2016_postTS2"); p_x_mins.push(0); p_x_maxs.push(0);
periods.push("2017_preTS2"); p_x_mins.push(0); p_x_maxs.push(0);
periods.push("2017_postTS2"); p_x_mins.push(0); p_x_maxs.push(0);

string rps[], rp_labels[];
rps.push("3"); rp_labels.push("45-210-fr-hr");
/*
rps.push("2"); rp_labels.push("45-210-nr-hr");
rps.push("102"); rp_labels.push("56-210-nr-hr");
rps.push("103"); rp_labels.push("56-210-fr-hr");
*/

string streams[];
pen s_pens[];
streams.push("ZeroBias"); s_pens.push(red);
//streams.push("DoubleEG"); s_pens.push(blue);
//streams.push("SingleMuon"); s_pens.push(heavygreen);

string quantity = "g_eff_pat_suff_or_tooFull_vs_run";


//xTicksDef = LeftTicks(5., 1.);

xSizeDef = 12cm;

TGraph_errorBar = None;

//----------------------------------------------------------------------------------------------------

NewPad();

for (int rpi : rps.keys)
{
	NewPad(false);
	label("{\SetFontSizesXX " + rp_labels[rpi] + "}");
}

for (int pi : periods.keys)
{
	NewRow();

	NewPad(false);
	label("{\SetFontSizesXX " + replace(periods[pi], "_", "\_") + "}");

	for (int rpi : rps.keys)
	{
		NewPad("run + ls/$10^4$", "efficiency");

		for (int sti : streams.keys)
		{
			string f = topDir + periods[pi] + "/" + streams[sti] + "/make_ratios.root";

			RootObject obj = RootGetObject(f, rps[rpi] + "/" + quantity, error=false);
			if (obj.valid)
				draw(obj, "p,ieb", s_pens[sti]);
		}
		
		//limits((x_mins[ri], 0.4), (x_maxs[ri], 1.), Crop);
	}
}

//----------------------------------------------------------------------------------------------------

NewPad(false);
for (int sti : streams.keys)
	AddToLegend(streams[sti], s_pens[sti]);
AttachLegend();

GShipout(hSkip=2mm, vSkip=0mm);
