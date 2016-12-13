#include "App.h"

#define DEBUG

#ifdef DEBUG
#define APP_DEBUG_PRINT(string) (Serial.print(string))
#define APP_DEBUG_PRINTLN(string) (Serial.println(string))
#endif
#ifndef DEBUG
#define APP_DEBUG_PRINT(string)
#define APP_DEBUG_PRINTLN(string)
#endif

App::App() {
	this->rfid = RFID(6, 7);

}

App::~App() {

}
void App::init() {

	Serial.begin(9600);
	SPI.begin();
	GLCD.Init();
	GLCD.SelectFont(System5x7);
	this->setStartApp('1');
	if (SD.begin(SD_CHIP_SELECT)) {
		APP_DEBUG_PRINTLN("SD: OK");

	} else {
		APP_DEBUG_PRINTLN("SD: not found");
	}

	if (this->rfid.PCD_Init()) {
		APP_DEBUG_PRINTLN("RFID: OK");
	} else {
		APP_DEBUG_PRINTLN("RFID: fail");
	}

	if (this->client.dhcp()) {
		APP_DEBUG_PRINTLN("DHCP: OK");
	} else {
		APP_DEBUG_PRINTLN("DHCP: fail");
	}
	this->initTiketGate();
	this->token = this->getToken();

}

char App::getStartApp() {
	return this->startApp;
}
void App::setStartApp(char startApp) {
	this->startApp = startApp;
}

void App::runApp() {
	this->waitingKeyboard();
	APP_DEBUG_PRINTLN(this->getKey());
	this->showSelectMenu(this->getKey());
}

/*
 -------------------------------------------------------------
 METODS KEYPAD
 -------------------------------------------------------------
 */

char App::getKey() {
	return this->key;
}

void App::setKey(char key) {
	this->key = key;
}

void App::waitingKeyboard() {
	if (this->getStartApp() == '0') {
		do {
			this->setKey(kpd.getKey());
		} while (!this->getKey());
		this->soundKeyPad();
	}
	if (this->getStartApp() == '1') {
		this->key = '0';
		this->setStartApp('0');
	}
}

/*
 -------------------------------------------------------------
 METODS MAIN MENU
 -------------------------------------------------------------
 */

void App::showSelectMenu(char key) {
	if (key) {
		this->draw(menu);
	}
	switch (key) {
	case '1':
		this->selection = key;
		this->setKey(' ');
		this->clear();
		this->draw(menu_leitor);
		break;
	case '2':
		this->selection = key;
		this->setKey(' ');
		this->clear();
		this->draw(menu_cadastro);
		break;
	case '3':
		this->selection = key;
		this->setKey(' ');
		this->clear();
		this->draw(menu_config);
		break;
	case '*':
		this->setKey(' ');
		this->selection = '0';
		this->clear();
		this->draw(menu);
		break;
	case '#':
		this->setMenu(this->selection);
		this->setKey(' ');
		this->clear();
		this->draw(menu);
		break;
	default:
		this->setKey(' ');
		this->selection = '0';
		this->clear();
		this->draw(menu);
		break;
	}
}

/*
 -------------------------------------------------------------
 METODS SUBMENU
 -------------------------------------------------------------
 */
void App::setMenu(char selection) {
	this->clear();
	while (true) {
		switch (selection) {
		case '1':
			this->setKey(' ');
			this->setLeitorMode();
			break;
		case '2':
			this->setKey(' ');
			this->setReservaMode();
			break;
		case '3':
			this->setKey(' ');
			this->setConfigMode();
			break;
		default:
			goto exteMenu;
			break;
		}
	}
	exteMenu: return;
}

/*
 -------------------------------------------------------------
 METODS SUBMENU LEITOR
 -------------------------------------------------------------
 */
void App::setLeitorMode() {
	APP_DEBUG_PRINTLN("Mod: read");
	while (true) {
		String response = this->getAutorizationResgister(
		ROUTE_AUTENTICATION, this->readCard(), PARAM_ROUTE_AUTENTICATION);
		this->parseAutentication(response);

	}
}

/*
 -------------------------------------------------------------
 METODS SUBMENU RESEVA
 -------------------------------------------------------------
 */
void App::setReservaMode() {
	APP_DEBUG_PRINTLN("Mod: reserve");
	while (true) {
		this->waitingKeyboard();
		char key = this->getKey();
		this->showSelectMenu(key);
	}
}

