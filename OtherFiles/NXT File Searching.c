///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                              Test File Iteration and Strings
//
// A simple little program to iterate through all the sound files on the NXT and play each
// one in turn.
//
// This program illustrates the following:
//  1. Searching through all the files on the NXT.
//  2. Playing a sound file by name
//  3. Use of 'string' variables.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma platform(NXT) // This example only runs on the NXT platform

//task main()
{
  string        sFileName;
  short         nFileSize;
  short         hFileHandle;
  TFileIOResult nIoResult;

  nVolume = 4; // Maximum volume

  // Begin searching for sound files -- i.e. those with "rso" file extension

  FindFirstFile(hFileHandle, nIoResult, "*.rso", sFileName, nFileSize);

  while (nIoResult == ioRsltSuccess)
  {
    // Display filename on LCD screen and play the sound file

    displayTextLine(2, sFileName);
    displayTextLine(4, "Size %d bytes", nFileSize);
    playSoundFile(sFileName);
    //Rename("temp.tmp", nIoResult, sFileName);
    //PlaySoundFile("*.rso");
    //Rename(sFileName, nIoResult, "temp.tmp");

    // Delay a little bit so that we can see the files on the LCD display

    wait1Msec(1000);         // Always delay 1 second
    while (bSoundActive)     // Keep delaying if the sound has not finished playing
    {}

    // Move on to the next file

    FindNextFile(hFileHandle, nIoResult, sFileName, nFileSize);
  }
  Close(hFileHandle, nIoResult);
}
