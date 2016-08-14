task displayManager;
task buttonChecker;
//BASIC GUI
const short rowLength = 9+9+9;
string calcFillResult;
//UselessStuff
bool isBooting;
//MAIN ScreenSelection
short Selection = 0;
//Hab vergessen wofür dass war...
string CustomPos1;
//Basic Screen-related STUFF
bool inScreen = false;
short inScreenType = 0;
bool blockScreenExit = false;
//GLOABAL Variables for SCREENS
short levelGoTo = -1;
short levelMMove;
//UselessStuff
bool NoGo;
bool NoGo2;
bool doResetExitAble;
//Buttons (Only if inScreen = true)
bool LeftButtonPress;
bool RightButtonPress;
bool UpButtonPress;
bool DownButtonPress;
bool EnterButtonPress;

void debug(const string ToFIdentifier,const string Type , const string DebugString)
{
	//if(debugLevel == "ALL" || debugLevel == "DEBUG" && Type == "DEBUG" || debugLevel == "DEBUG" && Type == "INFO" || Type == "INFO" || Type == "ERROR")
	{
		writeDebugStream(ToFIdentifier);
		writeDebugStream(" <");
		writeDebugStream(Type);
		writeDebugStream(">: ");
		writeDebugStream(DebugString);
		writeDebugStreamLine("");
	}
}

void error(const string Desc, const string ShortDesc, const string TaskFunction)
{
	writeDebugStream("[ERROR-REPORTER] ");
	debug(TaskFunction, "ERROR", Desc);
	writeDebugStream("[ERROR-REPORTER] ");
	displayCenteredBigTextLine(5, "ERROR");
	displayCenteredTextLine(7, "ShortDes: %s", ShortDesc);
	displayCenteredTextLine(8, "More Info -> DebugStream!");
	setLEDColor(ledRedFlash);
	playSound(soundException);
	playSound(soundException);
	playSound(soundException);
	setUserExceptionType1();
}

task main()
{
	isBooting = true;
	startTask(displayManager);
	wait1Msec(2000);
	setLEDColor(ledGreen);
	isBooting = false;
	startTask(buttonChecker);
	while(true){}
}

void resetButtons(){
	LeftButtonPress = false;
	RightButtonPress = false;
	UpButtonPress = false;
	DownButtonPress = false;
	EnterButtonPress = false;
}

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
void newScreen(short Type){
	setBlockBackButton(true);
	inScreen = true;
	blockScreenExit = false;
	inScreenType = Type;
	switch(Selection){
	case 1: displayCenteredBigTextLine(2, "LOADING..."); blockScreenExit = true; levelGoTo = 0;
		break;
	case 2: displayCenteredBigTextLine(5, "LOADING...");
		break;
	case 3: displayCenteredBigTextLine(7, "LOADING..."); blockScreenExit = true;
		break;
	case 4: displayCenteredBigTextLine(9, "LOADING...");
		break;
	case 5: displayCenteredBigTextLine(11, "LOADING...");
		break;
	default: inScreenType = 0; inScreen = false; error("E_graphical101 Unknown Selection/ScreenType resetiST&&iS", "E_G101 Unknown ScreenType", "newScreen");
		break;
	}
}

