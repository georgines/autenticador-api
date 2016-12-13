#include <Arduino.h>

//#include <SPI.h>
#include "RFID.h"
//#include <Time.h>
//#include <Wire.h>
#include <SD.h>
#include <Ethernet.h>
#include "RestClient.h"
//#include "ClientServer.h"
//#include "WEBServer.h"
#include <Keypad.h>
#include <openGLCD.h>
#include <bitmaps/catraca.h>
#include <bitmaps/tela_principal1.h>
#include <bitmaps/tela_principal2.h>
#include <bitmaps/a_frame1.h>
#include <bitmaps/a_frame2.h>
#include <bitmaps/a_frame3.h>
#include <bitmaps/a_frame4.h>
#include <bitmaps/a_frame5.h>
#include <bitmaps/a_frame6.h>
#include <bitmaps/a_frame7.h>
#include <bitmaps/a_frame8.h>
#include <bitmaps/sd_noticfication.h>
#include <ArduinoJson.h>
#include "Config.h"

#ifndef APP_H_
#define APP_H_

class App {
private:

	File file;
	RFID rfid;
	String token;
	RestClient client = RestClient(HOST, PORT);
	Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);
	char keys[4][3] = { { '1', '2', '3' }, { '4', '5', '6' }, { '7', '8', '9' },
			{ '*', '0', '#' } };
	byte rowPins[4] = { 42, 40, 38, 36 };
	byte colPins[3] = { 48, 46, 44 };
	char key;
	char selection;
	char startApp;
	String registration = "";
	String response;
	int statusCodeClient;
	void setDigits(int digit);
	void processSyncMessage();

public:

	App();
	virtual ~App();

	/*
	 initialize an object
	 */
	void init();

	/*
	 run app
	 */
	void runApp();
	char getStartApp();
	void setStartApp(char startApp);

	/*
	 methodos device
	 */
	void soundKeyPad();
	void soundUnlocked();
	void soundBlocked();
	void soundError();

	/*
	 methodos date time
	 */
	void setTimeSerial();
	void printHour();
	void printMinute();
	void printSecond();
	void printDay();
	void printMonth();
	void printYear();

	/*
	 methodos lcd graphic
	 */
	void draw(const uint8_t* file);
	void printLabel(String text, const char* lable, int height);
	void printCenter(String text);
	void print(const char* text);
	void clear();

	/*
	 methodos animations
	 */
	void drawPassCard();
	void animationWaitingCards(int interval);
	void animationAmpulheta();

	/*
	 methodos ethernet
	 */
	String sendRestClient(const char* route, const char* tag);
	String sendRestClient(const char* route);
	void destructRestClient();
	void serverOnline(String page);
	void codeStatusDebug();
	String clearAnswer(String response);

	/*
	 methodos sd
	 */
	String getFile(String filename);

	/*
	 methodos rfid
	 */
	String readCard();
	String readRegistration();
	String getAutorizationResgister(const char* route, String reg,
			String param);
	/*
	 methodos buzzer
	 */

	/*
	 methodos keyboar
	 */
	char getKey();
	void setKey(char key);
	void waitingKeyboard();

	/*
	 methodos main menu
	 */
	void showSelectMenu(char key);
	void setMenu(char selection);

	/*
	 methodos submenu
	 */
	void setLeitorMode();
	void setReservaMode();
	void setConfigMode();

	/*
	 methodos json
	 */
	bool parseAutentication(String json);

	/*
	 methodos tiket gate
	 */
	void initTiketGate();
	void openTiketGate();
	void closeTiketGate();
	void waitingTiketGate();
	/*
	 metodos Oauth
	 */
	String getToken();

};

#endif
