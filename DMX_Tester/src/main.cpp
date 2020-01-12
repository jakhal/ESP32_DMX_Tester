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
const char *ssid = "DMX_Tester";              // Name des Netzwerks
const char *password = "dmx_tester";         // Passwort des Netzwerks

WiFiServer server(80);                      // Server Instanz -> ESP als Access Point
WiFiUDP Udp;                                // UDP Instanz um Pakete über UDP zu senden / empfangen
                                            
const IPAddress outIp(192,168,4,2);         // IP des Clients
const unsigned int outPort = 9999;          // Incoming-Port des Clients
const unsigned int localPort = 8888;        // Incoming-Ports des Hosts für ankommende Nachrichten

OSCErrorCode error;

//******************************REC**************************************
const char *MultifaderAdressMatrix[64] = {"/ReceivePage/recordDisplay/1","/ReceivePage/recordDisplay/2","/ReceivePage/recordDisplay/3","/ReceivePage/recordDisplay/4","/ReceivePage/recordDisplay/5","/ReceivePage/recordDisplay/6","/ReceivePage/recordDisplay/7","/ReceivePage/recordDisplay/8","/ReceivePage/recordDisplay/9","/ReceivePage/recordDisplay/10","/ReceivePage/recordDisplay/11","/ReceivePage/recordDisplay/12","/ReceivePage/recordDisplay/13","/ReceivePage/recordDisplay/14","/ReceivePage/recordDisplay/15","/ReceivePage/recordDisplay/16","/ReceivePage/recordDisplay/17","/ReceivePage/recordDisplay/18","/ReceivePage/recordDisplay/19","/ReceivePage/recordDisplay/20","/ReceivePage/recordDisplay/21","/ReceivePage/recordDisplay/22","/ReceivePage/recordDisplay/23","/ReceivePage/recordDisplay/24","/ReceivePage/recordDisplay/25","/ReceivePage/recordDisplay/26","/ReceivePage/recordDisplay/27","/ReceivePage/recordDisplay/28","/ReceivePage/recordDisplay/29","/ReceivePage/recordDisplay/30","/ReceivePage/recordDisplay/31","/ReceivePage/recordDisplay/32","/ReceivePage/recordDisplay/33","/ReceivePage/recordDisplay/34","/ReceivePage/recordDisplay/35","/ReceivePage/recordDisplay/36","/ReceivePage/recordDisplay/37","/ReceivePage/recordDisplay/38","/ReceivePage/recordDisplay/39","/ReceivePage/recordDisplay/40","/ReceivePage/recordDisplay/41","/ReceivePage/recordDisplay/42","/ReceivePage/recordDisplay/43","/ReceivePage/recordDisplay/44","/ReceivePage/recordDisplay/45","/ReceivePage/recordDisplay/46","/ReceivePage/recordDisplay/47","/ReceivePage/recordDisplay/48","/ReceivePage/recordDisplay/49","/ReceivePage/recordDisplay/50","/ReceivePage/recordDisplay/51","/ReceivePage/recordDisplay/52","/ReceivePage/recordDisplay/53","/ReceivePage/recordDisplay/54","/ReceivePage/recordDisplay/55","/ReceivePage/recordDisplay/56","/ReceivePage/recordDisplay/57","/ReceivePage/recordDisplay/58","/ReceivePage/recordDisplay/59","/ReceivePage/recordDisplay/60","/ReceivePage/recordDisplay/61","/ReceivePage/recordDisplay/62","/ReceivePage/recordDisplay/63","/ReceivePage/recordDisplay/64"};
const char *channelDisplay_1_Adresses[64] = {"/RecordPage/channelDisplay_1/1","/RecordPage/channelDisplay_1/2","/RecordPage/channelDisplay_1/3","/RecordPage/channelDisplay_1/4","/RecordPage/channelDisplay_1/5","/RecordPage/channelDisplay_1/6","/RecordPage/channelDisplay_1/7","/RecordPage/channelDisplay_1/8","/RecordPage/channelDisplay_1/9","/RecordPage/channelDisplay_1/10","/RecordPage/channelDisplay_1/11","/RecordPage/channelDisplay_1/12","/RecordPage/channelDisplay_1/13","/RecordPage/channelDisplay_1/14","/RecordPage/channelDisplay_1/15","/RecordPage/channelDisplay_1/16","/RecordPage/channelDisplay_1/17","/RecordPage/channelDisplay_1/18","/RecordPage/channelDisplay_1/19","/RecordPage/channelDisplay_1/20","/RecordPage/channelDisplay_1/21","/RecordPage/channelDisplay_1/22","/RecordPage/channelDisplay_1/23","/RecordPage/channelDisplay_1/24","/RecordPage/channelDisplay_1/25","/RecordPage/channelDisplay_1/26","/RecordPage/channelDisplay_1/27","/RecordPage/channelDisplay_1/28","/RecordPage/channelDisplay_1/29","/RecordPage/channelDisplay_1/30","/RecordPage/channelDisplay_1/31","/RecordPage/channelDisplay_1/32","/RecordPage/channelDisplay_1/33","/RecordPage/channelDisplay_1/34","/RecordPage/channelDisplay_1/35","/RecordPage/channelDisplay_1/36","/RecordPage/channelDisplay_1/37","/RecordPage/channelDisplay_1/38","/RecordPage/channelDisplay_1/39","/RecordPage/channelDisplay_1/40","/RecordPage/channelDisplay_1/41","/RecordPage/channelDisplay_1/42","/RecordPage/channelDisplay_1/43","/RecordPage/channelDisplay_1/44","/RecordPage/channelDisplay_1/45","/RecordPage/channelDisplay_1/46","/RecordPage/channelDisplay_1/47","/RecordPage/channelDisplay_1/48","/RecordPage/channelDisplay_1/49","/RecordPage/channelDisplay_1/50","/RecordPage/channelDisplay_1/51","/RecordPage/channelDisplay_1/52","/RecordPage/channelDisplay_1/53","/RecordPage/channelDisplay_1/54","/RecordPage/channelDisplay_1/55","/RecordPage/channelDisplay_1/56","/RecordPage/channelDisplay_1/57","/RecordPage/channelDisplay_1/58","/RecordPage/channelDisplay_1/59","/RecordPage/channelDisplay_1/60","/RecordPage/channelDisplay_1/61","/RecordPage/channelDisplay_1/62","/RecordPage/channelDisplay_1/63","/RecordPage/channelDisplay_1/64"};
const char *channelDisplay_2_Adresses[64] = {"/RecordPage/channelDisplay_2/1","/RecordPage/channelDisplay_2/2","/RecordPage/channelDisplay_2/3","/RecordPage/channelDisplay_2/4","/RecordPage/channelDisplay_2/5","/RecordPage/channelDisplay_2/6","/RecordPage/channelDisplay_2/7","/RecordPage/channelDisplay_2/8","/RecordPage/channelDisplay_2/9","/RecordPage/channelDisplay_2/10","/RecordPage/channelDisplay_2/11","/RecordPage/channelDisplay_2/12","/RecordPage/channelDisplay_2/13","/RecordPage/channelDisplay_2/14","/RecordPage/channelDisplay_2/15","/RecordPage/channelDisplay_2/16","/RecordPage/channelDisplay_2/17","/RecordPage/channelDisplay_2/18","/RecordPage/channelDisplay_2/19","/RecordPage/channelDisplay_2/20","/RecordPage/channelDisplay_2/21","/RecordPage/channelDisplay_2/22","/RecordPage/channelDisplay_2/23","/RecordPage/channelDisplay_2/24","/RecordPage/channelDisplay_2/25","/RecordPage/channelDisplay_2/26","/RecordPage/channelDisplay_2/27","/RecordPage/channelDisplay_2/28","/RecordPage/channelDisplay_2/29","/RecordPage/channelDisplay_2/30","/RecordPage/channelDisplay_2/31","/RecordPage/channelDisplay_2/32","/RecordPage/channelDisplay_2/33","/RecordPage/channelDisplay_2/34","/RecordPage/channelDisplay_2/35","/RecordPage/channelDisplay_2/36","/RecordPage/channelDisplay_2/37","/RecordPage/channelDisplay_2/38","/RecordPage/channelDisplay_2/39","/RecordPage/channelDisplay_2/40","/RecordPage/channelDisplay_2/41","/RecordPage/channelDisplay_2/42","/RecordPage/channelDisplay_2/43","/RecordPage/channelDisplay_2/44","/RecordPage/channelDisplay_2/45","/RecordPage/channelDisplay_2/46","/RecordPage/channelDisplay_2/47","/RecordPage/channelDisplay_2/48","/RecordPage/channelDisplay_2/49","/RecordPage/channelDisplay_2/50","/RecordPage/channelDisplay_2/51","/RecordPage/channelDisplay_2/52","/RecordPage/channelDisplay_2/53","/RecordPage/channelDisplay_2/54","/RecordPage/channelDisplay_2/55","/RecordPage/channelDisplay_2/56","/RecordPage/channelDisplay_2/57","/RecordPage/channelDisplay_2/58","/RecordPage/channelDisplay_2/59","/RecordPage/channelDisplay_2/60","/RecordPage/channelDisplay_2/61","/RecordPage/channelDisplay_2/62","/RecordPage/channelDisplay_2/63","/RecordPage/channelDisplay_2/64"};
const char *channelDisplay_3_Adresses[64] = {"/RecordPage/channelDisplay_3/1","/RecordPage/channelDisplay_3/2","/RecordPage/channelDisplay_3/3","/RecordPage/channelDisplay_3/4","/RecordPage/channelDisplay_3/5","/RecordPage/channelDisplay_3/6","/RecordPage/channelDisplay_3/7","/RecordPage/channelDisplay_3/8","/RecordPage/channelDisplay_3/9","/RecordPage/channelDisplay_3/10","/RecordPage/channelDisplay_3/11","/RecordPage/channelDisplay_3/12","/RecordPage/channelDisplay_3/13","/RecordPage/channelDisplay_3/14","/RecordPage/channelDisplay_3/15","/RecordPage/channelDisplay_3/16","/RecordPage/channelDisplay_3/17","/RecordPage/channelDisplay_3/18","/RecordPage/channelDisplay_3/19","/RecordPage/channelDisplay_3/20","/RecordPage/channelDisplay_3/21","/RecordPage/channelDisplay_3/22","/RecordPage/channelDisplay_3/23","/RecordPage/channelDisplay_3/24","/RecordPage/channelDisplay_3/25","/RecordPage/channelDisplay_3/26","/RecordPage/channelDisplay_3/27","/RecordPage/channelDisplay_3/28","/RecordPage/channelDisplay_3/29","/RecordPage/channelDisplay_3/30","/RecordPage/channelDisplay_3/31","/RecordPage/channelDisplay_3/32","/RecordPage/channelDisplay_3/33","/RecordPage/channelDisplay_3/34","/RecordPage/channelDisplay_3/35","/RecordPage/channelDisplay_3/36","/RecordPage/channelDisplay_3/37","/RecordPage/channelDisplay_3/38","/RecordPage/channelDisplay_3/39","/RecordPage/channelDisplay_3/40","/RecordPage/channelDisplay_3/41","/RecordPage/channelDisplay_3/42","/RecordPage/channelDisplay_3/43","/RecordPage/channelDisplay_3/44","/RecordPage/channelDisplay_3/45","/RecordPage/channelDisplay_3/46","/RecordPage/channelDisplay_3/47","/RecordPage/channelDisplay_3/48","/RecordPage/channelDisplay_3/49","/RecordPage/channelDisplay_3/50","/RecordPage/channelDisplay_3/51","/RecordPage/channelDisplay_3/52","/RecordPage/channelDisplay_3/53","/RecordPage/channelDisplay_3/54","/RecordPage/channelDisplay_3/55","/RecordPage/channelDisplay_3/56","/RecordPage/channelDisplay_3/57","/RecordPage/channelDisplay_3/58","/RecordPage/channelDisplay_3/59","/RecordPage/channelDisplay_3/60","/RecordPage/channelDisplay_3/61","/RecordPage/channelDisplay_3/62","/RecordPage/channelDisplay_3/63","/RecordPage/channelDisplay_3/64"};
//const char *channelDisplay_4_Adresses[64] = {"/RecordPage/channelDisplay_4/1","/RecordPage/channelDisplay_4/2","/RecordPage/channelDisplay_4/3","/RecordPage/channelDisplay_4/4","/RecordPage/channelDisplay_4/5","/RecordPage/channelDisplay_4/6","/RecordPage/channelDisplay_4/7","/RecordPage/channelDisplay_4/8","/RecordPage/channelDisplay_4/9","/RecordPage/channelDisplay_4/10","/RecordPage/channelDisplay_4/11","/RecordPage/channelDisplay_4/12","/RecordPage/channelDisplay_4/13","/RecordPage/channelDisplay_4/14","/RecordPage/channelDisplay_4/15","/RecordPage/channelDisplay_4/16","/RecordPage/channelDisplay_4/17","/RecordPage/channelDisplay_4/18","/RecordPage/channelDisplay_4/19","/RecordPage/channelDisplay_4/20","/RecordPage/channelDisplay_4/21","/RecordPage/channelDisplay_4/22","/RecordPage/channelDisplay_4/23","/RecordPage/channelDisplay_4/24","/RecordPage/channelDisplay_4/25","/RecordPage/channelDisplay_4/26","/RecordPage/channelDisplay_4/27","/RecordPage/channelDisplay_4/28","/RecordPage/channelDisplay_4/29","/RecordPage/channelDisplay_4/30","/RecordPage/channelDisplay_4/31","/RecordPage/channelDisplay_4/32","/RecordPage/channelDisplay_4/33","/RecordPage/channelDisplay_4/34","/RecordPage/channelDisplay_4/35","/RecordPage/channelDisplay_4/36","/RecordPage/channelDisplay_4/37","/RecordPage/channelDisplay_4/38","/RecordPage/channelDisplay_4/39","/RecordPage/channelDisplay_4/40","/RecordPage/channelDisplay_4/41","/RecordPage/channelDisplay_4/42","/RecordPage/channelDisplay_4/43","/RecordPage/channelDisplay_4/44","/RecordPage/channelDisplay_4/45","/RecordPage/channelDisplay_4/46","/RecordPage/channelDisplay_4/47","/RecordPage/channelDisplay_4/48","/RecordPage/channelDisplay_4/49","/RecordPage/channelDisplay_4/50","/RecordPage/channelDisplay_4/51","/RecordPage/channelDisplay_4/52","/RecordPage/channelDisplay_4/53","/RecordPage/channelDisplay_4/54","/RecordPage/channelDisplay_4/55","/RecordPage/channelDisplay_4/56","/RecordPage/channelDisplay_4/57","/RecordPage/channelDisplay_4/58","/RecordPage/channelDisplay_4/59","/RecordPage/channelDisplay_4/60","/RecordPage/channelDisplay_4/61","/RecordPage/channelDisplay_4/62","/RecordPage/channelDisplay_4/63","/RecordPage/channelDisplay_4/64"};
//const char *channelDisplay_5_Adresses[64] = {"/RecordPage/channelDisplay_5/1","/RecordPage/channelDisplay_5/2","/RecordPage/channelDisplay_5/3","/RecordPage/channelDisplay_5/4","/RecordPage/channelDisplay_5/5","/RecordPage/channelDisplay_5/6","/RecordPage/channelDisplay_5/7","/RecordPage/channelDisplay_5/8","/RecordPage/channelDisplay_5/9","/RecordPage/channelDisplay_5/10","/RecordPage/channelDisplay_5/11","/RecordPage/channelDisplay_5/12","/RecordPage/channelDisplay_5/13","/RecordPage/channelDisplay_5/14","/RecordPage/channelDisplay_5/15","/RecordPage/channelDisplay_5/16","/RecordPage/channelDisplay_5/17","/RecordPage/channelDisplay_5/18","/RecordPage/channelDisplay_5/19","/RecordPage/channelDisplay_5/20","/RecordPage/channelDisplay_5/21","/RecordPage/channelDisplay_5/22","/RecordPage/channelDisplay_5/23","/RecordPage/channelDisplay_5/24","/RecordPage/channelDisplay_5/25","/RecordPage/channelDisplay_5/26","/RecordPage/channelDisplay_5/27","/RecordPage/channelDisplay_5/28","/RecordPage/channelDisplay_5/29","/RecordPage/channelDisplay_5/30","/RecordPage/channelDisplay_5/31","/RecordPage/channelDisplay_5/32","/RecordPage/channelDisplay_5/33","/RecordPage/channelDisplay_5/34","/RecordPage/channelDisplay_5/35","/RecordPage/channelDisplay_5/36","/RecordPage/channelDisplay_5/37","/RecordPage/channelDisplay_5/38","/RecordPage/channelDisplay_5/39","/RecordPage/channelDisplay_5/40","/RecordPage/channelDisplay_5/41","/RecordPage/channelDisplay_5/42","/RecordPage/channelDisplay_5/43","/RecordPage/channelDisplay_5/44","/RecordPage/channelDisplay_5/45","/RecordPage/channelDisplay_5/46","/RecordPage/channelDisplay_5/47","/RecordPage/channelDisplay_5/48","/RecordPage/channelDisplay_5/49","/RecordPage/channelDisplay_5/50","/RecordPage/channelDisplay_5/51","/RecordPage/channelDisplay_5/52","/RecordPage/channelDisplay_5/53","/RecordPage/channelDisplay_5/54","/RecordPage/channelDisplay_5/55","/RecordPage/channelDisplay_5/56","/RecordPage/channelDisplay_5/57","/RecordPage/channelDisplay_5/58","/RecordPage/channelDisplay_5/59","/RecordPage/channelDisplay_5/60","/RecordPage/channelDisplay_5/61","/RecordPage/channelDisplay_5/62","/RecordPage/channelDisplay_5/63","/RecordPage/channelDisplay_5/64"};

