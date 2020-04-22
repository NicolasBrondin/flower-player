// Author: Nicolas Brondin-Bernard, based on pcbreflux and DFRobot work.

#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"

HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;

int playPausePin = 35; 
int nextPin = 27; 
int volumeDownPin = 26; 
int volumeUpPin = 33; 

int volume = 1;
int musicMode = 1; //1=night,2=day
int songNumber = 1;
int songsCount = 1;
bool isPlaying = true;
bool isNextPressed = false;
unsigned long nextPressedTime;

void printDetail(uint8_t type, int value);
void checkButtons();

void setup()
{
	pinMode(playPausePin, INPUT);
	pinMode(nextPin, INPUT);
	pinMode(volumeDownPin, INPUT);
	pinMode(volumeUpPin, INPUT);
	digitalWrite(playPausePin, LOW);
	digitalWrite(nextPin, LOW);
	digitalWrite(volumeDownPin, LOW);
	digitalWrite(volumeUpPin, LOW);
		
	mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);  // speed, type, RX, TX
	Serial.begin(115200);
	
	Serial.println();
	Serial.println(F("DFRobot DFPlayer Mini"));
	Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
	
	while(!myDFPlayer.begin(mySoftwareSerial)) {
		Serial.println(F("Unable to begin"));
	}
	Serial.println(F("DFPlayer Mini online."));

	songsCount = myDFPlayer.readFileCountsInFolder(2);
	myDFPlayer.volume(volume);
	myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
	myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
	myDFPlayer.playFolder(1, 1);  
	myDFPlayer.start();
}

void loop()
{
	if (myDFPlayer.available()) {
		uint8_t type = myDFPlayer.readType();
		if(type == DFPlayerPlayFinished){
			if(musicMode == 1){
				myDFPlayer.playFolder(1, 1);  
			} else {
				nextSong();
			}
		}
	}
	checkButton();
}

void nextSong(){
	songNumber = ((songNumber)%(songsCount-1))+1;
	myDFPlayer.playFolder(2, songNumber);
}

void checkButton(){
  
	int volumeDown = digitalRead(volumeDownPin);
	int volumeUp = digitalRead(volumeUpPin);
	int playPause = digitalRead(playPausePin);
	int next = digitalRead(nextPin);

	if(volumeDown == 1){
		myDFPlayer.volumeDown(); //Dirty
		myDFPlayer.volumeDown();
		myDFPlayer.volumeDown();
		delay(500);
	}
	if(volumeUp == 1){
		myDFPlayer.volumeUp();
		myDFPlayer.volumeUp();
		myDFPlayer.volumeUp();
		delay(500);
	}
	if(next){
		if(!isNextPressed){
		nextPressedTime = millis();
		}
		isNextPressed = true;
	} else {
		if(isNextPressed){
			isNextPressed = false;
			if(millis() - nextPressedTime > 1000){
				musicMode = musicMode == 1 ? 2 : 1;
				if(musicMode == 1){
					myDFPlayer.playFolder(1, 1);
				} else {
					myDFPlayer.playFolder(2, songNumber);
				}
			} else if(musicMode != 1) {
				nextSong();
			}
			delay(500);
		} 
	}
	if(playPause){
		isPlaying = !isPlaying;
		if(isPlaying == true){
			myDFPlayer.start();
		} else {
			myDFPlayer.pause();
		}
		delay(500);
	}
}
