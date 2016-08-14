void Debug(string DebugString)
{
	char TempSaveChar;
	int INDEX = 0;
	bool StringReached19 = false;
	bool LoopEnd = false;
	bool LoopEnd1 = false;
	bool LoopEnd2 = false;
	bool LoopEnd3 = false;
	while(!LoopEnd)
	{
		TempSaveChar = stringGetChar(DebugString, INDEX);
		if(TempSaveChar == NULL)
		{
			LoopEnd = true;
		}
		if(INDEX == 19)
		{
			LoopEnd = true;
			StringReached19 = true;
		}
		INDEX++;
	}
	if(!StringReached19)
	{
		writeDebugStream(DebugString);
		writeDebugStreamLine("");
	}
	else
	{
		LoopEnd = false;
    INDEX = 0;
    int INDEXFinalWrite = 0;
		while(!LoopEnd1)
		{
			while(!LoopEnd)
			{
				TempSaveChar = stringGetChar(DebugString, INDEX);
				if(TempSaveChar == NULL)
				{
					string LeftChars;
					string NineteenChars;
					while(!LoopEnd3)
					{
						TempSaveChar = stringGetChar(DebugString, INDEXFinalWrite);
						if(TempSaveChar == NULL)
						{
							LoopEnd3 = true;
						}
						NineteenChars += TempSaveChar;
						writeDebugStream(NineteenChars);
						INDEXFinalWrite++;
					}
					LoopEnd = true;
					writeDebugStream(LeftChars);
					LoopEnd1 = true;
				}
				if(INDEX == 19)
				{
					string NineteenChars;
					INDEXFinalWrite = 0;
					while(!LoopEnd2)
					{
						TempSaveChar = stringGetChar(DebugString, INDEXFinalWrite);
						NineteenChars += TempSaveChar;
						INDEXFinalWrite++;
						if(INDEXFinalWrite == 19)
						{
							LoopEnd2 = true;
						}
					}
					LoopEnd = true;
					writeDebugStream(NineteenChars);
				}
				INDEX++;
			}
		}
		writeDebugStreamLine("");
	}
}