int sampleCounter = 0;
/************************************************************************

  callback for when DMX frame is received
  Note:  called from receive task
  
  Checks to see if the level of the designated slot has changed
  and sets the dataChanged flag.

  Processing in this callback should be minimal.

  Instead, use a flag and do more intense processing in a lower priority task.
  
*************************************************************************/

void receiveCallback(int slots) {
	if ( slots ) {
	    xSemaphoreTake( ESP32DMX.lxDataLock, portMAX_DELAY );
		if ( testLevel != ESP32DMX.getSlot(testChannel) ) {
			testLevel = ESP32DMX.getSlot(testChannel);
			dataChanged = 1;
		}
		xSemaphoreGive( ESP32DMX.lxDataLock );
	}
}

/***********************************************************************
 TIMER INTERRUPT
 ************************************************************************/
/* create a hardware timer */
hw_timer_t * timer = NULL;

/* LED pin */
int led = 13;
/* LED state */
volatile byte state = LOW;

void IRAM_ATTR onTimer(){ // Timer-Interrupt mit Samplingfrequenz, Matrix als Abweichung von Null-Matrix
  state = !state;
  digitalWrite(led, state);
  Serial.println("INTERRUPT"); // for debugging
}

/************************************************************************
	setup Funktion
*************************************************************************/

