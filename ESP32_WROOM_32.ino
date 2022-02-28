/*
  Project: ESP32 development board, Ai Thinker A6 GSM GPRS module, DHT11
  Function: Temperature and humidity readings over SMS.
  Ai Thinker A6 GSM GPRS module powered by power supply 5V DC 2A

  Check for more DIY projects on acoptex.com

  Ai Thinker A6 GSM GPRS module -> ESP32 development board
  GND           GND
  U_RXD         TX2 (GPIO17)
  U_TXD         RX2 (GPIO16)

  There are three serial ports on the ESP known as U0UXD, U1UXD and U2UXD.

  U0UXD is used to communicate with the ESP32 for programming and during reset/boot.
  U1UXD is unused and can be used for your projects. Some boards use this port for SPI Flash access though
  U2UXD is unused and can be used for your projects.

*/
#include <stdio.h>
#include <string.h>
#include "DHT.h"            //library for DHT sensor

// Uncomment one of the lines below for whatever DHT sensor type you're using
#define DHTTYPE DHT11      // DHT 11
//#define DHTTYPE DHT21    // DHT 21 (AM2301)
//#define DHTTYPE DHT22    // DHT 22  (AM2302), AM2321
//#define DEBUG true

const int DHTPin = 2;      // DHT11 connected to D4 (GPIO4) pin of ESP32 development board
DHT dht(DHTPin, DHTTYPE);  // create an instance of the dht class called DHT
float h, t, f;             // Variables for temperature and humidity

const String apiKey = "FE1A89S55LRZNQXI";
String textMessage;        // Variable to store text message

void setup()
{
  Serial.begin(115200);                      // Initialise serial commmunication at 115200 bps
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // Initialise serial commmunication at 115200 bps on U2UXD serial port
  dht.begin();                             // Initialize DHT sensor
  pinMode(DHTPin, INPUT);                  // Set DHTPin as INPUT
  delay(2000);              // Give time to your GSM module logon to GSM network

  //A9G Tests
  Serial.println("GSMmodule ready...");  //Print test in Serial Monitor
  Serial2.println("AT\r"); // AT command to set module to SMS mode
  delay(100); // Set delay for 100 ms
  Serial2.println("AT+CMGF=1\r"); // AT command to set module to SMS mode
  delay(100); // Set delay for 100 ms
  Serial2.println("AT+CNMI=2,2,0,0,0\r");// Set module to send SMS data to serial out upon receipt
  delay(100); // Set delay for 100 ms
  Serial2.print("AT+CMGD=1,4\r");//delete all the message in the storage
  delay(100); // Set delay for 100 ms
  Serial2.print("AT+CGATT=1\r");//check if the chip is registered to the network
  delay(100); // Set delay for 100 ms
  Serial2.print("AT+CGACT=1,1\r");//check if the chip is registered to the network
  delay(100); // Set delay for 100 ms

  delay(100); // Set delay for 100 ms
  //Serial2.print("ATD*99***1#\r");//PPP session
  /*Serial2.print("AT+GPS=1\r");//delete all the message in the storage
    delay(100); // Set delay for 100 ms
    Serial2.print("AT+GPSRD=10\r");//delete all the message in the storage
    delay(100); // Set delay for 100 ms
    Serial2.print("AT+LOCATION=2\r");//delete all the message in the storage
    delay(100); // Set delay for 100 ms*/
  delay(2000); // Set delay for 2 seconds
}

void loop()
{
  if (Serial.available() > 0)
    Serial2.write(Serial.read());
  if (Serial2.available() > 0)
    Serial.write(Serial2.read());
  if (Serial2.available() > 0) {
    textMessage = Serial2.readString();
    Serial.print(textMessage);
    delay(10);

  }

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  t = dht.readTemperature(); //Temperature in Celsius
  h = dht.readHumidity(); //Humidity
  f = dht.readTemperature(true); //Temperature in Fahrenheit
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  if (textMessage.indexOf("Status") >= 0) {
    textMessage = "";
    String message1 = "Temperature (Celsius): " + String(t);
    //String message1 = "Temperature (Fahrenheit): " + String(f);
    String message2 = " Humidity: " + String(h);
    String message = message1 + message2;
    sendSMS(message);
  }
  Serial2.print("AT+CGATT=1\r");//check if the chip is registered to the network
  Serial2.print("AT+CGACT=1,1\r");//check if the chip is registered to the network
  Serial2.print("AT+CGDCONT=1,\"IP\",\"safaricom\"\r");//APN configuration
  String cmdString = "AT+HTTPGET=\"http://api.thingspeak.com/update.json?api_key=" + apiKey + "&field1=" + t + "&field2=" + h + "&field3=0.0&field4=0.0" + "\"";
  Serial2.println(cmdString);
}

// Function that sends SMS
void sendSMS(String message) {
  // REPLACE xxxxxxxxxxxx WITH THE RECIPIENT'S NUMBER
  // REPLACE ZZZ WITH THE RECIPIENT'S COUNTRY CODE
  Serial2.print("AT+CMGS = \"+254793627314\"\r");
  delay(100);
  // Send the SMS
  Serial2.print(message);
  delay(100);

  // End AT command with a ^Z, ASCII code 26
  Serial2.println((char)26);
  delay(100);
  Serial2.println();
  // Give module time to send SMS
  delay(5000);


}

/*String sendData(String command, const int timeout, boolean debug)
  {
    String response = "";
    Serial.println(command);
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (Serial.available())
        {
            char c = Serial.read();
            response += c;
        }
    }
    if (debug)
    {
        Serial2.print(response);
    }
    return response;
  }*/
