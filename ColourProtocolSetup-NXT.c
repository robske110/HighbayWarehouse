#pragma config(Sensor, S1,COLOUR,sensorColorNxtNONE)
//Colour Sensor Interpretion
const short C_RED_MIN = 930;
const short C_RED_MAX = 1000;
const short C_GREEN_MIN = 875;
const short C_GREEN_MAX = 930; //890::RED
const short C_BLUE_MIN = 850;
const short C_BLUE_MAX = 880; //850::GREEN
const short C_ALL_MIN = 50;
const short C_ALL_MAX = 75;
typedef struct{
	short MIN;
	short MAX;
}C;
C C_RED;
C C_GREEN;
C C_BLUE;
C C_ALL;
bool  ExitProgramm = false;
//BOOLs
bool InitialCS;
//STRINGs
string CurrentC;
string LastDisplayMsg;
string LastDebugMsg;
string E_001 = "NULL";
//TASKs
task colourListener;
//OTHERs
word nRawValue;

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
		if(LastDisplayMsg != Msg)
		{
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
			}
		}
	}
	//debug("writeDisplay()END");
}

task main()
{
	displayCenteredTextLine(2, "FarbSensor laedt");
	//Struct Assingment
	C_RED.MIN = C_RED_MIN;
	C_RED.MAX = C_RED_MAX;
	C_GREEN.MIN = C_GREEN_MIN;
	C_GREEN.MAX = C_GREEN_MAX;
	C_BLUE.MIN = C_BLUE_MIN;
	C_BLUE.MAX = C_BLUE_MAX;
	C_ALL.MIN = C_ALL_MIN;
	C_ALL.MAX = C_ALL_MAX;
	InitialCS = true;
	SensorType[COLOUR] = sensorColorNxtNONE;
	while(InitialCS)
	{
		if (validColorSensor(COLOUR))
		{
			InitialCS = false;
		}
	}
	InitialCS = true;
	startTask(colourListener);
	while(ExitProgramm == false)
	{
		if (validColorSensor(COLOUR))
		{
			writeDisplay(true,CurrentC,2,1);
		}
		else
		{
			writeDisplay(true,"ERROR",2,1);
		}
	}
}
task colourListener()
{
	while (true)
	{
		wait1Msec(100);
		string TempString;
		nRawValue  = SensorRaw[COLOUR];
		TempString = nRawValue;
		writeDisplay(true, TempString, 6, 0);
		debug("colourListener","DEBUG",TempString);
		datalogAddValueWithTimeStamp(0,nRawValue);
		if(nRawValue > C_RED.MIN && nRawValue < C_RED.MAX)
		{
			CurrentC = "RED";
			debugOnChange("colourListener","DEBUG",CurrentC);
		}
		else
		{
			if(nRawValue > C_GREEN.MIN && nRawValue < C_GREEN.MAX)
			{
				CurrentC = "GREEN";
				debugOnChange("colourListener","DEBUG",CurrentC);
			}
			else
			{
				if(nRawValue > C_BLUE.MIN && nRawValue < C_BLUE.MAX)
				{
					CurrentC = "BLUE";
					debugOnChange("colourListener","DEBUG",CurrentC);
				}
				else
				{
					if(nRawValue > C_ALL.MIN && nRawValue < C_ALL.MAX)
					{
						CurrentC = "ALL";
						debugOnChange("colourListener","DEBUG",CurrentC);
					}
					else
					{
						CurrentC = "NULL";
						//debugOnChange("colourListener","DEBUG","NULL");
						E_001 = "GotUnknowValue";
					}
				}
			}
		}
	}
}