task displayManager(){
	//goTo Local(++)Variables
	short gpTypeGoTo = -1;
	short accessTypeGoTo = -1;
	short HIndexGoTo = -1;
	short LIndexGoTo = -1;
	//MMove Local(++)Variables
	short BasicTypeMMove = -1;
	short RealTypeMMove = -1;
	while(true){
		wait1Msec(500);
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
		string Pos3 = "CPos:%i";
		calcFill(strlen(Pos1));
		string Fill1 = calcFillResult;
		calcFill(strlen(Pos2));
		string Fill2 = calcFillResult;
		displayCenteredTextLine(1, "%s%s|%s%s|%s", Pos1,Fill1,Pos2,Fill2,Pos3);
		if(!isBooting && !inScreen && !NoGo){
			setLEDColor(ledGreen);
			//setBlockBackButton(false);
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
			}else{
			if(isBooting){displayCenteredBigTextLine(7, "BOOTING...");}
		}
		Pos1 = "JuFo-EV3";
		Pos2 = "Hochregallager";
		Pos3 = "0.6";
		calcFill(strlen(Pos1));
		Fill1 = calcFillResult;
		calcFill(strlen(Pos2));
		Fill2 = calcFillResult;
		displayCenteredTextLine(15, "%s%s|%s%s|%s", Pos1,Fill1,Pos2,Fill2,Pos3);
		string TempActualStep;
		if(!isBooting && inScreen && !NoGo){
			setBlockBackButton(true);
			if(!NoGo2){
				setLEDColor(ledOrange);
			}
			switch(inScreenType){
			case 1: //goTo
				string TempActualGPType;
				string TempActualLRaccess;
				string TempActualStringH;
				string TempActualStringL;
				string TempFinalGoToString;
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
				default: //debug("displayManager::parseMaingoTo","DEBUG","ERR_G-02goTo_WrongVal:HIndexGoTo");
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
				case 0: //WARNING: THIS PAGE HAS BEEN LATER ADDED Und wen interessiert es?!
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
						LeftButtonPress = false;
					}
					if(DownButtonPress == true){
						gpTypeGoTo = 1;
						RightButtonPress = false;
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
					sprintf(TempFinalGoToString, "%s%s",TempActualStringH, TempActualStringL);
					if(gpTypeGoTo == 1){
						//getContainer(Right);
					}
					//goTo(TempFinalGoToString);
					if(accessTypeGoTo == 0){
						TempActualLRaccess = "Left";
					}
					if(accessTypeGoTo == 1){
						TempActualLRaccess = "Right";
					}
					if(gpTypeGoTo == 0){
						//getContainer(TempActualLRaccess);
						//sendSignal(SresetA);
						//putContainer(Left);
					}
					if(gpTypeGoTo == 1){
						//putContainer(TempActualLRaccess);
						//sendSignal(SresetA);
					}
					wait1Msec(2500);
					inScreenType = 0; inScreen = false; levelGoTo = -1; HIndexGoTo = -1; LIndexGoTo = -1; accessTypeGoTo = -1;
					break;
				default: inScreenType = 0; inScreen = false; levelGoTo = -1; error("E_graphical201 Unknown goToLevel resetiST&&iS", "E_G201 Unknown goToLevel", "screenManager");
					break;
				}
				resetButtons();
				break;
			case 2: //manualMovement
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
						error("E_graphical302 Unknown BasicTypeMMove", "301 Unknown BTMM", "screenManager");
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
					inScreenType = 0; inScreen = false; levelMMove = 0; BasicTypeMMove = -1; RealTypeMMove = -1;
					break;
				default: inScreenType = 0; inScreen = false; levelMMove = 0; error("E_graphical301 Unknown MMoveLevel resetiST&&iS", "E_G201 Unknown MMoveLevel", "screenManager");
					break;
				}
				break;
			case 3: displayCenteredTextLine(7, "ASAP TODO");//sendSignal
				break;
			case 4: displayCenteredTextLine(7, "Still TODO"); //INFO
				break;
			case 5: displayCenteredTextLine(7, "Still TODO"); //LOG
				break;
			}
		}
		if(doResetExitAble){
			levelMMove = 0; BasicTypeMMove = -1; RealTypeMMove = -1;
			doResetExitAble = false;
		}
		if(NoGo){NoGo = false;}
	}
}

task buttonChecker(){
	while(true){
		wait1Msec(50);
		if(getButtonPress(LEFT_BUTTON)){
			while(getButtonPress(LEFT_BUTTON)){}
			if(!inScreen){
				}else{
				LeftButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(RIGHT_BUTTON)){
			while(getButtonPress(RIGHT_BUTTON)){}
			if(!inScreen){
				}else{
				RightButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(DOWN_BUTTON)){
			while(getButtonPress(DOWN_BUTTON)){}
			if(!inScreen){
				Selection++;
				if(Selection == 6){
					Selection = 1;
				}
				}else{
				DownButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(UP_BUTTON)){
			while(getButtonPress(UP_BUTTON)){}
			if(!inScreen){
				Selection--;
				if(Selection == 0){
					Selection = 1;
				}
				}else{
				UpButtonPress = true;
				displayCenteredBigTextLine(7, "LOADING...");
			}
		}
		if(getButtonPress(ENTER_BUTTON)){
			while(getButtonPress(ENTER_BUTTON)){}
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
