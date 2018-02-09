import root;
import pad_layout;

string topDir = "../";

int timestamp0 = 1451602800;

string periods[], p_labels[];
real p_x_mins[], p_x_maxs[];
periods.push("2017_preTS2"); p_x_mins.push(532.5); p_x_maxs.push(535.5); p_labels.push("2017, preTS2, start");
periods.push("2017_preTS2"); p_x_mins.push(603); p_x_maxs.push(606); p_labels.push("2017, preTS2, end");
periods.push("2017_postTS2"); p_x_mins.push(647.5); p_x_maxs.push(650.5); p_labels.push("2017, postTS2, start");
periods.push("2017_postTS2"); p_x_mins.push(671); p_x_maxs.push(674); p_labels.push("2017, postTS2, end");

string rps[], rp_labels[], rp_corr_pixels[];
rps.push("3"); rp_labels.push("45-210-fr-hr"); rp_corr_pixels.push("23");
rps.push("103"); rp_labels.push("56-210-fr-hr"); rp_corr_pixels.push("123");

string stream = "ZeroBias";

string quantity = "p_eff_pat_suff_or_tooFull_vs_time";
string quantity_pixel = "p_single_track_ratio_vs_time";

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
	label("{\SetFontSizesXX " + replace(p_labels[pi], "_", "\_") + "}");

	for (int rpi : rps.keys)
	{
		NewPad("days from 1 Jan 2016", "efficiency");

		TGraph_x_min = timestamp0 + 24*3600*p_x_mins[pi];
		TGraph_x_max = timestamp0 + 24*3600*p_x_maxs[pi];

		string f = topDir + periods[pi] + "/" + stream + "/make_ratios.root";

		RootObject obj = RootGetObject(f, rps[rpi] + "/" + quantity, error=false);
		if (obj.valid)
			draw(scale(1./24/3600, 1.) * shift(-timestamp0), obj, "d0,vl,eb", red);

		RootObject obj = RootGetObject(f, rp_corr_pixels[rpi] + "/" + quantity_pixel, error=false);
		if (obj.valid)
			draw(scale(1./24/3600, 1.) * shift(-timestamp0), obj, "d0,vl,eb", blue);
		
		limits((p_x_mins[pi], 0.2), (p_x_maxs[pi], 0.7), Crop);
	}
}

//----------------------------------------------------------------------------------------------------

NewPad(false);
AddToLegend("directly from strips", red);
AddToLegend("indirectly from pixels", blue);
AttachLegend();

GShipout(hSkip=2mm, vSkip=0mm);
