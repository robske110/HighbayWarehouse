#pragma config(Sensor, S1,     COLOUR,         sensorColorNxtAmbient)
#pragma DebuggerWindows("EV3LCDScreen", "taskStatus", "debugStream", "callStack")
//Settings
const short Hlength = 4;
const short Llength = 5;
const short DISP_REFRESH_RATE = 500;
const bool ignoreErrors = false;
short logLevel = 1; //1=ALL 0=INFO,CRITICAL,WARNING -1=CRITICAL,WARNING
//SHORTs
const short ERR_LEVEL_LOW = -1;
const short ERR_LEVEL_MID = 0;
const short ERR_LEVEL_HIGH = 1;
const short LOG_FLAG_NONEWLINE = 1;
const short DISP_TYPE_NORMAL = 1;
const short DISP_TYPE_CENTERED = 0;
short ErrCounter = 0;
//BOOLs
bool  isBooting;
bool  doExitProgramm = false;
//STRINGs
string CurrentC;
string newLogString;
string newLogStringType;
string LOG_LEVEL_DEBUG = "DEBUG"; //const
string LOG_LEVEL_INFO = "INFO"; //const
string LOG_LEVEL_WARNING = "WARNING"; //const
string LOG_LEVEL_CRITICAL = "BAUM"; //const
//string LastDisplayMsg;
//INTs
int lastSigSend;
//TASKs
task displayManager;
task buttonChecker;
task colourSender;
//BackTrace
short backTraceFunctionIDs[15];
string backTraceParams[15];
bool backTraceIsTruncatedEntry[15];
int backTraceCallTime[15];
short backTraceLength = 15;
//#############################################################################################################
//_UTILS
/*
#_    _ _______ _____ _       _____
| |  | |__   __|_   _| |     / ____|
| |  | |  | |    | | | |    | (___
| |  | |  | |    | | | |     \___ \
| |__| |  | |   _| |_| |____ ____) |
#\____/   |_|  |_____|______|_____/
*/
void displayText(const short Line,const string Text,const bool isBold = false,const short dispType = DISP_TYPE_CENTERED){
	switch(dispType){
		case DISP_TYPE_CENTERED:
			if(isBold){
				displayCenteredBigTextLine(Line, Text);
			}else{
				displayCenteredTextLine(Line, Text);
			}
		break;
		case DISP_TYPE_NORMAL:
			if(isBold){
				displayBigTextLine(Line, Text);
			}else{
				displayTextLine(Line, Text);
			}
		break;
	}
}

void writeTimeStamp(){
	//string TimeStamp1;
	//sprintf(TimeStamp1, "SYS:%i_", nSysTime);
	//writeDebugStream(TimeStamp1);
	int sec = nPgmTime / 1000;
	int ms = nPgmTime;
	writeDebugStream("PGM:%i.%i", sec);
}

void debug(const string ToFIdentifier,const string DebugType,const string DebugString,const short DebugFlag = 0){
	if(logLevel == 1 ||
		(logLevel == 0 && (DebugType == LOG_LEVEL_CRITICAL || DebugType == LOG_LEVEL_INFO || DebugType == LOG_LEVEL_WARNING || DebugType == "ERROR"))								 ||
	  (logLevel == -1 && (DebugType == LOG_LEVEL_CRITICAL || DebugType == LOG_LEVEL_WARNING || DebugType == "ERROR"))
	  ){
		writeTimeStamp();
		if(DebugType == LOG_LEVEL_CRITICAL || DebugType == "ERROR" || DebugType == LOG_LEVEL_WARNING){
			playSound(soundException);playSound(soundException);playSound(soundException);
		}
		writeDebugStream(ToFIdentifier);
		writeDebugStream(" <");
		writeDebugStream(DebugType);
		writeDebugStream(">: ");
		writeDebugStream(DebugString);
		if(DebugFlag != LOG_FLAG_NONEWLINE){ //NEWLY IMPLEMENTED ADDITIONAL FLAG
			writeDebugStreamLine("");
		}
		newLogStringType = DebugType; //quite newly implemented GUI mess, TODO make mess lessy
		newLogString = DebugString;
	}
}

//_addToB //BackT
void addToBackTrace(const short functionID, const string oparam1 = "-", const string oparam2 = "-", const string oparam3 = "-", const string oparam4 = "-"){
	int callTime = nPgmTime;
	short newBackTraceFunctionIDs[15];
	string newBackTraceParams[15];
	bool newBackTraceIsTruncatedEntry[15];
	short newBackTraceCallTime[15];
	for(int i = 1; i <= 14; i++){
		newBackTraceFunctionIDs[i] = backTraceFunctionIDs[i-1];
		newBackTraceParams[i] = backTraceParams[i-1];
		newBackTraceIsTruncatedEntry[i] = backTraceIsTruncatedEntry[i-1];
		newBackTraceCallTime[i] = backTraceCallTime[i-1];
	}
	for(int i = 1; i <= 14; i++){
		backTraceFunctionIDs[i] = newBackTraceFunctionIDs[i];
		backTraceParams[i] = newBackTraceParams[i];
		backTraceIsTruncatedEntry = newBackTraceIsTruncatedEntry[i];
		backTraceCallTime[i] = newBackTraceCallTime[i];
	}
	backTraceFunctionIDs[0] = functionID;
	backTraceIsTruncatedEntry[0] = false;
	backTraceCallTime[0] = callTime;
	string param1 = oparam1;
	string param2 = oparam2;
	string param3 = oparam3;
	string param4 = oparam4;
	string paramsString1;
	string paramsString2;
	string paramsString3;
	string finalParamsString;
	while(strlen(param1) + strlen(param2) + strlen(param3) + strlen(param4) + 3 > 19){
		//debug("addToBackTrace", LOG_LEVEL_DEBUG, "paramsOver19");
		backTraceIsTruncatedEntry[0] = true;
		stringDelete(param1, strlen(param1)-1, 1);
		stringDelete(param2, strlen(param2)-1, 1);
		stringDelete(param3, strlen(param3)-1, 1);
		stringDelete(param4, strlen(param4)-1, 1);
	}
	if(param1 != "-"){
		sprintf(paramsString1, "%s", param1);
	}
	if(param2 != "-"){
		sprintf(paramsString2, "%s,%s", paramsString1,param2);
	}
	if(param3 != "-"){
		sprintf(paramsString3, "%s,%s", paramsString2,param3);
	}
	if(param4 != "-"){
		sprintf(finalParamsString, "%s,%s",paramsString3,param4);
	}
	backTraceParams[0] = finalParamsString;
}
//_printBackT
void printBackTrace(const short Length = 10){
	if(Length >= backTraceLength){
		debug("E_116_0x074",LOG_LEVEL_CRITICAL,"printBackTrace");
		debug("TooHighBackTraceVal",LOG_LEVEL_CRITICAL,"printBackTrace");
		if(!ignoreErrors){
			doExitProgramm = true;
		}
	}
	writeDebugStreamLine("BackTrace:");
	for(int i = Length; i >= 0; i--){
		short functionID = backTraceFunctionIDs[i];
		string hex;
		sprintf(hex, "0x%x", functionID);
		if(i != 0){
			writeDebugStream("-%i", i);
		}else{
			writeDebugStream("%i", i);
		}
		writeDebugStream(": ");
		writeDebugStream("time:");
		writeDebugStream("%i ", backTraceCallTime[i]);
		writeDebugStream("%s", hex);
		writeDebugStream("::(");
		writeDebugStream(backTraceParams[i]);
		if(!backTraceIsTruncatedEntry[i]){
			writeDebugStreamLine(");");
		}else{
			writeDebugStreamLine(") [TRUNCATED];");
		}
	}
}
const string SM = ":SERIOUSmsg:";
const string TMC = ":TOO-MUCH-CRITICAL:";
void errMsg(const string TaskFunction, const string MsgString, const short Type = 0){
	writeDebugStream("[ERROR-HANDLER] ");
	if(Type == 0){
		writeDebugStream("[ERROR-HANDLER] ");
		debug(TaskFunction, "ERROR", MsgString);
	}
	if(Type == 1){
		writeDebugStream("%s",SM);
		debug(TaskFunction, LOG_LEVEL_CRITICAL, MsgString);
	}
	if(Type == -1){
		writeDebugStream("%s",TMC);
		debug(TaskFunction, LOG_LEVEL_CRITICAL, MsgString);
	}
}
void errShowDesc(const string Desc, const string ShortDesc, const string TaskFunction){
	errMsg(TaskFunction, ShortDesc);
	errMsg(TaskFunction, Desc);
	displayCenteredBigTextLine(5, "ERROR");
	displayCenteredTextLine(7, "ID:%s", ShortDesc);
	displayCenteredTextLine(8, "More Info -> DebugStream!");
}
void error(const string Desc, const string ShortDesc, const string TaskFunction, const short ErrLevel = ERR_LEVEL_MID){
	hogCPU();
	ErrCounter++;
	setLEDColor(ledRedFlash);
	playSound(soundException);playSound(soundException);
	playSound(soundException);playSound(soundException);
	errShowDesc(Desc, ShortDesc, TaskFunction);
	switch(ErrLevel){
	case ERR_LEVEL_LOW:
		printBackTrace(10);
		break;
	case ERR_LEVEL_MID:
		debug(TaskFunction, LOG_LEVEL_WARNING, "HIGH_LEVEL_ERROR");
		printBackTrace(15);
		playSoundFile("Backing alert");
		writeDebugStream("ProgramMayBeFaulty");
		if(!ignoreErrors){
			setUserExceptionType1();
		}
		break;
	case ERR_LEVEL_HIGH:
		writeDebugStream("[ERROR-HANDLER] ");
		debug(TaskFunction, LOG_LEVEL_CRITICAL, "UNRECOVERABLE ERROR");
		printBackTrace(15);
		playSoundFile("Backing alert");
		playSound(soundException);playSound(soundException);
		if(!ignoreErrors){
			setUserExceptionType2();
			doExitProgramm = true;
		}
		break;
	}
	releaseCPU();
}

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
//#############################################################################################################
//_COLOURPROTOCOL _CProtocol
/*
###_____      _                  _____           _                  _
##/ ____|    | |                |  __ \         | |                | |
#| |     ___ | | ___  _   _ _ __| |__) | __ ___ | |_ ___   ___ ___ | |
#| |    / _ \| |/ _ \| | | | '__|  ___/ '__/ _ \| __/ _ \ / __/ _ \| |
#| |___| (_) | | (_) | |_| | |  | |   | | | (_) | || (_) | (_| (_) | |
##\_____\___/|_|\___/ \__,_|_|  |_|   |_|  \___/ \__\___/ \___\___/|_|
*/
//Colour Patterns (Each of them is representing an signal)
//12 defined, ~? used 24 possible
//0 = ALL
//1 = RED
//2 = BLUE
//3 = GREEN
//(-1=NULL)
//Signals for MainMovement
const short STYPE__1  = 2; //TYPE1::1.A
const short STYPE__2  = 3; //TYPE1::1.A
const short Signal_V  = 4; //TYPE1::2.E
const short Signal_H  = 5; //TYPE1::2.E
const short SgC       = 6; //TYPE1::2.E
const short SpC       = 7; //TYPE1::2.E
const short SresetA   = 8; //TYPE2::1.A+E || THIS HAS (currently) NO STYPE BEFORE it!
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

