#pragma config(Sensor, S1,							COLOUR,			sensorColorNxtNONE)
#pragma config(Sensor, S2,     					HORIZONTAL,    sensorSONAR)
#pragma config(Sensor, S3,     					VERTICALsensor,    		 sensorTouch)
#pragma config(Sensor, S4,     					HORIZONTAL_T,    		 sensorTouch)
#pragma config(Motor,  motorA,          VERTICAL,      tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          HORIZONTAL,    tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          FETCHER,       tmotorNXT, PIDControl, encoder)
//##################################################################################
string DATABASEREADOUTVARintNAME[6];
//string DATABASEREADOUTVARstringNAME[6];
int LastState;
//##################################################################################
const string sFileName = "Data.txt";   //MAIN_FILENAME
const byte 	 CR = 0x13;   //define CR (carriage return)
const byte   LF = 0x10;   //define LF (line feed)
const short  DATABASElength = 6; //DataBaseLenreadgth
task  readSensors;
task checkButtons;
//##################################################################################
TFileHandle   hFileHandle;              // "Keeps track of the file"
TFileIOResult nIOResult; 								// Stores "IO results"
char          incommingChar;                          //SaveIncommingChar(DECODED)
string        StringsFromFile[DATABASElength];        //StoringStuff
short         LINE = 0;									//Internal Use
short         nFileSize = 200;          // FileSize
//const int      FileSize = 75;          // FileSize_I
short 				HORIZONTALSensor = 0;
//##################################################################################
//Colour Sensor Interpretion [MIN_DIFF=10]
const short C_RED_MIN = 930;
const short C_RED_MAX = 1000;
const short C_GREEN_MIN = 875;
const short C_GREEN_MAX = 930; //890::RED
const short C_BLUE_MIN = 850;
const short C_BLUE_MAX = 880; //850::GREEN
const short C_ALL_MIN = 50;
const short C_ALL_MAX = 75;
/*
const short C_RED_MIN = 890;
const short C_RED_MAX = 925;
const short C_GREEN_MIN = 850;
const short C_GREEN_MAX = 885; //890::RED
const short C_BLUE_MIN = 815;
const short C_BLUE_MAX = 840; //850::GREEN
const short C_ALL_MIN = 50;
const short C_ALL_MAX = 75;
*/
//##################################################################################
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
//##################################################################################
//Settings
const short SignalCount = 12; //6
const short SignalLength = 4;
bool 			  ExitProgramm = false;
const short MaxMovementQueue = 10;
//Signals for MainMovement
const short STYPE__1  = 2; //TYPE1::1.A
const short STYPE__2  = 3; //TYPE1::1.A
const short Signal_V  = 4; //TYPE1::2.E
const short Signal_H  = 5; //TYPE1::2.E
const short SgC       = 6; //TYPE1::2.E
const short SpC       = 7; //TYPE1::2.E
const short SresetA   = 8; //TYPE2::1.A+E || THIS HAS (currently) NO STYPE BEFORE it, -1 in TMQ.
//SHORTs
short ActualSignal;
short recieveBuffer[SignalLength];
short recieveBufferState;
short BasicMovementQueue[MaxMovementQueue];
short TypeMovementQueue[MaxMovementQueue];
short CurrentMQOffset = -1;
//BOOLs
bool InitialCS;
bool Override_hCC = true;
bool FullyBooted = false;
//STRINGs
string CurrentC;
string LastDisplayMsg;
string LastDebugMsg;
string E_001 = "NULL";
string LastColour[2];
//TASKs
//task changeColour;
task colourListener;
//OTHERs
int orgCHashes[SignalCount];
word nRawValue;