/*
 -------------------------------------------------------------
 METODS SUBMENU CONFIG
 -------------------------------------------------------------
 */
void App::setConfigMode() {
	APP_DEBUG_PRINTLN("Mod: config");
	while (true) {
		this->waitingKeyboard();
		char key = this->getKey();
		this->showSelectMenu(key);
	}
}

/*
 ------------------------------------------------------------------
 GET TAG RFID
 ------------------------------------------------------------------
 */

String App::readCard() {
	String conteudo = CLEAR_STRING;
	while (!this->rfid.PICC_IsNewCardPresent()
			|| !this->rfid.PICC_ReadCardSerial()) {
		passCard:

		this->drawPassCard();
		switch (kpd.getKey()) {
		case '#':
			this->soundKeyPad();
			this->clear();
			this->readRegistration();
			goto passCard;
			break;
		case '*':
			APP_DEBUG_PRINTLN(key);
			this->soundKeyPad();
			this->showSelectMenu(key);
			while (true) {
				this->waitingKeyboard();
				this->showSelectMenu(this->getKey());
			}
			break;
		}
	}

	for (byte i = 0; i < this->rfid.uid.size; i++) {
		conteudo.concat(String(this->rfid.uid.uidByte[i] < 0x10 ? " 0" : "-"));
		conteudo.concat(String(this->rfid.uid.uidByte[i], HEX));
	}
	conteudo.toUpperCase();
	return conteudo.substring(1);

}

String App::readRegistration() {
	this->printLabel(CLEAR_STRING, LABEL_ENTER_DATA,
	VERTICAL_LABEL_POSITION);
	while (true) {
		this->waitingKeyboard();
		char key = this->getKey();
		if (key) {
			switch (key) {
			case '#':
				if (this->registration.length() >= MINIMUM_SIZE_REGISTRATION
						&& this->registration.length()
								<= MAXIMO_SIZE_REGISTRATION) {

					this->parseAutentication(
							this->getAutorizationResgister(ROUTE_REGISTRATION,
									registration,
									PARAM_ROUTE_REGISTRATION));

					goto exit;

				} else {
					this->clear();
					this->printLabel(CLEAR_STRING, LABEL_DATA_INVALID,
					VERTICAL_LABEL_POSITION);
				}
				break;
			case '*':
				exit: this->registration = CLEAR_STRING;
				goto passCard;
				break;
			default:
				if (this->registration.length() < MAXIMO_SIZE_REGISTRATION) {
					registration.concat(key);
					APP_DEBUG_PRINTLN(this->registration);
					this->clear();
					this->printLabel(this->registration,
					LABEL_ENTER_DATA, VERTICAL_LABEL_POSITION);
				}

			}
		}

	}
	passCard: return "";
}

String App::getAutorizationResgister(const char* route, String reg,
		String param) {
	char url[BUFFER_URL] = CLEAR_STRING;
	String response;
	String conteudo = CLEAR_STRING;
	conteudo.concat(ROUTE_AUTENTICATION);
	conteudo.concat("?");
	conteudo.concat(param);
	conteudo.concat(reg);
	conteudo.trim();
	conteudo.toCharArray(url, BUFFER_URL);
	APP_DEBUG_PRINTLN(reg);
	APP_DEBUG_PRINTLN(url);
	APP_DEBUG_PRINTLN(response);
	unsigned int length = this->token.length();
	char buf[length + 1];
	this->token.toCharArray(buf, length + 1);
	APP_DEBUG_PRINTLN(buf);
	this->client.setHeader(buf);
	this->client.get(url, &response);
	response.trim();
	response = this->clearAnswer(response);
	return response;
}

/*
 ------------------------------------------------------------------
 RFID METHODOS
 ------------------------------------------------------------------
 */

String App::sendRestClient(const char* route, const char* tag) {
	String response;
	this->statusCodeClient = this->client.post(route, tag, &response);
	this->codeStatusDebug();
	return response;
}

String App::clearAnswer(String response) {
	int start = response.indexOf('{');
	int end = response.indexOf('}');
	if (start >= 0 && end >= 0) {
		return response.substring(start, end + 1);
	} else {
		return response;
	}

}

