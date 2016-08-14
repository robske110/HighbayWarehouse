#pragma config(Sensor, S1,COLOUR,sensorColorNxtNONE)
//Colour Sensor Interpretion
const short C_RED_MIN = 880;
const short C_RED_MAX = 900;
const short C_GREEN_MIN = 850;
const short C_GREEN_MAX = 870;
const short C_BLUE_MIN = 780;
const short C_BLUE_MAX = 805;
const short C_ALL_MIN = 55;
const short C_ALL_MAX = 75;
typedef struct{
	short MIN;
	short MAX;
}C;
C C_RED;
C C_GREEN;
C C_BLUE;
C C_ALL;
//Colour Patterns (Each of them is representing an signal)
//6 defined, ~5 used 6 possible TODO::4Length (4*3*2*1) instead of (3*2*1)
//0 = ALL
//1 = RED
//2 = BLUE
//3 = GREEN
//(-1=NULL)
typedef struct{
	short RED;
	short BLUE;
	short GREEN;
}ColourPattern;
ColourPattern CP1;
ColourPattern CP2;
ColourPattern CP3;
ColourPattern CP4;
ColourPattern CP5;
ColourPattern CP6;
ColourPattern TEMP_PCPS; //For the initial creation of hashes!
//Settings
const short TickRate = 5000; //ColourChangeRate (HAS TO BE EQUAL TO EV3) //INT::ChToShortOn_2113_18032016
const short SignalCount = 6; //12
const short SignalLength = 4;
//SHORTs
short TEMP_PedCPS[3];
short orgCHashes[SignalCount];
short ActualSignal;
short recieveBuffer[3];
short recieveBufferState;
//BOOLs
bool InitialCS;
//STRINGs
string CurrentC;
string LastDisplayMsg;
string LastDebugMsg;
string E_001 = "NULL";
//TASKs
task changeColour;
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

