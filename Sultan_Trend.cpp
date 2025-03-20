#include "sierrachart.h"

SCDLLName("Sultan - SultanTrend")

SCSFExport scsf_SultanTrend(SCStudyInterfaceRef sc)
{
	// SCString msg;
	SCSubgraphRef Subgraph_SultanTrend = sc.Subgraph[0];
	SCSubgraphRef Subgraph_HullATR = sc.Subgraph[1];
	SCSubgraphRef Subgraph_TrueRange = sc.Subgraph[2];
	SCSubgraphRef Subgraph_AvgTrueRange = sc.Subgraph[3];
	SCSubgraphRef Subgraph_UpperBandBasic = sc.Subgraph[4];
	SCSubgraphRef Subgraph_LowerBandBasic = sc.Subgraph[5];
	SCSubgraphRef Subgraph_UpperBand = sc.Subgraph[6];
	SCSubgraphRef Subgraph_LowerBand = sc.Subgraph[7];

	SCFloatArrayRef Array_TrueRange = Subgraph_SultanTrend.Arrays[0];
	SCFloatArrayRef Array_AvgTrueRange = Subgraph_SultanTrend.Arrays[1];
	SCFloatArrayRef Array_UpperBandBasic = Subgraph_SultanTrend.Arrays[2];
	SCFloatArrayRef Array_LowerBandBasic = Subgraph_SultanTrend.Arrays[3];
	SCFloatArrayRef Array_UpperBand = Subgraph_SultanTrend.Arrays[4];
	SCFloatArrayRef Array_LowerBand = Subgraph_SultanTrend.Arrays[5];

	SCInputRef Input_InputData = sc.Input[0];
	SCInputRef Input_ATRMultiplier = sc.Input[1];
	SCInputRef Input_ATRPeriod = sc.Input[2];
	SCInputRef Input_ATRMovAvgType = sc.Input[3];

	if (sc.SetDefaults)
	{
		sc.GraphName = "SultanTrend";

		sc.StudyDescription = "";
		sc.DrawZeros = true;
		sc.GraphRegion = 0;
		sc.ValueFormat = sc.BaseGraphValueFormat;

		sc.AutoLoop = 1;

		Subgraph_SultanTrend.Name = "SultanTrend";
		Subgraph_SultanTrend.DrawStyle = DRAWSTYLE_LINE;
		Subgraph_SultanTrend.LineWidth = 1;
		Subgraph_SultanTrend.PrimaryColor = RGB(160, 32, 40);
		Subgraph_SultanTrend.SecondaryColor = RGB(0,80,160);
		Subgraph_SultanTrend.SecondaryColorUsed = 1;

		Subgraph_TrueRange.Name = "True Range";
		Subgraph_TrueRange.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_AvgTrueRange.Name = "Average True Range";
		Subgraph_AvgTrueRange.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_UpperBandBasic.Name = "Basic Upper Band";
		Subgraph_UpperBandBasic.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_LowerBandBasic.Name = "Basic Lower Band";
		Subgraph_LowerBandBasic.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_UpperBand.Name = "Upper Band";
		Subgraph_UpperBand.DrawStyle = DRAWSTYLE_IGNORE;

		Subgraph_LowerBand.Name = "Lower Band";
		Subgraph_LowerBand.DrawStyle = DRAWSTYLE_IGNORE;

		Input_InputData.Name = "Input Data";
		Input_InputData.SetInputDataIndex(SC_HL);

		Input_ATRMultiplier.Name = "ATR Multiplier";
		Input_ATRMultiplier.SetFloat(3);
		Input_ATRMultiplier.SetFloatLimits(0.000001f, (float)MAX_STUDY_LENGTH);

		Input_ATRPeriod.Name = "ATR Period";
		Input_ATRPeriod.SetInt(10);
		Input_ATRPeriod.SetIntLimits(1, MAX_STUDY_LENGTH);

		Input_ATRMovAvgType.Name = "ATR Moving Average Type";
		Input_ATRMovAvgType.SetCustomInputStrings("Exponential Moving Avg;Linear Regression Moving Avg;Simple Moving Avg;Weighted Moving Avg;Wilders Moving Avg;Simple Moving Avg SkipZeros;Smoothed Moving Avg;Hull Moving Avg");
		Input_ATRMovAvgType.SetCustomInputIndex(6);

		return;
	}

	// Calculate TR and ATR
	sc.TrueRange(sc.BaseDataIn, Array_TrueRange);

	if (Input_ATRMovAvgType.GetIndex() == 0)
		sc.ATR(sc.BaseDataIn, Array_TrueRange, Array_AvgTrueRange, sc.Index, Input_ATRPeriod.GetInt(), MOVAVGTYPE_EXPONENTIAL);
	else if (Input_ATRMovAvgType.GetIndex() == 1)
		sc.ATR(sc.BaseDataIn, Array_TrueRange, Array_AvgTrueRange, sc.Index, Input_ATRPeriod.GetInt(), MOVAVGTYPE_LINEARREGRESSION);
	else if (Input_ATRMovAvgType.GetIndex() == 2)
		sc.ATR(sc.BaseDataIn, Array_TrueRange, Array_AvgTrueRange, sc.Index, Input_ATRPeriod.GetInt(), MOVAVGTYPE_SIMPLE);
	else if (Input_ATRMovAvgType.GetIndex() == 3)
		sc.ATR(sc.BaseDataIn, Array_TrueRange, Array_AvgTrueRange, sc.Index, Input_ATRPeriod.GetInt(), MOVAVGTYPE_WEIGHTED);
	else if (Input_ATRMovAvgType.GetIndex() == 4)
		sc.ATR(sc.BaseDataIn, Array_TrueRange, Array_AvgTrueRange, sc.Index, Input_ATRPeriod.GetInt(), MOVAVGTYPE_WILDERS);
	else if (Input_ATRMovAvgType.GetIndex() == 5)
		sc.ATR(sc.BaseDataIn, Array_TrueRange, Array_AvgTrueRange, sc.Index, Input_ATRPeriod.GetInt(), MOVAVGTYPE_SIMPLE_SKIP_ZEROS);
	else if (Input_ATRMovAvgType.GetIndex() == 6)
		sc.ATR(sc.BaseDataIn, Array_TrueRange, Array_AvgTrueRange, sc.Index, Input_ATRPeriod.GetInt(), MOVAVGTYPE_SMOOTHED);
	else //HMA could be interesing to use
	{
		sc.HullMovingAverage(Array_TrueRange, Subgraph_HullATR, Input_ATRPeriod.GetInt());
		Array_AvgTrueRange[sc.Index] = Subgraph_HullATR[sc.Index];
	}

	// Calculate Basic Upper and Lower Bands
	float Price = sc.BaseDataIn[Input_InputData.GetInputDataIndex()][sc.Index];

	Array_UpperBandBasic[sc.Index] = Price + Input_ATRMultiplier.GetFloat() * Array_AvgTrueRange[sc.Index];
	Array_LowerBandBasic[sc.Index] = Price - Input_ATRMultiplier.GetFloat() * Array_AvgTrueRange[sc.Index];

	// Calculate Upper and Lower Bands
	if (Array_UpperBandBasic[sc.Index] < Array_UpperBand[sc.Index - 1] || sc.Low[sc.Index - 1] > Array_UpperBand[sc.Index - 1])
		Array_UpperBand[sc.Index] = Array_UpperBandBasic[sc.Index];
	else
		Array_UpperBand[sc.Index] = Array_UpperBand[sc.Index - 1];

	if (Array_LowerBandBasic[sc.Index] > Array_LowerBand[sc.Index - 1] || sc.High[sc.Index - 1] < Array_LowerBand[sc.Index - 1])
		Array_LowerBand[sc.Index] = Array_LowerBandBasic[sc.Index - 1];
	else
		Array_LowerBand[sc.Index] = Array_LowerBand[sc.Index - 1];

	// Extra Subgraphs
	Subgraph_TrueRange[sc.Index] = Array_TrueRange[sc.Index];
	Subgraph_AvgTrueRange[sc.Index] = Array_AvgTrueRange[sc.Index];
	Subgraph_UpperBandBasic[sc.Index] = Array_UpperBandBasic[sc.Index];
	Subgraph_LowerBandBasic[sc.Index] = Array_LowerBandBasic[sc.Index];
	Subgraph_UpperBand[sc.Index] = Array_UpperBand[sc.Index];
	Subgraph_LowerBand[sc.Index] = Array_LowerBand[sc.Index];

	// Calculate SultanTrend
	// This is I think where I need to make the changes, essentially only want the ST to flip if a full candle prints behind the ST
	if (sc.Index == 0)
		Subgraph_SultanTrend[sc.Index] = Array_UpperBand[sc.Index];

	if (Subgraph_SultanTrend[sc.Index - 1] == Array_UpperBand[sc.Index - 1] && sc.Close[sc.Index] < Array_UpperBand[sc.Index]){//Remain Short
		// msg.Format("Remain Short");
		// sc.AddMessageToLog(msg, 0);
		Subgraph_SultanTrend[sc.Index] = Array_UpperBand[sc.Index];
	}
	else if (Subgraph_SultanTrend[sc.Index - 1] == Array_UpperBand[sc.Index - 1] && sc.Low[sc.Index] > Array_UpperBand[sc.Index]){//Flip Long
		// msg.Format("Flip Long");
		// sc.AddMessageToLog(msg, 0);
		Subgraph_SultanTrend[sc.Index] = Array_LowerBand[sc.Index];
	}
	else if (Subgraph_SultanTrend[sc.Index - 1] == Array_LowerBand[sc.Index - 1] && sc.Close[sc.Index] > Array_LowerBand[sc.Index]){//Remain Long
		// msg.Format("Remain Long");
		// sc.AddMessageToLog(msg, 0);
		Subgraph_SultanTrend[sc.Index] = Array_LowerBand[sc.Index];
	}
	else if (Subgraph_SultanTrend[sc.Index - 1] == Array_LowerBand[sc.Index - 1] && sc.High[sc.Index] < Array_LowerBand[sc.Index]){ //Flip Short
		// msg.Format("Flip Short");
		// sc.AddMessageToLog(msg, 0);
		Subgraph_SultanTrend[sc.Index] = Array_UpperBand[sc.Index];
	}
	else{
		//Don't know why it is going in here, which is causing some issues with the studies
		// msg.Format("E");
		// sc.AddMessageToLog(msg, 0);
		Subgraph_SultanTrend[sc.Index] = Subgraph_SultanTrend[sc.Index - 1];
	}


	//Setting the color
	if (Subgraph_SultanTrend[sc.Index] == Array_UpperBand[sc.Index])
		Subgraph_SultanTrend.DataColor[sc.Index] = Subgraph_SultanTrend.PrimaryColor;
	else
		Subgraph_SultanTrend.DataColor[sc.Index] = Subgraph_SultanTrend.SecondaryColor;

}