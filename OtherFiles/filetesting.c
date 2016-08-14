//FOR EV3 - ?NXT?




// The name of the file that we're saving data to
char * filename = "testFile";

// The read buffer, which is the amount of data we can read at once
char readBuffer[128];

// Routine to test the writing and subsequent reading
// of long-sized variables
void testLongs()
{
  writeDebugStreamLine("Testing Longs");

  // The file handle, this is a reference to
  // the file you have opened
  long fileHandle;

  long writeValue = 0x11DD0000;
  long readValue;

  // Open the file for writing and return a handle,
  // which we can use for subsequent writes
  fileHandle = fileOpenWrite(filename);
  for (int i = 0; i < 10; i++)
  {
  	// Write a long value (4 byte) to the file, this is
  	// binary, not a string
    fileWriteLong(fileHandle, writeValue++);
  }
  writeDebugStreamLine("fileHandle: %d", fileHandle);

  // Close the file, this automatically flushes the
  // buffers to the "disk".
  fileClose(fileHandle);

  // Open the file for reading and return a handle.
  fileHandle = fileOpenRead(filename);

  // Read long-sized values for as long as there's data.
  // The fileReadLong() function returns "false" when
  // an error occurred or not data is left
  while (fileReadLong(fileHandle, &readValue))
  {
    writeDebugStreamLine("Read: 0x%X", readValue);
  }

  // Close the file
  fileClose(fileHandle);
}

// Routine to test the writing and subsequent reading
// of float variables
void testFloats()
{
  writeDebugStreamLine("Testing Floats");

  // The file handle, this is a reference to
  // the file you have opened
  long fileHandle;

  float writeValue = 8.167;
  float readValue;

  // Open the file for writing and return a handle,
  // which we can use for subsequent writes
  fileHandle = fileOpenWrite(filename);
  for (int i = 0; i < 10; i++)
  {
  	// Write a float value to the file, this is
  	// binary, not a string
    fileWriteFloat(fileHandle, writeValue);
    writeValue*=5;
  }
  writeDebugStreamLine("fileHandle: %d", fileHandle);

  // Close the file, this automatically flushes the
  // buffers to the "disk".
  fileClose(fileHandle);

  // Open the file for reading and return a handle.
  fileHandle = fileOpenRead(filename);

  // Read float values for as long as there's data.
  // The fileReadFloat() function returns "false" when
  // an error occurred or not data is left
  while (fileReadFloat(fileHandle, &readValue))
  {
    writeDebugStreamLine("Read: %f", readValue);
  }

  // Close the file
  fileClose(fileHandle);
}

// Routine to test the writing and subsequent reading
// of float variables
void testShorts()
{
  writeDebugStreamLine("Testing Shorts");

  // The file handle, this is a reference to
  // the file you have opened
  long fileHandle;

  short writeValue = 2;
  short readValue;

  // Open the file for writing and return a handle,
  // which we can use for subsequent writes
  fileHandle = fileOpenWrite(filename);
  for (int i = 0; i < 10; i++)
  {
  	// Write a short (2 byte) value to the file, this is
  	// binary, not a string
    fileWriteShort(fileHandle, writeValue);
    writeValue*=2;
  }
  writeDebugStreamLine("fileHandle: %d", fileHandle);

  // Close the file, this automatically flushes the
  // buffers to the "disk".
  fileClose(fileHandle);

  // Open the file for reading and return a handle.
  fileHandle = fileOpenRead(filename);

  // Read short values for as long as there's data.
  // The fileReadShort() function returns "false" when
  // an error occurred or not data is left
  while (fileReadShort(fileHandle, &readValue))
  {
    writeDebugStreamLine("Read: %d", readValue);
  }

  // Close the file
  fileClose(fileHandle);
}

// Routine to test the writing and subsequent reading
// of float variables
void testChars()
{
  writeDebugStreamLine("Testing Chars");

  // The file handle, this is a reference to
  // the file you have opened
  long fileHandle;

  char writeValue = 1;
  char readValue;

  // Open the file for writing and return a handle,
  // which we can use for subsequent writes
  fileHandle = fileOpenWrite(filename);
  for (int i = 0; i < 10; i++)
  {
  	// Write a char (1 byte) value to the file, this is
  	// binary, not a string
    fileWriteChar(fileHandle, writeValue);
    writeValue+=5;
  }
  writeDebugStreamLine("fileHandle: %d", fileHandle);

  // Close the file, this automatically flushes the
  // buffers to the "disk".
  fileClose(fileHandle);

  // Open the file for reading and return a handle.
  fileHandle = fileOpenRead(filename);

  // Read char values for as long as there's data.
  // The fileReadChar() function returns "false" when
  // an error occurred or not data is left
  while (fileReadChar(fileHandle, &readValue))
  {
    writeDebugStreamLine("Read: %d", readValue);
  }

  // Close the file
  fileClose(fileHandle);
}

void testData()
{
  writeDebugStreamLine("Testing Data");

  long fileHandle;

  char * string1 = "I like peanutbutter";
  int strlen1 = strlen(string1);

  char * string2 = "ROBOTC is cool";
  int strlen2 = strlen(string2);

  // Open the file for writing and return a handle,
  // which we can use for subsequent writes
  fileHandle = fileOpenWrite(filename);
  writeDebugStreamLine("fileHandle: %d", fileHandle);

  writeDebugStreamLine("strlen1: %d", strlen1);
  fileWriteData(fileHandle, string1, strlen1 + 1); // gets the 0 terminator as well
  fileWriteData(fileHandle, string2, strlen2 + 1); // gets the 0 terminator as well

  // Close the file, this automatically flushes the
  // buffers to the "disk".
  fileClose(fileHandle);

  // Open the file for reading and return a handle.
  fileHandle = fileOpenRead(filename);

  memset(readBuffer, 0, 128);
  fileReadData(fileHandle, readBuffer, strlen1 + 1);
  writeDebugStreamLine(readBuffer);
  fileReadData(fileHandle, readBuffer, strlen2 + 1);
  writeDebugStreamLine(readBuffer);

  // Close the file
  fileClose(fileHandle);
}


task main()
{
  testLongs();
  testFloats();
  testShorts();
  testChars();
  testData();
}