/*
 ------------------------------------------------------------------
 DESTROY CLIENT/SERVER
 ------------------------------------------------------------------
 */

void App::destructRestClient() {
//	this->client.~ClientServer();
}

/*
 ------------------------------------------------------------------
 PUTTING SERVER ONLINE
 ------------------------------------------------------------------
 */

void App::serverOnline(String page) {
//	this->client.serverOnline(getFile(page));
}

/*
 ------------------------------------------------------------------
 DEBUG HTTP METHODO
 ------------------------------------------------------------------
 */
void App::codeStatusDebug() {
	switch (this->statusCodeClient) {
	case 200:
		APP_DEBUG_PRINTLN("HTTP: Ok");
		break;
	case 404:
		APP_DEBUG_PRINTLN("HTTP: Page not found");
		break;
	case 500:
		APP_DEBUG_PRINTLN("HTTP: Internal server error");
		break;
	default:
		APP_DEBUG_PRINTLN("HTTP: Unknown error");
		break;
	}
}

/*
 ------------------------------------------------------------------
 GET FILE DATA
 ------------------------------------------------------------------
 */
String App::getFile(String filename) {
	String contentFile = CLEAR_STRING;
	char dataRead;
	this->file = SD.open(filename);
	if (this->file) {
		APP_DEBUG_PRINTLN("SD: file OK");
		while (this->file.available()) {

			dataRead = (char) this->file.read();
			contentFile.concat(dataRead);
			APP_DEBUG_PRINT(dataRead);

		}
		file.close();
	} else {
		APP_DEBUG_PRINTLN("SD: file not found");
	}
	APP_DEBUG_PRINT(contentFile);
	return contentFile;
}

/*
 ------------------------------------------------------------------
 SOUNDS
 ------------------------------------------------------------------
 */
void App::soundUnlocked() {
	noTone(BUZZER_OUTPUT);
	tone(BUZZER_OUTPUT, 3000, 100);
}

void App::soundKeyPad() {
	noTone(BUZZER_OUTPUT);
	tone(BUZZER_OUTPUT, 5000, 50);
}
void App::soundBlocked() {
	noTone(BUZZER_OUTPUT);
	tone(BUZZER_OUTPUT, 900, 100);
}

void App::soundError() {
	noTone(BUZZER_OUTPUT);
	tone(BUZZER_OUTPUT, 400, 50);
	delay(150);
	tone(BUZZER_OUTPUT, 400, 50);
}

/*
 ------------------------------------------------------------------
 ANIMATIONS
 ------------------------------------------------------------------
 */

void App::animationWaitingCards(int interval) {
	this->draw(tela_principal1);
	delay(interval);
	this->draw(tela_principal2);
	delay(interval);
}

void App::animationAmpulheta() {
	this->draw(a_frame1);
	delay(TIME_AMPULHETA);
	this->draw(a_frame2);
	delay(TIME_AMPULHETA);
	this->draw(a_frame3);
	delay(TIME_AMPULHETA);
	this->draw(a_frame4);
	delay(TIME_AMPULHETA);
	this->draw(a_frame5);
	delay(TIME_AMPULHETA);
	this->draw(a_frame6);
	delay(TIME_AMPULHETA);
	this->draw(a_frame7);
	delay(TIME_AMPULHETA);
	this->draw(a_frame8);
	delay(TIME_AMPULHETA);
	this->draw(a_frame1);
	delay(TIME_AMPULHETA);
	this->draw(a_frame2);
	delay(TIME_AMPULHETA);
	this->draw(a_frame3);
	delay(TIME_AMPULHETA);
	this->draw(a_frame4);
	delay(TIME_AMPULHETA);
	this->draw(a_frame5);
	delay(TIME_AMPULHETA);
	this->draw(a_frame6);
	delay(TIME_AMPULHETA);
	this->draw(a_frame7);
	delay(TIME_AMPULHETA);
	this->draw(a_frame8);
	delay(TIME_AMPULHETA);
}
void App::drawPassCard() {
	this->draw(tela_principal1);
}

/*
 ------------------------------------------------------------------
 SCREEN
 ------------------------------------------------------------------
 */
void App::draw(const uint8_t* file) {
	GLCD.DrawBitmap(file, GLCD.Width / 2 - 64, 0);
}
void App::clear() {
	GLCD.ClearScreen();
}

