import root;
import pad_layout;

string topDir = "../";

int timestamp0 = 1451602800;

string periods[];
real p_x_mins[], p_x_maxs[];
//periods.push("2016_preTS2"); p_x_mins.push(0); p_x_maxs.push(0);
//periods.push("2016_postTS2"); p_x_mins.push(0); p_x_maxs.push(0);
//periods.push("2017_preTS2"); p_x_mins.push(0); p_x_maxs.push(0);
periods.push("2017_postTS2"); p_x_mins.push(655); p_x_maxs.push(660);

string rps[], rp_labels[];
rps.push("3"); rp_labels.push("45-210-fr-hr");
//rps.push("103"); rp_labels.push("56-210-fr-hr");

string stream = "ZeroBias";

string quantity_g = "g_eff_pat_suff_or_tooFull_vs_time";
string quantity_p = "p_eff_pat_suff_or_tooFull_vs_time";

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
		NewPad("days from 1 Jan 2016", "efficiency");

		TGraph_x_min = timestamp0 + 24*3600*p_x_mins[pi];
		TGraph_x_max = timestamp0 + 24*3600*p_x_maxs[pi];

		string f = topDir + periods[pi] + "/" + stream + "/make_ratios.root";

		RootObject obj = RootGetObject(f, rps[rpi] + "/" + quantity_g, error=false);
		if (obj.valid)
			draw(scale(1./24/3600, 1.) * shift(-timestamp0), obj, "p,ieb", red);

		RootObject obj = RootGetObject(f, rps[rpi] + "/" + quantity_p, error=false);
		if (obj.valid)
			draw(scale(1./24/3600, 1.) * shift(-timestamp0), obj, "d0,vl,eb", blue+1pt);
		
		limits((p_x_mins[pi], 0.3), (p_x_maxs[pi], 0.7), Crop);
	}
}

//----------------------------------------------------------------------------------------------------

NewPad(false);
AddToLegend("1 point per lumisection", red);
AddToLegend("1 point per 30 minutes", blue);
AttachLegend();

GShipout(hSkip=2mm, vSkip=0mm);
