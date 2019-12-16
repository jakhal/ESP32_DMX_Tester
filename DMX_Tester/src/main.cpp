/**************************************************************************/
/*!
    @file     outputTest.ino
    @author   Claude Heintz
    @license  BSD (see LXESP32DMX LICENSE)
    @copyright 2017 by Claude Heintz

    Simple Fade test of ESP32 DMX Driver
    @section  HISTORY

    v1.00 - First release
*/
/**************************************************************************/

// DMX Bibliotheken
#include <LXESP32DMX.h> 
#include <esp_task_wdt.h>

// WiFi Bibliotheken
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiAP.h>

// OSC Bibliotheken
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>


#define DMX_DIRECTION_PIN 21
#define DMX_SERIAL_OUTPUT_PIN 17
#define DMX_SERIAL_INPUT_PIN 16

//*****************************DMX**************************************************

uint8_t level;
uint8_t dmxbuffer[DMX_MAX_FRAME];


// the addresses of the slots to observe
int testChannel = 10;

// the levels of those slots
uint8_t testLevel = 0;

// flag set, when dmx data is changed
uint8_t dataChanged = 1;

//***************************WIFI & OSC *********************************
const char *labelMatrix[512] = {"/SendPage/Level", "LevelChannel2", "LevelChannel3", "LevelChannel4", "LevelChannel5", "LevelChannel6", "LevelChannel7", "LevelChannel8", "LevelChannel9", "LevelChannel10", "LevelChannel11", "LevelChannel12", "LevelChannel13", "LevelChannel14", "LevelChannel15", "LevelChannel16", "LevelChannel17", "LevelChannel18", "LevelChannel19", "LevelChannel20", "LevelChannel21", "LevelChannel22", "LevelChannel23", "LevelChannel24", "LevelChannel25", "LevelChannel26", "LevelChannel27", "LevelChannel28", "LevelChannel29", "LevelChannel30", "LevelChannel31", "LevelChannel32", "LevelChannel33", "LevelChannel34", "LevelChannel35", "LevelChannel36", "LevelChannel37", "LevelChannel38", "LevelChannel39", "LevelChannel40", "LevelChannel41", "LevelChannel42", "LevelChannel43", "LevelChannel44", "LevelChannel45", "LevelChannel46", "LevelChannel47", "LevelChannel48", "LevelChannel49", "LevelChannel50", "LevelChannel51", "LevelChannel52", "LevelChannel53", "LevelChannel54", "LevelChannel55", "LevelChannel56", "LevelChannel57", "LevelChannel58", "LevelChannel59", "LevelChannel60", "LevelChannel61", "LevelChannel62", "LevelChannel63", "LevelChannel64", "LevelChannel65", "LevelChannel66", "LevelChannel67", "LevelChannel68", "LevelChannel69", "LevelChannel70", "LevelChannel71", "LevelChannel72", "LevelChannel73", "LevelChannel74", "LevelChannel75", "LevelChannel76", "LevelChannel77", "LevelChannel78", "LevelChannel79", "LevelChannel80", "LevelChannel81", "LevelChannel82", "LevelChannel83", "LevelChannel84", "LevelChannel85", "LevelChannel86", "LevelChannel87", "LevelChannel88", "LevelChannel89", "LevelChannel90", "LevelChannel91", "LevelChannel92", "LevelChannel93", "LevelChannel94", "LevelChannel95", "LevelChannel96", "LevelChannel97", "LevelChannel98", "LevelChannel99", "LevelChannel100", "LevelChannel101", "LevelChannel102", "LevelChannel103", "LevelChannel104", "LevelChannel105", "LevelChannel106", "LevelChannel107", "LevelChannel108", "LevelChannel109", "LevelChannel110", "LevelChannel111", "LevelChannel112", "LevelChannel113", "LevelChannel114", "LevelChannel115", "LevelChannel116", "LevelChannel117", "LevelChannel118", "LevelChannel119", "LevelChannel120", "LevelChannel121", "LevelChannel122", "LevelChannel123", "LevelChannel124", "LevelChannel125", "LevelChannel126", "LevelChannel127", "LevelChannel128", "LevelChannel129", "LevelChannel130", "LevelChannel131", "LevelChannel132", "LevelChannel133", "LevelChannel134", "LevelChannel135", "LevelChannel136", "LevelChannel137", "LevelChannel138", "LevelChannel139", "LevelChannel140", "LevelChannel141", "LevelChannel142", "LevelChannel143", "LevelChannel144", "LevelChannel145", "LevelChannel146", "LevelChannel147", "LevelChannel148", "LevelChannel149", "LevelChannel150", "LevelChannel151", "LevelChannel152", "LevelChannel153", "LevelChannel154", "LevelChannel155", "LevelChannel156", "LevelChannel157", "LevelChannel158", "LevelChannel159", "LevelChannel160", "LevelChannel161", "LevelChannel162", "LevelChannel163", "LevelChannel164", "LevelChannel165", "LevelChannel166", "LevelChannel167", "LevelChannel168", "LevelChannel169", "LevelChannel170", "LevelChannel171", "LevelChannel172", "LevelChannel173", "LevelChannel174", "LevelChannel175", "LevelChannel176", "LevelChannel177", "LevelChannel178", "LevelChannel179", "LevelChannel180", "LevelChannel181", "LevelChannel182", "LevelChannel183", "LevelChannel184", "LevelChannel185", "LevelChannel186", "LevelChannel187", "LevelChannel188", "LevelChannel189", "LevelChannel190", "LevelChannel191", "LevelChannel192", "LevelChannel193", "LevelChannel194", "LevelChannel195", "LevelChannel196", "LevelChannel197", "LevelChannel198", "LevelChannel199", "LevelChannel200", "LevelChannel201", "LevelChannel202", "LevelChannel203", "LevelChannel204", "LevelChannel205", "LevelChannel206", "LevelChannel207", "LevelChannel208", "LevelChannel209", "LevelChannel210", "LevelChannel211", "LevelChannel212", "LevelChannel213", "LevelChannel214", "LevelChannel215", "LevelChannel216", "LevelChannel217", "LevelChannel218", "LevelChannel219", "LevelChannel220", "LevelChannel221", "LevelChannel222", "LevelChannel223", "LevelChannel224", "LevelChannel225", "LevelChannel226", "LevelChannel227", "LevelChannel228", "LevelChannel229", "LevelChannel230", "LevelChannel231", "LevelChannel232", "LevelChannel233", "LevelChannel234", "LevelChannel235", "LevelChannel236", "LevelChannel237", "LevelChannel238", "LevelChannel239", "LevelChannel240", "LevelChannel241", "LevelChannel242", "LevelChannel243", "LevelChannel244", "LevelChannel245", "LevelChannel246", "LevelChannel247", "LevelChannel248", "LevelChannel249", "LevelChannel250", "LevelChannel251", "LevelChannel252", "LevelChannel253", "LevelChannel254", "LevelChannel255", "LevelChannel256", "LevelChannel257", "LevelChannel258", "LevelChannel259", "LevelChannel260", "LevelChannel261", "LevelChannel262", "LevelChannel263", "LevelChannel264", "LevelChannel265", "LevelChannel266", "LevelChannel267", "LevelChannel268", "LevelChannel269", "LevelChannel270", "LevelChannel271", "LevelChannel272", "LevelChannel273", "LevelChannel274", "LevelChannel275", "LevelChannel276", "LevelChannel277", "LevelChannel278", "LevelChannel279", "LevelChannel280", "LevelChannel281", "LevelChannel282", "LevelChannel283", "LevelChannel284", "LevelChannel285", "LevelChannel286", "LevelChannel287", "LevelChannel288", "LevelChannel289", "LevelChannel290", "LevelChannel291", "LevelChannel292", "LevelChannel293", "LevelChannel294", "LevelChannel295", "LevelChannel296", "LevelChannel297", "LevelChannel298", "LevelChannel299", "LevelChannel300", "LevelChannel301", "LevelChannel302", "LevelChannel303", "LevelChannel304", "LevelChannel305", "LevelChannel306", "LevelChannel307", "LevelChannel308", "LevelChannel309", "LevelChannel310", "LevelChannel311", "LevelChannel312", "LevelChannel313", "LevelChannel314", "LevelChannel315", "LevelChannel316", "LevelChannel317", "LevelChannel318", "LevelChannel319", "LevelChannel320", "LevelChannel321", "LevelChannel322", "LevelChannel323", "LevelChannel324", "LevelChannel325", "LevelChannel326", "LevelChannel327", "LevelChannel328", "LevelChannel329", "LevelChannel330", "LevelChannel331", "LevelChannel332", "LevelChannel333", "LevelChannel334", "LevelChannel335", "LevelChannel336", "LevelChannel337", "LevelChannel338", "LevelChannel339", "LevelChannel340", "LevelChannel341", "LevelChannel342", "LevelChannel343", "LevelChannel344", "LevelChannel345", "LevelChannel346", "LevelChannel347", "LevelChannel348", "LevelChannel349", "LevelChannel350", "LevelChannel351", "LevelChannel352", "LevelChannel353", "LevelChannel354", "LevelChannel355", "LevelChannel356", "LevelChannel357", "LevelChannel358", "LevelChannel359", "LevelChannel360", "LevelChannel361", "LevelChannel362", "LevelChannel363", "LevelChannel364", "LevelChannel365", "LevelChannel366", "LevelChannel367", "LevelChannel368", "LevelChannel369", "LevelChannel370", "LevelChannel371", "LevelChannel372", "LevelChannel373", "LevelChannel374", "LevelChannel375", "LevelChannel376", "LevelChannel377", "LevelChannel378", "LevelChannel379", "LevelChannel380", "LevelChannel381", "LevelChannel382", "LevelChannel383", "LevelChannel384", "LevelChannel385", "LevelChannel386", "LevelChannel387", "LevelChannel388", "LevelChannel389", "LevelChannel390", "LevelChannel391", "LevelChannel392", "LevelChannel393", "LevelChannel394", "LevelChannel395", "LevelChannel396", "LevelChannel397", "LevelChannel398", "LevelChannel399", "LevelChannel400", "LevelChannel401", "LevelChannel402", "LevelChannel403", "LevelChannel404", "LevelChannel405", "LevelChannel406", "LevelChannel407", "LevelChannel408", "LevelChannel409", "LevelChannel410", "LevelChannel411", "LevelChannel412", "LevelChannel413", "LevelChannel414", "LevelChannel415", "LevelChannel416", "LevelChannel417", "LevelChannel418", "LevelChannel419", "LevelChannel420", "LevelChannel421", "LevelChannel422", "LevelChannel423", "LevelChannel424", "LevelChannel425", "LevelChannel426", "LevelChannel427", "LevelChannel428", "LevelChannel429", "LevelChannel430", "LevelChannel431", "LevelChannel432", "LevelChannel433", "LevelChannel434", "LevelChannel435", "LevelChannel436", "LevelChannel437", "LevelChannel438", "LevelChannel439", "LevelChannel440", "LevelChannel441", "LevelChannel442", "LevelChannel443", "LevelChannel444", "LevelChannel445", "LevelChannel446", "LevelChannel447", "LevelChannel448", "LevelChannel449", "LevelChannel450", "LevelChannel451", "LevelChannel452", "LevelChannel453", "LevelChannel454", "LevelChannel455", "LevelChannel456", "LevelChannel457", "LevelChannel458", "LevelChannel459", "LevelChannel460", "LevelChannel461", "LevelChannel462", "LevelChannel463", "LevelChannel464", "LevelChannel465", "LevelChannel466", "LevelChannel467", "LevelChannel468", "LevelChannel469", "LevelChannel470", "LevelChannel471", "LevelChannel472", "LevelChannel473", "LevelChannel474", "LevelChannel475", "LevelChannel476", "LevelChannel477", "LevelChannel478", "LevelChannel479", "LevelChannel480", "LevelChannel481", "LevelChannel482", "LevelChannel483", "LevelChannel484", "LevelChannel485", "LevelChannel486", "LevelChannel487", "LevelChannel488", "LevelChannel489", "LevelChannel490", "LevelChannel491", "LevelChannel492", "LevelChannel493", "LevelChannel494", "LevelChannel495", "LevelChannel496", "LevelChannel497", "LevelChannel498", "LevelChannel499", "LevelChannel500", "LevelChannel501", "LevelChannel502", "LevelChannel503", "LevelChannel504", "LevelChannel505", "LevelChannel506", "LevelChannel507", "LevelChannel508", "LevelChannel509", "LevelChannel510", "LevelChannel511", "LevelChannel512"}; // Matrix die Namer der Level-Labels enthält
const char *ssid = "DMX_Tester";              // Name des Netzwerks
const char *password = "dmx_tester";         // Passwort des Netzwerks

