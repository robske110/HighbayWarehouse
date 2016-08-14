const string sFileName = "TestDATA.dat";
const int kMaxFileSize = 5000;
int Stuff;

TFileHandle hFileWriteHandle = NOT_A_HANDLE;

bool bFirstNumberOnLine      = true;
TFileIOResult nIoResult      = ioRsltSuccess;

bool bOpenWriteTextFile(const string &sFileName, int nFileSize)
{
	bFirstNumberOnLine = true;
	Delete(sFileName, nIoResult);
	OpenWrite(hFileWriteHandle, nIoResult, sFileName, nFileSize);
	return nIoResult == ioRsltSuccess;
}

void closeWriteTextFile()
{
	Close(hFileWriteHandle, nIoResult);
	hFileWriteHandle = NOT_A_HANDLE;
}

void writeNewLine()
{
	WriteText(hFileWriteHandle, nIoResult, "\r\n");
	bFirstNumberOnLine = true;
	return;
}

void writeDelimiterBetweenNumbers()
{
	if (bFirstNumberOnLine)
		bFirstNumberOnLine = false;
	else
		WriteText(hFileWriteHandle, nIoResult, ", ");
	return;
}

void writeIntegerNumber(long nNumber)
{
	string sTemp;
	writeDelimiterBetweenNumbers();
	//
	// Modify format code ("%d") if you want to change the format -- say to line up the
	// columns for your application; e.g. "%5d" will make every number five characters.
	//
	stringFormat(sTemp, "%d", (long) nNumber);
	WriteText(hFileWriteHandle, nIoResult, sTemp);
	return;
}

task main ()
{

  nxtDisplayCenteredTextLine(0, "Dexter Ind.");
  nxtDisplayCenteredBigTextLine(1, "GPS");
  nxtDisplayCenteredTextLine(3, "Test 1");
  nxtDisplayCenteredTextLine(5, "Connect sensor");
  nxtDisplayCenteredTextLine(6, "to S1");
  wait1Msec(2000);
  eraseDisplay();

  bOpenWriteTextFile(sFileName, kMaxFileSize);

        if (Stuff) {
          writeIntegerNumber(Stuff);
          writeIntegerNumber(Stuff);
          writeDelimiterBetweenNumbers();
          writeIntegerNumber(Stuff);
          writeNewLine();
          nxtDisplayTextLine(7, "Link Stat: [UP]");
        }
        else {
          nxtDisplayTextLine(7, "Link Stat: [DOWN]");
          nxtDrawLine(0, 20, 99, 20);
          wait1Msec(500);
        }

    closeWriteTextFile();
    stopAllTasks();
}
