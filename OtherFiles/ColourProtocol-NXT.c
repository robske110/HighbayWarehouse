#pragma config(Sensor, S1,     COLOUR,         sensorColorNxtNONE)
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

//Settings
const short TickRate = 5000; //ColourChangeRate //INT::ChToShortOn_2113_18032016
const short SignalCount = 12; //6
const short SignalLength = 4;
const short MaxCSigsInQ = 24;
//SHORTs
short CSigQueque[4*MaxCSigsInQ];
const short RealMaxCQ = 4*MaxCSigsInQ;
//BOOLs
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
	stopTask(colourSender);
	wait1Msec(10000);
	debug("[ERROR-REPORTER]", "WARNING", "5secEXIT");
	wait1Msec(5000);
	debug("[ERROR-REPORTER]", "WARNING", "WILL_EXIT_NOW");
	stopAllTasks();
}

bool addCSigToQueque(short C1,short C2,short C3,short C4){
	bool AlreadyWrote = false;
	string TempSprintfString;
	for(int i = 3; i < RealMaxCQ; i=i+4){
		if(CSigQueque[i] == -1 && AlreadyWrote == false){
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
	playSound(soundFastUpwardTones);
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
	default: error("gotUnknownSignal","E_001","sendSignal");
		break;
	}
	if(!addCSigToQueque(TEMP_PedCPS[0],TEMP_PedCPS[1],TEMP_PedCPS[2],TEMP_PedCPS[3])){
		error("CSigQuequeIsFull","E_003","sendSignal");
		//wait1Msec(TickRate);
	}
}

task main()
{
	displayCenteredTextLine(2, "FarbSensor laedt");
	SensorType[COLOUR] = sensorColorNxtNONE;
	wait1Msec(1000);
	for(int i = 0; i < RealMaxCQ; i++){
		CSigQueque[i] = -1;
	}
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
	startTask(colourSender);
	debug("main::ListQueque", "DEBUG", "Contents of CSigQueque:");
	for(int i = 0; i < RealMaxCQ; i++){
		string TempSprintfString;
		sprintf(TempSprintfString, "QID: %i QVa: %i", i, CSigQueque[i]);
		writeDebugStream(TempSprintfString);
		if(i != RealMaxCQ-1){
			writeDebugStream(" , ");
		}
	}
	writeDebugStreamLine("");
	eraseDisplay();
	while(ExitProgramm == false)
	{
		writeDisplay(true,CurrentC,2,1);
	}
}
//0 = ALL
//1 = RED
//2 = BLUE
//3 = GREEN
//(-1=NULL)
void changeC(short Colour){
	if(Colour == -1){
		error("Got-1E_002","Got-1","changeC");
	}
	if(Colour == 1){
		SensorType[COLOUR] = sensorColorNxtRED;
		CurrentC = "RED";
	}
	if(Colour == 2){
		SensorType[COLOUR] = sensorColorNxtBLUE;
		CurrentC = "BLUE";
	}
	if(Colour == 3){
		SensorType[COLOUR] = sensorColorNxtGREEN;
		CurrentC = "GREEN";
	}
	if(Colour == 0){
		SensorType[COLOUR] = sensorColorNxtFULL;
		CurrentC = "ALL";
	}
	if(Colour != 0){
		string TempSprintfString;
		sprintf(TempSprintfString, "Colour: %i", Colour);
		debug("colourSender::changeC","DEBUG",TempSprintfString);
	}
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
		debug("cS::rem4Elements","DEBUG",TempSprintfString);
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
	short SignalPointer = 0;
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
					remFourElements();
					debug("colourSender::endSS", "DEBUG", "Contents of CSigQueque:");
					for(int i = 0; i < RealMaxCQ; i++){
						string TempSprintfString;
						sprintf(TempSprintfString, "QID: %i QVa: %i", i, CSigQueque[i]);
						writeDebugStream(TempSprintfString);
						if(i != RealMaxCQ-1){
							writeDebugStream(" , ");
						}
					}
					writeDebugStreamLine("");
				}
			}
		}
	}
}
//326LinesOfEV3CP