void wdt_reset ( void * parameter);

void setup() { 
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
                  wdt_reset,          /* Task function. */
                  "wdt_reset",        /* String with name of task. */
                  1024,            /* Stack size in bytes. */
                  NULL,             /* Parameter passed as input of the task */
                  5,                /* Priority of the task. */
                  NULL);            /* Task handle. */
  

}

void wdt_reset ( void * parameter){
  while(true){
      esp_task_wdt_feed();
      vTaskDelay(10);
  }
  
}
// Funktion: DMX in Output-Buffer geben
void copyDMXToOutput(void) {
  xSemaphoreTake( ESP32DMX.lxDataLock, portMAX_DELAY );
	for (int i=1; i<DMX_MAX_FRAME; i++) {
    	ESP32DMX.setSlot(i , dmxbuffer[i]);
   }
   xSemaphoreGive( ESP32DMX.lxDataLock );
}



int channelRecord = 1;
int timeRecord = 0;
int totalSamples = 160;

unsigned long eventInterval = 40;
unsigned long previousTime = 0;
TaskHandle_t _memcpyHandle = NULL;
boolean taskCreated = 0;
boolean record = 0;

void start_recordDMX(OSCMessage &msg){
  if(!taskCreated){
  xTaskCreate(
                    recordDMX,          /* Task function. */
                    "recordDMX_task",        /* String with name of task. */
                    40000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    &_memcpyHandle);            /* Task handle. */
  taskCreated = 1;
  //esp_task_wdt_add(_memcpyHandle);
 }
else {record = 1;}
}

