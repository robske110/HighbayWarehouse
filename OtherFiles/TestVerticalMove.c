#pragma config(Sensor, S1,							COLOUR,			sensorColorNxtNONE)
#pragma config(Sensor, S2,     					HORIZONTAL,    sensorSONAR)
#pragma config(Sensor, S3,     					VERTICALsensor,    		 sensorTouch)
#pragma config(Sensor, S4,     					HORIZONTAL_T,    		 sensorTouch)
#pragma config(Motor,  motorA,          VERTICAL,      tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          HORIZONTAL,    tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          FETCHER,       tmotorNXT, PIDControl, encoder)
task main()
{
	while(true){
		if(SensorValue(HORIZONTAL_T) == 1 && nNxtButtonPressed == -1){
			motor[VERTICAL] = 0;
			playSound(soundBeepBeep);
			wait1Msec(1000);
			}else{
			motor[VERTICAL] = -10;
		}
	}
}