//############################################################################################################
/*_    _ _______ _____ _       _____
#| |  | |__   __|_   _| |     / ____|
#| |  | |  | |    | | | |    | (___
#| |  | |  | |    | | | |     \___ \
#| |__| |  | |   _| |_| |____ ____) |
##\____/   |_|  |_____|______|_____/
*/
void valueDebug(const string ToFIdentifier, const string DebugString)
{
	//if(debugLevel == "ALL")
	{
		writeDebugStream(ToFIdentifier);
		writeDebugStream(" ");
		writeDebugStream(DebugString);
		writeDebugStreamLine("");
	}
}
void debug(const string ToFIdentifier,const string DebugType , const string DebugString)
{
	//if(debugLevel == "ALL" || debugLevel == "DEBUG" && Type == "DEBUG" || debugLevel == "DEBUG" && Type == "INFO" || Type == "INFO" || Type == "ERROR")
	{
		string TimeStamp1;
		string TimeStamp2;
		sprintf(TimeStamp1, "SYS:%i_", nSysTime);
		sprintf(TimeStamp2, "PGM:%i ", nPgmTime);
		writeDebugStream(TimeStamp1);
		writeDebugStream(TimeStamp2);
		if(DebugType == "CRITICAL" || DebugType == "ERROR"){
			playSound(soundException);
		}
		writeDebugStream(ToFIdentifier);
		writeDebugStream(" <");
		writeDebugStream(DebugType);
		writeDebugStream(">: ");
		writeDebugStream(DebugString);
		writeDebugStreamLine("");
	}
}