WiFiServer server(80);                      // Server Instanz -> ESP als Access Point
WiFiUDP Udp;                                // UDP Instanz um Pakete über UDP zu senden / empfangen
                                            
const IPAddress outIp(192,168,4,2);         // IP des Clients
const unsigned int outPort = 9999;          // Incoming-Port des Clients
const unsigned int localPort = 8888;        // Incoming-Ports des Hosts für ankommende Nachrichten

OSCErrorCode error;

//******************************REC**************************************

int sampleCounter = 0;
int recordMatrix[][512]=
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


void receiveCallback_recordMode(int slots){ // Timer-Interrupt mit Samplingfrequenz, Matrix als Abweichung von Null-Matrix
  if ( slots ) {                            // 
    xSemaphoreTake( ESP32DMX.lxDataLock, portMAX_DELAY );
    for(int i = 0; i<512 ; i++){
      recordMatrix[sampleCounter][i] = ESP32DMX.getSlot(i+1)-recordMatrix[sampleCounter-1][i];
    }
    //recordMatrix[] = ESP32DMX.getSlot(testChannel);
  }
  sampleCounter++;
  xSemaphoreGive( ESP32DMX.lxDataLock );
}



/************************************************************************
	setup Funktion
*************************************************************************/

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
}

// Funktion: DMX in Output-Buffer geben
void copyDMXToOutput(void) {
  xSemaphoreTake( ESP32DMX.lxDataLock, portMAX_DELAY );
	for (int i=1; i<DMX_MAX_FRAME; i++) {
    	ESP32DMX.setSlot(i , dmxbuffer[i]);
   }
   xSemaphoreGive( ESP32DMX.lxDataLock );
}

