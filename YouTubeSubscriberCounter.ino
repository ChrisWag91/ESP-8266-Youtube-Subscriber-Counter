
// YouTube Subscriber Counter
// by Becky Stern 2017
// adapted by Christoph Wagner

// based on library sample code by:
//   Giacarlo Bacchio (Gianbacchio on Github)
//   Brian Lough (witnessmenow on Github)
//   Adafruit (adafruit on github)

// Displays your current subscriber count on a seven-segment display

// requires the following libraries, search in Library Manager or download from github):
#include <Wire.h>                 // installed by default
#include <YoutubeApi.h>            // https://github.com/witnessmenow/arduino-youtube-api
#include <ArduinoJson.h>           // https://github.com/bblanchon/ArduinoJson
#include <LedControl.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//------- Replace the following! ------
char ssid[] = "XXXXXXXXXXX"; // your network SSID (name)
char password[] = "XXXXXXXXXXXXXXXXXX";   // your network password

// google API key
// create yours: https://support.google.com/cloud/answer/6158862?hl=en
#define API_KEY "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

// youtube channel ID
// find yours: https://support.google.com/youtube/answer/3250431?hl=en
#define CHANNEL_ID "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

/* we always wait a bit between updates of the display */
unsigned long delaytime = 250;

int subs = 0;
int views = 0;
int vids = 0;
char temp;
bool startNumberFlag = false;

/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 7 is connected to the DataIn
  pin 5 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.
*/
LedControl lc = LedControl(D7, D5, D8, 1);

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

void setup() {

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 2);
  /* and clear the display */
  lc.clearDisplay(0);
}

void loop() {


  if (api.getChannelStatistics(CHANNEL_ID))
  {
    Serial.println("---------Stats---------");
    Serial.print("Subscriber Count: ");
    Serial.println(api.channelStats.subscriberCount);

    subs = api.channelStats.subscriberCount;

    Serial.print("View Count: ");
    Serial.println(api.channelStats.viewCount);

    views = api.channelStats.viewCount;

    Serial.print("Comment Count: ");
    Serial.println(api.channelStats.commentCount);

    Serial.print("Video Count: ");
    Serial.println(api.channelStats.videoCount);

    vids = api.channelStats.videoCount;

    Serial.println("------------------------");
  }
  else {
    Serial.println("Channel not found");
  }

  Serial.println(subs);
  writeArduinoOn7Segment(subs);
  lc.setRow(0, 7, B01000000);
  delay (10000);

  Serial.println(views);
  writeArduinoOn7Segment(views);
  lc.setRow(0, 7, B00000001);
  delay (10000);

  Serial.println(vids);
  writeArduinoOn7Segment(vids);
  lc.setRow(0, 7, B00001000);
  delay (10000);
}

void writeArduinoOn7Segment(int digit) {

  if (digit < 1000)
  {

    temp = (digit / 10000) % 10;
    writeDigit(temp, 6);

    temp = (digit / 1000) % 10;
    writeDigit(temp, 5);

    temp = (digit / 100) % 10;
    writeDigit(temp, 4);

    temp = (digit / 10) % 10;
    writeDigit(temp, 3);

    temp = digit % 10;
    writeDigit(temp, 2);

    lc.setChar(0, 1, ' ', false);
    lc.setRow(0, 0, B01000000);
    startNumberFlag = false;
  }

  else if (digit < 1000000)
  {
    digit = digit / 100;

    temp = (digit / 10000) % 10;
    writeDigit(temp, 6);

    temp = (digit / 1000) % 10;
    writeDigit(temp, 5);

    temp = (digit / 100) % 10;
    writeDigit(temp, 4);

    temp = (digit / 10) % 10;
    writeDigitWithPoint(temp, 3);

    temp = digit % 10;
    writeDigit(temp, 2);

    lc.setChar(0, 1, ' ', false);
    lc.setRow(0, 0, B00000001);
    startNumberFlag = false;
  }

  else
  {
    digit = digit / 100000;

    temp = (digit / 10000) % 10;
    writeDigit(temp, 6);

    temp = (digit / 1000) % 10;
    writeDigit(temp, 5);

    temp = (digit / 100) % 10;
    writeDigit(temp, 4);

    temp = (digit / 10) % 10;
    writeDigitWithPoint(temp, 3);

    temp = digit % 10;
    writeDigit(temp, 2);

    lc.setChar(0, 1, ' ', false);
    lc.setRow(0, 0, B00001000);
    startNumberFlag = false;
  }
}

void writeDigit(int digit, int digitPosition)
{
  if (digit != 0 || startNumberFlag == true)
  {
    lc.setDigit(0, digitPosition, digit, false);
    startNumberFlag = true;
  }
  else {
    lc.setChar(0, digitPosition, ' ', false);
  }

}

void writeDigitWithPoint(int digit, int digitPosition)
{
  if (digit != 0 || startNumberFlag == true)
  {
    lc.setDigit(0, digitPosition, digit, true);
  }
}