//Settings
const short TickRate = 1000; //ColourChangeRate //INT::ChToShortOn_2113_18032016
const short SignalLength = 4;
const short MaxCSigsInQ = 30;
//const short SignalCount = 12; //6
//SHORTs
short CSigQueque[4*MaxCSigsInQ];
const short RealMaxCQ = 4*MaxCSigsInQ;
short SignalPointer = 0;

void listCQueue(const string who = "colourSender"){
	string TempSprintfString;
	sprintf(TempSprintfString, "%s::ListCQueue", who);
	debug(TempSprintfString, LOG_LEVEL_DEBUG, "Contents of CSigQueque:");
	for(int i = 0; i < RealMaxCQ; i++){
		sprintf(TempSprintfString, "QID: %i QVa: %i", i, CSigQueque[i]);
		writeDebugStream(TempSprintfString);
		if(i != RealMaxCQ-1){
			writeDebugStream(" , ");
		}
	}
	writeDebugStreamLine("");
}

bool addCSigToQueque(short C1,short C2,short C3,short C4){
	bool AlreadyWrote = false;
	string TempSprintfString;
	for(int i = 3; i < RealMaxCQ; i=i+4){
		if(CSigQueque[i] == -1 && AlreadyWrote == false){
			sprintf(TempSprintfString, "writeTo:%i", i);
			debug("addCSigToQueque", LOG_LEVEL_DEBUG, TempSprintfString);
			CSigQueque[i-3] = C1;
			CSigQueque[i-2] = C2;
			CSigQueque[i-1] = C3;
			CSigQueque[i]   = C4;
			AlreadyWrote = true;
		}
	}
	return AlreadyWrote;
}

void sendSignal(short Signal){
	//playSound(soundFastUpwardTones);
	string TempSprintfString;
	sprintf(TempSprintfString, "SendSignal: %i", Signal);
	debug("sendSignal", "INFO", TempSprintfString);
	short TEMP_PedCPS[SignalLength];
	switch(Signal){
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
	default: error("gotUnknownSignal","E_111_0x06f","sendSignal", ERR_LEVEL_MID);
		break;
	}
	if(!addCSigToQueque(TEMP_PedCPS[0],TEMP_PedCPS[1],TEMP_PedCPS[2],TEMP_PedCPS[3])){
		error("CSigQuequeIsFull","E_113_0x071","sendSignal", ERR_LEVEL_HIGH);
		//wait1Msec(TickRate);
	}
}

//0 = ALL
//1 = RED
//2 = BLUE
//3 = GREEN
//(-1=NULL)
void changeC(short Colour){
	switch(Colour){
		case 0:
			SensorType[COLOUR] = sensorColorNxtRED;
			debug("colourSender::changeC",LOG_LEVEL_DEBUG,"RED");
			CurrentC = "RED";
		break;
		case 1:
			SensorType[COLOUR] = sensorColorNxtBLUE;
			debug("colourSender::changeC",LOG_LEVEL_DEBUG,"BLUE");
			CurrentC = "BLUE";
		break;
		case 2:
			SensorType[COLOUR] = sensorColorNxtGREEN;
			debug("colourSender::changeC",LOG_LEVEL_DEBUG,"GREEN");
			CurrentC = "GREEN";
		break;
		case 3:
			SensorType[COLOUR] = sensorColorNxtFULL;
			debug("colourSender::changeC",LOG_LEVEL_DEBUG,"ALL");
			CurrentC = "ALL";
		break;
		default: error("GotUnknownColor","E_112_0x070","changeC",ERR_LEVEL_HIGH);
		break;
	}
	//if(Colour != 0){
	string TempSprintfString;
	sprintf(TempSprintfString, "Colour: %i", Colour);
	debug("colourSender::changeC",LOG_LEVEL_DEBUG,TempSprintfString);
	//}
}

void remFourElements(){
	short newCSigQueque[RealMaxCQ];
	for(int i = 0; i < RealMaxCQ; i++){
		newCSigQueque[i] = -1;
	}
	for(int i = 4; i < RealMaxCQ; i++){
		/*
		string TempSprintfString;
		sprintf(TempSprintfString, "WnCSQ:%i VWnCSQ:%i",i-4,CSigQueque[i]);
		debug("cS::rem4Elements",LOG_LEVEL_DEBUG,TempSprintfString);
		*/
		newCSigQueque[i-4] = CSigQueque[i];
	}
	for(int i = 0; i < RealMaxCQ; i++){
		CSigQueque[i] = newCSigQueque[i];
	}
}

