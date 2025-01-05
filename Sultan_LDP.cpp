#include "sierrachart.h"
/*
	Written by Sultan

	This study draws a Horizantal Ray, signifying that a Large Delta Participant (LDP) has shown up.
	LDP's are shown if there position is above the DeltaThreshold and are color-coordinated if they are
	an aggressive Buyer/Seller (LDB/LDS).

	TODO's:
	- Study does not update any LDP's if they are not on the last index bar
		-- Study should update all previous LDP's that are on the chart
	- DeltaThreshold should be determined as the NumberBars do, not as an input
	- Study could be more efficient
	- Create input to show/hide price labels for LDP's
	- (RESOLVED)Study is being tested on Footprint Chart for testing, but is intedended to be used on a candlestick chart
*/
SCDLLName("Sultan - Large Delta Participants")

SCSFExport scsf_LDP(SCStudyInterfaceRef sc)
{
	// Declare Variables
	SCString msg;
	const s_VolumeAtPriceV2* p_VAP = NULL;
	s_UseTool Draw;

	//Set Inputs
	SCInputRef i_DeltaThreshold = sc.Input[0];
	SCInputRef i_LDB_COLOR = sc.Input[1];
	SCInputRef i_LDS_COLOR = sc.Input[2];

	// Set configuration variables
	if (sc.SetDefaults)
	{

		sc.GraphName = "Large Delta Participants";
		sc.GraphRegion = 0;	

		//Auto-looping
		sc.AutoLoop = 1;
		//Delta Threshold
		i_DeltaThreshold.Name="Delta Threshold";
		i_DeltaThreshold.SetInt(100);

		//LDB_COLOR
		i_LDB_COLOR.Name="LDB Color";
		i_LDB_COLOR.SetColor(0,130,255);

		//LDS_COLOR
		i_LDS_COLOR.Name="LDB Color";
		i_LDS_COLOR.SetColor(255,0,0);
		return;
	}

	//Create Delta at Price
	//Currently hard-coding the deltaThreshold, but this should be based off the percentages
	// much like the NumberBars does when highlighting the DeltaAtPrice to have background change
	int deltaAtPrice = 0;
	float priceInPoints = 0;
	int deltaThreshold = i_DeltaThreshold.GetInt();
	
	int NumPriceLevels = sc.VolumeAtPriceForBars->GetSizeAtBarIndex(sc.IndexOfLastVisibleBar);
	for (int i=0; i<NumPriceLevels; i++){
		sc.VolumeAtPriceForBars->GetVAPElementAtIndex(sc.IndexOfLastVisibleBar, i, &p_VAP);

		//Calculate deltaAtPrice
  		deltaAtPrice = p_VAP->AskVolume - p_VAP->BidVolume; 

		if (abs(deltaAtPrice)>=deltaThreshold){
			//LDP criterea met
			priceInPoints = p_VAP->PriceInTicks/4.0;
			msg.Format("[%f] - %d x %d : %d", priceInPoints, p_VAP->BidVolume, p_VAP->AskVolume, deltaAtPrice);
			sc.AddMessageToLog(msg, 0);

			// Draw Horizantal Ray based on the entry of the LDP
			Draw.LineStyle = LINESTYLE_DOT;
			Draw.DrawingType = DRAWING_HORIZONTAL_RAY;
			Draw.LineWidth = 1;
			Draw.BeginValue = priceInPoints;
			Draw.BeginIndex = sc.IndexOfLastVisibleBar;
			// Draw.BeginDateTime = ; //For Horizantal Ray
			// Draw.EndDateTime = ; //For Horizantal Line Non-Extended
			Draw.AddMethod = UTAM_ADD_OR_ADJUST; 
			Draw.Color = i_LDB_COLOR.GetColor();
			if (deltaAtPrice<0){
				Draw.Color = i_LDS_COLOR.GetColor();
			}
			sc.UseTool(Draw);

		}
	}

}
