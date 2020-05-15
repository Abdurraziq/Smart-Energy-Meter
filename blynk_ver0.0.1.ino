#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <ACS712.h>
#include <ZMPT101B.h>

// Auth Token Blynk App.
char auth[] = "youtAuthToke";

// WiFi credentials.
char ssid[] = "S50D";
char pass[] = "12345678";

BlynkTimer timer;

float Voltage;
float Current;

float Power;
int PowerSet;
float Energy;

#define EspSerial Serial

// ESP8266 baud rate:
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);
ZMPT101B voltageSensor(A0);
ACS712 currentSensor(ACS712_05B, A1);

void setup()
{
	pinMode(13, OUTPUT);
	pinMode(12, OUTPUT);

	digitalWrite(12, LOW);

	delay(1000);

	voltageSensor.calibrate();
	currentSensor.calibrate();

	EspSerial.begin(ESP8266_BAUD);
	delay(10);

	Blynk.begin(auth, wifi, ssid, pass);
	timer.setInterval(1000, calculate);
	timer.setInterval(5000, sendData);
}

void loop()
{
	Blynk.run();
	timer.run();
}

void calculate()
{
	Voltage = voltageSensor.getVoltageAC();
	Voltage = round(Voltage * 10) / 10.0;

	Current = currentSensor.getCurrentAC();
	Current = round(Current * 10) / 10.0;

	Power = Voltage * Current;
	Power = round(Power * 10) / 10.0;

	Energy += (Power/3600000);

	check();
}

void sendData()
{
	Blynk.virtualWrite(V0, Voltage);
	Blynk.virtualWrite(V1, Current);
	Blynk.virtualWrite(V2, Power);
	Blynk.virtualWrite(V3, Energy);
}

BLYNK_CONNECTED()
{
	Blynk.syncAll();
}

BLYNK_WRITE(V4)
{
	PowerSet = param.asInt();
}

BLYNK_WRITE(V5)
{
	int pinValue = param.asInt();
	digitalWrite(13, pinValue);
	digitalWrite(12, pinValue);
}

void check()
{
	if (Power > PowerSet)
	{
		Blynk.virtualWrite(V5, LOW);
		digitalWrite(13, LOW);
		digitalWrite(12, LOW);
	}
}
