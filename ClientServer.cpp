#include "ClientServer.h"
//#define HTTP_DEBUG

#ifdef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string) (Serial.print(string))
#endif

#ifndef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)
#endif

ClientServer::ClientServer(const char* _host, int _port) {

	host = _host;
	port = _port;
	num_headers = 0;
	contentType = "application/x-www-form-urlencoded";

}

ClientServer::~ClientServer() {

}

bool ClientServer::dhcp() {
	byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
	if (begin(mac) == 0) {
		return false;HTTP_DEBUG_PRINT("Failed to configure Ethernet using DHCP");
	} else {
		server.begin();
		HTTP_DEBUG_PRINT(Ethernet.localIP());
		return true;
	}
	delay(250);
}

int ClientServer::begin(byte mac[]) {
	return Ethernet.begin(mac);
	delay(250);
}

int ClientServer::get(const char* path, String* response) {
	return request("GET", path, NULL, response);
}

int ClientServer::get(const char* path, const char* body, String* response) {
	return request("GET", path, body, response);
}

int ClientServer::post(const char* path, const char* body, String* response) {
	return request("POST", path, body, response);
}

int ClientServer::request(const char* method, const char* path,
		const char* body, String* response) {

	HTTP_DEBUG_PRINT("HTTP: connect\n");

	if (client.connect(host, port)) {

		HTTP_DEBUG_PRINT("HTTP: connected\n");HTTP_DEBUG_PRINT("REQUEST: \n");

		write(method);
		write(" ");
		write(path);
		write(" HTTP/1.1\r\n");

		for (int i = 0; i < num_headers; i++) {
			write(headers[i]);
			write("\r\n");
		}

		write("Host: ");
		write(host);
		write("\r\n");
		write("Connection: close\r\n");

		if (body != NULL) {
			char contentLength[30];
			sprintf(contentLength, "Content-Length: %d\r\n", strlen(body));
			write(contentLength);
			write("Content-Type: ");
			write(contentType);
			write("\r\n");
		}

		write("\r\n");

		if (body != NULL) {
			write(body);
			write("\r\n");
			write("\r\n");
		}

		delay(100);

		HTTP_DEBUG_PRINT("HTTP: call readResponse\n");

		int statusCode = readResponse(response);

		HTTP_DEBUG_PRINT("HTTP: return readResponse\n");

		HTTP_DEBUG_PRINT("HTTP: stop client\n");

		num_headers = 0;

		client.stop();

		delay(50);

		HTTP_DEBUG_PRINT("HTTP: client stopped\n");

		return statusCode;
	} else {
		HTTP_DEBUG_PRINT("HTTP Connection failed\n");
		return 0;
	}
}

int ClientServer::readResponse(String* response) {

	boolean currentLineIsBlank = true;
	boolean httpBody = false;
	boolean inStatus = false;

	char statusCode[4];
	int i = 0;
	int code = 0;

	if (response == NULL) {
		HTTP_DEBUG_PRINT("HTTP: NULL RESPONSE POINTER: \n");
	} else {
		HTTP_DEBUG_PRINT("HTTP: NON-NULL RESPONSE POINTER: \n");
	}

	HTTP_DEBUG_PRINT("HTTP: RESPONSE: \n");
	while (client.connected()) {
		HTTP_DEBUG_PRINT(".");

		if (client.available()) {
			HTTP_DEBUG_PRINT(",");

			char c = client.read();
			HTTP_DEBUG_PRINT(c);

			if (c == ' ' && !inStatus) {
				inStatus = true;
			}

			if (inStatus && i < 3 && c != ' ') {
				statusCode[i] = c;
				i++;
			}
			if (i == 3) {
				statusCode[i] = '\0';
				code = atoi(statusCode);
			}

			if (httpBody) {

				if (response != NULL)
					response->concat(c);
			} else {
				if (c == '\n' && currentLineIsBlank) {
					httpBody = true;
				}

				if (c == '\n') {

					currentLineIsBlank = true;
				} else if (c != '\r') {

					currentLineIsBlank = false;
				}
			}
		}
	}

	HTTP_DEBUG_PRINT("HTTP: return readResponse3\n");
	return code;
}

void ClientServer::write(const char* string) {
	HTTP_DEBUG_PRINT(string);
	client.print(string);
}

void ClientServer::setHeader(const char* header) {
	headers[num_headers] = header;
	num_headers++;
}

void ClientServer::setContentType(const char* contentTypeValue) {
	contentType = contentTypeValue;
}

void ClientServer::serverOnline(String page) {
	if (!client.connected()) {
		HTTP_DEBUG_PRINT("HTTP:disconnecting client to a server online");
		client.stop();

		while (true) {
			client = server.available();
			if (client) {
				HTTP_DEBUG_PRINT("new client");
				boolean currentLineIsBlank = true;
				while (client.connected()) {
					if (client.available()) {
						char read = client.read();
						HTTP_DEBUG_PRINT(c);
						if (read == '\n' && currentLineIsBlank) {
							client.println("HTTP/1.1 200 OK");
							client.println("Content-Type: text/html");
							client.println("Connection: close");
//							client.println("Refresh: 5");
							client.println();
							if (page) {
								client.println(page);
							} else {
								client.println("<!DOCTYPE HTML>");
								client.println("<html>");
								client.println("<h1> blank page </h1>");
								client.println("</html>");
							}
							break;
						}
						if (read == '\n') {
							currentLineIsBlank = true;
						} else if (read != '\r') {
							currentLineIsBlank = false;
						}
					}
				}
				delay(1);
				client.stop();
				HTTP_DEBUG_PRINT("HTTP: client disconnected\n");
			}
		}

	}
}