void debugOnChange(const string ToFIdentifier,const string Type , const string DebugString)
{
	if(LastDebugMsg != DebugString)
	{
		LastDebugMsg = DebugString;
		string TimeStamp1;
		string TimeStamp2;
		sprintf(TimeStamp1, "SYS:%i_", nSysTime);
		sprintf(TimeStamp2, "PGM:%i ", nPgmTime);
		writeDebugStream(TimeStamp1);
		writeDebugStream(TimeStamp2);
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

void error(const string Desc, const string ShortDesc, const string TaskFunction, const bool CRITICAL = false)
{
	string TimeStamp1;
	string TimeStamp2;
	sprintf(TimeStamp1, "SYS:%i ", nSysTime);
	sprintf(TimeStamp2, "PGM:%i", nPgmTime);
	writeDebugStream(TimeStamp1);
	writeDebugStream(TimeStamp2);
	writeDebugStreamLine("");
	writeDebugStream("[ERROR-REPORTER] ");
	debug(TaskFunction, "ERROR", Desc);
	writeDebugStream("[ERROR-REPORTER] ");
	debug(TaskFunction, "ERROR", ShortDesc);
	//writeDebugStreamLine("Listing Errors:");
	//debug("[ERROR-REPORTER]", "ERR_1", E_001);
	displayCenteredBigTextLine(1, "ERROR");
	displayTextLine(3, "ShortDes:%s", ShortDesc);
	displayTextLine(4, "More Info->DebugStream!");
	playSound(soundException);playSound(soundException);
	playSound(soundException);playSound(soundException);
	playSound(soundException);playSound(soundException);
	playSound(soundException);playSound(soundException);
	wait1Msec(1000);
	if(CRITICAL == true){
		playSound(soundException);
		if(TaskFunction != "colourListener" && TaskFunction != "colourListener::cDetect" && TaskFunction != "coulorL::ParseSig"){
			stopTask(colourListener);
		}
		writeDebugStream("[ERROR-REPORTER] ");
		debug(TaskFunction, "CRITICAL_ERROR", "CriticalError::sT1");
		setUserExceptionType1();
		debug("[ERROR-REPORTER]", "WARNING", "WILL_EXIT_NOW");
		ExitProgramm = true;
	}
}

//############################################################################################################
//_COLOURPROTOCOL
/*
###_____      _                  _____           _                  _
##/ ____|    | |                |  __ \         | |                | |
#| |     ___ | | ___  _   _ _ __| |__) | __ ___ | |_ ___   ___ ___ | |
#| |    / _ \| |/ _ \| | | | '__|  ___/ '__/ _ \| __/ _ \ / __/ _ \| |
#| |___| (_) | | (_) | |_| | |  | |   | | | (_) | || (_) | (_| (_) | |
##\_____\___/|_|\___/ \__,_|_|  |_|   |_|  \___/ \__\___/ \___\___/|_|
*/
int makeCPHash(short CValue1, short CValue2, short CValue3, short CValue4){
	int cHash;
	cHash = CValue1*10 + CValue2*100 + CValue3*1000 + CValue4*10000;
	return cHash;
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
//##################################################################################
void broadcastSignal(){
	playSound(soundFastUpwardTones);
	string TempSprintfString;
	sprintf(TempSprintfString, "Signal: %i", ActualSignal);
	debug("cL::broadcastSignal", "INFO", TempSprintfString);
	if(ActualSignal == Signal_V || ActualSignal == Signal_H ||
		ActualSignal == SgC || ActualSignal == SpC){
		if(CurrentMQOffset >= MaxMovementQueue){
			error("errCMQOexceedMMQ","E_012","broadcastSignal::cS_addMQ"); //And again, if this is seen CP has raged out,probaly bcause of CBuffClE or EV3QOW
			}else{
			BasicMovementQueue[CurrentMQOffset] = ActualSignal;
		}
		}else{
		if(ActualSignal == STYPE__1 || ActualSignal == STYPE__2){
			CurrentMQOffset++;
			TypeMovementQueue[CurrentMQOffset] = ActualSignal;
			}else{
			if(ActualSignal == SresetA){
				CurrentMQOffset++;
				BasicMovementQueue[CurrentMQOffset] = ActualSignal;
				TypeMovementQueue[CurrentMQOffset] = -1; //useless? Maybe.
				}else{
				error("broadcastSigUnusedSig","W_005","broadcastSig::checkSig", false);
				//Other Signals [1,...]
			}
		}
	}
}
//##################################################################################
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
		//_DATALOG
		//DATALOG MAPPING:
		//0 = EVERYTHING 1 = RED 2 = GREEN 3 = BLUE 4 = ALL
		datalogAddValueWithTimeStamp(0,nRawValue);
		writeDisplay(true, TempString, 5, 0);
		//ExperimentalColourFetching & Parsing (That's still experimental? In the final developments steps?)
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
				error("parseCBUnknownSig","E_003","coulorL::ParseSig", true); //CRITICAL::error
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
				if(!FullyBooted){
					error("101: FirstChasToBeALL", "ERR_101", "colourListener::cDetect", true); //CRITICAL::error
					return;
				}
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
					if(!FullyBooted){
						error("102: FirstChasToBeALL", "ERR_102", "colourListener::cDetect", true); //CRITICAL::error
						return;
					}
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
						if(!FullyBooted){
							error("103: FirstChasToBeALL", "ERR_103", "colourListener::cDetect", true); //CRITICAL::error
							return;
						}
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
						if(!FullyBooted){
							FullyBooted = true;
						}
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
/*
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
*/
//#############################################################################################################
//_DATABASE
/*
#_____        _        ____
|  __ \      | |      |  _ \
| |  | | __ _| |_ __ _| |_) | __ _ ___  ___
| |  | |/ _` | __/ _` |  _ < / _` / __|/ _ \
| |__| | (_| | || (_| | |_) | (_| \__ \  __/
|_____/ \__,_|\__\__,_|____/ \__,_|___/\___|
*/
void ReadDataBase(bool DoExecute)
{
	if(DoExecute==true)
	{
		OpenRead(hFileHandle, nIOResult, sFileName, nFileSize);     //OpenFile (DoGetIfNotExist->CANCELwithCritical)

		for(int i = 0; i < nFileSize; i++) //Loop noch zu meinem CodeStyle...
		{
			ReadByte(hFileHandle, nIOResult, incommingChar);            //READ_BYTE(ifNew)

			if(incommingChar == CR || incommingChar == LF)
			{
				if(incommingChar == LF)
					LINE++;
			}
			else
			{
				StringsFromFile[LINE] += incommingChar;             //AddCharToString
				writeDebugStreamLine(StringsFromFile[LINE]);
			}
		}
		Close(hFileHandle, nIOResult);                              //SAVE
		//##################################################################################
		for(int i = 1; i <= DATABASElength; i++) // Loop noch zu meinem CodeStyle... NEIN! !!!!
		{
			displayCenteredTextLine(i, StringsFromFile[i-1]);
			writeDebugStreamLine(StringsFromFile[i-1]);
			writeDebugStreamLine("LINE_BREAK");
		}
		writeDebugStreamLine("FINISHED");
		//######################################WORKinPROGRESSstart#################################
		for(int i = 1; i <= DATABASElength; i++)
		{
			if(StringsFromFile[i-1] == "VAR_STRING")
			{
				DATABASEREADOUTVARintNAME[i] = StringsFromFile[i];
			}
		}
		int DATABASEREADOUTstateSearch = 0;
		while(DATABASEREADOUTstateSearch < DATABASElength)
		{
			if(DATABASEREADOUTVARintNAME[DATABASEREADOUTstateSearch] == "LastState")
			{
				LastState = atoi(StringsFromFile[DATABASEREADOUTstateSearch+1]);
				nxtDisplayCenteredTextLine(5, StringsFromFile[DATABASEREADOUTstateSearch+1]);
			}
			DATABASEREADOUTstateSearch++;
		}
	}
	//######################################WORKinPROGRESSend#################################
}
void WriteDataBase(bool DoExecute)
{
	if(DoExecute==true)
	{
		//VARIABLE1
		string        DEFINE_1 = "VAR_INT";
		string        NAME_1 = "LastState";
		string        VALUE_1 = "0";

		string        DEFINE_2 = "VAR_STRING";
		string        NAME_2 = "LastState";
		string        VALUE_2 = "Works";
		//##################################################################################
		Delete("Data.txt",nIOResult);
		OpenWrite(hFileHandle, nIOResult, sFileName, nFileSize);
		//##################################################################################
		WriteText(hFileHandle, nIOResult, DEFINE_1);
		WriteByte(hFileHandle, nIOResult, CR);
		WriteByte(hFileHandle, nIOResult, LF);
		WriteText(hFileHandle, nIOResult, NAME_1);
		WriteByte(hFileHandle, nIOResult, CR);
		WriteByte(hFileHandle, nIOResult, LF);
		WriteText(hFileHandle, nIOResult, VALUE_1);

		WriteByte(hFileHandle, nIOResult, CR);
		WriteByte(hFileHandle, nIOResult, LF);

		WriteText(hFileHandle, nIOResult, DEFINE_2);
		WriteByte(hFileHandle, nIOResult, CR);
		WriteByte(hFileHandle, nIOResult, LF);
		WriteText(hFileHandle, nIOResult, NAME_2);
		WriteByte(hFileHandle, nIOResult, CR);
		WriteByte(hFileHandle, nIOResult, LF);
		WriteText(hFileHandle, nIOResult, VALUE_2);
		//##################################################################################
		Close(hFileHandle, nIOResult);
	}
}
//#############################################################################################################
//_MOVEMENT
/*__  __                                     _
#|  \/  |                                   | |
#| \  / | _____   _____ _ __ ___   ___ _ __ | |_
#| |\/| |/ _ \ \ / / _ \ '_ ` _ \ / _ \ '_ \| __|
#| |  | | (_) \ V /  __/ | | | | |  __/ | | | |_
#|_|  |_|\___/ \_/ \___|_| |_| |_|\___|_| |_|\__|
*/
bool inVerticalMovement = false;
short CurrentVerticalPos = 0;
int Rotation(float Rotations)
{
	int Degrees;
	Degrees = Rotations*360;
	return Degrees;
}
const short moveHorizontalSpeed = 10;
const float moveHorizontalRotation = 1.9;
const short moveHorizontalWait = 11000;
const short moveHorizontalCSpeed = 10;
const float moveHorizontalCRotation = 0.4;
const short moveHorizontalCWait = 2000;
const short moveFetcherSpeed = 35;
const float moveFetcherRotation = 5; //5.5
const short moveFetcherWait = 7500;
const short moveVerticalSpeed = 25;
const short moveVerticalWait = 2000;
typedef struct{
	short Speed;
	float Rotation;
	short Wait;
}move;
move Horizontal;
move HorizontalC;
move Fetcher;
move Vertical;
/*
task moveBlock;
task moveBlock{
	while(!ExitProgramm){
		if(!inVerticalMovement){
			if(nMotorEncoder[HORIZONTAL] < 0){
				motor[HORIZONTAL] = -5;
				writeDebugStreamLine("Block");
				}else{
				motor[HORIZONTAL] = 0;
				writeDebugStreamLine("NotBlock");
			}
		}
	}
}
*/
void moveHorizontalC(const string UpDown)
{
	if(UpDown == "Up")
	{
		debug("Main","INFO","Moving HORIZONTALC Up");
		string HR = HorizontalC.Rotation;
		string HS = HorizontalC.Speed;
		valueDebug("moveHorizontalC",HR);
		valueDebug("moveHorizontalC",HS);
		nMotorEncoder[HORIZONTAL] = 0;
		nMotorEncoderTarget[HORIZONTAL] = Rotation(HorizontalC.Rotation);
		motor[HORIZONTAL] = -HorizontalC.Speed;
	}
	else
	{
		if(UpDown == "Down")
		{
			debug("Main","INFO","Moving HORIZONTALC Down");
			string HR = HorizontalC.Rotation;
			string HS = HorizontalC.Speed;
			valueDebug("moveHorizontalC",HR);
			valueDebug("moveHorizontalC",HS);
			nMotorEncoder[HORIZONTAL] = 0;
			nMotorEncoderTarget[HORIZONTAL] = Rotation(HorizontalC.Rotation);
			motor[HORIZONTAL] = HorizontalC.Speed;
		}
		else
		{
			error("UpDownE","WrongUpDownVal","moveHorizontalC");
		}
	}
	wait1Msec(HorizontalC.Wait);
}
void moveHorizontal(const string UpDown)
{
	if(UpDown == "Up")
	{
		debug("Main","INFO","Moving HORIZONTAL Up");
		string HR = Horizontal.Rotation;
		string HS = Horizontal.Speed;
		valueDebug("moveHorizontal",HR);
		valueDebug("moveHorizontal",HS);
		nMotorEncoder[HORIZONTAL] = 0;
		nMotorEncoderTarget[HORIZONTAL] = Rotation(Horizontal.Rotation);
		motor[HORIZONTAL] = -Horizontal.Speed;
		wait1Msec(Horizontal.Wait);
		motor[HORIZONTAL] = 0;
	}
	else
	{
		if(UpDown == "Down")
		{
			debug("Main","INFO","Moving HORIZONTAL Down");
			string HR = Horizontal.Rotation;
			string HS = Horizontal.Speed;
			valueDebug("moveHorizontal",HR);
			valueDebug("moveHorizontal",HS);
			nMotorEncoder[HORIZONTAL] = 0;
			nMotorEncoderTarget[HORIZONTAL] = Rotation(Horizontal.Rotation);
			motor[HORIZONTAL] = Horizontal.Speed;
			wait1Msec(Horizontal.Wait);
			motor[HORIZONTAL] = 0;
		}
		else
		{
			if(UpDown == "RESET")
			{
				motor[HORIZONTAL] = Horizontal.Speed;
				while(SensorValue(VERTICALsensor) != 1){
				}
				motor[HORIZONTAL] = 0;
				moveHorizontal("Up");
				motor[HORIZONTAL] = 0;
			}
			else
			{
				error("UpDownE","WrongUpDownVal","moveHorizontal");
			}
		}
	}
}
void moveFetcher(const string LeftRight, const string InOut)
{
	if(LeftRight == "Left")
	{
		debug("Main","INFO","Moving FETCHER Left");
		if(InOut == "In")
		{
			string FR = Fetcher.Rotation;
			string FS = Fetcher.Speed;
			valueDebug("moveHorizontal",FR);
			valueDebug("moveHorizontal",FS);
			nMotorEncoder[FETCHER] = 0;
			nMotorEncoderTarget[FETCHER] = Rotation(Fetcher.Rotation);
			motor[FETCHER] = Fetcher.Speed;
		}
		else
		{
			if(InOut == "Out")
			{
				string FR = Fetcher.Rotation;
				string FS = Fetcher.Speed;
				valueDebug("moveHorizontal",FR);
				valueDebug("moveHorizontal",FS);
				nMotorEncoder[FETCHER] = 0;
				nMotorEncoderTarget[FETCHER] = Rotation(Fetcher.Rotation);
				motor[FETCHER] = -Fetcher.Speed;
			}
			else
			{
				error("InOutE","WrongInOutVal","moveFetcher");
			}
		}
	}
	else
	{
		if(LeftRight == "Right")
		{
			debug("Main","INFO","Moving FETCHER Right");
			if(InOut == "In")
			{
				string FR = Fetcher.Rotation;
				string FS = Fetcher.Speed;
				valueDebug("moveHorizontal",FR);
				valueDebug("moveHorizontal",FS);
				nMotorEncoder[FETCHER] = 0;
				nMotorEncoderTarget[FETCHER] = Rotation(Fetcher.Rotation);
				motor[FETCHER] = -Fetcher.Speed;
			}
			else
			{
				if(InOut == "Out")
				{
					string FR = Fetcher.Rotation;
					string FS = Fetcher.Speed;
					valueDebug("moveHorizontal",FR);
					valueDebug("moveHorizontal",FS);
					nMotorEncoder[FETCHER] = 0;
					nMotorEncoderTarget[FETCHER] = Rotation(Fetcher.Rotation);
					motor[FETCHER] = Fetcher.Speed;
				}
				else
				{
					error("InOutE","WrongInOutVal","moveFetcher");
				}
			}
		}
		else
		{
			error("LeftRightE","WrongLeftRightVal","moveFetcher");
		}
	}
	wait1Msec(Fetcher.Wait);
}
const short sensorVERTICALNullOffset = 10;
const short sensorVERTICALShaftOffset = 13;
typedef struct{
	short nullOffset;
	short shaftOffset;
}sensor;
sensor VerticalS;
void moveVertical(const string ForwardBackward)
{
	if(ForwardBackward == "Forward")
	{
		inVerticalMovement = true;
		debug("moveVertical","INFO","Moving VERTICAL Forward");
		motor[VERTICAL] = -Vertical.Speed;
		int LastShaftSensor = HORIZONTALSensor;
		writeDebugStreamLine("LSS: %i", LastShaftSensor);
		bool ExitLoop = false;
		wait1Msec(1500);
		while(!ExitLoop)
		{
			if(SensorValue(HORIZONTAL_T) == 1)
			{
				ExitLoop = true;
				playSound(soundBeepBeep);
			}
			//if(HORIZONTALSensor == LastShaftSensor+VerticalS.shaftOffset)
			{
				//ExitLoop = true;
				//error("EMD::AS","ExceededMaxDistance","moveVertical::F");
			}
		}
		motor[VERTICAL] = 0;
	}
	else{
		if(ForwardBackward == "Backward")
		{
			inVerticalMovement = true;
			debug("moveVertical","INFO","Moving VERTICAL Backward");
			motor[VERTICAL] = Vertical.Speed;
			int LastShaftSensor = HORIZONTALSensor;
			bool ExitLoop = false;
			while(!ExitLoop)
			{
				if(SensorValue(HORIZONTAL_T) == 1)
				{
					ExitLoop = true;
					playSound(soundBeepBeep);
				}
				//if(HORIZONTALSensor == LastShaftSensor-VerticalS.shaftOffset)
				{
					//ExitLoop = true;
					//error("EMD::AS","ExceededMaxDistance","moveVertical::B");
				}
			}
			motor[VERTICAL] = 0;
		}
		else{
			if(ForwardBackward == "RESET")
			{
				inVerticalMovement = true;
				debug("moveVertical","INFO","Reset VERTICAL");
				motor[VERTICAL] = Vertical.Speed;
				while(HORIZONTALSensor >= VerticalS.nullOffset)
				{
				}
				motor[VERTICAL] = 0;
			}
			else{
				error("ForwardBackE","WrongFBValue_FBR","moveVertical");
			}
		}
	}
	wait1Msec(Vertical.Wait);
	inVerticalMovement = false;
}
//####################################################################
void ReInitVars(const short Type = 1)
{
	switch(Type){
	case 1:
		VerticalS.nullOffset = sensorVERTICALNullOffset;
		VerticalS.shaftOffset = sensorVERTICALShaftOffset;
		Horizontal.Rotation = moveHorizontalRotation;
		Horizontal.Speed = moveHorizontalSpeed;
		Horizontal.Wait = moveHorizontalWait;
		HorizontalC.Rotation = moveHorizontalCRotation;
		HorizontalC.Speed = moveHorizontalCSpeed;
		HorizontalC.Wait = moveHorizontalCWait;
		Fetcher.Rotation = moveFetcherRotation;
		Fetcher.Speed = moveFetcherSpeed;
		Fetcher.Wait = moveFetcherWait;
		Vertical.Speed = moveVerticalSpeed;
		Vertical.Wait = moveVerticalWait;
		bFloatDuringInactiveMotorPWM = false;
		break;
	case 2:
		bFloatDuringInactiveMotorPWM = false;
		break;
	}
}
void getContainer(const string LeftRight, bool Wait = true)
{
	if(Wait)
	{
		displayCenteredBigTextLine(1,"Press");
		nxtDisplayCenteredBigTextLine(3,"Button");
		while(nNxtButtonPressed == -1)
		{
			bFloatDuringInactiveMotorPWM = true;
		}
		bFloatDuringInactiveMotorPWM = false;
		ReInitVars(2);
		eraseDisplay();
	}
	moveFetcher(LeftRight,"In");
	moveHorizontalC("Up");
	Fetcher.Rotation = 1;
	Fetcher.Wait = 5000;
	moveFetcher(LeftRight,"Out");
	moveHorizontalC("Down");
	moveFetcher(LeftRight,"In");
	moveHorizontalC("Up");
	ReInitVars();
	moveFetcher(LeftRight,"Out");
	moveHorizontalC("Down");
}
void putContainer(const string LeftRight, bool Wait = false)
{
	if(Wait)
	{
		nxtDisplayCenteredBigTextLine(1,"Press");
		nxtDisplayCenteredBigTextLine(3,"Button");
		while(nNxtButtonPressed == -1)
		{
			bFloatDuringInactiveMotorPWM = true;
		}
		ReInitVars(2);
		eraseDisplay();
		bFloatDuringInactiveMotorPWM = false;
	}
	moveHorizontalC("Up");
	moveFetcher(LeftRight,"In");
	moveHorizontalC("Down");
	Fetcher.Rotation = 1;
	Fetcher.Wait = 5000;
	moveFetcher(LeftRight,"Out");
	moveHorizontalC("Up");
	moveFetcher(LeftRight,"In");
	moveHorizontalC("Down");
	ReInitVars();
	moveFetcher(LeftRight,"Out");
	//moveHorizontalC("Up");
}
//#############################################################################################################
//_MQProcess _processMQ(
/*__  __  ____
#|  \/  |/ __ \
#| \  / | |  | |_ __  _ __ ___   ___ ___  ___ ___
#| |\/| | |  | | '_ \| '__/ _ \ / __/ _ \/ __/ __|
#| |  | | |__| | |_) | | | (_) | (_|  __/\__ \__ \
#|_|  |_|\___\_\ .__/|_|  \___/ \___\___||___/___/
###############| |
###############|_|
*/
void remFirstMQElements(){
	short newBasicMovementQueue[MaxMovementQueue];
	short newTypeMovementQueue[MaxMovementQueue];
	for(int i = 0; i < MaxMovementQueue-1; i++){
		newBasicMovementQueue[i] = -1;
		newTypeMovementQueue[i] = -1;
	}
	for(int i = 1; i < MaxMovementQueue-1; i++){
		newBasicMovementQueue[i-1] = BasicMovementQueue[i];
		newTypeMovementQueue[i-1] = TypeMovementQueue[i];
	}
	for(int i = 0; i < MaxMovementQueue-1; i++){
		BasicMovementQueue[i] = newBasicMovementQueue[i];
		TypeMovementQueue[i] = newTypeMovementQueue[i];
	}
	CurrentMQOffset--;
}
void processMQ(){
	//Horizontal
	if(BasicMovementQueue[0] == Signal_H){
		debugOnChange("main::processMQ","INFO","HorizontalMove");
		if(TypeMovementQueue[0] == STYPE__1){
			moveHorizontal("Up");
			debugOnChange("main::processMQ","INFO","Up");
			remFirstMQElements();
		}
		else{if(TypeMovementQueue[0] == STYPE__2){
				moveHorizontal("Down");
				debugOnChange("main::processMQ","INFO","Down");
				remFirstMQElements();
			}
		}
	}
	//Vertical
	if(BasicMovementQueue[0] == Signal_V){
		debugOnChange("main::processMQ","INFO","VerticalMove");
		if(TypeMovementQueue[0] == STYPE__1){
			moveVertical("Forward");
			debugOnChange("main::processMQ","INFO","Forward");
			remFirstMQElements();
		}
		else{if(TypeMovementQueue[0] == STYPE__2){
				moveVertical("Backward");
				debugOnChange("main::processMQ","INFO","Backward");
				remFirstMQElements();
			}
		}
	}
	//getContainer
	if(BasicMovementQueue[0] == SgC){
		debugOnChange("main::processMQ","INFO","getContainer");
		if(TypeMovementQueue[0] == STYPE__1){
			getContainer("Left");
			debugOnChange("main::processMQ","INFO","Left");
			remFirstMQElements();
		}
		else{if(TypeMovementQueue[0] == STYPE__2){
				getContainer("Right");
				debugOnChange("main::processMQ","INFO","Right");
				remFirstMQElements();
			}
		}
	}
	//putContainer
	if(BasicMovementQueue[0] == SpC){
		debugOnChange("main::processMQ","INFO","putContainer");
		if(TypeMovementQueue[0] == STYPE__1){
			putContainer("Left");
			debugOnChange("main::processMQ","INFO","Left");
			remFirstMQElements();
		}
		else{if(TypeMovementQueue[0] == STYPE__2){
				putContainer("Right");
				debugOnChange("main::processMQ","INFO","Right");
				remFirstMQElements();
			}
		}
	}
	if(BasicMovementQueue[0] == SresetA){
		debugOnChange("main::processMQ","INFO","resetAll");
		moveHorizontal("RESET");
		moveVertical("RESET");
		remFirstMQElements();
	}
}
//#############################################################################################################
//_MAIN
/*###############_
################(_)
#_ __ ___   __ _ _ _ __
| '_ ` _ \ / _` | | '_ \
| | | | | | (_| | | | | |
|_| |_| |_|\__,_|_|_| |_|
*/
task main()
{
	//Initate Variables
	ReInitVars();
	bFloatDuringInactiveMotorPWM = false;
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
	nVolume = 4;
	/*
	C_RED.MIN = 895;
	C_RED.MAX = 925;
	C_GREEN.MIN = 865;
	C_GREEN.MAX = 885;
	C_BLUE.MIN = 820;
	C_BLUE.MAX = 840;
	*/
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
	CP8.P3 	= 2;
	CP8.P4 	= 3;
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
	startTask(readSensors);
	startTask(checkButtons);
	//startTask(moveBlock);
	/*
	moveHorizontal("RESET");
	moveVertical("RESET");

	moveHorizontal("Up");
	moveVertical("Forward");
	moveVertical("Forward");
	getContainer("Left", true);

	moveHorizontal("RESET");
	moveVertical("RESET");

	putContainer("Left", true);

	wait1Msec(9999);

	ActualSignal = 2;
	broadcastSignal();
	ActualSignal = 4;
	broadcastSignal();
	ActualSignal = 2;
	broadcastSignal();
	ActualSignal = 4;
	broadcastSignal();

	wait1Msec(10000);

	ActualSignal = 3;
	broadcastSignal();
	ActualSignal = 6;
	broadcastSignal();

	*/

	WriteDataBase(false);
	ReadDataBase(false);
	while(ExitProgramm == false)
	{
		if(validColorSensor(COLOUR)){
			processMQ();
			}else{
			playSound(soundBeepBeep);
			debug("main::MainLoop_end", "WARNING", "WILL_EXIT_NOW");
		}
	}
}
task readSensors()
{
	//string debugBuffer;
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
		wait1Msec(100);
		HORIZONTALSensor = SensorValue(HORIZONTAL);
		//sprintf(debugBuffer,"SVU: %i",HORIZONTALSensor);
		//debugOnChange("readSensor::Loop","DEBUG",debugBuffer);
		//displayCenteredTextLine(6, "SVU: %i",HORIZONTALSensor);
	}
}
task checkButtons()
{
	int Time;
	while(true)
	{
		wait1Msec(100);
		while(nNxtButtonPressed == -1)
		{
			Time = 0;
		}
		Time++;
		if(Time == 10)
		{
			debug("checkButtons","INFO","Reset of Pos+MQs FLUSH!");
			playSound(soundLowBuzz);
			for(int i = 0; i < MaxMovementQueue-1; i++){
				BasicMovementQueue[i] = -1;
				TypeMovementQueue[i] = -1;
			}
			moveVertical("RESET");
			moveHorizontal("RESET");
			debug("checkButtons","CRITICAL","PERFOM THE RESET ON THE EV3 TOO, OTHERWISE THERE WILL BE MOST LIKELY AN CRASH!");
		}
	}
}

/*
//const short TickRate = 5000; //ColourChangeRate (HAS TO BE EQUAL TO EV3)<-NOPE,USELESS //INT::ChToShortOn_2113_18032016
*/