task colourSender()
{
	changeC(0);
	while(true)
	{
		SignalPointer = 0;
		wait1Msec(TickRate);
		for(int i = 0; i < RealMaxCQ; i++){
			if(CSigQueque[i] != -1){
				changeC(CSigQueque[i]);
				SignalPointer++;
				wait1Msec(TickRate/2);
				changeC(0);
				wait1Msec(TickRate/2);
				if(SignalPointer == SignalLength){
					i = RealMaxCQ+1;
					debug("colourSender","INFO","SentAnSignal");
					lastSigSend = nPgmTime;
					remFourElements();
					listCQueue();
					writeDebugStreamLine("");
				}
			}
		}
	}
}
//326LinesOfEV3CP
//#############################################################################################################
//_MOVEMENT
/*__  __                                     _
#|  \/  |                                   | |
#| \  / | _____   _____ _ __ ___   ___ _ __ | |_
#| |\/| |/ _ \ \ / / _ \ '_ ` _ \ / _ \ '_ \| __|
#| |  | | (_) \ V /  __/ | | | | |  __/ | | | |_
#|_|  |_|\___/ \_/ \___|_| |_| |_|\___|_| |_|\__|
*/
void moveHorizontal(const string UpDown , const string TaskFunction = "GUI::moveHandler")
{
	if(UpDown == "Up")
	{
		debug(TaskFunction,"INFO","Moving HORIZONTAL Up");
		sendSignal(STYPE__1);
		sendSignal(Signal_H);
	}
	else
	{
		if(UpDown == "Down")
		{
			debug(TaskFunction,"INFO","Moving HORIZONTAL Down");
			sendSignal(STYPE__2);
			sendSignal(Signal_H);
		}
		else
		{
			if(UpDown == "RESET")
			{
				//TODO
			}
			else
			{
				error("WrongUpDownVal","E_120_0x078","moveHorizontal",ERR_LEVEL_HIGH);
			}
		}
	}
}
void moveVertical(const string ForwardBackward)
{
	if(ForwardBackward == "Forward")
	{
		sendSignal(STYPE__1);
		sendSignal(Signal_V);
	}
	else{
		if(ForwardBackward == "Backward")
		{
			sendSignal(STYPE__2);
			sendSignal(Signal_V);
		}
		else{
			if(ForwardBackward == "RESET")
			{
				//TODO
			}
			else{
				error("WrongFBValue_FBR","E_121_0x079","moveVertical",ERR_LEVEL_HIGH);
			}
		}
	}
}
//##################################################################################
void getContainer(const string LeftRight, const string TaskFunction = "GUI::moveHandler")
{
	if(LeftRight == "Left")
	{
		debug(TaskFunction,"INFO","getContainer Left");
		sendSignal(STYPE__1);
		sendSignal(SgC);
	}
	else
	{
		if(LeftRight == "Right")
		{
			debug(TaskFunction,"INFO","getContainer Right");
			sendSignal(STYPE__2);
			sendSignal(SgC);
		}
		else
		{
			error("WrongLeftRightVal","E_122_0x07a","getContainer",ERR_LEVEL_HIGH);
		}
	}
}
void putContainer(const string LeftRight, const string TaskFunction = "GUI::moveHandler")
{
	if(LeftRight == "Left")
	{
		debug(TaskFunction,"INFO","putContainer Left");
		sendSignal(STYPE__1);
		sendSignal(SpC);
	}
	else
	{
		if(LeftRight == "Right")
		{
			debug(TaskFunction,"INFO","putContainer Right");
			sendSignal(STYPE__2);
			sendSignal(SpC);
		}
		else
		{
			error("WrongLeftRightVal","E_123_0x07b","putContainer",ERR_LEVEL_HIGH);
		}
	}
}
void goTo(const string UI){
	string TempSprintfString;
	sprintf(TempSprintfString, "goTo(%s)", UI);
	debug("goTo","INFO",TempSprintfString);
	sendSignal(SresetA);
	string SavedUI = UI;
	stringDelete(SavedUI,0,1);
	short L = atoi(SavedUI);
	SavedUI = UI;
	stringDelete(SavedUI,1,1);
	string HRaw = SavedUI;
	short H = 0;
	switch(HRaw){
	case 'A': H = 1;
		break;
	case 'B': H = 2;
		break;
	case 'C': H = 3;
		break;
	case 'D': H = 4;
		break;
	default: error("goToWrongVal:H","E_114_0x072","goTo::firstParse");
	}
	if(L > Llength || L <= 0 ){
		error("goToWrongVal:L","E_115.1_0x47f","goTo::checkParse");
	}
	if(H > Hlength || H <= 0 ){
		error("goToWrongVal:H","E_115.2_0x480","goTo::checkParse");
	}
	sprintf(TempSprintfString, "HRaw: %s, H: %i, L: %i", HRaw, H, L);
	debug("goTo",LOG_LEVEL_DEBUG,TempSprintfString);
	//Ich dachte erst dass würde schwierig werden. Naja, wars ned.
	//Very high-difficult math: (NOT)
	if(H == 1){
		moveHorizontal("Down");
		}else{
		short moveH = H-2;
		for(int i = moveH; i > 0; i--){
			moveHorizontal("Up");
		}
	}
	short moveL = L-1;
	for(int i = moveL; i >= 0; i--){
		moveVertical("Forward");
	}
}
//#############################################################################################################
//_GUI
/*
##_____ _    _ _____
#/ ____| |  | |_   _|
| |  __| |  | | | |
| | |_ | |  | | | |
| |__| | |__| |_| |_
#\_____|\____/|_____|
*/
//Settings
const short rowLength = 9+9+9;
//Misc
string calcFillResult;
short Selection = 0;
bool NoGo; //_PUBLIC_priv use doNoGo()
bool NoGo2;
bool doResetExitAble;
string CustomPos1; //_PUBLIC Custom string ALWAYS (Except if isBooting) displayed. (oben links) MAX rowLength/3.
//Basic ScreenHandling
bool inScreen = false;
short inScreenType = 0;
bool blockScreenExit = false;
//GLOABAL Variables for SCREENS
short levelGoTo = -1;
short levelMMove = -1;
short levelSendSignal = -1;
string LogStrings[11];
string LogStringsTypes[11];
string newLogStrings[11];
string newLogStringsTypes[11];
//Buttons (Only if inScreen = true)
bool LeftButtonPress;
bool RightButtonPress;
bool UpButtonPress;
bool DownButtonPress;
bool EnterButtonPress;
bool realLeftButtonPress;
bool realRightButtonPress;
bool realUpButtonPress;
bool realDownButtonPress;
bool realEnterButtonPress;

void calcFill(const short Length){
	short between = rowLength/3;
	string finalString;
	string TempString;
	string TempSprintfString;
	sprintf(TempSprintfString, "between: %i", between);
	//writeDebugStreamLine(TempSprintfString);
	sprintf(TempSprintfString, "Length: %i", Length);
	//writeDebugStreamLine(TempSprintfString);
	for(int i = between; i > 0+Length; i--){
		sprintf(TempString, "%s ", finalString, i);
		finalString = TempString;
	}
	calcFillResult = finalString;
}

void doNoGo(){
	setLEDColor(ledRed);
	playSound(soundBlip);
	drawBmpfile(0, 125,"No go");
	NoGo = true;
	NoGo2 = true;
}
void resetButtons(){
	if(LeftButtonPress){
		doNoGo();
		LeftButtonPress = false;
	}else{if(realLeftButtonPress){playSoundFile("Click");}}
	if(RightButtonPress){
		doNoGo();
		RightButtonPress = false;
	}else{if(realRightButtonPress){playSoundFile("Click");}}
	if(UpButtonPress){
		doNoGo();
		UpButtonPress = false;
	}else{if(realUpButtonPress){playSoundFile("Click");}}
	if(DownButtonPress){
		doNoGo();
		DownButtonPress = false;
	}else{if(realDownButtonPress){playSoundFile("Click");}}
	if(EnterButtonPress){
		doNoGo();
		EnterButtonPress = false;
	}else{if(realEnterButtonPress){playSoundFile("Click");}}
	realLeftButtonPress = false;
	realRightButtonPress = false;
	realUpButtonPress = false;
	realDownButtonPress = false;
	realEnterButtonPress = false;
}

void newScreen(short Type){
	setBlockBackButton(true);
	inScreen = true;
	blockScreenExit = false;
	inScreenType = Type;
	string TempSprintfString;
	sprintf(TempSprintfString, "toScreen: %i", Type);
	debug("GUI::newScreen", LOG_LEVEL_DEBUG, TempSprintfString);
	switch(Selection){
	case 1: displayCenteredBigTextLine(2, "LOADING..."); blockScreenExit = true; levelGoTo = 0;
		break;
	case 2: displayCenteredBigTextLine(5, "LOADING..."); levelMMove = 0;
		break;
	case 3: displayCenteredBigTextLine(7, "LOADING..."); levelSendSignal = 0;
		break;
	case 4: displayCenteredBigTextLine(9, "LOADING...");
		break;
	case 5: displayCenteredBigTextLine(11, "LOADING...");
		break;
	default:
		inScreenType = 0; inScreen = false; doResetExitAble = true;
		error("Unknown Selection/ScreenType resetiST&&iS", "E_G201_0x0c9", "newScreen", ERR_LEVEL_LOW);
		doNoGo();
		break;
	}
}

