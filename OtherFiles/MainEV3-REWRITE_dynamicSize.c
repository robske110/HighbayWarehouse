const char *  FileName = "Data.txt";   //MAIN_FILENAME
short   DatabaseLength = 9;
//const string 	debugLevel = "ALL";  //ALL, DEBUG (INFO)
//##################################################################################
string 				DEFINEStr = "VAR_STRING";
string 				DEFINEInt = "VAR_INT";
const char 	 	CR = 0x13;   //CR (carriage return)
const char  	LF = 0x10;   //LF (line feed)
//##################################################################################
long 					fileHandleMAIN;
long 					fileHandleWRITE;
string 				ReadError;
//string 				rawStrings[DatabaseLength];
string 				ReadStringResult;
int 					ReadIntResult;
//##################################################################################
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
						debug("ReadString", "DEBUG", readOutStrFileHeader);
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
		debug("ReadString", "Debug", "Listing rawStrings...");
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
					debug("ReadString", "Debug", "FoundDefStr");
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
						debug("ReadInt", "DEBUG", readOutStrFileHeader);
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
		debug("ReadInt", "Debug", "Listing rawStrings...");
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
					debug("ReadInt", "Debug", "FoundDefStr");
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
						debug("ReadInt", "DEBUG", readOutStrFileHeader);
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
		debug("WriteInt", "Debug", "Listing rawStrings [AT1]");
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
			debug("WriteInt", "DEBUG", "NewVariable");
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
			for(int i = 0; i >= DatabaseLength; i++)
			{
				fileWriteChar(fileHandleWRITE, CR);
				fileWriteChar(fileHandleWRITE, LF);
				fileWriteData(fileHandleWRITE, rawStrings[Index], strlen(rawStrings[Index]));
			}
		}
		else
		{
			fileWriteData(fileHandleWRITE, "DATABASE_MAIN_ev3", strlen("DATABASE_MAIN_ev3"));
			for(int i = 0; i >= DatabaseLength; i++)
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
						debug("WriteInt", "DEBUG", readOutStrFileHeader);
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
		debug("ReadInt", "Debug", "Listing rawStrings...[AT2]");
		IndexDebugrS = 0;
		while(IndexDebugrS < DatabaseLength)
		{
			string TempSprintfString;
			sprintf(TempSprintfString,"%i %s",IndexDebugrS,rawStrings[IndexDebugrS]);
			valueDebug("ReadInt", TempSprintfString);
			IndexDebugrS++;
		}
		while(IndexDebugrS < DatabaseLength)
		{
			rawStrings[IndexDebugrS] = "";
			IndexDebugrS++;
		}
	}
	return false;
}

void error(const string Desc, const string ShortDesc, const string TaskFunction)
{
	writeDebugStream("[ERROR-REPORTER] ");
	debug(TaskFunction, "ERROR", Desc);
	writeDebugStream("[ERROR-REPORTER] ");
	debug(TaskFunction, "ERROR", "Listing errors:");
	debug("[ERROR-REPORTER]", "Read", ReadError);
	displayCenteredBigTextLine(5, "ERROR");
	displayTextLine(7, "ShortDes: %s", ShortDesc);
	displayTextLine(8, "Please consider the DebugStream!");
	setLEDColor(ledRedFlash);
	playSound(soundException);
	setUserExceptionType1();
}

//##################################################################################

task main()
{
	setLEDColor(ledGreenPulse);
	displayTextLine(0, "Writing DataBase...");
	debug("MAIN", "Debug", "WriteDataBase(true)");
	WriteDataBase(true);
	displayTextLine(1, "Reading DataBase...");
	debug("MAIN", "Debug", "ReadString(true,TestString)");
	if(ReadString(true, "TestString") == true){error("rS returned ERROR=true", "rS returned true", "MAIN");}
	else
	{
		playSound(soundBeepBeep);
		setLEDColor(ledGreen);
	}
	wait1Msec(1000);
	displayTextLine(2, "Writing DataBase...");
	WriteDataBase(true);
	debug("MAIN", "Debug", "WriteDataBase(true)");
	wait1Msec(1000);
	debug("MAIN", "Debug", "ReadString(true,AnSt)");
	displayTextLine(4, "Reading DataBase...");
	if(ReadString(true, "AnotherString") == true){error("rS returned ERROR=true", "rSreturnedTrue", "MAIN");}
	else
	{
		playSound(soundBeepBeep);
		setLEDColor(ledGreen);
	}
	wait1Msec(1000);
	debug("MAIN", "Debug", "ReadInt(true,LS)");
	if(ReadInt(true, "LastState") == true){error("rI returned ERROR=true", "rIreturnedTrue", "MAIN");}
	else
	{
		playSound(soundBeepBeep);
		setLEDColor(ledGreen);
	}
	wait1Msec(1000);
	debug("MAIN", "Debug", "WriteInt(true,LS,10)");
	WriteInt(true, "LastState", 10);
	wait1Msec(1000);
	debug("MAIN", "Debug", "ReadInt(true,LS)");
	if(ReadInt(true, "LastState") == true){error("rI returned ERROR=true", "rIreturnedTrue", "MAIN");}
	else
	{
		playSound(soundBeepBeep);
		setLEDColor(ledGreen);
	}
	while(true);
}