uint8_t recordArray[160][513];

void recordDMX( void * parameter){
  


while(true){

/* Updates frequently */
if(record){  
boolean memcpyDone = false;
sendValue(1,"/ReceivePage/recordLED");
while(!memcpyDone){
  unsigned long currentTime = millis();
  esp_task_wdt_feed(); // DMX-Library spezifisch, damit WDT nicht überläuft?
    /* This is the event */
  if (currentTime - previousTime >= eventInterval && sampleCounter < totalSamples && !memcpyDone) {
    /* Event code */
    xSemaphoreTake( ESP32DMX.lxDataLock, portMAX_DELAY );
    memcpy(recordArray[sampleCounter], ESP32DMX.dmxData(), sizeof(uint8_t)*513);  
    xSemaphoreGive( ESP32DMX.lxDataLock ); 
    Serial.print("Copied Sample: ");
    Serial.println(sampleCounter+1);
    sampleCounter++;
   /* Update the timing for the next time around */
    previousTime = currentTime;
    if(sampleCounter == totalSamples){memcpyDone = true;sendValue(0,"/ReceivePage/recordLED");sendValue(0,"/RecordPage/recordLED");}
  }
}

memcpyDone = false;
sampleCounter = 0;





for(int i = 0; i < totalSamples; i++){
          esp_task_wdt_feed(); // DMX-Library spezifisch, damit WDT nicht überläuft?
          Serial.print("\n\nSample: ");
          Serial.println(i+1);
          Serial.println("_______________________________________________");
          if(i<64){sendValue(recordArray[i][testChannel],MultifaderAdressMatrix[i]);}
        /*for(int j = 0; j<512; j++){
          Serial.print(" ");
          Serial.print(recordArray[i][j]);
          Serial.print(" ");
          if((j+1) % 32 == 0 ){
            Serial.print("\n");
          }
        }
        Serial.println("_______________________________________________");
        */
  }

record = false;
}

esp_task_wdt_feed(); // DMX-Library spezifisch, damit WDT nicht überläuft?
vTaskDelay(10);       // 1 Tick Delay


}
}

