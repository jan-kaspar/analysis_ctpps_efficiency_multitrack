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

for (int rpi : rps.keys)
{
	NewPad("days from 1 Jan 2016", "efficiency");

	TGraph_x_min = timestamp0 + 24*3600*x_min;
	TGraph_x_max = timestamp0 + 24*3600*x_max;

	string f = topDir + period + "/" + stream + "/make_ratios.root";

	RootObject obj = RootGetObject(f, rps[rpi] + "/" + quantity, error=false);
	if (obj.valid)
		draw(scale(1./24/3600, 1.) * shift(-timestamp0), obj, "d0,vl,eb", red, stream);
	
	limits((x_min, 0.3), (x_max, 0.9), Crop);

	AttachLegend(rp_labels[rpi]);
}

//----------------------------------------------------------------------------------------------------

NewRow();

TGraph_x_min = 24*x_min;
TGraph_x_max = 24*x_max;

NewPad("days from 1 Jan 2016", "pile-up");
TGraph_reducePoints = 10;
draw(scale(1./24, 1.), RootGetObject(topDir + "pile_up/pileup.root", "g_pileup_vs_hour"), "d", blue);

limits((x_min, 0.), (x_max, 60.), Crop);

//----------------------------------------------------------------------------------------------------

GShipout(hSkip=2mm, vSkip=0mm);
