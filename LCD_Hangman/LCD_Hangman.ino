/*
 Name:		LCD_Hangman.ino
 Created:	13.04.2023 11:40:15
 Author:	simim
*/

#include <ezButton.h>
#include <LiquidCrystal.h>



#define VRx A1
#define SW 2
#define LEDPIN 3

#define IDLE 0x3

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
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


/**
 * \brief Kontrolliert ob der versuchte Buchstabe im zu erratenden Wort vorkommt und ändert
 * das zu ausgebende Wort auf dem LCD Display
 * \param Input Der Buchstabe der Eingegeben wird
 * \param WordToGuess Das zu erratende Wort
 * \param HiddenWordToGuess Das Wort das auf dem LCD Display ausgegeben wird
 * \param AvailableLetters Die Buchstaben die zur verfügung stehen
 * \param WonGame Wenn keine '_' mehr zu finden sind: true
 */
void CheckForMatch(char Input,const String& WordToGuess, String& HiddenWordToGuess, String &AvailableLetters, bool &WonGame)
{
	bool FoundLetter = false;
	bool IsInWord = false;
	int remainingLetters = 0;

	for (int i = 0; i < AvailableLetters.length() || FoundLetter != true; i++) //Entfernt den Buchstaben aus der Varaible AvilableLetters
	{
		if (AvailableLetters.charAt(i) == Input)
		{
			String RemainingLettersAfterIndex = AvailableLetters.substring(i+1);
			AvailableLetters.remove(i);
			AvailableLetters += RemainingLettersAfterIndex;
			FoundLetter = true;
		}
	}

	for (int i = 0; i < WordToGuess.length(); i++) //Sucht den BUchstaben im zu suchenden Wort
	{
		if (WordToGuess.charAt(i) == Input)
		{
			HiddenWordToGuess.setCharAt(i, Input);
			IsInWord = true;
		}
		if (HiddenWordToGuess.charAt(i) == '_') remainingLetters++;
		
	}

	if (!IsInWord) Fehler++; //Falscher Buchstabe
	if (remainingLetters == 0) WonGame = true; //Keine '_' mehr. Spiel ist gewonnen
}


#pragma endregion

unsigned long lastmillis = 0;
unsigned long currentmillis = 0;

// long ist ein Datentyp für große Zahlen; unsigned bedeutet, dass nur positive Zahlenwerte angenommen werden können.
unsigned long letzteAenderung = 0; // der letzte Zeitpunkt, an dem eine Änderung stattfand
unsigned long entprellZeit = 1000;   // Zeit, die der Taster-Zustand gleich bleiben soll, um einen stabilen Zustand zu erkennen

String WordToGuess("Test"); //Das Wort welches erraten wird vom Benutzer
String HiddenWord; // Wird auf dem LCD ausgegeben
String AvailableLetters; //Wenn alle buchstaben verfügbar: a-z. Nach jeden Versuch wird der Versuchte BUchstabe gelöscht



// the setup function runs once when you press reset or power the board
void setup() {
	lcd.begin(16, 2);

	//Erstellt die Hangman Zeichnung
	lcd.createChar(1, Stage_1);
	lcd.createChar(2, Stage_2);
	lcd.createChar(3, Stage_3);
	lcd.createChar(4, Stage_4);
	lcd.createChar(5, Stage_5);
	lcd.createChar(6, Stage_6);
	lcd.createChar(7, Stage_7);

	lcd.setCursor(0, 0);

	Joystick.setDebounceTime(50);

	Serial.begin(9600);

	for (int i = 0; i < WordToGuess.length(); i++) //Großbuchstaben werden in Kleinbuchstaben verwandelt und Hiddenword wird initialisiert
	{
		if (WordToGuess.charAt(i) >= 'A' && WordToGuess.charAt(i) <= 'Z')
		{
			WordToGuess.setCharAt(i,WordToGuess.charAt(i) + 32);
		}
		HiddenWord += '_';
	}

	for (int i = 'a', currentindex = 0; i <= 'z'; i++, currentindex++) //Setzt buchstaben von a-z
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
	if (GameFinished) return; //Spiel ist fertig, Neustarten um nochmal zu spielen

	Joystick.loop();


	PrevHiddenWord = HiddenWord;


	if (Joystick.isPressed()) //Buchstabe wurde ausgewählt
	{
		CheckForMatch(AvailableLetters.charAt(currentindex), WordToGuess, HiddenWord, AvailableLetters, WonGame);
		currentindex = 0; 
	}

	int AmountVRx = analogRead(VRx); //JOystick auf der X-Achse

	if (AmountVRx >= 800) //Joystick nach oben
	{
		CurrentJoystickStatus = HIGH;
	}else if (AmountVRx <= 300) //Joystick nach unten
	{
		CurrentJoystickStatus = LOW;
	}else //Joystick nicht auf der X-Achse Bewegt
	{
		CurrentJoystickStatus = IDLE;
	}


	if (CurrentJoystickStatus != IDLE && PrevJoystickStatus == IDLE) //Der Joystick wurde entweder nach oben oder nach unten bewegt
	{
		PrevJoystickStatus = CurrentJoystickStatus;

		
		if (CurrentJoystickStatus == HIGH) //Nach Oben
		{
			currentindex++;
			if (currentindex > (int)AvailableLetters.length() - 1) currentindex = 0;
		}
		else //Nach unten
		{
			currentindex--;
			if (currentindex < 0) currentindex = (int)AvailableLetters.length() - 1;
		}
		
	}
	else if (CurrentJoystickStatus == IDLE) //IDLE
	{
		PrevJoystickStatus = IDLE;
	}


	lcd.setCursor(15, 1);
	lcd.print(AvailableLetters.charAt(currentindex)); //Zeigt den gerade ausgewählten BUchstabe auf dem lcd Display an

	if (PrevHiddenWord != HiddenWord) //Falls Hiddenword sich geändert hat, aktualisiere ausgabe auf lcd
	{
		lcd.setCursor(0, 0);
		lcd.print(HiddenWord);
	}


	if (Fehler > 0) //Zeichnet Hangman, abhängig der Anzahl der Fehler
	{
		lcd.setCursor(0, 1);
		lcd.write(byte(Fehler));
		if (Fehler > 7) //Spiel Verloren
		{
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print("You Lost");
			GameFinished = true;
		}
	}
	if (WonGame) //Spiel gewonnen
	{
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("You Won");
		GameFinished = true;
	}

	
}