#include <DMX_Tester.h>
//*****************************DMX**************************************************

uint8_t level;
uint8_t dmxbuffer[DMX_MAX_FRAME];

// the addresses of the slots to observe
int testChannel = 1;

// the levels of those slots
uint8_t testLevel = 0;

// flag set, when dmx data is changed
uint8_t dataChanged = 1;

//***************************WIFI & OSC *********************************
const char *ssid = "DMX_Tester";     // Name des Netzwerks
const char *password = "dmx_tester"; // Passwort des Netzwerks

WiFiServer server(80); // Server Instanz -> ESP als Access Point
WiFiUDP Udp;           // UDP Instanz um Pakete über UDP zu senden / empfangen

const IPAddress outIp(192, 168, 4, 2); // IP des Clients
const unsigned int outPort = 9999;     // Incoming-Port des Clients
const unsigned int localPort = 8888;   // Incoming-Ports des Hosts für ankommende Nachrichten

OSCErrorCode error;

//******************************REC**************************************
int sampleCounter = 0;
/************************************************************************

  callback for when DMX frame is received
  Note:  called from receive task
  
  Checks to see if the level of the designated slot has changed
  and sets the dataChanged flag.

  Processing in this callback should be minimal.

  Instead, use a flag and do more intense processing in a lower priority task.
  
*************************************************************************/
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
void feedTheDog()
{
  // feed dog 0
  TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG0.wdt_feed = 1;                       // feed dog
  TIMERG0.wdt_wprotect = 0;                   // write protect
  // feed dog 1
  TIMERG1.wdt_wprotect = TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG1.wdt_feed = 1;                       // feed dog
  TIMERG1.wdt_wprotect = 0;                   // write protect
}

void receiveCallback(int slots)
{
  if (slots)
  {
    xSemaphoreTake(ESP32DMX.lxDataLock, portMAX_DELAY);
    if (testLevel != ESP32DMX.getSlot(testChannel))
    {
      testLevel = ESP32DMX.getSlot(testChannel);
      dataChanged = 1;
    }
    xSemaphoreGive(ESP32DMX.lxDataLock);
  }
}

/************************************************************************
	setup Funktion
*************************************************************************/

void wdt_reset(void *parameter);

void setup()
{
  Serial.begin(115200);

  // Server starten + Feedback
  Serial.println("Configuring access point...");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("ESP IP address: ");
  Serial.println(myIP);
  server.begin();

  // UDP starten + Feedback
  Serial.println("Starting UDP...");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(localPort);

  xTaskCreate(
      wdt_reset,   /* Task function. */
      "wdt_reset", /* String with name of task. */
      1024,        /* Stack size in bytes. */
      NULL,        /* Parameter passed as input of the task */
      1,           /* Priority of the task. */
      NULL);       /* Task handle. */

  xTaskCreate(
      refreshRecordDisplay, /* Task function. */
      "recordD_task",       /* String with name of task. */
      16384,                /* Stack size in bytes. */
      NULL,                 /* Parameter passed as input of the task */
      1,                    /* Priority of the task. */
      NULL);                /* Task handle. */

  TaskHandle_t _memcpyHandle = NULL;
  xTaskCreate(
      recordDMX,        /* Task function. */
      "recordDMX_task", /* String with name of task. */
      40000,            /* Stack size in bytes. */
      NULL,             /* Parameter passed as input of the task */
      1,                /* Priority of the task. */
      &_memcpyHandle);  /* Task handle. */

  //xTaskCreate(
  //    getMsg,    /* Task function. */
  //    "get_Msg", /* String with name of task. */
  //    1024,      /* Stack size in bytes. */
  //    NULL,      /* Parameter passed as input of the task */
  //    2,         /* Priority of the task. */
  //    NULL);     /* Task handle. */

  sendValue(0, "/RecordPage/recordLED");
}

void wdt_reset(void *parameter)
{
  while (true)
  {
    esp_task_wdt_feed();
    vTaskDelay(10);
  }
}
// Funktion: DMX in Output-Buffer geben
void copyDMXToOutput(void)
{
  xSemaphoreTake(ESP32DMX.lxDataLock, portMAX_DELAY);
  for (int i = 1; i < DMX_MAX_FRAME; i++)
  {
    ESP32DMX.setSlot(i, dmxbuffer[i]);
  }
  xSemaphoreGive(ESP32DMX.lxDataLock);
}

int channelRecord = 1;
int timeRecord = 0;
int totalSamples = 160;

unsigned long eventInterval = 40;
unsigned long previousTime = 0;

boolean record = 0;
boolean play = 0;

void start_recordDMX(OSCMessage &msg)
{
  record = true;
}
boolean refreshDisplay = 0;

uint8_t recordArray[160][513];

