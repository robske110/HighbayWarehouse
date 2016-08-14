#pragma config(Sensor,S3,COLOUR,sensorEV3_Color,modeEV3Color_Ambient)
task changeColour;
task colourListener;
string CurrentC;
bool InitialCS;
const int TickRate = 5000;
string LastDisplayMsg;

void debug(const string ToFIdentifier,const string Type , const string DebugString)
{
	writeDebugStream(ToFIdentifier);
	writeDebugStream(" <");
	writeDebugStream(Type);
	writeDebugStream(">: ");
	writeDebugStream(DebugString);
	writeDebugStreamLine("");
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
				nxtDisplayCenteredTextLine(Line, Msg);
				LastDisplayMsg = Msg;
			}
			if(Type == 1)
			{
				nxtDisplayCenteredBigTextLine(Line, Msg);
				LastDisplayMsg = Msg;
			}
			else
			{
				debug("writeDisplay","ERROR","InvalidTypewD");
			}
		}
	}
	//debug("writeDisplay()END");
}

task main()
{
	nxtDisplayCenteredTextLine(2, "FarbSensor laedt");
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
	startTask(changeColour);
	//startTask(colourListener);
	while(true)
	{
		if (validColorSensor(COLOUR))
		{
			writeDisplay(true,CurrentC,2,1);
		}
		else
		{
			writeDisplay(true,"Loading",2,1);
		}
	}
}

task colourListener()
{
	while (true)
	{
		wait1Msec(20);
		switch(SensorValue[COLOUR])
		{
		case BLACKCOLOR:  writeDisplay(true, "Black", 6, 0);   break;
		case BLUECOLOR:   writeDisplay(true, "Blue", 6, 0);    break;
		case GREENCOLOR:  writeDisplay(true, "Green", 6, 0);   break;
		case YELLOWCOLOR: writeDisplay(true, "Yellow", 6, 0);  break;
		case REDCOLOR:    writeDisplay(true, "Red", 6, 0);     break;
		case WHITECOLOR:  writeDisplay(true, "White", 6, 0);   break;
		default:          writeDisplay(true, "UNKNOWN", 6, 0); break;
		}
	}
}

task changeColour()
{
	while(true)
	{
		SensorType[COLOUR] = sensorColorNxtRED;
		CurrentC = "RED";
		debug("changeColour","DEBUG",CurrentC);
		wait1Msec(TickRate);
		SensorType[COLOUR] = sensorColorNxtGREEN;
		CurrentC = "GREEN";
		debug("changeColour","DEBUG",CurrentC);
		wait1Msec(TickRate);
		SensorType[COLOUR] = sensorColorNxtBLUE;
		CurrentC = "BLUE";
		debug("changeColour","DEBUG",CurrentC);
		wait1Msec(TickRate);
		SensorType[COLOUR] = sensorColorNxtFULL;
		CurrentC = "ALL";
		debug("changeColour","DEBUG",CurrentC);
		wait1Msec(TickRate);
	}
}