void App::printCenter(String text) {
	APP_DEBUG_PRINTLN(text);
	char buffer[BUFFER_SIZE_LABEL] = CLEAR_STRING;
	text.toCharArray(buffer, BUFFER_SIZE_LABEL);
	GLCD.DrawString(buffer, gTextfmt_center, gTextfmt_center);
}

void App::printLabel(String text, const char* lable, int height) {
	APP_DEBUG_PRINTLN(text);
	char buffer[BUFFER_SIZE_LABEL] = CLEAR_STRING;
	text.toCharArray(buffer, BUFFER_SIZE_LABEL);
	GLCD.DrawString(lable, gTextfmt_center, height);
	GLCD.DrawString(buffer, gTextfmt_center, gTextfmt_center);
}

void App::print(const char* text) {
	APP_DEBUG_PRINTLN(text);
	GLCD.print(text);
}
/*
 ------------------------------------------------------------------
 JSON
 ------------------------------------------------------------------
 */
bool App::parseAutentication(String json) {
	DynamicJsonBuffer jsonBuffer;
	APP_DEBUG_PRINTLN(json);
	json.replace("[", "");
	json.replace("]", "");
	json.replace("86", "");
	json.replace("\0", "");
	json.trim();
	APP_DEBUG_PRINTLN(json);
	JsonObject& root = jsonBuffer.parseObject(json);
	if (!root.success()) {

		APP_DEBUG_PRINTLN("JSON: fail");
		this->soundError();
		this->clear();
		this->printLabel(CLEAR_STRING, LABEL_NOT_AUTHORIZED_MESSAGE,
		VERTICAL_LABEL_POSITION);
		delay(TIME_SHOW_NOT_AUTHORIZED_MESSAGE);
		return false;
	}
	APP_DEBUG_PRINTLN("JSON: OK");

	String name = root[PARAM_PARSE_NAME];
	String auth = root[PARAM_PARSE_AUTH];

	APP_DEBUG_PRINTLN(name);
	APP_DEBUG_PRINTLN(auth);
	if (auth == "true") {
		this->clear();
		this->printLabel(name, root["message"], VERTICAL_LABEL_POSITION);
		this->openTiketGate();
		this->soundUnlocked();
		this->waitingTiketGate();
		this->closeTiketGate();
		return true;
	} else {
		this->soundError();
		this->clear();
		this->printLabel(CLEAR_STRING, root["message"],
		VERTICAL_LABEL_POSITION);
		delay(TIME_SHOW_NOT_AUTHORIZED_MESSAGE);
	}
	return false;
}

/*
 ------------------------------------------------------------------
 TICKET GATE
 ------------------------------------------------------------------
 */
void App::initTiketGate() {
	APP_DEBUG_PRINTLN("TIKET_GATE: init");
	pinMode(TICKET_GATE_INPUT, INPUT);
	pinMode(TICKET_GATE_OUTPUT, OUTPUT);
	digitalWrite(TICKET_GATE_INPUT, HIGH);
	digitalWrite(TICKET_GATE_OUTPUT, LOW);
}

void App::openTiketGate() {
	APP_DEBUG_PRINTLN("TIKET_GATE: open");
	digitalWrite(TICKET_GATE_OUTPUT, HIGH);
}

void App::closeTiketGate() {
	APP_DEBUG_PRINTLN("TIKET_GATE: close");
	digitalWrite(TICKET_GATE_OUTPUT, LOW);
}

void App::waitingTiketGate() {
	while (!digitalRead(TICKET_GATE_INPUT) == 0) {

	}
}

/*
 ------------------------------------------------------------------
 OAUTH
 ------------------------------------------------------------------
 */
String App::getToken() {
	String response_token;
	this->client.post(OAUTH_ROUTE, OAUTH_PARAMS, &response_token);
	response_token = this->clearAnswer(response_token);
	APP_DEBUG_PRINTLN(response_token);
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(response_token);
	APP_DEBUG_PRINT("TOKEN: ");
	if (root.success()) {
		char* access_token;
		char* token_type;
		String token = "";
		access_token = root["access_token"];
		token_type = root["token_type"];
		token += "Authorization: ";
		token += String(token_type);
		token += " ";
		token += String(access_token);
		token.trim();
		APP_DEBUG_PRINTLN(token);
		return token;
	}
	return "";
}