void recordDMX(void *parameter)
{

  while (true)
  {
    if (play)
    {
      boolean playDone = false;
      int playbackCounter = 0;
      while (!playDone)
      {
        unsigned long currentTime = millis();
        /* This is the event */
        if (currentTime - previousTime >= eventInterval && playbackCounter < totalSamples && !playDone)
        {
          /* Event code */
          memcpy(dmxbuffer, recordArray[playbackCounter], sizeof(uint8_t) * 513);
          copyDMXToOutput();
          Serial.print("Played Sample: ");
          Serial.println(playbackCounter + 1);
          playbackCounter++;
          /* Update the timing for the next time around */
          previousTime = currentTime;
          if (playbackCounter == totalSamples)
          {
            playDone = true;
          }
        }
      }
      play=0;
    }
    if (record)
    {
      boolean memcpyDone = false;
      sendValue(1, "/RecordPage/recordLED");
      while (!memcpyDone)
      {
        unsigned long currentTime = millis();
        if (currentTime - previousTime >= eventInterval && sampleCounter < totalSamples && !memcpyDone)
        {
          xSemaphoreTake(ESP32DMX.lxDataLock, portMAX_DELAY);
          memcpy(recordArray[sampleCounter], ESP32DMX.dmxData(), sizeof(uint8_t) * 513);
          xSemaphoreGive(ESP32DMX.lxDataLock);
          Serial.print("Copied Sample: ");
          Serial.println(sampleCounter + 1);
          sampleCounter++;
          previousTime = currentTime;
          if (sampleCounter == totalSamples)
          {
            memcpyDone = true;
            sendValue(0, "/RecordPage/recordLED");
          }
        }
      }

      //memcpyDone = false;
      sampleCounter = 0;
      feedTheDog();

      refreshDisplay = 1;

      record = false;
    }

    feedTheDog();
  }
}

void playRecorded(OSCMessage &msg)
{
  play=1;
}
void setChannelRecord(OSCMessage &msg)
{
  if (msg.isFloat(0))
  {
    channelRecord = msg.getFloat(0);
    sendValue(channelRecord, "/RecordPage/channelDisplayLabel1");
    sendValue(channelRecord + 1, "/RecordPage/channelDisplayLabel2");
    sendValue(channelRecord + 2, "/RecordPage/channelDisplayLabel3");
    sendValue(channelRecord + 3, "/RecordPage/channelDisplayLabel4");
    sendValue(channelRecord + 4, "/RecordPage/channelDisplayLabel5");
  }
  refreshDisplay = 1;
}

void setTimeRecord(OSCMessage &msg)
{
  if (msg.isFloat(0))
  {
    timeRecord = msg.getFloat(0);
    sendValue(timeRecord, "/RecordPage/sampleNumber_left");
    sendValue(timeRecord + 64, "/RecordPage/sampleNumber_right");
  }
  refreshDisplay = 1;
}

void refreshRecordDisplay(void *parameter)
{
  while (true)
  {
    if (refreshDisplay)
    {
      for (int i = 0; i < 64; i++)
      {
        char channelDisplayAdress[40];
        snprintf(channelDisplayAdress, sizeof(channelDisplayAdress), "/RecordPage/channelDisplay_1/%d", i + 1);
        sendValue(recordArray[i + timeRecord][channelRecord], channelDisplayAdress);
        snprintf(channelDisplayAdress, sizeof(channelDisplayAdress), "/RecordPage/channelDisplay_2/%d", i + 1);
        sendValue(recordArray[i + timeRecord][channelRecord + 1], channelDisplayAdress);
        snprintf(channelDisplayAdress, sizeof(channelDisplayAdress), "/RecordPage/channelDisplay_3/%d", i + 1);
        sendValue(recordArray[i + timeRecord][channelRecord + 2], channelDisplayAdress);
        snprintf(channelDisplayAdress, sizeof(channelDisplayAdress), "/RecordPage/channelDisplay_4/%d", i + 1);
        sendValue(recordArray[i + timeRecord][channelRecord + 3], channelDisplayAdress);
        snprintf(channelDisplayAdress, sizeof(channelDisplayAdress), "/RecordPage/channelDisplay_5/%d", i + 1);
        sendValue(recordArray[i + timeRecord][channelRecord + 4], channelDisplayAdress);
      }
      refreshDisplay = 0;
    }
    feedTheDog();
  }
}

// Funktion: OSC-Messages abrufen und weitergeben
void getMsg()
{

  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0)
  {
    while (size--)
    {
      msg.fill(Udp.read());
    }
    if (!msg.hasError())
    {

      // Weiterleitung von Adressaufrufen an passende Funktion
      msg.dispatch("/SendPage", setModeSend);
      msg.dispatch("/ReceivePage", setModeReceive);
      msg.dispatch("/RecordPage", setModeReceive);
      msg.dispatch("/ReceivePage/Channel-", channelDekrement);
      msg.dispatch("/ReceivePage/Channel+", channelInkrement);
      msg.dispatch("/SendPage/Channel-", channelDekrement);
      msg.dispatch("/SendPage/Channel+", channelInkrement);
      msg.dispatch("/SendPage/Level-", levelDekrement);
      msg.dispatch("/SendPage/Level+", levelInkrement);
      msg.dispatch("/RecordPage/record", start_recordDMX);
      msg.dispatch("/RecordPage/fSample", setfSample);
      msg.dispatch("/SendPage/play", playRecorded);
      msg.dispatch("/RecordPage/channelScrollbar", setChannelRecord);
      msg.dispatch("/RecordPage/timeScrollbar", setTimeRecord);
    }
    else
    {
      error = msg.getError();
      Serial.print("Error: ");
      Serial.println(error);
    }
  }
}

