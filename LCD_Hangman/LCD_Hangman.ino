/*
 Name:		LCD_Hangman.ino
 Created:	13.04.2023 11:40:15
 Author:	simim
*/

#include <ezButton.h>
#include <LiquidCrystal.h>



#define VRx A1
#define VRy A0
#define SW 2
#define LEDPIN 3

#define IDLE 0x3

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
ezButton button(4);
ezButton Joystick(SW);

int Fehler = 0;

#pragma region Stages

byte Stage_1[8] =
{
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b11111

};

byte Stage_2[8] =
{
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b11111

};

byte Stage_3[8] =
{
	0b11111,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b11111

};
byte Stage_4[8] =
{
	0b11111,
	0b10010,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b11111

};

byte Stage_5[8] =
{
	0b11111,
	0b10010,
	0b10101,
	0b10010,
	0b10000,
	0b10000,
	0b10000,
	0b11111

};

byte Stage_6[8] =
{
	0b11111,
	0b10010,
	0b10101,
	0b10010,
	0b10010,
	0b10000,
	0b10000,
	0b11111

};

byte Stage_7[8] =
{
	0b11111,
	0b10010,
	0b10101,
	0b10010,
	0b10010,
	0b10101,
	0b10000,
	0b11111

};

#pragma endregion


#pragma region Functions

int PrintNumberOnLCD(LiquidCrystal lcd, int Number, int column, int row, int highestPotenz)
{
	
	int potenzx = (int)log10(Number);
	int AmountToClear = highestPotenz - potenzx;

	while (AmountToClear > 0)
	{
		lcd.setCursor(column + highestPotenz - AmountToClear, 0);
		lcd.print(" ");
		AmountToClear--;
	}
}

void CheckForMatch(char Input,const String& WordToGuess, String& HiddenWordToGuess, String &AvailableLetters, bool &WonGame)
{
	bool FoundLetter = false;
	bool IsInWord = false;
	int remainingLetters = 0;

	for (int i = 0; i < AvailableLetters.length() || FoundLetter != true; i++)
	{
		if (AvailableLetters.charAt(i) == Input)
		{
			String RemainingLettersAfterIndex = AvailableLetters.substring(i+1);
			AvailableLetters.remove(i);
			AvailableLetters += RemainingLettersAfterIndex;
			FoundLetter = true;
		}
		else if (AvailableLetters.charAt(i) > Input)
		{

		}
	}

	for (int i = 0; i < WordToGuess.length(); i++)
	{
		if (WordToGuess.charAt(i) == Input)
		{
			HiddenWordToGuess.setCharAt(i, Input);
			IsInWord = true;
		}
		if (HiddenWordToGuess.charAt(i) == '_') remainingLetters++;
		
	}

	if (!IsInWord) Fehler++;
	if (remainingLetters == 0) WonGame = true;
}


#pragma endregion

unsigned long lastmillis = 0;
unsigned long currentmillis = 0;

// long ist ein Datentyp für große Zahlen; unsigned bedeutet, dass nur positive Zahlenwerte angenommen werden können.
unsigned long letzteAenderung = 0; // der letzte Zeitpunkt, an dem eine Änderung stattfand
unsigned long entprellZeit = 1000;   // Zeit, die der Taster-Zustand gleich bleiben soll, um einen stabilen Zustand zu erkennen

String WordToGuess("FAbian");
String HiddenWord;
String AvailableLetters;



// the setup function runs once when you press reset or power the board
void setup() {
	lcd.begin(16, 2);

	lcd.createChar(1, Stage_1);
	lcd.createChar(2, Stage_2);
	lcd.createChar(3, Stage_3);
	lcd.createChar(4, Stage_4);
	lcd.createChar(5, Stage_5);
	lcd.createChar(6, Stage_6);
	lcd.createChar(7, Stage_7);

	lcd.setCursor(0, 0);

	button.setDebounceTime(50);
	Joystick.setDebounceTime(50);
	Serial.begin(9600);

	for (int i = 0; i < WordToGuess.length(); i++)
	{
		if (WordToGuess.charAt(i) >= 'A' && WordToGuess.charAt(i) <= 'Z')
		{
			WordToGuess.setCharAt(i,WordToGuess.charAt(i) + 32);
		}
		HiddenWord += '_';
	}

	for (int i = 'a', currentindex = 0; i <= 'z'; i++, currentindex++)
	{
		AvailableLetters += (char)i;
	}
	lcd.setCursor(0, 0);
	lcd.print(HiddenWord);
}

int n = 1;
int currentindex = 0;

String PrevHiddenWord;

int PrevJoystickStatus = IDLE;
int CurrentJoystickStatus = LOW;
int LEDStatus = LOW;

bool WonGame = false;
bool GameFinished = false;

// the loop function runs over and over again until power down or reset
void loop() {
	if (GameFinished) return;
	button.loop();
	Joystick.loop();


	PrevHiddenWord = HiddenWord;


	if (Joystick.isPressed())
	{
		CheckForMatch(AvailableLetters.charAt(currentindex), WordToGuess, HiddenWord, AvailableLetters, WonGame);
		currentindex = 0;
	}



	int AmountVRx = analogRead(VRx);

	if (AmountVRx >= 800)
	{
		CurrentJoystickStatus = HIGH;
	}else if (AmountVRx <= 300)
	{
		CurrentJoystickStatus = LOW;
	}else
	{
		CurrentJoystickStatus = IDLE;
	}

	if (Serial.available())
	{
		char PCInput = static_cast<char>(Serial.read());

		if (PCInput >= 'a' && PCInput <= 'z' || PCInput >= 'A' && PCInput <= 'Z')
		{
			currentindex =  PCInput - 'a';
		}
	}

	

	lcd.setCursor(9, 0);
	lcd.print(AmountVRx);


	if (CurrentJoystickStatus != IDLE && PrevJoystickStatus == IDLE)
	{
		PrevJoystickStatus = CurrentJoystickStatus;

		
		if (CurrentJoystickStatus == HIGH)
		{
			currentindex++;
			if (currentindex > (int)AvailableLetters.length() - 1) currentindex = 0;
		}
		else
		{
			currentindex--;
			if (currentindex < 0) currentindex = (int)AvailableLetters.length() - 1;
		}
		
	}
	else if (CurrentJoystickStatus == IDLE)
	{
		PrevJoystickStatus = IDLE;
	}


	lcd.setCursor(15, 1);
	lcd.print(AvailableLetters.charAt(currentindex));

	if (PrevHiddenWord != HiddenWord)
	{
		lcd.setCursor(0, 0);
		lcd.print(HiddenWord);
	}




	/*if (((millis() - lastmillis) >= 1000))
	{
		lastmillis = millis();
		lcd.setCursor(0, 1);
		lcd.write(byte(n));
		n++;
		if (n == 8) n = 1;
	}*/

	if (Fehler > 0)
	{
		lcd.setCursor(0, 1);
		lcd.write(byte(Fehler));
		if (Fehler > 7)
		{
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("You Lost");
			GameFinished = true;
		}
	}
	if (WonGame)
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("You Won");
		GameFinished = true;
	}

	
}