void error(const string Desc, const string ShortDesc, const string TaskFunction)
{
	writeDebugStream("[ERROR-REPORTER] ");
	debug(TaskFunction, "ERROR", Desc);
	writeDebugStream("[ERROR-REPORTER] ");
	debug("error()", "ERROR", "Listing errors:");
	debug("[ERROR-REPORTER]", "C::NULL", E_001);
	displayCenteredBigTextLine(1, "ERROR");
	displayTextLine(3, "ShortDes: %s", ShortDesc);
	displayTextLine(4, "Please consider the DebugStream!");
	playSound(soundException);
	wait1Msec(10000);
	debug("[ERROR-REPORTER]", "WARNING", "5secSHUTDOWN");
	wait1Msec(5000);
	debug("[ERROR-REPORTER]", "WARNING", "WILL_SHUTDOWN_NOW");
	powerOff();
}
void Coffset(const string TYPE)
{
	if(TYPE == "DAFUC"){
	C_RED.MIN = 905;
	C_RED.MAX = 920;
	C_GREEN.MIN = 865;
	C_GREEN.MAX = 880;
	C_BLUE.MIN = 815;
	C_BLUE.MAX = 835;
	}
	if(TYPE == "DAFUC-1"){
	C_RED.MIN = 885;
	C_RED.MAX = 905;
	C_GREEN.MIN = 845;
	C_GREEN.MAX = 860;
	C_BLUE.MIN = 800;
	C_BLUE.MAX = 825;
  }
}
short makeCPHash(short CValue1, short CValue2, short CValue3){
	short cHash;
	cHash = CValue1*10 + CValue2*100 + CValue3*1000;
	return cHash;
}
//1 = RED
//2 = BLUE
//3 = GREEN
short parseCPStructs(){
	if(TEMP_PCPS.RED == 1){
		TEMP_PedCPS[0] = 1;
	}else{
		if(TEMP_PCPS.RED == 2){
			TEMP_PedCPS[1] = 1;
		}else{
			if(TEMP_PCPS.RED == 3){
				TEMP_PedCPS[2] = 1;
			}
		}
	}
	if(TEMP_PCPS.GREEN == 1){
		TEMP_PedCPS[0] = 3;
	}else{
		if(TEMP_PCPS.GREEN == 2){
			TEMP_PedCPS[1] = 3;
		}else{
			if(TEMP_PCPS.GREEN == 3){
				TEMP_PedCPS[2] = 3;
			}
		}
	}
	if(TEMP_PCPS.BLUE == 1){
		TEMP_PedCPS[0] = 2;
	}else{
		if(TEMP_PCPS.BLUE == 2){
			TEMP_PedCPS[1] = 2;
		}else{
			if(TEMP_PCPS.BLUE == 3){
				TEMP_PedCPS[2] = 2;
			}
		}
	}
	short Legacy = -1;
	return Legacy;
}
task main()
{
	nxtDisplayCenteredTextLine(2, "FarbSensor laedt");
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
	//Hacks
	Coffset("DAFUC-1");
	SensorType[COLOUR] = sensorColorNxtNONE;
	while(InitialCS)
	{
		if (validColorSensor(COLOUR))
		{
			InitialCS = false;
		}
	}
	InitialCS = true;
	//Construct ColourPatterns
	//1
	CP1.RED 	= 1;
	CP1.GREEN = 2;
	CP1.BLUE 	= 3;
	//2
	CP2.RED 	= 1;
	CP2.GREEN = 3;
	CP2.BLUE 	= 2;
	//3
	CP3.RED 	= 2;
	CP3.GREEN = 1;
	CP3.BLUE	= 3;
	//4
	CP4.RED 	= 3;
	CP4.GREEN = 1;
	CP4.BLUE  = 2;
	//5
	CP5.RED   = 2;
	CP5.GREEN = 3;
	CP5.BLUE  = 1;
	//6
	CP6.RED   = 3;
	CP6.GREEN = 2;
	CP6.BLUE  = 1;
	//Make CPHashs
	for(int i = 1; i <= SignalCount; i++){
		switch(i){
			case 1: TEMP_PCPS.RED = CP1.RED; TEMP_PCPS.BLUE = CP1.BLUE; TEMP_PCPS.GREEN = CP1.GREEN;
			break;
			case 2: TEMP_PCPS.RED = CP2.RED; TEMP_PCPS.BLUE = CP2.BLUE; TEMP_PCPS.GREEN = CP2.GREEN;
			break;
			case 3: TEMP_PCPS.RED = CP3.RED; TEMP_PCPS.BLUE = CP3.BLUE; TEMP_PCPS.GREEN = CP3.GREEN;
			break;
			case 4: TEMP_PCPS.RED = CP4.RED; TEMP_PCPS.BLUE = CP4.BLUE; TEMP_PCPS.GREEN = CP4.GREEN;
			break;
			case 5: TEMP_PCPS.RED = CP5.RED; TEMP_PCPS.BLUE = CP5.BLUE; TEMP_PCPS.GREEN = CP5.GREEN;
			break;
			case 6: TEMP_PCPS.RED = CP6.RED; TEMP_PCPS.BLUE = CP6.BLUE; TEMP_PCPS.GREEN = CP6.GREEN;
			break;
			default: error("E_84743","makeOrgCHashes","main::BootSequence");
			break;
		}
		parseCPStructs();
		string TempSprintfString;
		sprintf(TempSprintfString,"iVal:%i",i);
		debug("main::BootSequence","DEBUG",TempSprintfString);
		orgCHashes[i-1] = makeCPHash(TEMP_PedCPS[0], TEMP_PedCPS[1], TEMP_PedCPS[2]);
	}
	//startTask(changeColour);
	startTask(colourListener);
	while(true)
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
void broadcastRGBColourChange()
{
	LastColour[0] = LastColour[1];
	LastColour[1] = CurrentC;
}
bool hasColourChanged()
{
	bool hasChanged = true;
	if(CurrentC == LastColour[0])
	{
		hasChanged = false;
	}
	return hasChanged;
}
short parseCBuffer(){
		short CurrentCBHash;
		CurrentCBHash = makeCPHash(recieveBuffer[0], recieveBuffer[1], recieveBuffer[2]);
		for(int i = 1; i <= SignalCount; i++){
			if(CurrentCBHash == orgCHashes[i-1]){
				return i;
			}
		}
		return -1;
}
void broadcastSignal(){
	playSound(soundFastUpwardTones);
	string YeY;
	sprintf(YeY, "Signal: %i", ActualSignal);
	debug("cL::ParseSig::brSig", "INFO", YeY);
	//FutureInstructions
}
task colourListener()
{
	recieveBuffer[0] = -1;
	recieveBuffer[1] = -1;
	recieveBuffer[2] = -1;
	string TempString;
	while (true)
	{
		//0 = ALL
		//1 = RED
		//2 = BLUE
		//3 = GREEN
		//(-1=NULL)
		wait1Msec(50);
		nRawValue     = SensorRaw[COLOUR];
		TempString = nRawValue;
		writeDisplay(true, TempString, 6, 0);
		//ExperimentalColourFetching & Parsing
		if(recieveBuffer[0] != -1 && recieveBuffer[1] != -1 && recieveBuffer[2] != -1)
		{
			for(int i = 0; i<=2; i++)
			{
				string TempSCRAP;
				sprintf(TempSCRAP,"Farbe: %i",recieveBuffer[i]);
				debug("colourListener","DEBUG",TempSCRAP);
			}
			ActualSignal = parseCBuffer();
			if(ActualSignal != -1){
				broadcastSignal();
			}else{
				error("E_42857","parseCBfailed","coulorL::ParseSig");
			}
			recieveBuffer[0] = -1;
			recieveBuffer[1] = -1;
			recieveBuffer[2] = -1;
			recieveBufferState = 0;
			LastColour[0] = "";
			LastColour[1] = "";
		}
		if(nRawValue > C_RED.MIN && nRawValue < C_RED.MAX)
		{
			CurrentC = "RED";
			broadcastRGBColourChange();
			debugOnChange("colourListener","DEBUG",CurrentC);
			if(hasColourChanged())
			{
				recieveBuffer[recieveBufferState] = 1;
				recieveBufferState++;
			}
		}
		else
		{
			if(nRawValue > C_GREEN.MIN && nRawValue < C_GREEN.MAX)
			{
				CurrentC = "GREEN";
				broadcastRGBColourChange();
				debugOnChange("colourListener","DEBUG",CurrentC);
				if(hasColourChanged())
				{
					recieveBuffer[recieveBufferState] = 2;
					recieveBufferState++;
				}
			}
			else
			{
				if(nRawValue > C_BLUE.MIN && nRawValue < C_BLUE.MAX)
				{
					CurrentC = "BLUE";
					broadcastRGBColourChange();
					debugOnChange("colourListener","DEBUG",CurrentC);
					if(hasColourChanged())
					{
						recieveBuffer[recieveBufferState] = 3;
						recieveBufferState++;
					}
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
						debugOnChange("colourListener","DEBUG","NULL");
						E_001 = "GotUnknowValue";
						//error("ERR_ColorIsNULL","E_C=N","colourListener");
					}
				}
			}
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