void setChannelRecord(OSCMessage &msg){
  if(msg.isFloat(0)){
    channelRecord = msg.getFloat(0);
    sendValue(channelRecord,  "/RecordPage/channelDisplayLabel1");
    sendValue(channelRecord+1,"/RecordPage/channelDisplayLabel2");
    sendValue(channelRecord+2,"/RecordPage/channelDisplayLabel3");
    sendValue(channelRecord+3,"/RecordPage/channelDisplayLabel4");
    sendValue(channelRecord+4,"/RecordPage/channelDisplayLabel5");
    }
  refreshRecordDisplay();
}

void setTimeRecord(OSCMessage &msg){
  if(msg.isFloat(0)){
    timeRecord = msg.getFloat(0);
    sendValue(timeRecord,     "/RecordPage/sampleNumber_left");
    sendValue(timeRecord+64,  "/RecordPage/sampleNumber_right");
    }
  refreshRecordDisplay();
}

void refreshRecordDisplay(){
  for(int i = 0; i<64; i++){
    sendValue(recordArray[i+timeRecord][channelRecord],  channelDisplay_1_Adresses[i]);
    sendValue(recordArray[i+timeRecord][channelRecord+1],channelDisplay_2_Adresses[i]);
    sendValue(recordArray[i+timeRecord][channelRecord+2],channelDisplay_3_Adresses[i]);
    //sendValue(recordArray[i+timeRecord][channelRecord+3],channelDisplay_4_Adresses[i]);
    //sendValue(recordArray[i+timeRecord][channelRecord+4],channelDisplay_5_Adresses[i]);
  }
}

