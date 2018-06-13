import root;
import pad_layout;

string topDir = "../";

int timestamp0 = 1451602800;

string period = "2016_postTS2";
real x_min = 280;
real x_max = 300;

string rps[], rp_labels[];
//rps.push("3"); rp_labels.push("45-210-fr-hr");
//rps.push("2"); rp_labels.push("45-210-nr-hr");
//rps.push("102"); rp_labels.push("56-210-nr-hr");
rps.push("103"); rp_labels.push("56-210-fr-hr");

string stream = "ZeroBias";

string quantity = "p_eff_pat_suff_or_tooFull_vs_time";

xSizeDef = 25cm;

//----------------------------------------------------------------------------------------------------

TH1_x_min = timestamp0 + 24*3600*x_min;
TH1_x_max = timestamp0 + 24*3600*x_max;

for (int rpi : rps.keys)
{
	NewPad("days from 1 Jan 2016", "efficiency");

	string f = topDir + period + "/" + stream + "/make_ratios.root";

	RootObject obj = RootGetObject(f, rps[rpi] + "/" + quantity, error=false);
	if (obj.valid)
		draw(scale(1./24/3600, 1.) * shift(-timestamp0), obj, "d0,vl,eb", red+1pt, stream);
	
	limits((x_min, 0.3), (x_max, 0.9), Crop);

	AttachLegend(rp_labels[rpi]);
}

//----------------------------------------------------------------------------------------------------

NewRow();


NewPad("days from 1 Jan 2016", "pile-up");

string f = topDir + period + "/" + stream + "/make_ratios.root";

draw(scale(1./24/3600, 1.) * shift(-timestamp0), RootGetObject(f, "p_pileup_vs_time"), "d0,eb,vl", blue+1pt);

limits((x_min, 0.), (x_max, 60.), Crop);

//----------------------------------------------------------------------------------------------------

GShipout(hSkip=2mm, vSkip=0mm);
