#pragma config(Sensor, S1,     COLOUR,         sensorColorNxtNONE)
//STRINGs
string CurrentC;
string LastDisplayMsg;
string LastDebugMsg;
string E_001 = "NULL";
//TASKs
task colourSender;
//OTHERs
bool  ExitProgramm = false;

void debug(const string ToFIdentifier,const string Type , const string DebugString)
{
	writeDebugStream(ToFIdentifier);
	writeDebugStream(" <");
	writeDebugStream(Type);
	writeDebugStream(">: ");
	writeDebugStream(DebugString);
	writeDebugStreamLine("");
}

void debugOnChange(const string ToFIdentifier,const string Type , const string DebugString)
{
	if(LastDebugMsg != DebugString)
	{
		LastDebugMsg = DebugString;
		writeDebugStream(ToFIdentifier);
		writeDebugStream(" <");
		writeDebugStream(Type);
		writeDebugStream(">: ");
		writeDebugStream(DebugString);
		writeDebugStreamLine("");
	}
}

void writeDisplay(bool DoExecute,const string Msg,short Line,short Type)
{
	//debug("writeDisplay()");
	if(DoExecute == true)
	{
		//if(LastDisplayMsg != Msg)
		//{
			if(Type == 0)
			{
				displayCenteredTextLine(Line, Msg);
				LastDisplayMsg = Msg;
				}else{
				if(Type == 1)
				{
					displayCenteredBigTextLine(Line, Msg);
					LastDisplayMsg = Msg;
					}else{
					debug("writeDisplay","ERROR","InvalidTypewD");
				}
			//}
		}
	}
	//debug("writeDisplay()END");
}

task main()
{
	displayCenteredTextLine(2, "FarbSensor laedt");
	SensorType[COLOUR] = sensorColorNxtNONE;
	wait1Msec(1000);
	startTask(colourSender);
	while(ExitProgramm == false)
	{
	}
}

task colourSender()
{
	while(true){
		SensorType[COLOUR] = sensorColorNxtRED;
		CurrentC = "RED";
		writeDisplay(true,CurrentC,2,1);
		debug("changeColour","DEBUG",CurrentC);
		wait1Msec(1000);
		while(!getButtonPress(buttonRight)){}
		SensorType[COLOUR] = sensorColorNxtGREEN;
		CurrentC = "GREEN";
		writeDisplay(true,CurrentC,2,1);
		debug("changeColour","DEBUG",CurrentC);
		wait1Msec(2000);
		while(!getButtonPress(buttonRight)){}
		SensorType[COLOUR] = sensorColorNxtBLUE;
		CurrentC = "BLUE";
		writeDisplay(true,CurrentC,2,1);
		debug("changeColour","DEBUG",CurrentC);
		wait1Msec(1000);
		while(!getButtonPress(buttonRight)){}
		SensorType[COLOUR] = sensorColorNxtFULL;
		CurrentC = "ALL";
		writeDisplay(true,CurrentC,2,1);
		debug("changeColour","DEBUG",CurrentC);
		wait1Msec(1000);
		while(!getButtonPress(buttonRight)){}
	}
}
