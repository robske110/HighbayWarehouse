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
//12 defined, ~? used 24 possible
//0 = ALL
//1 = RED
//2 = BLUE
//3 = GREEN
//(-1=NULL)
typedef struct{
	short P1;
	short P2;
	short P3;
	short P4;
}ColourPattern;
ColourPattern CP1;
ColourPattern CP2;
ColourPattern CP3;
ColourPattern CP4;
ColourPattern CP5;
ColourPattern CP6;
ColourPattern CP7;
ColourPattern CP8;
ColourPattern CP9;
ColourPattern CP10;
ColourPattern CP11;
ColourPattern CP12;
//ColourPattern TEMP_PCPS; //For the initial creation of hashes!
//Settings
const short TickRate = 5000; //ColourChangeRate (HAS TO BE EQUAL TO EV3)<-NOPE, it's useless //INT::ChToShortOn_2113_18032016
const short SignalCount = 12; //6
const short SignalLength = 4;
bool  ExitProgramm = false;
//SHORTs
short ActualSignal;
short recieveBuffer[SignalLength];
short recieveBufferState;
//BOOLs
bool InitialCS;
bool Override_hCC = true;
//STRINGs
string CurrentC;
string LastDisplayMsg;
string LastDebugMsg;
string E_001 = "NULL";
string LastColour[2];
//TASKs
task changeColour;
task colourListener;
//OTHERs
int orgCHashes[SignalCount];
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

