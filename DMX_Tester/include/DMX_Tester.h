#ifndef DMXTESTER_H
#define DMXTESTER_H

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

void sendMode();
	/*!
	 * @brief SendMode loop
	 * @discussion 
	*/
void receiveMode();
	/*!
	 * @brief ReceiveMode loop
	 * @discussion 
	 * 
	*/
void setModeSend(OSCMessage &msg);
	/*!
	 * @brief Exits ReceiveMode and enters SendMode loop
	 * @discussion called by OSC
	 * @param msg  the msg handed by TouchOSC
	*/
void setModeReceive(OSCMessage &msg);
	/*!
	 * @brief Exits SendMode and enters ReceiveMode loop
	 * @discussion called by OSC
	 * @param msg  the msg handed by TouchOSC
	*/
void channelDekrement(OSCMessage &msg);
	/*!
	 * @brief function to decrement the active test channel
	 * @discussion called by OSC "Channel -"
	 * @param msg  the msg handed by TouchOSC
	*/
void channelInkrement(OSCMessage &msg);
	/*!
	 * @brief function to increment the active test channel
	 * @discussion called by OSC "Channel +"
	 * @param msg  the msg handed by TouchOSC
	*/
void levelDekrement(OSCMessage &msg);
	/*!
	 * @brief function to decrement the test level in SendMode
	 * @discussion called by OSC "Level -"
	 * @param msg  the msg handed by TouchOSC
	*/
void levelInkrement(OSCMessage &msg);
	/*!
	 * @brief function to increment the test level in SendMode
	 * @discussion called by OSC "Level +"
	 * @param msg  the msg handed by TouchOSC
	*/
void sendMsg(char text[], const char adress[]);
	/*!
	 * @brief function to send text to a label in TouchOSC
	 * @discussion 
	 * @param text the text to send
     * @param adress the adress in TouchOSC the text is to be sent to
	*/
void sendValue(int value, const char adress[]);
	/*!
	 * @brief function to send a value to a label or fader in TouchOSC
	 * @discussion 
	 * @param value the value to send
     * @param adress the adress in TouchOSC the value is to be sent to
	*/
void setfSample(OSCMessage &msg);
	/*!
	 * @brief function to set the sampling interval for recording incoming DMX
     * @discussion called when fader "fSample" is changed
	 * @param msg the msg containing the value set in TouchOSC-fader "fSample"
    */
void setChannelRecord(OSCMessage &msg);
/*!
	* @brief function to set the sampling interval for recording incoming DMX
	* @discussion called when fader "fSample" is changed
	* @param msg the msg containing the value set in TouchOSC-fader "fSample"
*/
void setTimeRecord(OSCMessage &msg);
	/*!
	 * @brief function to set the sampling interval for recording incoming DMX
     * @discussion called when fader "fSample" is changed
	 * @param msg the msg containing the value set in TouchOSC-fader "fSample"
    */
void refreshRecordDisplay();
	/*!
	 * @brief function to record incoming DMX data 
     * @discussion is a task function, task created by calling start_recordDMX function
	 * @param parameter parameter could be handled when creating task - NOT USED
    */
void recordDMX( void * parameter);
	/*!
	 * @brief function to record incoming DMX data 
     * @discussion is a task function, task created by calling start_recordDMX function
	 * @param parameter parameter could be handled when creating task - NOT USED
    */
void playRecorded(OSCMessage &msg);
	 /*!
     * @brief function to playback recorded DMX data, must called in sendMode 
     * @discussion called by TouchOSC button "play" on SendPage
	 * @param msg the msg handed by OSC
    */
void start_recordDMX(OSCMessage &msg);
	/*!
	 * @brief function to start the DMX recording task or if task was already started, set the record flag
     * @discussion called by TouchOSC button "record" on ReceivePage
	 * @param msg the msg handed by OSC
    */

#endif // ifndef DMXTESTER_H