task main()
{
  TFileHandle   hFileHandle;              // will keep track of our file
  TFileIOResult nIOResult;                // will store our IO results
  string        sFileName = "test.txt";   // the name of our file
  short           nFileSize = 100;          // will store our file size

  byte CR = 0x13;   // define CR (carriage return)
  byte LF = 0x10;   // define LF (line feed)

  string        sMessageToWrite = "ROBOTC IO test!";    // we will write this to the file
  string        sMesageToWrite_2 = "A new line!";       // we will also write this to the file on the next line
  char          incommingChar;                          // this will store each char as we read back in from the file
  string        incommingString[5];                     // this will store the final, fully-read strings (with new strings getting put into new indexes
  int           nLineCounter = 0;                       // this will let us know which line we are on when reading and writing (used as the index to 'incommingString[]')

  Delete("test.txt",nIOResult);
  OpenWrite(hFileHandle, nIOResult, sFileName, nFileSize);    // open the file for writing (creates the file if it does not exist)
  WriteText(hFileHandle, nIOResult, sMessageToWrite);         // write 'sMessageToWrite' to the file
  WriteByte(hFileHandle, nIOResult, CR);                      // write 'CR' to the file (carriage return)
  WriteByte(hFileHandle, nIOResult, LF);                      // write 'LF' to the file (line feed)
  WriteText(hFileHandle, nIOResult, sMesageToWrite_2);        // write 'sMesageToWrite_2' to the file
  Close(hFileHandle, nIOResult);                              // close the file (DON'T FORGET THIS STEP!)

  wait1Msec(1000);  // just a wait so we can see the variables updating in the ROBOTC debugger in order (this is not necessary)

  OpenRead(hFileHandle, nIOResult, sFileName, nFileSize);     // open our file 'sFileName' for reading

  for(int i = 0; i < nFileSize; i++)                          // iterate through the file until we've hit the end:
  {
    ReadByte(hFileHandle, nIOResult, incommingChar);            // read in a single byte

    if(incommingChar == CR || incommingChar == LF)              // if the incomming byte is a carriage return or a line feed:
    {
      if(incommingChar == LF)                                     // if it's a line feed:
        nLineCounter++;                                             // increment our index (will now store next char on a 'new line' (a new index into our 'incommingString[]')
    }
    else
    {
      incommingString[nLineCounter] += incommingChar;             // append that byte (char) to the end of our final string, at the right index
    }
  }
  Close(hFileHandle, nIOResult);                              // close our file (DON'T FORGET THIS STEP!)

  for(int i = 1; i <= 5; i++)
  {
    nxtDisplayCenteredTextLine(i, incommingString[i-1]);
  }


  while(true);  // infinite loop to keep our program running so that we can view the ROBOTC debugger variables
}
