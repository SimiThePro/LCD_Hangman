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

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
ezButton button(4);
ezButton Joystick(SW);

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

unsigned long lastmillis = 0;
unsigned long currentmillis = 0;

// long ist ein Datentyp für große Zahlen; unsigned bedeutet, dass nur positive Zahlenwerte angenommen werden können.
unsigned long letzteAenderung = 0; // der letzte Zeitpunkt, an dem eine Änderung stattfand
unsigned long entprellZeit = 1000;   // Zeit, die der Taster-Zustand gleich bleiben soll, um einen stabilen Zustand zu erkennen

String WordToGuess("Ben");
String HiddenWord;

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
	Serial.begin(9600);

	for (int i = 0; i < WordToGuess.length(); i++)
	{
		HiddenWord += '_';
	}

	lcd.setCursor(0, 0);
	lcd.print(HiddenWord);
}

int n = 1;
char currentchar = 'a';

String Word_to_guess("BenIsGay");

// the loop function runs over and over again until power down or reset
void loop() {
	button.loop();
	int btnstate = button.getState();

	if (Serial.available())
	{
		char PCInput = static_cast<char>(Serial.read());

		if (PCInput >= 'a' && PCInput <= 'z' || PCInput >= 'A' && PCInput <= 'Z')
		{
			currentchar = PCInput;
		}
	}

	
	lcd.setCursor(6,0);
	lcd.print(analogRead(VRx));


	lcd.setCursor(6, 1);
	lcd.print(analogRead(VRy));

	lcd.setCursor(15, 1);
	lcd.write(currentchar);

	


	if (((millis() - lastmillis) >= 1000))
	{
		lastmillis = millis();
		lcd.setCursor(0, 1);
		lcd.write(byte(n));
		n++;
		if (n == 8) n = 1;
	}
	

	
}