task displayManager(){
	//_displayManager
	//goTo Local(++)Variables
	short gpTypeGoTo = -1;
	short accessTypeGoTo = -1;
	short HIndexGoTo = -1;
	short LIndexGoTo = -1;
	//MMove Local(++)Variables
	short BasicTypeMMove = -1;
	short RealTypeMMove = -1;
	//sendSignal Local(++)Variables
	short SignalsendSig = -1;
	short currentMsec = 0;
	while(true){
		wait1Msec(1);
		currentMsec++;
		if(newLogString != LogStrings[0]){
			for(int i = 1; i <= 10; i++){
				newLogStrings[i] = LogStrings[i-1];
				newLogStringsTypes[i] = LogStringsTypes[i-1];
			}
			for(int i = 1; i <= 10; i++){
				LogStrings[i] = newLogStrings[i];
				LogStringsTypes[i] = newLogStringsTypes[i];
			}
			LogStrings[0] = newLogString;
			LogStringsTypes[0] = newLogStringType;
		}
		if(currentMsec == DISP_REFRESH_RATE){
			currentMsec = 0;
			eraseDisplay();
			if(NoGo || NoGo2){
				drawBmpfile(0,125,"No go");
				setLEDColor(ledRed);
				if(!NoGo){
					NoGo2 = false;
				}
			}
			string TempSprintfString;
			string Pos1;
			if(isBooting){
				Pos1 = "Booting";
			}
			else{
				Pos1 = CustomPos1;
			}
			sprintf(TempSprintfString,"CColour:%s",CurrentC);
			string Pos2 = TempSprintfString;
			sprintf(TempSprintfString,"CPos:%i",SignalPointer);
			string Pos3 = TempSprintfString;
			calcFill(strlen(Pos1));
			string Fill1 = calcFillResult;
			calcFill(strlen(Pos2));
			string Fill2 = calcFillResult;
			displayCenteredTextLine(1, "%s%s|%s%s|%s", Pos1,Fill1,Pos2,Fill2,Pos3);
			if(!isBooting && !inScreen && !NoGo){
				setLEDColor(ledGreen);
				//setBlockBackButton(false); //TODO::checkIfIsSendingSIGs
				displayText(3, "get/putBox",(Selection == 1));
				displayText(5, "manualMovement",(Selection == 2));
				displayText(7, "sendSignal",(Selection == 3));
				displayText(9, "INFO",(Selection == 4));
				displayText(11, "LOG",(Selection == 5));
				/*
				switch(Selection){
				case 1: displayCenteredBigTextLine(2, "get/putBox"); displayCenteredTextLine(5, "manualMovement"); displayCenteredTextLine(7, "sendSignal"); displayCenteredTextLine(9, "INFO"); displayCenteredTextLine(11, "LOG");
					break;
				case 2: displayCenteredTextLine(3, "get/putBox"); displayCenteredBigTextLine(5, "manualMovement"); displayCenteredTextLine(7, "sendSignal"); displayCenteredTextLine(9, "INFO"); displayCenteredTextLine(11, "LOG");
					break;
				case 3: displayCenteredTextLine(3, "get/putBox"); displayCenteredTextLine(5, "manualMovement"); displayCenteredBigTextLine(7, "sendSignal"); displayCenteredTextLine(9, "INFO"); displayCenteredTextLine(11, "LOG");
					break;
				case 4: displayCenteredTextLine(3, "get/putBox"); displayCenteredTextLine(5, "manualMovement"); displayCenteredTextLine(7, "sendSignal"); displayCenteredBigTextLine(9, "INFO"); displayCenteredTextLine(11, "LOG");
					break;
				case 5: displayCenteredTextLine(3, "get/putBox"); displayCenteredTextLine(5, "manualMovement"); displayCenteredTextLine(7, "sendSignal"); displayCenteredTextLine(9, "INFO"); displayCenteredBigTextLine(11, "LOG");
					break;
				default: displayCenteredTextLine(3, "get/putBox"); displayCenteredTextLine(5, "manualMovement"); displayCenteredTextLine(7, "sendSignal"); displayCenteredTextLine(9, "INFO"); displayCenteredTextLine(11, "LOG");
					break;
				}
				*/
			}else{
				if(isBooting){displayCenteredBigTextLine(7, "BOOTING...");}
			}
			Pos1 = "JuFoEV3";
			Pos2 = "Hochregallager";
			Pos3 = "0.7.7";
			calcFill(strlen(Pos1)+1);
			Fill1 = calcFillResult;
			calcFill(strlen(Pos2));
			Fill2 = calcFillResult;
			displayCenteredTextLine(15, "%s%s|%s%s|%s", Pos1,Fill1,Pos2,Fill2,Pos3);
			string TempActualStep;
			//_SCREEN
			if(!isBooting && inScreen && !NoGo){
				setBlockBackButton(true);
				if(!NoGo2){
					setLEDColor(ledOrange);
				}
				switch(inScreenType){
				case 1: //_goTo
					string TempActualGPType;
					string TempActualLRaccess;
					string TempActualStringH;
					string TempActualStringL;
					string TempFinalGoToString;
					if(levelGoTo == 4){
						drawBmpfile(0, 125,"Accept");
						playSoundFile("Confirm");
					}
					levelGoTo++;
					sprintf(TempActualStep, "Schritt %i von 4", levelGoTo);
					displayCenteredTextLine(3, TempActualStep);
					levelGoTo--;
					if(accessTypeGoTo == 0){
						TempActualLRaccess = "L";
					}
					if(accessTypeGoTo == 1){
						TempActualLRaccess = "R";
					}
					if(gpTypeGoTo == 0){
						TempActualGPType = "get";
					}
					if(gpTypeGoTo == 1){
						TempActualGPType = "put";
					}
					switch(HIndexGoTo){
					case 1: TempActualStringH = "A";
						break;
					case 2: TempActualStringH = "B";
						break;
					case 3: TempActualStringH = "C";
						break;
					case 4: TempActualStringH = "D";
						break;
					default: //debug("displayManager::parseMaingoTo",LOG_LEVEL_DEBUG,"ERR_G-02goTo_WrongVal:HIndexGoTo");
					}
					if(LIndexGoTo == -1){LIndexGoTo = 0;}
					sprintf(TempActualStringL,"%i",LIndexGoTo);
					if(LIndexGoTo == 0){LIndexGoTo = -1;}
					if(HIndexGoTo == -1 && LIndexGoTo == -1){
						displayCenteredBigTextLine(8, "%s %s", TempActualGPType, TempActualLRaccess);
						}else{
						displayCenteredBigTextLine(8, "%s %s %s%s", TempActualGPType, TempActualLRaccess, TempActualStringH, TempActualStringL);
					}
					switch(levelGoTo){
					case 0: //WARNING: THIS level HAS BEEN LATER ADDED-Und wen interessiert es?!
						displayCenteredTextLine(5, "Druecke OBEN um eine");
						displayCenteredTextLine(7, "Box zu bekommen.");
						displayCenteredTextLine(10, "Druecke UNTEN um eine");
						displayCenteredTextLine(12, "Box einzustortieren.");
						if(EnterButtonPress == true){
							if(gpTypeGoTo != -1){
								levelGoTo++;
								debug("screenManager","INFO","levelGoTo++");
								}else{
								doNoGo();
							}
							EnterButtonPress = false;
						}
						if(UpButtonPress == true){
							gpTypeGoTo = 0;
							UpButtonPress = false;
						}
						if(DownButtonPress == true){
							gpTypeGoTo = 1;
							DownButtonPress = false;
						}
						break;
					case 1:
						displayCenteredTextLine(5, "Druecke LINKS um links");
						displayCenteredTextLine(7, "zu arbeiten!");
						displayCenteredTextLine(10, "Druecke RECHTS um rechts");
						displayCenteredTextLine(12, "zu arbeiten!");
						if(EnterButtonPress == true){
							if(accessTypeGoTo != -1){
								levelGoTo++;
								debug("screenManager","INFO","levelGoTo++");
								}else{
								doNoGo();
							}
							EnterButtonPress = false;
						}
						if(LeftButtonPress == true){
							accessTypeGoTo = 0;
							LeftButtonPress = false;
						}
						if(RightButtonPress == true){
							accessTypeGoTo = 1;
							RightButtonPress = false;
						}
						break;
					case 2:
						displayCenteredTextLine(5, "Druecke OBEN um die naechste");
						displayCenteredTextLine(7, "Hoehe auszuwaehelen.");
						displayCenteredTextLine(10, "Druecke UNTEN um die vorheri-");
						displayCenteredTextLine(12, "ge Hoehe auszuwaehelen.");
						if(EnterButtonPress == true){
							if(HIndexGoTo != -1 && HIndexGoTo != 0){
								levelGoTo++;
								debug("screenManager","INFO","levelGoTo++");
								}else{
								doNoGo();
							}
							EnterButtonPress = false;
						}
						if(LeftButtonPress){
							levelGoTo--;
							LeftButtonPress = false;
						}
						if(UpButtonPress == true){
							if(HIndexGoTo == -1){HIndexGoTo = 0;}
							HIndexGoTo++;
							if(HIndexGoTo == 5){
								HIndexGoTo = 1;
							}
							UpButtonPress = false;
						}
						if(DownButtonPress == true){
							if(HIndexGoTo == -1){HIndexGoTo = 1;}
							HIndexGoTo--;
							if(HIndexGoTo == 0){
								HIndexGoTo = 4;
							}
							DownButtonPress = false;
						}
						break;
					case 3:
						displayCenteredTextLine(5, "Druecke OBEN um die hoehere");
						displayCenteredTextLine(7, "Laenge auszuwaehelen.");
						displayCenteredTextLine(10, "Druecke UNTEN um die niedrig-");
						displayCenteredTextLine(12, "ere Laenge auszuwaehelen.");
						if(EnterButtonPress == true){
							if(LIndexGoTo != -1 && LIndexGoTo != 0){
								levelGoTo++;
								debug("screenManager","INFO","levelGoTo++");
								}else{
								doNoGo();
							}
							EnterButtonPress = false;
						}
						if(LeftButtonPress){
							levelGoTo--;
							LeftButtonPress = false;
						}
						if(UpButtonPress == true){
							if(LIndexGoTo == -1){LIndexGoTo = 0;}
							LIndexGoTo++;
							if(LIndexGoTo == 6){
								LIndexGoTo = 1;
							}
							UpButtonPress = false;
						}
						if(DownButtonPress == true){
							if(LIndexGoTo == -1){LIndexGoTo = 1;}
							LIndexGoTo--;
							if(LIndexGoTo == 0){
								LIndexGoTo = 5;
							}
							DownButtonPress = false;
						}
						break;
					case 4:
						//_finalGUIsend _irgendwas
						sprintf(TempFinalGoToString, "%s%s",TempActualStringH, TempActualStringL);
						if(gpTypeGoTo == 1){
							sendSignal(SresetA);
							getContainer("Right");
							moveVertical("Forward");
						}
						goTo(TempFinalGoToString);
						if(accessTypeGoTo == 0){
							TempActualLRaccess = "Left";
						}
						if(accessTypeGoTo == 1){
							TempActualLRaccess = "Right";
						}
						if(gpTypeGoTo == 0){
							getContainer(TempActualLRaccess);
							sendSignal(SresetA);
							putContainer("Left");
							moveVertical("Forward");
						}
						if(gpTypeGoTo == 1){
							putContainer(TempActualLRaccess);
							sendSignal(SresetA);
						}
						wait1Msec(2500);
						inScreenType = 0; inScreen = false; levelGoTo = -1; HIndexGoTo = -1; LIndexGoTo = -1; accessTypeGoTo = -1; gpTypeGoTo = -1;
						break;
					default: inScreenType = 0; inScreen = false; levelGoTo = -1; error("Unknown goToLevel resetiST&&iS", "E_G202_0x0ca UnknownGoToLevel", "screenManager");
						break;
					}
					break;
				case 2: //_manualMovement
					if(levelMMove == 2){
						drawBmpfile(0, 125,"Accept");
						playSoundFile("Confirm");
					}
					levelMMove++;
					sprintf(TempActualStep, "Schritt %i von 2", levelMMove);
					displayCenteredTextLine(3, TempActualStep);
					levelMMove--;
					switch(levelMMove){
					case 0:
						switch(BasicTypeMMove){
						case -1:
						case 0:
							displayCenteredTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "get/putLEFT || get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							break;
						case 1:
							displayCenteredBigTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "get/putLEFT || get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							break;
						case 2:
							displayCenteredTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "!!get/putLEFT!!|get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							break;
						case 3:
							displayCenteredTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "get/putLEFT|!!get/putRIGHT!!"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							break;
						case 4:
							displayCenteredTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "get/putLEFT || get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredBigTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							break;
						default:
							error("Unknown BasicTypeMMove", "E_G211_0x0d3 UnknownBTMM", "screenManager");
							break;
						}
						if(EnterButtonPress == true){
							if(BasicTypeMMove != -1){
								levelMMove++;
								debug("screenManager","INFO","levelMM++");
								}else{
								doNoGo();
							}
							EnterButtonPress = false;
						}
						if(UpButtonPress == true){
							BasicTypeMMove = 1;
							UpButtonPress = false;
						}
						if(LeftButtonPress){
							BasicTypeMMove = 2;
							LeftButtonPress = false;
						}
						if(RightButtonPress){
							BasicTypeMMove = 3;
							RightButtonPress = false;
						}
						if(DownButtonPress == true){
							BasicTypeMMove = 4;
							DownButtonPress = false;
						}
						break;
					case 1:
						switch(BasicTypeMMove){
						case 1:
							displayCenteredBigTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "get/putLEFT || get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							if(RealTypeMMove == 1){
								displayCenteredBigTextLine(5, "!UP!/FORWARD"); //BTMM:1 (RTMM:1/2)
							}
							if(RealTypeMMove == 2){
								displayCenteredBigTextLine(5, "UP/!FORWARD!"); //BTMM:1 (RTMM:1/2)
							}
							break;
						case 2:
							displayCenteredTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "!!get/putLEFT!!|get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							if(RealTypeMMove == 3){
								displayCenteredTextLine(7, "!!getLEFT!!||get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							}
							if(RealTypeMMove == 4){
								displayCenteredTextLine(7, "!!getRIGHT!!||get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							}
							break;
						case 3:
							displayCenteredTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "get/putLEFT|!!get/putRIGHT!!"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							if(RealTypeMMove == 5){
								displayCenteredTextLine(7, "get/putLEFT||!!getLEFT!!"); //BTMM:2||3 (RTMM:3/4||5/6)
							}
							if(RealTypeMMove == 6){
								displayCenteredTextLine(7, "get/putLEFT||!!getRIGHT!!"); //BTMM:2||3 (RTMM:3/4||5/6)
							}
							break;
						case 4:
							displayCenteredTextLine(5, "UP/FORWARD"); //BTMM:1 (RTMM:1/2)
							displayCenteredTextLine(7, "get/putLEFT || get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							displayCenteredBigTextLine(9, "DOWN/BACKWARD"); //BTMM:4 (RTMM:7/8)
							if(RealTypeMMove == 7){
								displayCenteredBigTextLine(9, "!DOWN!/BACKWARD"); //BTMM:4 (RTMM:7/8)
							}
							if(RealTypeMMove == 8){
								displayCenteredBigTextLine(9, "DOWN/!BACKWARD!"); //BTMM:4 (RTMM:7/8)
							}
							break;
						}
						if(EnterButtonPress == true){
							if(RealTypeMMove != -1){
								levelMMove++;
								debug("screenManager","INFO","levelMM++");
								}else{
								doNoGo();
							}
							EnterButtonPress = false;
						}
						if(LeftButtonPress){
							switch(BasicTypeMMove){
							case 1:
								RealTypeMMove = 1;
								break;
							case 2:
								RealTypeMMove = 3;
								break;
							case 3:
								RealTypeMMove = 5;
								break;
							case 4:
								RealTypeMMove = 7;
								break;
							}
							LeftButtonPress = false;
						}
						if(RightButtonPress){
							switch(BasicTypeMMove){
							case 1:
								RealTypeMMove = 2;
								break;
							case 2:
								RealTypeMMove = 4;
								break;
							case 3:
								RealTypeMMove = 6;
								break;
							case 4:
								RealTypeMMove = 8;
								break;
							}
							RightButtonPress = false;
						}
						break;
					case 2:
						switch(BasicTypeMMove){
						case 1:
							if(RealTypeMMove == 1){
								displayCenteredBigTextLine(5, "!UP!/FORWARD"); //BTMM:1 (RTMM:1/2)
							}
							if(RealTypeMMove == 2){
								displayCenteredBigTextLine(5, "UP/!FORWARD!"); //BTMM:1 (RTMM:1/2)
							}
							break;
						case 2:
							if(RealTypeMMove == 3){
								displayCenteredTextLine(7, "!!getLEFT!!||get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							}
							if(RealTypeMMove == 4){
								displayCenteredTextLine(7, "!!getRIGHT!!||get/putRIGHT"); //BTMM:2||3 (RTMM:3/4||5/6)
							}
							break;
						case 3:
							if(RealTypeMMove == 5){
								displayCenteredTextLine(7, "get/putLEFT||!!getLEFT!!"); //BTMM:2||3 (RTMM:3/4||5/6)
							}
							if(RealTypeMMove == 6){
								displayCenteredTextLine(7, "get/putLEFT||!!getRIGHT!!"); //BTMM:2||3 (RTMM:3/4||5/6)
							}
							break;
						case 4:
							if(RealTypeMMove == 7){
								displayCenteredBigTextLine(9, "!DOWN!/BACKWARD"); //BTMM:4 (RTMM:7/8)
							}
							if(RealTypeMMove == 8){
								displayCenteredBigTextLine(9, "DOWN/!BACKWARD!"); //BTMM:4 (RTMM:7/8)
							}
							break;
						}
						wait1Msec(2500);
						inScreenType = 0; inScreen = false; levelMMove = -1; BasicTypeMMove = -1; RealTypeMMove = -1;
						break;
					default: inScreenType = 0; inScreen = false; levelMMove = -1;
						error("Unknown MMoveLevel resetiST&&iS", "E_G212_0x0d4 UnknownMMoveLevel", "screenManager");
						break;
					}
					break;
				case 3: //_sendSignal
					string TempActualSignal;
					if(levelSendSignal == 1){
						drawBmpfile(0, 125,"Accept");
						playSoundFile("Confirm");
					}
					levelSendSignal++;
					sprintf(TempActualStep, "Schritt %i von 1", levelSendSignal);
					displayCenteredTextLine(3, TempActualStep);
					levelSendSignal--;
					sprintf(TempActualSignal, "%i", SignalsendSig);
					displayCenteredBigTextLine(8, TempActualSignal);
					switch(SignalsendSig){
					case STYPE__1: displayCenteredTextLine(14, "STYPE__1");
						break;
					case STYPE__2: displayCenteredTextLine(14, "STYPE__2");
						break;
					case Signal_V: displayCenteredTextLine(14, "Signal_V");
						break;
					case Signal_H: displayCenteredTextLine(14, "Signal_H");
						break;
					case SgC: displayCenteredTextLine(14, "SgC");
						break;
					case SpC: displayCenteredTextLine(14, "SpC");
						break;
					case SresetA: displayCenteredTextLine(14, "SresetA");
						break;
					default: displayCenteredTextLine(14, "UNKNOWN_SIG");
						break;
					}
					switch(levelSendSignal){
					case 0:
						displayCenteredTextLine(5, "Druecke OBEN um ein");
						displayCenteredTextLine(7, "höheres Signal zu senden.");
						displayCenteredTextLine(10, "Druecke UNTEN um ein");
						displayCenteredTextLine(12, "niedrigeres Signal zu senden.");
						if(EnterButtonPress){
							if(levelSendSignal != -1){
								levelSendSignal++;
								debug("screenManager","INFO","levelSendSignal++");
								}else{
								doNoGo();
							}
							EnterButtonPress = false;
						}
						if(UpButtonPress){
							SignalsendSig++;
							UpButtonPress = false;
						}
						if(DownButtonPress){
							SignalsendSig--;
							DownButtonPress = false;
						}
						break;
					case 1: sendSignal(SignalsendSig);
						wait1Msec(2500); inScreenType = 0; inScreen = false; SignalsendSig = -1;
						break;
					default: inScreenType = 0; inScreen = false; levelSendSignal = 0; error("UnknownLSS resetiST&&iS", "E_G213_0x0d5", "screenManager");
						break;
					}
					break;
				case 4: //_INFO
					string TempSprintfString;
					float toDoCSigs = 0;
					float toDoCChange = 0;
					float Progress;
					string PS = "%";
					for(int i = 0; i < RealMaxCQ; i++){
						if(CSigQueque[i] != -1){
							toDoCChange++;
						}
					}
					for(int i = 0; i < RealMaxCQ; i=i+4){
						if(CSigQueque[i] != -1){
							toDoCSigs++;
						}
					}
					toDoCChange = toDoCChange-SignalPointer;
					displayCenteredBigTextLine(3, "CSQInfo:");
					int nextSigSend;
					int siglength = TickRate*5;
					short actualToDoCSigs = toDoCSigs-1;
					nextSigSend = lastSigSend - nPgmTime + siglength;
					int finishedIn = nextSigSend + actualToDoCSigs*siglength;
					finishedIn /= 1000;
					nextSigSend /= 1000;
					sprintf(TempSprintfString, "toDoCChanges:%i", toDoCChange);
					displayCenteredTextLine(5, TempSprintfString);
					sprintf(TempSprintfString, "toDoCSigs:%i", toDoCSigs);
					displayCenteredTextLine(6, TempSprintfString);
					displayCenteredTextLine(7, "finishIn:%is",finishedIn);
					Progress = toDoCChange / RealMaxCQ;
					Progress = Progress*100;
					sprintf(TempSprintfString, "Fill:%i", Progress);
					displayCenteredTextLine(8, "%s%snextSs:%is",TempSprintfString,PS,nextSigSend);
					displayCenteredBigTextLine(9, "OtherInfo:");
					displayCenteredTextLine(11, "LogLvl:%iDispRefresh:%i",logLevel,DISP_REFRESH_RATE);
					displayCenteredTextLine(12, "PgmTime:%iErrC:%i",nPgmTime,ErrCounter);
					string ignoreERRs;
					if(ignoreErrors){
						ignoreERRs = "ignoreERR:true";
					}else{
						ignoreERRs = "ignoreERR:false";
					}
					displayCenteredTextLine(13, "CTickRate:%i%s",TickRate,ignoreERRs);
					break;
				case 5: //_LOG
					for(int i = 0; i < 10; i++){
						displayCenteredTextLine(12-i,"<%s>:%s",LogStringsTypes[i],LogStrings[i]);
					}
					break;
				}
			}
			resetButtons();
			if(doResetExitAble){
				debug("GUI", LOG_LEVEL_DEBUG, "doResetExitAble");
				levelMMove = -1; BasicTypeMMove = -1; RealTypeMMove = -1;
				levelSendSignal = -1; SignalsendSig = -1;
				doResetExitAble = false;
			}
			if(NoGo){NoGo = false;}
		}
	}
}

//_buttonChecker
task buttonChecker(){
	while(true){
		wait1Msec(50);
		if(getButtonPress(LEFT_BUTTON)){
			while(getButtonPress(LEFT_BUTTON)){}
			realLeftButtonPress = true;
			if(!inScreen){
				}else{
				LeftButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(RIGHT_BUTTON)){
			while(getButtonPress(RIGHT_BUTTON)){}
			realRightButtonPress = true;
			if(!inScreen){
				}else{
				RightButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(DOWN_BUTTON)){
			while(getButtonPress(DOWN_BUTTON)){}
			realDownButtonPress = true;
			if(!inScreen){
				Selection++;
				if(Selection >= 6){
					Selection = 1;
				}
				}else{
				DownButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(UP_BUTTON)){
			while(getButtonPress(UP_BUTTON)){}
			realUpButtonPress = true;
			if(!inScreen){
				Selection--;
				if(Selection == 0){
					Selection = 1;
				}
				if(Selection <= 0){
					debug("buttonChecker", LOG_LEVEL_WARNING, "Unexcepted Selection value!");
					doResetExitAble = true;
				}
				}else{
				UpButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(ENTER_BUTTON)){
			while(getButtonPress(ENTER_BUTTON)){}
			realEnterButtonPress = true;
			if(!inScreen){
				newScreen(Selection);
				}else{
				EnterButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(BACK_BUTTON)){
			while(getButtonPress(BACK_BUTTON)){}
			if(inScreen && !blockScreenExit){
				inScreen = false;
				inScreenType = 0;
				doResetExitAble = true;
			}
			if(inScreen && blockScreenExit){
				doNoGo();
			}
		}
	}
}
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
const char *  FileName = "Data.txt";   //MAIN_FILENAME
const short   DatabaseLength = 3*3;
//const string 	debugLevel = "ALL";  //ALL, DEBUG (INFO)
string 				DEFINEStr = "VAR_STRING";
string 				DEFINEInt = "VAR_INT";
const char 	 	CR = 0x13;   //CR (carriage return)
const char  	LF = 0x10;   //LF (line feed)
long 					fileHandleMAIN;
long 					fileHandleWRITE;
string 				ReadError;
string 				rawStrings[DatabaseLength]; //FIRST level of File readout (THERE IS NO -1)
string 				ReadStringResult;
int 					ReadIntResult;

void WriteDataBase(bool DoExecute)
{
	if(DoExecute==true)
	{
		//VARIABLE1
		char *        DEFINE_1 = "VAR_INT";
		char *        NAME_1 = "LastState";
		char *        VALUE_1 = "0";

		//VARIABLE2
		char *        DEFINE_2 = "VAR_STRING";
		char *        NAME_2 = "TestString";
		char *        VALUE_2 = "Works";

		//VARIABLE3
		char *        DEFINE_3 = "VAR_STRING";
		char *        NAME_3 = "AnotherString";
		char *        VALUE_3 = "HALLO_:)";
		//##################################################################################
		//fileDelete("Data.txt",nIOResult);
		fileHandleWRITE = fileOpenWrite(FileName);
		//##################################################################################
		fileWriteData(fileHandleWRITE, "DATABASE_MAIN_ev3", strlen("DATABASE_MAIN_ev3"));

		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);

		fileWriteData(fileHandleWRITE, DEFINE_1, strlen(DEFINE_1));
		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);
		fileWriteData(fileHandleWRITE, NAME_1, strlen(NAME_1));
		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);
		fileWriteData(fileHandleWRITE, VALUE_1, strlen(VALUE_1));

		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);

		fileWriteData(fileHandleWRITE, DEFINE_2, strlen(DEFINE_2));
		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);
		fileWriteData(fileHandleWRITE, NAME_2, strlen(NAME_2));
		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);
		fileWriteData(fileHandleWRITE, VALUE_2, strlen(VALUE_2));

		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);

		fileWriteData(fileHandleWRITE, DEFINE_3, strlen(DEFINE_3));
		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);
		fileWriteData(fileHandleWRITE, NAME_3, strlen(NAME_3));
		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);
		fileWriteData(fileHandleWRITE, VALUE_3, strlen(VALUE_3));
		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);
		//##################################################################################
		fileClose(fileHandleWRITE);
	}
}

