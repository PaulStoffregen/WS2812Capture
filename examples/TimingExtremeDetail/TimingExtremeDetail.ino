#include <WS2812Capture.h>

#define PIN 8 // supported pins: 2, 4, 5, 6, 8, 22, 23, 29
#define MAXLED 250
WS2812Capture my2812(PIN, WS2812_GRB);

void setup() {
	Serial.begin(9600);
	while (!Serial) ;
	Serial.println("WS2812Capture TimingExtremeDetail");
	static byte buffer[MAXLED*99]; // each LED needs 99 bytes
	my2812.begin(buffer, sizeof(buffer));
}

void loop() {
	if (my2812.available()) {
		Serial.printf("Reset %u us before LED data\n", my2812.getResetMicroseconds());
		for (unsigned int n=0; n < my2812.numPixels(); n++) {
			Serial.printf("Pixel %03u: high ns: ", n);
			for (unsigned int b=0; b < 24; b++) {
				float high_ns = my2812.bitHighNanoseconds(n * 24 + b);
				Serial.printf(" %4.0f", high_ns);
				if (b == 7 || b == 15) Serial.print("  ");
			}
			Serial.println();
			Serial.printf(" (%06X)   low ns: ", my2812.getPixel(n));
			for (unsigned int b=0; b < 24; b++) {
				float low_ns = my2812.bitLowNanoseconds(n * 24 + b);
				Serial.printf(" %4.0f", low_ns);
				if (b == 7 || b == 15) Serial.print("  ");
			}
			Serial.println();
		}
		Serial.println();
	}
}