// Funktion: Text an ein Label auf der Oberfläche zu senden
void sendMsg(char text[], const char adress[])
{
  OSCMessage msg(adress);
  msg.add(text);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

// Funktion: Werte an ein Label auf der Oberfläche zu senden
void sendValue(int value, const char adress[])
{
  OSCMessage msg(adress);
  msg.add(value);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

void setfSample(OSCMessage &msg)
{
  if (msg.isFloat(0))
  {
    eventInterval = msg.getFloat(0);
    sendValue(eventInterval, "/ReceivePage/fSampleLabel");
  }
}

bool modeFlag = 1; // ReceiveMode = 0, SendMode = 1

// Funktion die aufgerufen wenn OSC-Message zum Wechsel in Send-Mode kommt
void setModeSend(OSCMessage &msg)
{

  bool sendState = msg.getFloat(0); // 1 für Send, 0 für Receive
  if (sendState && !modeFlag)
  {
    // Setup Send-spezifisch
    Serial.print("Setup DMX");

    pinMode(DMX_DIRECTION_PIN, OUTPUT);
    digitalWrite(DMX_DIRECTION_PIN, HIGH);

    pinMode(DMX_SERIAL_OUTPUT_PIN, OUTPUT);
    ESP32DMX.startOutput(DMX_SERIAL_OUTPUT_PIN);
    Serial.println("Setup DMX complete");
    Serial.println("***sendMode activated***");
    modeFlag = 1;
    sendMode();
  }
  else
  {
    sendMode();
  }
}

// Funktion die aufgerufen wenn OSC-Message zum Wechsel in Receive-Mode kommt
void setModeReceive(OSCMessage &msg)
{
  bool receiveState = msg.getFloat(0); // 1 für Send, 0 für Receive
  if (receiveState && modeFlag)
  {
    // Setup Receive-spezifisch
    ESP32DMX.setDirectionPin(DMX_DIRECTION_PIN);

    Serial.print(", set callback");
    ESP32DMX.setDataReceivedCallback(receiveCallback);

    Serial.print(", start dmx input");
    ESP32DMX.startInput(DMX_SERIAL_INPUT_PIN);

    Serial.println(", setup complete.");
    Serial.println("***receiveMode activated***");
    modeFlag = 0;
    receiveMode();
  }
  else
  {
    receiveMode();
  }
}

// Funktion: Im Send-Mode Test-Level inkrementieren
void levelInkrement(OSCMessage &msg)
{
  if (testLevel == 255)
  {
    testLevel = 0;
  }
  else
  {
    testLevel++;
  }
  sendValue(testLevel, "/SendPage/Level"); // labelMatrix[0]
  dmxbuffer[testChannel] = testLevel;
}

//Funktion: Im Send-Mode Test-Level dektrementieren
void levelDekrement(OSCMessage &msg)
{
  if (testLevel > 0)
  {
    testLevel--;
  }
  else
  {
    testLevel = 255;
  }
  sendValue(testLevel, "/SendPage/Level");
  dmxbuffer[testChannel] = testLevel;
}

// Funktionen um den Channel zu wählen:

// Channel --
void channelDekrement(OSCMessage &msg)
{
  if (testChannel > 1)
  {
    testChannel--;
  }
  else
  {
    testChannel = 512;
  }
  sendValue(testChannel, "/ReceivePage/Channel");
  sendValue(testChannel, "/SendPage/Channel");
  sendValue(dmxbuffer[testChannel], "/SendPage/Level");
}

// Channel ++
void channelInkrement(OSCMessage &msg)
{
  if (testChannel == 512)
  {
    testChannel = 1;
  }
  else
  {
    testChannel++;
  }
  sendValue(testChannel, "/ReceivePage/Channel");
  sendValue(testChannel, "/SendPage/Channel");
  sendValue(dmxbuffer[testChannel], "/SendPage/Level");
}

// Send-Modus-Schleife

void sendMode()
{

  while (true)
  {
    copyDMXToOutput();
    feedTheDog();
    getMsg();
  }
}

// Receive-Modus-Schleife

bool flag = false;

void receiveMode()
{

  while (true)
  {
    if (dataChanged)
    {
      dataChanged = 0;
      sendValue(testLevel, "/ReceivePage/Level");
    }
    else
    {
      feedTheDog();
      getMsg();
    }
  }
}

void loop()
{
  OSCMessage startmsg;
  setModeReceive(startmsg); // State nach Reset: Send-Modus
}