// Funktion: OSC-Messages abrufen und weitergeben
void getMsg() {
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      
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
      
      
    } else {
      error = msg.getError();
      Serial.print("Error: ");
      Serial.println(error);
    }
  }
}

// Funktion: Text an ein Label auf der Oberfläche zu senden
void sendMsg(char text[], const char adress[]){
  OSCMessage msg(adress);
  msg.add(text);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

// Funktion: Werte an ein Label auf der Oberfläche zu senden
void sendValue(int value, const char adress[]){
  OSCMessage msg(adress);
  msg.add(value);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
}

void setfSample(OSCMessage &msg){
  if(msg.isFloat(0)){
    eventInterval = msg.getFloat(0);
    sendValue(eventInterval,"/ReceivePage/fSampleLabel");
    }
}


void playRecorded(OSCMessage &msg){
  boolean playDone = false;
  int playbackCounter = 0;
  while(!playDone){
  unsigned long currentTime = millis();
  esp_task_wdt_feed(); // DMX-Library spezifisch, damit WDT nicht überläuft?
    /* This is the event */
  if (currentTime - previousTime >= eventInterval && playbackCounter < totalSamples && !playDone) {
    /* Event code */
    memcpy(dmxbuffer, recordArray[playbackCounter], sizeof(uint8_t)*513);
    copyDMXToOutput();  
    Serial.print("Played Sample: ");
    Serial.println(playbackCounter+1);
    playbackCounter++;
   /* Update the timing for the next time around */
    previousTime = currentTime;
    if(playbackCounter == totalSamples){playDone = true;}
  }
}
}


// Funktion die aufgerufen wenn OSC-Message zum Wechsel in Send-Mode kommt
void setModeSend(OSCMessage &msg){
  bool sendState = msg.getFloat(0); // 1 für Send, 0 für Receive
  if(sendState){
    // Setup Send-spezifisch
    Serial.print("Setup DMX");
    
    pinMode(DMX_DIRECTION_PIN, OUTPUT);
    digitalWrite(DMX_DIRECTION_PIN, HIGH);

    pinMode(DMX_SERIAL_OUTPUT_PIN, OUTPUT);
    ESP32DMX.startOutput(DMX_SERIAL_OUTPUT_PIN);
    Serial.println("Setup DMX complete");
    Serial.println("***sendMode activated***");
    sendMode();
  }
}

// Funktion die aufgerufen wenn OSC-Message zum Wechsel in Receive-Mode kommt
void setModeReceive(OSCMessage &msg){
  bool receiveState = msg.getFloat(0); // 1 für Send, 0 für Receive
  if(receiveState){
          // Setup Receive-spezifisch
      ESP32DMX.setDirectionPin(DMX_DIRECTION_PIN);

      Serial.print(", set callback");
      ESP32DMX.setDataReceivedCallback(receiveCallback);

      Serial.print(", start dmx input");
      ESP32DMX.startInput(DMX_SERIAL_INPUT_PIN);
      
      Serial.println(", setup complete.");
      Serial.println("***receiveMode activated***");
      
      receiveMode();
  }
}

// Funktion: Im Send-Mode Test-Level inkrementieren
void levelInkrement(OSCMessage &msg){
  if(testLevel == 255){
    testLevel = 0;
  }
  else{
    testLevel++;
  }
  sendValue(testLevel,"/SendPage/Level"); // labelMatrix[0]
  dmxbuffer[testChannel] = testLevel;
}

//Funktion: Im Send-Mode Test-Level dektrementieren
void levelDekrement(OSCMessage &msg){
  if(testLevel > 0){
    testLevel--;
  }
  else{
    testLevel = 255;
  }
  sendValue(testLevel,"/SendPage/Level"); 
  dmxbuffer[testChannel] = testLevel; 
}

// Funktionen um den Channel zu wählen:

// Channel --
void channelDekrement(OSCMessage &msg){
  if(testChannel > 1){
    testChannel--;
  }
  else{
    testChannel = 512;
  }
  sendValue(testChannel,"/ReceivePage/Channel");
  sendValue(testChannel,"/SendPage/Channel");
  sendValue(dmxbuffer[testChannel], "/SendPage/Level");
}

// Channel ++
void channelInkrement(OSCMessage &msg){
  if(testChannel == 512){
    testChannel = 1;
  }
  else{
    testChannel++;
  }
  sendValue(testChannel,"/ReceivePage/Channel");
  sendValue(testChannel,"/SendPage/Channel");
  sendValue(dmxbuffer[testChannel], "/SendPage/Level");
}

// Send-Modus-Schleife

void sendMode(){

  while(true){
    getMsg();
    copyDMXToOutput();
    esp_task_wdt_feed(); // DMX-Library spezifisch, damit WDT nicht überläuft?
    vTaskDelay(1);       // 1 Tick Delay
  }
}

// Receive-Modus-Schleife

bool flag = false;

void receiveMode(){

  while(true){
    esp_task_wdt_feed();
    getMsg();
    if ( dataChanged ) {
      dataChanged = 0;
      sendValue(testLevel, "/ReceivePage/Level");
  } else {
      vTaskDelay(150);  // vTaskDelay is called to prevent wdt timeout
      
      
  }
  }
}

void loop(){
OSCMessage startmsg;
setModeReceive(startmsg); // State nach Reset: Send-Modus
}