void error(const string Desc, const string ShortDesc, const string TaskFunction)
{
	writeDebugStream("[ERROR-REPORTER] ");
	debug(TaskFunction, "ERROR", Desc);
	writeDebugStream("[ERROR-REPORTER] ");
	writeDebugStreamLine("Listing Errors:");
	debug("[ERROR-REPORTER]", "ERR_1", E_001);
	displayCenteredBigTextLine(1, "ERROR");
	displayTextLine(3, "ShortDes: %s", ShortDesc);
	displayTextLine(4, "More Info -> DebugStream!");
	playSound(soundException);
	stopTask(colourListener);
	wait1Msec(10000);
	debug("[ERROR-REPORTER]", "WARNING", "5secEXIT");
	wait1Msec(5000);
	debug("[ERROR-REPORTER]", "WARNING", "WILL_EXIT_NOW");
	ExitProgramm = true;
}
void Coffset(const string TYPE)
{
	if(TYPE == "DAFUC"){
		C_RED.MIN = 885;
		C_RED.MAX = 905;
		C_GREEN.MIN = 850;
		C_GREEN.MAX = 870;
		C_BLUE.MIN = 800;
		C_BLUE.MAX = 825;
	}
	if(TYPE == "DAFUC-1"){
		C_RED.MIN = 885;
		C_RED.MAX = 905;
		C_GREEN.MIN = 850;
		C_GREEN.MAX = 870;
		C_BLUE.MIN = 800;
		C_BLUE.MAX = 825;
	}
}
int makeCPHash(short CValue1, short CValue2, short CValue3, short CValue4){
	int cHash;
	cHash = CValue1*10 + CValue2*100 + CValue3*1000 + CValue4*10000;
	return cHash;
}
//1 = RED
//2 = BLUE
//3 = GREEN
/*
void parseCPStructs(){
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
}
*/
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
	//Hacks
	C_RED.MIN = 885;
	C_RED.MAX = 910;
	C_GREEN.MIN = 850;
	C_GREEN.MAX = 870;
	C_BLUE.MIN = 800;
	C_BLUE.MAX = 825;
	//Coffset("DAFUC-1");
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
	//12 defined, ~? used 24 possible
	//1
	CP1.P1 	= 1;
	CP1.P2	= 2;
	CP1.P3 	= 1;
	CP1.P4 	= 2;
	//2
	CP2.P1 	= 1;
	CP2.P2	= 2;
	CP2.P3 	= 1;
	CP2.P4 	= 3;
	//3
	CP3.P1 	= 1;
	CP3.P2	= 2;
	CP3.P3 	= 3;
	CP3.P4 	= 1;
	//4
	CP4.P1 	= 1;
	CP4.P2	= 2;
	CP4.P3 	= 3;
	CP4.P4 	= 2;
	//5
	CP5.P1 	= 1;
	CP5.P2	= 3;
	CP5.P3 	= 1;
	CP5.P4 	= 3;
	//6
	CP6.P1 	= 1;
	CP6.P2	= 3;
	CP6.P3 	= 1;
	CP6.P4 	= 2;
	//7
	CP7.P1 	= 1;
	CP7.P2	= 3;
	CP7.P3 	= 2;
	CP7.P4 	= 1;
	//8
	CP8.P1 	= 1;
	CP8.P2	= 3;
	CP9.P3 	= 2;
	CP9.P4 	= 3;
	//9
	CP9.P1 	= 2;
	CP9.P2	= 1;
	CP9.P3 	= 2;
	CP9.P4 	= 1;
	//10
	CP10.P1 = 2;
	CP10.P2	= 1;
	CP10.P3 = 2;
	CP10.P4 = 3;
	//11
	CP11.P1 = 2;
	CP11.P2	= 1;
	CP11.P3 = 3;
	CP11.P4 = 1;
	//12
	CP12.P1 = 2;
	CP12.P2	= 1;
	CP12.P3 = 3;
	CP12.P4 = 2;
	//Make CPHashs
	short TEMP_PedCPS[SignalLength];
	for(int i = 1; i <= SignalCount; i++){
		switch(i){
		case 1: TEMP_PedCPS[0] = CP1.P1; TEMP_PedCPS[1] = CP1.P2; TEMP_PedCPS[2] = CP1.P3; TEMP_PedCPS[3] = CP1.P4;
			break;
		case 2: TEMP_PedCPS[0] = CP2.P1; TEMP_PedCPS[1] = CP2.P2; TEMP_PedCPS[2] = CP2.P3; TEMP_PedCPS[3] = CP2.P4;
			break;
		case 3: TEMP_PedCPS[0] = CP3.P1; TEMP_PedCPS[1] = CP3.P2; TEMP_PedCPS[2] = CP3.P3; TEMP_PedCPS[3] = CP3.P4;
			break;
		case 4: TEMP_PedCPS[0] = CP4.P1; TEMP_PedCPS[1] = CP4.P2; TEMP_PedCPS[2] = CP4.P3; TEMP_PedCPS[3] = CP4.P4;
			break;
		case 5: TEMP_PedCPS[0] = CP5.P1; TEMP_PedCPS[1] = CP5.P2; TEMP_PedCPS[2] = CP5.P3; TEMP_PedCPS[3] = CP5.P4;
			break;
		case 6: TEMP_PedCPS[0] = CP6.P1; TEMP_PedCPS[1] = CP6.P2; TEMP_PedCPS[2] = CP6.P3; TEMP_PedCPS[3] = CP6.P4;
			break;
		case 7: TEMP_PedCPS[0] = CP7.P1; TEMP_PedCPS[1] = CP7.P2; TEMP_PedCPS[2] = CP7.P3; TEMP_PedCPS[3] = CP7.P4;
			break;
		case 8: TEMP_PedCPS[0] = CP8.P1; TEMP_PedCPS[1] = CP8.P2; TEMP_PedCPS[2] = CP8.P3; TEMP_PedCPS[3] = CP8.P4;
			break;
		case 9: TEMP_PedCPS[0] = CP9.P1; TEMP_PedCPS[1] = CP9.P2; TEMP_PedCPS[2] = CP9.P3; TEMP_PedCPS[3] = CP9.P4;
			break;
		case 10: TEMP_PedCPS[0] = CP10.P1; TEMP_PedCPS[1] = CP10.P2; TEMP_PedCPS[2] = CP10.P3; TEMP_PedCPS[3] = CP10.P4;
			break;
		case 11: TEMP_PedCPS[0] = CP11.P1; TEMP_PedCPS[1] = CP11.P2; TEMP_PedCPS[2] = CP11.P3; TEMP_PedCPS[3] = CP11.P4;
			break;
		case 12: TEMP_PedCPS[0] = CP12.P1; TEMP_PedCPS[1] = CP12.P2; TEMP_PedCPS[2] = CP12.P3; TEMP_PedCPS[3] = CP12.P4;
			break;
		default: error("E_002","makeOrgCHashes","main::BootSequence");
			break;
		}
		//parseCPStructs();
		string TempSprintfString;
		sprintf(TempSprintfString,"iVal:%i",i);
		debug("main::BootSequence","DEBUG",TempSprintfString);
		orgCHashes[i-1] = makeCPHash(TEMP_PedCPS[0], TEMP_PedCPS[1], TEMP_PedCPS[2], TEMP_PedCPS[3]);
		sprintf(TempSprintfString,"CHash:%i",orgCHashes[i-1]);
		debug("main::BootSequence","DEBUG",TempSprintfString);
	}
	//startTask(changeColour);
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
		//debug("cL::hCC", "DEBUG", "CC==LC");
		hasChanged = false;
	}
	if(Override_hCC == true){
		//debug("cL::hCC", "DEBUG", "O_hCC=true");
		hasChanged = false;
	}
	return hasChanged;
}
short parseCBuffer(){
	short CurrentCBHash;
	CurrentCBHash = makeCPHash(recieveBuffer[0], recieveBuffer[1], recieveBuffer[2], recieveBuffer[3]);
	string TempSprintfString;
	sprintf(TempSprintfString,"CBHash: %i",CurrentCBHash);
	debug("cL::parseCBuffer", "DEBUG", TempSprintfString);
	for(int i = 1; i <= SignalCount; i++){
		if(CurrentCBHash == orgCHashes[i-1]){
			return i;
		}
	}
	return -1;
}
void broadcastSignal(){
	playSound(soundFastUpwardTones);
	string TempSprintfString;
	sprintf(TempSprintfString, "Signal: %i", ActualSignal);
	debug("cL::broadcastSignal", "INFO", TempSprintfString);
	//FutureInstructions
}
task colourListener()
{
	recieveBuffer[0] = -1;
	recieveBuffer[1] = -1;
	recieveBuffer[2] = -1;
	recieveBuffer[3] = -1;
	string TempString;
	while (true)
	{
		//0 = ALL
		//1 = RED
		//2 = BLUE
		//3 = GREEN
		//(-1=NULL)
		wait1Msec(100);
		nRawValue  = SensorRaw[COLOUR];
		TempString = nRawValue;
		writeDisplay(true, TempString, 6, 0);
		//ExperimentalColourFetching & Parsing
		if(recieveBuffer[0] != -1 && recieveBuffer[1] != -1 && recieveBuffer[2] != -1 && recieveBuffer[3] != -1)
		{
			for(int i = 0; i<=SignalLength-1; i++)
			{
				string TempSprintfString;
				sprintf(TempSprintfString,"Colour:%i",recieveBuffer[i]);
				debug("colourListener","DEBUG",TempSprintfString);
			}
			ActualSignal = parseCBuffer();
			if(ActualSignal != -1){
				broadcastSignal();
				}else{
				error("parseCBUnknownSig","E_003","coulorL::ParseSig");
			}
			recieveBuffer[0] = -1;
			recieveBuffer[1] = -1;
			recieveBuffer[2] = -1;
			recieveBuffer[3] = -1;
			recieveBufferState = 0;
			LastColour[0] = "";
			LastColour[1] = "";
			Override_hCC = true;
		}
		if(nRawValue > C_RED.MIN && nRawValue < C_RED.MAX)
		{
			wait1Msec(100);
			nRawValue  = SensorRaw[COLOUR];
			TempString = nRawValue;
			writeDisplay(true, TempString, 6, 0);
			if(nRawValue > C_RED.MIN && nRawValue < C_RED.MAX){
				CurrentC = "RED";
				broadcastRGBColourChange();
				LastDebugMsg = "";
				//debug("colourListener","DEBUG","RED_bef_hCC");
				if(hasColourChanged())
				{
					debug("colourListener","DEBUG",CurrentC);
					recieveBuffer[recieveBufferState] = 1;
					recieveBufferState++;
				}
			}
		}
		else
		{
			if(nRawValue > C_GREEN.MIN && nRawValue < C_GREEN.MAX)
			{
				wait1Msec(100);
				nRawValue  = SensorRaw[COLOUR];
				TempString = nRawValue;
				writeDisplay(true, TempString, 6, 0);
				if(nRawValue > C_GREEN.MIN && nRawValue < C_GREEN.MAX){
					CurrentC = "GREEN";
					broadcastRGBColourChange();
					LastDebugMsg = "";
					if(hasColourChanged())
					{
						debug("colourListener","DEBUG",CurrentC);
						recieveBuffer[recieveBufferState] = 3;
						recieveBufferState++;
					}
				}
			}
			else
			{
				if(nRawValue > C_BLUE.MIN && nRawValue < C_BLUE.MAX)
				{
					wait1Msec(100);
					nRawValue  = SensorRaw[COLOUR];
					TempString = nRawValue;
					writeDisplay(true, TempString, 6, 0);
					if(nRawValue > C_BLUE.MIN && nRawValue < C_BLUE.MAX){
						CurrentC = "BLUE";
						broadcastRGBColourChange();
						LastDebugMsg = "";
						if(hasColourChanged())
						{
							debug("colourListener","DEBUG",CurrentC);
							recieveBuffer[recieveBufferState] = 2;
							recieveBufferState++;
						}
					}
				}
				else
				{
					if(nRawValue > C_ALL.MIN && nRawValue < C_ALL.MAX)
					{
						CurrentC = "ALL";
						debugOnChange("colourListener","DEBUG",CurrentC);
						if(Override_hCC == true){
							Override_hCC = false;
							LastColour[0] = "";
							LastColour[1] = "";
						}
					}
					else
					{
						//CurrentC = "NULL";
						//debugOnChange("colourListener","DEBUG","NULL");
						E_001 = "GotUnknowValue";
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
