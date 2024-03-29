#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <dht.h>
#include <EEPROM.h>
// itverse

#define DT 3
#define CLK 2

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1
#define i2c_Address 0x3c

#define DHTPIN 5	  // pin connected to DHT22 sensor
#define RELAY_1_PIN 7 // Relay 1 control pin
#define RELAY_2_PIN 8 // Relay 2 control pin
#define RELAY_3_PIN 9
#define RELAY_4_PIN 10

#define Mode_Btn_Pin 4
#define Select_Btn_Pin 6

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
dht DHT;

float tempC;
float humi;

int modeNumber, selectNumber = 0;
int t_upperOffset, t_lowerOffset, h_upperOffset, h_lowerOffset = 0;

int modeBtnState = HIGH;
int lastModeBtnState = HIGH;
unsigned long lastModeBtnDebounceTime = 0;

int selectBtnState = HIGH;
int lastSelectBtnState = HIGH;
unsigned long lastSelectBtnDebounceTime = 0;

unsigned long debounceDelay = 25;
const unsigned char epd_bitmap_logo[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xc0, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf0, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x07, 0xf8, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0xfe, 0x0e, 0x70, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x3f, 0x1e, 0x78, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x0f, 0x87, 0x60, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x07, 0xc3, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x03, 0xe1, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x01, 0xf1, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xf9, 0xdf, 0xe6, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xf1, 0xbf, 0xe2, 0x00, 0x00, 0x07, 0x7f, 0xc0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xfc, 0x71, 0xa4, 0xe6, 0x65, 0x98, 0xc7, 0x07, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0xf0, 0x71, 0x84, 0x34, 0xf7, 0xb9, 0xe7, 0x81, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0xc0, 0xf1, 0x84, 0x35, 0xb6, 0xeb, 0x67, 0x80, 0xf0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x80, 0xf1, 0x84, 0x35, 0xf6, 0x7b, 0xe3, 0x80, 0x70, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x07, 0x80, 0xf1, 0x84, 0x3d, 0x86, 0x3b, 0x03, 0x80, 0xf0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x80, 0xe1, 0x84, 0x19, 0x86, 0x4f, 0x03, 0x81, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0xe0, 0xe1, 0x84, 0x18, 0xf6, 0x69, 0xe3, 0x87, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0xf8, 0xf1, 0xce, 0x18, 0xe6, 0x79, 0xc3, 0x9f, 0xc0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xfc, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x79, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x07, 0xf0, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x07, 0xf0, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x04, 0x10, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x05, 0xd0, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x05, 0x18, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x05, 0xd8, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x04, 0x18, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x03, 0xf0, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x1f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void updateEncoder()
{
	// Read the current state of CLK
	currentStateCLK = digitalRead(CLK);

	// If last and current state of CLK are different, then pulse occurred
	// React to only 1 state change to avoid double count
	if (currentStateCLK != lastStateCLK && currentStateCLK == 1)
	{

		// If the DT state is different than the CLK state then
		// the encoder is rotating CCW so decrement
		if (digitalRead(DT) != currentStateCLK)
		{
			counter--;
			currentDir = "CCW";
			if (selectNumber == 1)
			{
				t_upperOffset = t_upperOffset + 1;
			}
			else if (selectNumber == 2)
			{
				t_lowerOffset = t_lowerOffset + 1;
			}
		}
		else
		{
			// Encoder is rotating CW so increment
			counter++;
			currentDir = "CW";
			if (selectNumber == 1)
			{
				t_upperOffset = t_upperOffset - 1;
			}
			else if (selectNumber == 2)
			{
				t_lowerOffset = t_lowerOffset - 1;
			}
		}

		Serial.print("Direction: ");
		Serial.print(currentDir);
		Serial.print(" | Counter: ");
		Serial.println(counter);
	}

	// Remember last CLK state
	lastStateCLK = currentStateCLK;
}

void saveOffsets()
{
	EEPROM.put(0, t_upperOffset);
	EEPROM.put(sizeof(int), t_lowerOffset);
}

void loadOffsets()
{
	EEPROM.get(0, t_upperOffset);
	EEPROM.get(sizeof(int), t_lowerOffset);
}

void welcomScreen()
{
	oled.drawBitmap(0, 0, epd_bitmap_logo, 128, 64, 1);
	oled.display();
	oled.clearDisplay();
	delay(1000);
}

void oledDisplay()
{
	oled.clearDisplay();
	oled.setCursor(0, 0);
	oled.setTextSize(1.5);
	oled.println("temperature: ");
	oled.setTextSize(2);
	oled.print(tempC);
	oled.print("");
	oled.setTextSize(1);
	oled.print(char(223));
	oled.setTextSize(2);
	oled.print("C");

	oled.setTextSize(1);
	oled.setCursor(95, 8);
	oled.print(char(0x18));
	oled.print(t_upperOffset);
	if (tempC > t_upperOffset)
	{
		oled.fillCircle(116, 12, 2, 1);
	}
	oled.setCursor(95, 18);
	oled.print(char(0x19));
	oled.print(t_lowerOffset);
	if (tempC < t_lowerOffset)
	{
		oled.fillCircle(116, 22, 2, 1);
	}
	oled.setCursor(0, 30);
	oled.setTextSize(1.5);
	oled.println("Humidity: ");
	oled.setTextSize(2);
	oled.print(humi);
	oled.println("%");

	if (modeNumber > 0)
	{
		oled.setTextSize(1);
		oled.clearDisplay();
		oled.setCursor(10, 0);
		oled.print("Temp & Humidity");
		if (selectNumber == 1)
		{
			oled.setCursor(0, 18);
			oled.print(">");
		}
		else if (selectNumber == 2)
		{
			oled.setCursor(0, 28);
			oled.print(">");
		}
		oled.setCursor(10, 18);
		oled.print("MAX Temp: ");
		oled.print(t_upperOffset);
		oled.print("(");
		oled.print(tempC);
		oled.print(")");
		oled.setCursor(10, 28);
		oled.print("Min Temp: ");
		oled.println(t_lowerOffset);
	}
	oled.display();
}

void controlRelays()
{

	if (tempC > t_upperOffset)
	{
		digitalWrite(RELAY_1_PIN, HIGH); // change LOW if use active LOW realay
	}
	else
	{
		digitalWrite(RELAY_1_PIN, LOW); // change HIGH if use active LOW realay
	}

	if (tempC < t_lowerOffset)
	{
		digitalWrite(RELAY_2_PIN, HIGH); // change LOW if use active LOW realay
	}
	else
	{
		digitalWrite(RELAY_2_PIN, LOW); // change HIGH if use active LOW realay
	}
}

void setup()
{

	pinMode(CLK, INPUT);
	pinMode(DT, INPUT);
	pinMode(Mode_Btn_Pin, INPUT_PULLUP);
	pinMode(Select_Btn_Pin, INPUT_PULLUP);
	pinMode(Select_Btn_Pin, INPUT_PULLUP);

	pinMode(RELAY_1_PIN, OUTPUT);
	pinMode(RELAY_2_PIN, OUTPUT);
	pinMode(RELAY_3_PIN, OUTPUT);
	pinMode(RELAY_4_PIN, OUTPUT);

	Serial.begin(9600);
	lastStateCLK = digitalRead(CLK);
	loadOffsets();

	// Call updateEncoder() when any high/low changed seen
	// on interrupt 0 (pin 2), or interrupt 1 (pin 3)
	attachInterrupt(0, updateEncoder, CHANGE);
	attachInterrupt(1, updateEncoder, CHANGE);

	if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{
		Serial.println(F("SSD1306 allocation failed"));
		while (true)
			;
	}
	delay(2000);		 // wait for initializing
	oled.clearDisplay(); // clear display
	welcomScreen();

	oled.setTextColor(WHITE); // text color
	oled.display();
}

void handleButtons()
{
	int newModeState = digitalRead(Mode_Btn_Pin);
	int newSelectState = digitalRead(Select_Btn_Pin);

	if (newModeState != lastModeBtnState)
	{
		lastModeBtnDebounceTime = millis();
	}

	if (newSelectState != lastSelectBtnState)
	{
		lastSelectBtnDebounceTime = millis();
	}

	if ((millis() - lastModeBtnDebounceTime) > debounceDelay)
	{
		if (newModeState != modeBtnState)
		{
			modeBtnState = newModeState;
			if (modeBtnState == LOW)
			{
				if (modeNumber == 1)
				{
					modeNumber = 0;
					saveOffsets();
				}
				else
				{
					modeNumber = modeNumber + 1;
				}
			}
		}
	}

	if ((millis() - lastSelectBtnDebounceTime) > debounceDelay)
	{
		if (newSelectState != selectBtnState)
		{
			selectBtnState = newSelectState;
			if (selectBtnState == LOW)
			{
				if (selectNumber == 2)
				{
					selectNumber = 0;
				}
				else
				{
					selectNumber = selectNumber + 1;
				}
			}
		}
	}

	lastModeBtnState = newModeState;
	lastSelectBtnState = newSelectState;
}

void loop()
{
	DHT.read22(DHTPIN);
	humi = DHT.humidity;	 // read humidity
	tempC = DHT.temperature; // read temperature

	// check if any reads failed
	if (isnan(humi) || isnan(tempC))
	{
		Serial.println("DHT22 Failed");
	}
	oledDisplay();
	handleButtons();
	controlRelays();
}