bool ReadString(bool DoExecute, const string VarName)
{
	bool Error = false;
	if(DoExecute==true)
	{
		fileHandleMAIN = fileOpenRead(FileName); //OpenFile (DoGetIfNotExist->CANCELwithCritical)
		bool EndLoop = false;
		short Index = 0;
		short FileIndex = -1; //(First chars returned should be the header, written by the original writeDatabase() function)
		//string readDataStr;
		char rawChar; //ROOT level of File readout
		string readOutStrFileHeader; //SECOUND-FileHeader level of File readout
		bool readOutHeaderStarted = false;
		//##################################################################################
		while (fileReadChar(fileHandleMAIN, &rawChar))
		{
			if(rawChar == CR)
			{
				fileReadChar(fileHandleMAIN, &rawChar);
				if(rawChar == LF)
				{
					if(FileIndex == -1)
					{
						debug("ReadString", LOG_LEVEL_DEBUG, readOutStrFileHeader);
					}
					FileIndex++;
				}
			}
			else
			{
				if(FileIndex == -1) //READOUT_HEADER
				{
					if(!readOutHeaderStarted)
					{
						debug("ReadString", "INFO", "Reading Header");
					}
					readOutHeaderStarted = true;
					readOutStrFileHeader += rawChar;
				}
				else
				{
					rawStrings[FileIndex] += rawChar;
				}
			}
		}
		debug("ReadString", LOG_LEVEL_DEBUG, "Listing rawStrings...");
		short IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			string TempSprintfString;
			sprintf(TempSprintfString,"%i %s",IndexDebugrS,rawStrings[IndexDebugrS]);
			valueDebug("ReadString", TempSprintfString);
			IndexDebugrS++;
		}
		while(EndLoop == false) //Search for our variable
		{
			if(Index > DatabaseLength)
			{
				Error = true;
				EndLoop = true;
				ReadError = "strFnotFound";
			}
			else
			{
				if(rawStrings[Index] == DEFINEStr)
				{
					debug("ReadString", LOG_LEVEL_DEBUG, "FoundDefStr");
					Index++;
					if(rawStrings[Index] == VarName)
					{
						debug("ReadString", "INFO", "SUCESS! Found string!");
						Index++;
						ReadStringResult = rawStrings[Index];
						valueDebug("ReadString",VarName);
						valueDebug("ReadString",ReadStringResult);
						EndLoop = true;
					}
				}
				else
				{
					Index++;
				}
			}
		}
		if(Error == true)
		{
			debug("ReadString", "ERROR", ReadError);
			valueDebug("ReadString",VarName);
		}
		IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			rawStrings[IndexDebugrS] = "";
			IndexDebugrS++;
		}
		//##################################################################################
		fileClose(fileHandleMAIN);                   //SAVE
	}
	return Error;
}

