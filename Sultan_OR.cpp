// The top of every source code file must include this line
#include "sierrachart.h"


SCDLLName("Sultan Opening Range")
const SCString ContactInformation = "<a href = \"https://github.com/svltvn\">Sultan Github</a>";
/*
	Opening Range (OR) study that wil highlight the first 5 min of trade and will be extended until the next RTH open

	Study is based off of 2 other studies:
	- High/Low for Time Period - Extended -> OR High and Low
	- Study Subgraph Average -> OR_MID
*/
SCSFExport scsf_TemplateFunction(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_OR_H = sc.Subgraph[0];
	SCSubgraphRef Subgraph_OR_L = sc.Subgraph[1];
	SCSubgraphRef Subgraph_OR_MID = sc.Subgraph[2];
	SCSubgraphRef Subgraph_OR_BG_Top = sc.Subgraph[3];
	SCSubgraphRef Subgraph_OR_BG_Bottom = sc.Subgraph[4];

	SCInputRef Input_StartTime = sc.Input[0];
	SCInputRef Input_EndTime = sc.Input[1];
	SCInputRef Input_NumberDaysToCalculate = sc.Input[2];
	SCInputRef Input_Version = sc.Input[4];
	SCInputRef Input_LineStopTimeInput = sc.Input[5];
	SCInputRef Input_DisplayHighLowIncrementally = sc.Input[6];

	SCInputRef Input_InputDataHigh = sc.Input[7];
	SCInputRef Input_InputDataLow = sc.Input[8];
	SCInputRef Input_FridayEveningExtendsIntoSunday = sc.Input[9];

	if (sc.SetDefaults)
	{
		sc.GraphName = "Opening Range";
		sc.GraphRegion = 0;
		sc.AutoLoop = 0;
		
		Subgraph_OR_H.Name = "High";
		Subgraph_OR_H.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_OR_H.PrimaryColor = RGB(72,88,136);
		Subgraph_OR_H. DrawZeros = false;
		
		Subgraph_OR_L.Name = "Low";
		Subgraph_OR_L.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_OR_L.PrimaryColor = RGB(72,88,136);
		Subgraph_OR_L.DrawZeros = false;

		Subgraph_OR_MID.Name = "Midpoint";
		Subgraph_OR_MID.DrawStyle = DRAWSTYLE_DASH;
		Subgraph_OR_MID.PrimaryColor = RGB(32,255,144);
		Subgraph_OR_MID. DrawZeros = false;

		Subgraph_OR_BG_Top.Name = "Background";
		Subgraph_OR_BG_Top.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_BOTTOM;
		Subgraph_OR_BG_Top.PrimaryColor = RGB(32,40,64);
		Subgraph_OR_BG_Top. DrawZeros = false;

		Subgraph_OR_BG_Bottom.Name = "Background";
		Subgraph_OR_BG_Bottom.DrawStyle = DRAWSTYLE_TRANSPARENT_FILL_RECTANGLE_TOP;
		Subgraph_OR_BG_Bottom.PrimaryColor = RGB(32,40,64);
		Subgraph_OR_BG_Bottom. DrawZeros = false;
		
		Input_StartTime.Name = "Start Time";
		Input_StartTime.SetTime(HMS_TIME(9,30,0));

		Input_EndTime.Name = "End Time";
		Input_EndTime.SetTime(HMS_TIME(9,34,59));
	
		Input_NumberDaysToCalculate.Name= "Number Of Days To Calculate";
		Input_NumberDaysToCalculate.SetInt(120);

		Input_LineStopTimeInput.Name = "Line Stop Time";
		Input_LineStopTimeInput.SetTime(HMS_TIME(9,29,59));

		Input_DisplayHighLowIncrementally.Name = "Display High Low Incrementally";
		Input_DisplayHighLowIncrementally.SetYesNo(false);

		Input_InputDataHigh.Name = "Input Data High";
		Input_InputDataHigh.SetInputDataIndex(SC_HIGH);

		Input_InputDataLow.Name = "Input Data Low";
		Input_InputDataLow.SetInputDataIndex(SC_LOW);

		Input_FridayEveningExtendsIntoSunday.Name = "Friday Evening Extends Into Sunday";
		Input_FridayEveningExtendsIntoSunday.SetYesNo(false);

		Input_Version.SetInt(3);

		return;
	}

	if (Input_Version.GetInt() == 1)
	{
		Input_InputDataHigh.SetInputDataIndex(SC_HIGH);
		Input_InputDataLow.SetInputDataIndex(SC_LOW);

		Input_Version.SetInt(2);
	}

	if (Input_Version.GetInt() <= 2)
	{
		Input_LineStopTimeInput.SetTime(sc.EndTime1);
		Input_Version.SetInt(3);
	}

	if (Input_NumberDaysToCalculate.GetInt() <= 0)
		Input_NumberDaysToCalculate.SetInt(10000);
	
	float & HighOfPeriod = sc.GetPersistentFloatFast(1);
	float & LowOfPeriod = sc.GetPersistentFloatFast(2);

	const bool IsInputTimesReversed = Input_StartTime.GetTime() > Input_EndTime.GetTime();

	const bool EndTimeHasMilliseconds = Input_EndTime.GetDateTime().GetMillisecond() > 0;

	//Make sure the Line Stop Time is outside of the time range or at the end of the time range.
	if (IsInputTimesReversed)
	{
		if (Input_LineStopTimeInput.GetTime() >= Input_StartTime.GetTime()
			|| Input_LineStopTimeInput.GetTime() < Input_EndTime.GetTime())
		{
			Input_LineStopTimeInput.SetTime(Input_EndTime.GetTime());
		}
	}
	else
	{
		if (Input_LineStopTimeInput.GetTime() >= Input_StartTime.GetTime()
			&& Input_LineStopTimeInput.GetTime() < Input_EndTime.GetTime())
		{
			Input_LineStopTimeInput.SetTime(Input_EndTime.GetTime());
		}
	}

	SCDateTimeMS DaysToCalculateStartDateTime;
	DaysToCalculateStartDateTime.SetDate(sc.BaseDateTimeIn[sc.ArraySize - 1].GetDate());
	DaysToCalculateStartDateTime.SubtractDays(Input_NumberDaysToCalculate.GetInt() - 1);
	DaysToCalculateStartDateTime.SetTime(Input_StartTime.GetTime());

	if (IsInputTimesReversed)
	{
		if(sc.BaseDateTimeIn[sc.ArraySize - 1].GetTime() < Input_StartTime.GetTime())
			DaysToCalculateStartDateTime.SubtractDays(1);
	}
	

	int InitialCalculationIndex = 0;

	// Loop through chart bars starting at the Update Start Index
	for (int Index = sc.UpdateStartIndex; Index < sc.ArraySize; Index++ )
	{
		const SCDateTimeMS CurrentBarDateTime = sc.BaseDateTimeIn[Index];

		const SCDateTimeMS PreviousBarDateTime = sc.BaseDateTimeIn[max(0, Index - 1)];
		const SCDateTimeMS NextBarDateTime = sc.BaseDateTimeIn[Index + 1];

		SCDateTimeMS ResetTime;
		ResetTime.SetTime(Input_LineStopTimeInput.GetTime());

		bool NeedReset = 
			(PreviousBarDateTime.GetDate() == CurrentBarDateTime.GetDate()
				&& PreviousBarDateTime.GetTime() < ResetTime.GetTime()
				&& CurrentBarDateTime.GetTime() >= ResetTime.GetTime())
			|| (PreviousBarDateTime.GetDate() != CurrentBarDateTime.GetDate()
				&& PreviousBarDateTime.GetTime() < ResetTime.GetTime());


		NeedReset |= Index == InitialCalculationIndex;

		if (sc.BaseDateTimeIn[Index] < DaysToCalculateStartDateTime)
		{
			InitialCalculationIndex = Index + 1;
			continue;
		}

		SCDateTimeMS StartDateTime;
		SCDateTimeMS EndDateTime;

		StartDateTime =  CurrentBarDateTime.GetDate();
		EndDateTime = CurrentBarDateTime.GetDate();

		StartDateTime.SetTime(Input_StartTime.GetTime());
		EndDateTime.SetTime(Input_EndTime.GetTime());

		if (!EndTimeHasMilliseconds)
		{
			//To make EndDateTime at the end of the specified second.
			EndDateTime.AddSeconds(1);
			EndDateTime.SubtractMicroseconds(1);
		}

		if (IsInputTimesReversed)
		{
			if (CurrentBarDateTime.GetTimeInSeconds() < Input_StartTime.GetTime())
			{
				StartDateTime.SubtractDays(1);
			}
			else
			{
				EndDateTime.AddDays(1);
			}
		}

		if(IsInputTimesReversed && Input_FridayEveningExtendsIntoSunday.GetYesNo())
		{
			SCDateTime TradingDayDate(sc.GetTradingDayDate(CurrentBarDateTime));
			int DayOfWeek = TradingDayDate.GetDayOfWeek();
			if (DayOfWeek == MONDAY)
			{
				StartDateTime.SubtractDays(2);
				NeedReset = false;
			}
		}

		//reset
		if (NeedReset)
		{
			HighOfPeriod = -FLT_MAX;
			LowOfPeriod = FLT_MAX;		
		}


		bool OutsideTimeRange = true;

		bool IsCurrentBarContainingOrGreaterThanStartDateTime = 
			(CurrentBarDateTime < StartDateTime
				&& NextBarDateTime > StartDateTime)
			|| CurrentBarDateTime >= StartDateTime;

		if (IsCurrentBarContainingOrGreaterThanStartDateTime && CurrentBarDateTime <= EndDateTime)
		{
			OutsideTimeRange = false;

			if (HighOfPeriod < sc.BaseData[Input_InputDataHigh.GetInputDataIndex()][Index])
				HighOfPeriod = sc.BaseData[Input_InputDataHigh.GetInputDataIndex()][Index];

			if (LowOfPeriod > sc.BaseData[Input_InputDataLow.GetInputDataIndex()][Index])
				LowOfPeriod = sc.BaseData[Input_InputDataLow.GetInputDataIndex()][Index];
		}

		if (HighOfPeriod == -FLT_MAX)
			continue;

		// Set/update all values for current day
		int BackIndex = Index;

		while (true)
		{
			if(BackIndex < 0)
				break;

			const SCDateTimeMS BackIndexDateTime = sc.BaseDateTimeIn[BackIndex];
			const SCDateTimeMS NextBackIndexDateTime = sc.BaseDateTimeIn[BackIndex+1];

			bool IsCurrentBarContainingOrGreaterThanStartDateTime =
				(BackIndexDateTime < StartDateTime
					&& NextBackIndexDateTime > StartDateTime)
				|| BackIndexDateTime >= StartDateTime;

			if (!OutsideTimeRange && !IsCurrentBarContainingOrGreaterThanStartDateTime)
				break;

			Subgraph_OR_H[BackIndex] = HighOfPeriod;
			Subgraph_OR_L[BackIndex] = LowOfPeriod;
			Subgraph_OR_MID[BackIndex] = (HighOfPeriod+LowOfPeriod)/2;
			Subgraph_OR_BG_Top[BackIndex] = HighOfPeriod;
			Subgraph_OR_BG_Bottom[BackIndex] = LowOfPeriod;

			if (OutsideTimeRange || Input_DisplayHighLowIncrementally.GetYesNo())
				break;

			BackIndex--;

			if(sc.UpdateStartIndex != 0 && BackIndex  >= 0)
				sc.EarliestUpdateSubgraphDataArrayIndex = BackIndex;
		}
	}

}
