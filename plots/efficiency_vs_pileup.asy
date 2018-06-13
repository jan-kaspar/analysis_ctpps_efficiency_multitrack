import root;
import pad_layout;

string topDir = "../";

int timestamp0 = 1451602800;

string periods[];
periods.push("2016_preTS2");
periods.push("2016_postTS2");
periods.push("2017_preTS2");
periods.push("2017_postTS2");

string rps[], rp_labels[];
rps.push("3"); rp_labels.push("45-210-fr-hr");
rps.push("2"); rp_labels.push("45-210-nr-hr");
rps.push("102"); rp_labels.push("56-210-nr-hr");
rps.push("103"); rp_labels.push("56-210-fr-hr");

string stream = "ZeroBias";

//----------------------------------------------------------------------------------------------------

for (int pi : periods.keys)
{
	NewRow();

	NewPad(false);
	label(replace(periods[pi], "_", "\_"));

	for (int rpi : rps.keys)
	{
		NewPad("pileup", "efficiency");

		string f = topDir + periods[pi] + "/" + stream + "/make_ratios.root";

		RootObject obj = RootGetObject(f, rps[rpi] + "/h2_eff_vs_pileup", error=false);
		if (!obj.valid)
			continue;
		
		//obj.vExec("Rebin2D", 2, 2);
		draw(obj, "def");
		
		limits((10, 0.3), (60, 0.9), Crop);

		AttachLegend(rp_labels[rpi]);
	}
}

GShipout(hSkip=2mm, vSkip=0mm);