bool ReadInt(bool DoExecute, const string VarName)
{
	bool Error = false;
	if(DoExecute==true)
	{
		fileHandleMAIN = fileOpenRead(FileName); //OpenFile (DoGetIfNotExist->CANCELwithCritical)
		bool EndLoop = false;
		short Index = 0;
		short FileIndex = -1; //(First chars returned should be the header, written by the original writeDatabase() function)
		//string readDataStr;
		char rawChar; //ROOT level of File readout
		string readOutStrFileHeader; //SECOUND-FileHeader level of File readout
		bool readOutHeaderStarted = false;
		//##################################################################################
		while (fileReadChar(fileHandleMAIN, &rawChar))
		{
			if(rawChar == CR)
			{
				fileReadChar(fileHandleMAIN, &rawChar);
				if(rawChar == LF)
				{
					if(FileIndex == -1)
					{
						debug("ReadInt", LOG_LEVEL_DEBUG, readOutStrFileHeader);
					}
					FileIndex++;
				}
			}
			else
			{
				if(FileIndex == -1) //READOUT_HEADER
				{
					if(!readOutHeaderStarted)
					{
						debug("ReadInt", "INFO", "Reading Header");
					}
					readOutHeaderStarted = true;
					readOutStrFileHeader += rawChar;
				}
				else
				{
					rawStrings[FileIndex] += rawChar;
				}
			}
		}
		debug("ReadInt", LOG_LEVEL_DEBUG, "Listing rawStrings...");
		short IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			string TempSprintfString;
			sprintf(TempSprintfString,"%i %s",IndexDebugrS,rawStrings[IndexDebugrS]);
			valueDebug("ReadInt", TempSprintfString);
			IndexDebugrS++;
		}
		while(EndLoop == false) //Search for our variable
		{
			if(Index > DatabaseLength)
			{
				Error = true;
				EndLoop = true;
				ReadError = "intFnotFound";
			}
			else
			{
				if(rawStrings[Index] == DEFINEInt)
				{
					debug("ReadInt", LOG_LEVEL_DEBUG, "FoundDefStr");
					Index++;
					if(rawStrings[Index] == VarName)
					{
						debug("ReadInt", "INFO", "SUCESS! Found int!");
						Index++;
						ReadIntResult = atoi(rawStrings[Index]);
						valueDebug("ReadInt",VarName);
						string TempSprintfString;
						sprintf(TempSprintfString,"%i %s",Index,rawStrings[Index]);
						valueDebug("ReadInt",TempSprintfString);
						EndLoop = true;
					}
				}
				else
				{
					Index++;
				}
			}
		}
		if(Error == true)
		{
			debug("ReadInt", "ERROR", ReadError);
			valueDebug("ReadInt",VarName);
		}
		IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			rawStrings[IndexDebugrS] = "";
			IndexDebugrS++;
		}
		//##################################################################################
		fileClose(fileHandleMAIN);                   //SAVE
	}
	return Error;
}

