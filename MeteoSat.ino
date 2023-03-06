
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp; // I2C

#define LEDPIN D4
#define GPIO_HC12 D3

String readSerialData() {
  if (Serial.available() > 0) {
    String inString = "";
    while (Serial.available() > 0) {
      int inChar = Serial.read();
      inString += (char)inChar;
      if (inChar == '\n') {
        return inString;
      }
    }
  }
  return "";
}
void goToSleep() {

  Serial.println(F("DV_MSG:GoToSleepNow"));
  Serial.flush();
  delay(1000);
  digitalWrite(GPIO_HC12, LOW);
  delay(200);
  Serial.println("AT+SLEEP");
  int tries = 200;
  String atData = readSerialData();
  while (atData == "" && tries > 0) {
    delay(100);
    tries--;
    atData = readSerialData();
  }
  if (atData.startsWith("OK+SLEEP"))
  {
    //ESP.deepSleep(10 * (60 * 1000000));
    for (int i = 0 ; i < 15; i++)
    {
      digitalWrite(LEDPIN, LOW);
      delay(20);
      digitalWrite(LEDPIN, HIGH);
      delay(100);
    }
    ESP.deepSleep((20 * 1000000));
  }

}
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
float batPercent(float voltage) {
  float bat_percentage = 0;
  bat_percentage = mapfloat(voltage, 3, 4.22, 0, 100); //2.8V as Battery Cut off Voltage & 4.2V as Maximum Voltage
  bat_percentage = (bat_percentage >= 100) ? 100 : ((bat_percentage <= 0) ? 1 : bat_percentage);
  return bat_percentage ;
}
float batVoltage()
{
  float voltage;
  int analogInPin  = A0;    // Analog input pin
  int sensorValue;
  float calibration = -0.10; // Check Battery voltage using multimeter & add/subtract the value
  sensorValue = analogRead(analogInPin);
  // (32k/(100k+32)) * 4.2V = sensor V.
  voltage = (sensorValue / 0.242424242424) / 1000; //multiply by two as voltage divider network is 100K & 100K Resistor
  voltage = voltage + calibration;
  return voltage;

}
void setup() {
  //Serial.begin(115200);

  Serial.begin(9600);
  pinMode(LEDPIN, OUTPUT);
  pinMode(GPIO_HC12, OUTPUT);
  digitalWrite(GPIO_HC12, HIGH);

  if (!bmp.begin(0x76)) {
    Serial.println(F("DV_ERROR:BMP_error"));
    while (1);
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */



  for (int i = 0 ; i < 3; i++)
  {
    digitalWrite(LEDPIN, LOW);
    delay(30);
    digitalWrite(LEDPIN, HIGH);
    delay(300);
  }
  Serial.println(F("DV_MSG:AKAG!"))           ;
}

void loop() {
  Serial.print(F("DV_DATA:"));
  Serial.print(F("T:"));
  Serial.print(bmp.readTemperature());
  Serial.print(F(","));

  Serial.print(F("P:"));
  Serial.print(bmp.readPressure());
  Serial.print(F(","));

  Serial.print(F("V:"));
  Serial.print(batVoltage(), 3 );
  //Serial.print(",");
  Serial.println("");
  goToSleep();
}
