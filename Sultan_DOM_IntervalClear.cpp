#include "sierrachart.h"

SCDLLName("Sultan - DOM Interval Clear")
const SCString ContactInformation = "<a href = \"https://github.com/svltvn\">Sultan Github</a>";
/*
	This is a study that is based off of gcUserStudies:https://github.com/gcUserStudies/sierrachart/blob/main/gcUserStudies_2500.cpp.
		- scsf_gcAutoClearRecentBidAskVolume	
	Study clears the Recent/Current Bid/Ask Volume at a set interval (bar period)

*/

SCSFExport scsf_IntervalClear(SCStudyInterfaceRef sc)
{
	SCString msg;
	SCInputRef Input_RecentIntervalClearEnabled = sc.Input[0]; // Yes or No
	SCInputRef Input_CurrentIntervalClearEnabled  = sc.Input[1]; //Yes or No

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults
		sc.GraphName = "DOM Interval Clear";
		SCString StudyDescription;
		StudyDescription.Format("<b>%s</b> by %s", sc.GraphName.GetChars(), ContactInformation.GetChars());
		sc.StudyDescription = StudyDescription.AppendFormat("<br><br>Study clears the Recent/Current Bid/Ask Volume at a set interval (bar period)");
		sc.AutoLoop = 0;
		sc.GraphRegion = 0;

		Input_RecentIntervalClearEnabled.Name = "Clear Recent Bid/Ask Volume Enabled";
		Input_RecentIntervalClearEnabled.SetDescription("Clear Recent Bid/Ask Volume Columns");
		Input_RecentIntervalClearEnabled.SetYesNo(1);

		Input_CurrentIntervalClearEnabled.Name = "Clear Current Bid/Ask Volume Enabled";
		Input_CurrentIntervalClearEnabled.SetDescription("Clear Current Bid/Ask Volume Columns");
		Input_CurrentIntervalClearEnabled.SetYesNo(1);

		return;
	}

	bool RecentClearEnabled = Input_RecentIntervalClearEnabled.GetYesNo();
	bool CurrentClearEnabled = Input_CurrentIntervalClearEnabled.GetYesNo();
	int& PriorArraySize = sc.GetPersistentInt(1);

	//https://www.sierrachart.com/index.php?page=doc/ACSILProgrammingConcepts.html#DetectingNewBarsAddedToChart
	if (sc.Index == 0) //Same Bar
	{
		// msg.Format("Same Bar");
		// sc.AddMessageToLog(msg, 0);
		PriorArraySize = sc.ArraySize;
	}

	if (PriorArraySize < sc.ArraySize) //New Bar Added
	{
		// msg.Format("New Bar");
		// sc.AddMessageToLog(msg, 0);
		if (RecentClearEnabled){
			sc.ClearRecentBidAskVolume();
		}
		if (CurrentClearEnabled){
			sc.ClearCurrentTradedBidAskVolume();
		}
	}

	PriorArraySize = sc.ArraySize;
}
