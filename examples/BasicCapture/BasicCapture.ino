#include <WS2812Capture.h>

#define PIN 8 // supported pins: 2, 4, 5, 6, 8, 22, 23, 29
#define MAXLED 250
WS2812Capture my2812(PIN, WS2812_GRB);

void setup() {
	Serial.begin(9600);
	while (!Serial) ;
	Serial.println("WS2812Capture BasicCapture");
	static byte buffer[MAXLED*99]; // each LED needs 99 bytes
	my2812.begin(buffer, sizeof(buffer));
}

void loop() {
	if (my2812.available()) {
		Serial.print("Pixels:");
		for (unsigned int n=0; n < my2812.numPixels(); n++) {
			Serial.printf(" %06X", my2812.getPixel(n));
		}
		Serial.println();
	}
}


