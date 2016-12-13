#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

#include <SD.h>



#ifndef CLIENTSERVER_H_
#define CLIENTSERVER_H_

class ClientServer {
public:
	const char* host;
	const char* _host;
	ClientServer(const char* _host, int _port);
	virtual ~ClientServer();
	bool dhcp();
	int begin(byte*);
	int post(const char* path, const char* body, String* response);
	int get(const char* path, const char* body, String* response);
	int get(const char*, String*);
	void setHeader(const char*);
	void setContentType(const char*);
	int request(const char* method, const char* path, const char* body,
			String* response);
	void serverOnline(String page);

private:
	EthernetClient client;
	EthernetServer server = (80);
	int readResponse(String*);
	void write(const char*);

	int port;
	int _port;
	int num_headers;
	const char* headers[10];
	const char* contentType;
};

#endif
