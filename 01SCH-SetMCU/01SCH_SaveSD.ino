/*
 Name:		_01SCH_SetMCU.ino
 Created:	6/23/2020 07:40:44 PM
 Author:	mrodriguez
*/

// Librreias SD
#include "FS.h"
#include "SD.h"
#include <SPI.h>

#define SD_CS 5						// Define CS pin para modulo SD

void setup() {
	Serial.begin(115200);

}


void loop() {
}


bool SD_leer() {
	return 0;
}

bool SD_escribir() {
	return 0;
}

bool SD_borrar() {
	bool funciono = 0;
	Serial.println("Iniciando SD...");
	SD.begin(SD_CS);
	if (!SD.begin(SD_CS)) {
		Serial.println("Error modulo SD!");
		funciono = 0;
	}
	uint8_t cardType = SD.cardType();
	if (cardType == CARD_NONE) {
		Serial.println("Error tarjeta SD!");
		funciono = 0;
	}
	if (!SD.begin(SD_CS)) {
		Serial.println("ERROR - Falla en tarjeta SD!");
		funciono = 0;
	}
	schFile = SD.open("/log.txt", FILE_READ);
	if (schFile) {
		String confLine;
		while (schFile.available()) {
			confLine += schFile.readString();
		}
		if (SD.remove("/log.txt")) {
			Serial.println("registro.txt borrado.");
			return 1;
		}
		else {
			Serial.println("No se pudo borrar registro.txt");
			return 0;
		}
	}
	else {
		Serial.println("Error al abrir configuración!");
		return false;
	}

}