bool WriteInt(bool DoExecute, const string VarName,const int VarRawValue)
{
	bool Error = false;
	if(DoExecute==true)
	{
		string TempSprintfString;
		sprintf(TempSprintfString, "%i", VarRawValue);
		string VarValue = TempSprintfString;
		fileHandleMAIN = fileOpenRead(FileName); //OpenFile (DoGetIfNotExist->CANCELwithCritical)
		bool EndLoop = false;
		short Index = 0;
		short FileIndex = -1; //(First chars returned should be the header, written by the original writeDatabase() function)
		string readDataStr;
		char rawChar; //ROOT level of File readout
		string readOutStrFileHeader; //SECOUND-FileHeader level of File readout
		bool readOutHeaderStarted = false;
		//##################################################################################
		while (fileReadChar(fileHandleMAIN, &rawChar))
		{
			if(rawChar == CR)
			{
				fileReadChar(fileHandleMAIN, &rawChar);
				if(rawChar == LF)
				{
					if(FileIndex == -1)
					{
						debug("ReadInt", LOG_LEVEL_DEBUG, readOutStrFileHeader);
					}
					FileIndex++;
				}
			}
			else
			{
				if(FileIndex == -1) //READOUT_HEADER
				{
					if(!readOutHeaderStarted)
					{
						debug("ReadInt", "INFO", "Reading Header");
					}
					readOutHeaderStarted = true;
					readOutStrFileHeader += rawChar;
				}
				else
				{
					rawStrings[FileIndex] += rawChar;
				}
			}
		}
		//##################################################################################
		debug("WriteInt", LOG_LEVEL_DEBUG, "Listing rawStrings [AT1]");
		short IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			sprintf(TempSprintfString,"%i %s",IndexDebugrS,rawStrings[IndexDebugrS]);
			valueDebug("ReadInt", TempSprintfString);
			IndexDebugrS++;
		}
		while(EndLoop == false) //Search for our variable
		{
			if(Index > DatabaseLength)
			{
				Error = true;
				EndLoop = true;
				ReadError = "intFnotFound";
			}
			else
			{
				if(rawStrings[Index] == DEFINEInt)
				{
					Index++;
					if(rawStrings[Index] == VarName)
					{
						Index++;
						EndLoop = true;
					}
				}
				else
				{
					Index++;
				}
			}
		}
		if(Error == true)
		{
			debug("WriteInt", LOG_LEVEL_DEBUG, "NewVariable");
			valueDebug("WriteInt",VarName);
		}
		fileClose(fileHandleMAIN);
		//##################################################################################
		//fileDelete("Data.txt",nIOResult);
		fileHandleWRITE = fileOpenWrite(FileName);
		if(Error == true){
			char *        DEFINE_New = DEFINEInt;
			char *        NAME_New = VarName;
			char *        VALUE_New = VarValue;
			//##################################################################################
			fileWriteData(fileHandleWRITE, "DATABASE_MAIN_ev3", strlen("DATABASE_MAIN_ev3"));

			fileWriteChar(fileHandleWRITE, CR);
			fileWriteChar(fileHandleWRITE, LF);

			fileWriteData(fileHandleWRITE, DEFINE_New, strlen(DEFINE_New));
			fileWriteChar(fileHandleWRITE, CR);
			fileWriteChar(fileHandleWRITE, LF);
			fileWriteData(fileHandleWRITE, NAME_New, strlen(NAME_New));
			fileWriteChar(fileHandleWRITE, CR);
			fileWriteChar(fileHandleWRITE, LF);
			fileWriteData(fileHandleWRITE, VALUE_New, strlen(VALUE_New));
			for(int i = 0; i < DatabaseLength; i++)
			{
				fileWriteChar(fileHandleWRITE, CR);
				fileWriteChar(fileHandleWRITE, LF);
				char * NoERRWrite = rawStrings[i];
				fileWriteData(fileHandleWRITE, NoERRWrite, strlen(NoERRWrite));
			}
		}
		else
		{
			fileWriteData(fileHandleWRITE, "DATABASE_MAIN_ev3", strlen("DATABASE_MAIN_ev3"));
			for(int i = 0; i < DatabaseLength; i++)
			{
				fileWriteChar(fileHandleWRITE, CR);
				fileWriteChar(fileHandleWRITE, LF);
				char * ERRfalseWrite;
				if(i == Index){
					ERRfalseWrite = VarValue;
					}else{
					ERRfalseWrite = rawStrings[i];
				}
				fileWriteData(fileHandleWRITE, ERRfalseWrite, strlen(ERRfalseWrite));
			}
		}
		fileWriteChar(fileHandleWRITE, CR);
		fileWriteChar(fileHandleWRITE, LF);
		fileClose(fileHandleWRITE);
		IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			rawStrings[IndexDebugrS] = "";
			IndexDebugrS++;
		}
		fileHandleMAIN = fileOpenRead(FileName); //OpenFile (DoGetIfNotExist->CANCELwithCritical)
		EndLoop = false;
		FileIndex = -1; //(First chars returned should be the header, written by the write() functions)
		readDataStr = "";
		char JustAnotherHack;
		rawChar = JustAnotherHack; //ROOT level of File readout
		readOutStrFileHeader = ""; //SECOUND-FileHeader level of File readout
		readOutHeaderStarted = false;
		//##################################################################################
		while (fileReadChar(fileHandleMAIN, &rawChar))
		{
			if(rawChar == CR)
			{
				fileReadChar(fileHandleMAIN, &rawChar);
				if(rawChar == LF)
				{
					if(FileIndex == -1)
					{
						debug("WriteInt", LOG_LEVEL_DEBUG, readOutStrFileHeader);
					}
					FileIndex++;
				}
			}
			else
			{
				if(FileIndex == -1) //READOUT_HEADER
				{
					if(!readOutHeaderStarted)
					{
						debug("WriteInt", "INFO", "Reading Header");
					}
					readOutHeaderStarted = true;
					readOutStrFileHeader += rawChar;
				}
				else
				{
					rawStrings[FileIndex] += rawChar;
				}
			}
		}
		//##################################################################################
		fileClose(fileHandleMAIN);
		debug("ReadInt", LOG_LEVEL_DEBUG, "Listing rawStrings...[AT2]");
		IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			sprintf(TempSprintfString,"%i %s",IndexDebugrS,rawStrings[IndexDebugrS]);
			valueDebug("ReadInt", TempSprintfString);
			IndexDebugrS++;
		}
		IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			rawStrings[IndexDebugrS] = "";
			IndexDebugrS++;
		}
	}
	return false;
}
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
	setSoundVolume(5);
	playImmediateTone(500, 100);
	isBooting = true;
	displayCenteredTextLine(2, "root_booting");
	string TempSprintfString;
	sprintf(TempSprintfString, "RealMaxCQ:%i", RealMaxCQ);
	debug("main", LOG_LEVEL_DEBUG, TempSprintfString);
	startTask(displayManager);
	SensorType[COLOUR] = sensorColorNxtNONE;
	wait1Msec(1000);
	for(int i = 0; i < RealMaxCQ; i++){
		CSigQueque[i] = -1;
	}
	addToBackTrace(38, "1", "2", "3", "4");
	addToBackTrace(138, "1", "2", "3", "4");
	addToBackTrace(338, "1", "2", "3", "4");
	addToBackTrace(358, "1", "2", "3", "4");
	addToBackTrace(328, "1", "2", "3", "4");
	addToBackTrace(368, "1", "2", "3", "4");
	addToBackTrace(338, "1", "2", "3", "4");
	addToBackTrace(438, "1", "2", "3", "4");
	addToBackTrace(338, "1", "2", "3", "4444");
	addToBackTrace(338, "ABCDEF", "ABCDEF", "ABCDEF", "ABCDEF");
	printBackTrace();
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
	startTask(colourSender);
	setLEDColor(ledGreen);
	isBooting = false;
	startTask(buttonChecker);
	while(!doExitProgramm)
	{
		wait1Msec(1000);
		float toDoCChange;
		float Progress;
		for(int i = 0; i < RealMaxCQ; i++){
			if(CSigQueque[i] != -1){
				toDoCChange++;
			}
		}
		Progress = toDoCChange / RealMaxCQ;
		Progress = Progress*100;
		if(Progress >= 100){
			//debug("FillState of CSigQueque is too high!","Fill of CSQ too high!","displayManager::Info");
		}
	}
	debug("main", "INFO", "EXITING NOW!");
}
//OldDatabaseHandling:
/*
setLEDColor(ledGreenPulse);
displayTextLine(0, "Writing DataBase...");
debug("MAIN", LOG_LEVEL_DEBUG, "WriteDataBase(true)");
WriteDataBase(true);
displayTextLine(1, "Reading DataBase...");
debug("MAIN", LOG_LEVEL_DEBUG, "ReadString(true,TestString)");
if(ReadString(true, "TestString") == true){error("rS returned ERROR=true", "rS returned true", "MAIN");}
else
{
playSound(soundBeepBeep);
setLEDColor(ledGreen);
}
wait1Msec(1000);
displayTextLine(2, "Writing DataBase...");
WriteDataBase(true);
debug("MAIN", LOG_LEVEL_DEBUG, "WriteDataBase(true)");
wait1Msec(1000);
debug("MAIN", LOG_LEVEL_DEBUG, "ReadString(true,AnSt)");
displayTextLine(4, "Reading DataBase...");
if(ReadString(true, "AnotherString") == true){error("rS returned ERROR=true", "rSreturnedTrue", "MAIN");}
else
{
playSound(soundBeepBeep);
setLEDColor(ledGreen);
}
wait1Msec(1000);
debug("MAIN", LOG_LEVEL_DEBUG, "ReadInt(true,LS)");
if(ReadInt(true, "LastState") == true){error("rI returned ERROR=true", "rIreturnedTrue", "MAIN");}
else
{
playSound(soundBeepBeep);
setLEDColor(ledGreen);
}
wait1Msec(1000);
debug("MAIN", LOG_LEVEL_DEBUG, "WriteInt(true,LS,10)");
WriteInt(true, "LastState", 10);
wait1Msec(1000);
debug("MAIN", LOG_LEVEL_DEBUG, "ReadInt(true,LS)");
if(ReadInt(true, "LastState") == true){error("rI returned ERROR=true", "rIreturnedTrue", "MAIN");}
else
{
playSound(soundBeepBeep);
setLEDColor(ledGreen);
}
wait1Msec(1000);
debug("MAIN", LOG_LEVEL_DEBUG, "WriteInt(true,AT,20)");
//WriteInt(true, "AnotherTest", 20);
wait1Msec(1000);
debug("MAIN", LOG_LEVEL_DEBUG, "ReadInt(true,LS)");
if(ReadInt(true, "AnotherTest") == true){error("rI returned ERROR=true", "rIreturnedTrue", "MAIN");}
else
{
playSound(soundBeepBeep);
setLEDColor(ledGreen);
}
wait1Msec(1000);
*/