void sendMode();
void receiveMode();
void setModeSend(OSCMessage &msg);
void setModeReceive(OSCMessage &msg);
void channelDekrement(OSCMessage &msg);
void channelInkrement(OSCMessage &msg);
void levelDekrement(OSCMessage &msg);
void levelInkrement(OSCMessage &msg);

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
      msg.dispatch("/ReceivePage/Channel-", channelDekrement);
      msg.dispatch("/ReceivePage/Channel+", channelInkrement);
      msg.dispatch("/SendPage/Channel-", channelDekrement);
      msg.dispatch("/SendPage/Channel+", channelInkrement);
      msg.dispatch("/SendPage/Level-", levelDekrement);
      msg.dispatch("/SendPage/Level+", levelInkrement);
      
      
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

// Funktion die aufgerufen wenn OSC-Message zum Wechsel in Send-Mode kommt
void setModeSend(OSCMessage &msg){
  bool sendState = msg.getFloat(0); // 1 für Send, 0 für Receive
  if(sendState){
    sendMode();
  }
}

// Funktion die aufgerufen wenn OSC-Message zum Wechsel in Receive-Mode kommt
void setModeReceive(OSCMessage &msg){
  bool receiveState = msg.getFloat(0); // 1 für Send, 0 für Receive
  if(receiveState){
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
  // Setup Send-spezifisch
    Serial.print("Setup DMX");
  
  pinMode(DMX_DIRECTION_PIN, OUTPUT);
  digitalWrite(DMX_DIRECTION_PIN, HIGH);

  pinMode(DMX_SERIAL_OUTPUT_PIN, OUTPUT);
  ESP32DMX.startOutput(DMX_SERIAL_OUTPUT_PIN);
  Serial.println("Setup DMX complete");
  Serial.println("***sendMode activated***");
  while(true){
    getMsg();
    copyDMXToOutput();
    esp_task_wdt_feed(); // DMX-Library spezifisch, damit WDT nicht überläuft?
    vTaskDelay(1);       // 1 Tick Delay
  }
}

// Receive-Modus-Schleife

void receiveMode(){
  // Setup Receive-spezifisch
  ESP32DMX.setDirectionPin(DMX_DIRECTION_PIN);

  Serial.print(", set callback");
  ESP32DMX.setDataReceivedCallback(receiveCallback);

  Serial.print(", start dmx input");
  ESP32DMX.startInput(DMX_SERIAL_INPUT_PIN);
  
  Serial.println(", setup complete.");
  Serial.println("***receiveMode activated***");
  while(true){
    getMsg();
    if ( dataChanged ) {
      dataChanged = 0;
      sendValue(testLevel, "/ReceivePage/Level");
      Serial.print(testChannel);
      Serial.print(" => ");
      Serial.println(testLevel);
  } else {
      esp_task_wdt_feed();
      vTaskDelay(100);  // vTaskDelay is called to prevent wdt timeout
  }
  }
}

void loop(){
sendMode(); // State nach Reset: Send-Modus
}

