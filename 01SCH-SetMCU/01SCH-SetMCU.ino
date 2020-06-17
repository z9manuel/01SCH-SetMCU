/*
 Name:		_01SCH_SetMCU.ino
 Created:	6/16/2020 11:13:44 PM
 Author:	mrodriguez
*/

// Librreias SD
#include "FS.h"
#include "SD.h"
#include <SPI.h>
// Librerias Red
#include <ArduinoJson.h>
#include <DateTime.h>
#include <WiFi.h>

#define SD_CS 5						// Define CS pin para modulo SD8

File schFile;
boolean okSD = 0, okNET = 0;
boolean error = 0;
String servidorAPI;

void setup() {
	Serial.begin(115200);
	iniciarMCU() == true ? Serial.println("MCU Listo!") : Serial.println("MCU Falla!");

}


void loop() {

}

boolean iniciarMCU() {
	byte  ipA, ipB, ipC, ipD;
	byte  gwA, gwB, gwC, gwD;
	byte  msA, msB, msC, msD;
	byte  dns1A, dns1B, dns1C, dns1D;
	byte  dns2A, dns2B, dns2C, dns2D;
	String red, contrred, apiser, cronos;
	String serNTP;

	// Iniciando SD
	Serial.println("Iniciando SD...");
	okSD = 1;
	SD.begin(SD_CS);
	if (!SD.begin(SD_CS)) {
		Serial.println("Error modulo SD!");
		okSD = 0;
	}
	uint8_t cardType = SD.cardType();
	if (cardType == CARD_NONE) {
		Serial.println("Error tarjeta SD!");
		okSD = 0;
	}
	if (!SD.begin(SD_CS)) {
		Serial.println("ERROR - Falla en tarjeta SD!");
		okSD = 0;
	}
	schFile = SD.open("/schconf.json", FILE_READ);
	if (schFile) {
		String confLine;
		while (schFile.available()) {
			confLine += schFile.readString();
		}
		schFile.close();
		char JSONMessage[confLine.length() + 1];
		confLine.toCharArray(JSONMessage, confLine.length() + 1);
		Serial.println(JSONMessage);

		DynamicJsonDocument doc(1024);
		DeserializationError error = deserializeJson(doc, JSONMessage);
		if (error) {
			Serial.print("Error en configuraciones!");
			Serial.println(error.c_str());
			okSD = 0;
		}

		ipA = int(doc["ipA"]); ipB = int(doc["ipB"]); ipC = int(doc["ipC"]); ipD = int(doc["ipD"]);
		gwA = int(doc["gwA"]); gwB = int(doc["gwB"]); gwC = int(doc["gwC"]); gwD = int(doc["gwD"]);
		msA = int(doc["msA"]); msB = int(doc["msB"]); msC = int(doc["msC"]); msD = int(doc["msD"]);
		dns1A = int(doc["dns1A"]); dns1B = int(doc["dns1B"]); dns1C = int(doc["dns1C"]); dns1D = int(doc["dns1D"]);
		dns2A = int(doc["dns2A"]); dns2B = int(doc["dns2B"]); dns2C = int(doc["dns2C"]); dns2D = int(doc["dns2D"]);
		String textwifi = doc["wifi"];
		String textpasswifi = doc["passwifi"];
		String textapi = doc["API"];
		String textntp = doc["NTP"];
		red = textwifi;
		contrred = textpasswifi;
		apiser = textapi;
		cronos = textntp;
		okSD = 1;
	}
	else {
		Serial.println("Error al abrir configuración!");
		return false;
	}


	if (okSD == 1) {
		okNET = 0;
		Serial.println("SD OK!");
		// Configuraciones de direccionamiento IP
		IPAddress local_IP(ipA, ipB, ipC, ipD);
		IPAddress gateway(gwA, gwB, gwC, gwD);
		IPAddress subnet(msA, msB, msC, msD);
		IPAddress DNS1(dns1A, dns1B, dns1C, dns1D);
		IPAddress DNS2(dns2A, dns2B, dns2C, dns2D);
		//uint8_t   mac[6]{ mac1, mac2, mac3, mac4, mac5, mac6 };
		char ssid[red.length() + 1];
		red.toCharArray(ssid, red.length() + 1);
		char password[contrred.length() + 1];
		contrred.toCharArray(password, contrred.length() + 1);
		char servidorNTP[cronos.length() + 1];
		cronos.toCharArray(servidorNTP, cronos.length() + 1);
		servidorAPI = apiser;

		WiFi.disconnect();
		delay(500);
		WiFi.begin(ssid, password);
		if (!WiFi.config(local_IP, gateway, subnet, DNS1, DNS2)) {
			Serial.println("Error al configurar IP...");
			okNET = 0;
		}
		delay(500);
		int i = 0;
		while (WiFi.status() != WL_CONNECTED) {
			WiFi.disconnect();
			Serial.println("Conectando a WiFi..");
			delay(500);
			WiFi.begin(ssid, password);
			if (!WiFi.config(local_IP, gateway, subnet, DNS1, DNS2))
				Serial.println("Error al configurar IP...");
			i == 9 ? ESP.restart() : delay(1000);
			i++;
		}
		Serial.print("WiFi OK: ");
		okNET = 1;
		Serial.println(local_IP);
		Serial.println("Buscando NTP...");
		DateTime.setTimeZone(-6);
		DateTime.setServer(servidorNTP);
		DateTime.begin();
		delay(500);
		int intento = 0;
		while (!DateTime.isTimeValid() && intento < 9) {
			Serial.println("NTP Error!");
			delay(500);
			intento++;
		}
		//Ajuste de horario de verano por mes
		String mes = "";
		String fecha = DateTime.toString();
		mes = mes + fecha[5]; mes = mes + fecha[6];
		((mes.toInt()) > 3 && (mes.toInt()) < 11) ? DateTime.setTimeZone(-5) : DateTime.setTimeZone(-6);
		DateTime.forceUpdate();
		Serial.print("Tiempo: ");
		Serial.println(DateTime.toString());
	}
	return okNET;
}