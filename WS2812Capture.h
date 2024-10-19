#pragma once
#include <Arduino.h>
#include <DMAChannel.h>

#define WS2812_RGB      0
#define WS2812_RBG      1
#define WS2812_GRB      2
#define WS2812_GBR      3
#define WS2812_BRG      4
#define WS2812_BGR      5
#define WS2812_RGBW     6
#define WS2812_RBGW     7
#define WS2812_GRBW     8
#define WS2812_GBRW     9
#define WS2812_BRGW     10
#define WS2812_BGRW     11
#define WS2812_WRGB     12
#define WS2812_WRBG     13
#define WS2812_WGRB     14
#define WS2812_WGBR     15
#define WS2812_WBRG     16
#define WS2812_WBGR     17
#define WS2812_RWGB     18
#define WS2812_RWBG     19
#define WS2812_GWRB     20
#define WS2812_GWBR     21
#define WS2812_BWRG     22
#define WS2812_BWGR     23
#define WS2812_RGWB     24
#define WS2812_RBWG     25
#define WS2812_GRWB     26
#define WS2812_GBWR     27
#define WS2812_BRWG     28
#define WS2812_BGWR     29

class WS2812Capture {
public:
	WS2812Capture(uint8_t mypin, uint8_t f=WS2812_GRB) : pin(mypin), format(f) {
		switch (pin) {
			case 2:
				flexpwm = &IMXRT_FLEXPWM4;
				submodule = 2;
				trigger = DMAMUX_SOURCE_FLEXPWM4_READ2;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_04;
				muxval = 1; // page 429
				selectinputreg = &IOMUXC_FLEXPWM4_PWMA2_SELECT_INPUT;
				selectinputval = 0; // page 813
				break;
			case 4:
				flexpwm = &IMXRT_FLEXPWM2;
				submodule = 0;
				trigger = DMAMUX_SOURCE_FLEXPWM2_READ0;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_06;
				muxval = 1; // page 431
				selectinputreg = &IOMUXC_FLEXPWM2_PWMA0_SELECT_INPUT;
				selectinputval = 0; // page 805
				break;
			case 5:
				flexpwm = &IMXRT_FLEXPWM2;
				submodule = 1;
				trigger = DMAMUX_SOURCE_FLEXPWM2_READ1;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_08;
				muxval = 1; // page 434
				selectinputreg = &IOMUXC_FLEXPWM2_PWMA1_SELECT_INPUT;
				selectinputval = 0; // page 806
				break;
			case 6:
				flexpwm = &IMXRT_FLEXPWM2;
				submodule = 2;
				trigger = DMAMUX_SOURCE_FLEXPWM2_READ2;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_B0_10;
				muxval = 2; // page 514
				selectinputreg = &IOMUXC_FLEXPWM2_PWMA2_SELECT_INPUT;
				selectinputval = 1; // page 807
				break;
			case 8:
				flexpwm = &IMXRT_FLEXPWM1;
				submodule = 3;
				trigger = DMAMUX_SOURCE_FLEXPWM1_READ3;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_00;
				muxval = 6; // page 520
				selectinputreg = &IOMUXC_FLEXPWM1_PWMA3_SELECT_INPUT;
				selectinputval = 4; // page 798
				break;
			case 22:
				flexpwm = &IMXRT_FLEXPWM4;
				submodule = 0;
				trigger = DMAMUX_SOURCE_FLEXPWM4_READ0;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_08;
				muxval = 1; // page 496
				selectinputreg = &IOMUXC_FLEXPWM4_PWMA0_SELECT_INPUT;
				selectinputval = 1; // page 811
				break;
			case 23:
				flexpwm = &IMXRT_FLEXPWM4;
				submodule = 1;
				trigger = DMAMUX_SOURCE_FLEXPWM4_READ1;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_09;
				muxval = 1; // page 497
				selectinputreg = &IOMUXC_FLEXPWM4_PWMA1_SELECT_INPUT;
				selectinputval = 1; // page 812
				break;
			case 29:
				flexpwm = &IMXRT_FLEXPWM3;
				submodule = 1;
				trigger = DMAMUX_SOURCE_FLEXPWM3_READ1;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_31;
				muxval = 1; // page 457
				selectinputreg = nullptr;
				selectinputval = 0;
				break;
#ifdef ARDUINO_TEENSY41
			case 36:
				flexpwm = &IMXRT_FLEXPWM2;
				submodule = 3;
				trigger = DMAMUX_SOURCE_FLEXPWM2_READ3;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_B1_02;
				muxval = 6; // page 522
				selectinputreg = &IOMUXC_FLEXPWM2_PWMA3_SELECT_INPUT;
				selectinputval = 4; // page 805
				break;
			case 49:
				flexpwm = &IMXRT_FLEXPWM1;
				submodule = 2;
				trigger = DMAMUX_SOURCE_FLEXPWM1_READ2;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_27;
				muxval = 1; // page 453
				selectinputreg = &IOMUXC_FLEXPWM1_PWMA2_SELECT_INPUT;
				selectinputval = 0; // page 801
				break;
			case 53:
				flexpwm = &IMXRT_FLEXPWM1;
				submodule = 1;
				trigger = DMAMUX_SOURCE_FLEXPWM1_READ1;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_25;
				muxval = 1; // page 451
				selectinputreg = &IOMUXC_FLEXPWM1_PWMA1_SELECT_INPUT;
				selectinputval = 0; // page 800
				break;
			case 54:
				flexpwm = &IMXRT_FLEXPWM3;
				submodule = 0;
				trigger = DMAMUX_SOURCE_FLEXPWM3_READ0;
				muxreg = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_29;
				muxval = 1; // page 455
				selectinputreg = nullptr;
				selectinputval = 0;
				break;
#endif
			default:
				flexpwm = NULL;
				submodule = 0;
				trigger = 0;
				muxreg = nullptr;
				muxval = 0;
				selectinputreg = nullptr;
				selectinputval = 0;
		}
	}
	bool begin(void *buffer, size_t bufsize);
	size_t available();
	size_t numBits() { return decodedbitcount; }
	size_t numPixels() { return (format < 6) ? decodedbitcount / 24 : decodedbitcount / 32; }
	uint32_t getPixel(size_t index);
	void end();

	float bitHighNanoseconds(size_t bitindex) {
		if (bitindex >= maxbytes * 8) return 0;
		const uint16_t *capture = (uint16_t *)buffer;
		uint16_t diff = capture[bitindex * 2 + 1] - capture[bitindex * 2];
		return (float)diff * capture_to_nanoseconds;
	}
	float bitLowNanoseconds(size_t bitindex) {
		if (bitindex >= maxbytes * 8) return 0;
		// TODO: handle last bit gracefully, low time blends into reset time
		const uint16_t *capture = (uint16_t *)buffer;
		uint16_t diff = capture[bitindex * 2 + 2] - capture[bitindex * 2 + 1];
		return (float)diff * capture_to_nanoseconds;
	}
	uint32_t getResetMicroseconds() { return resetMicros; }

	void getPixel(size_t index, uint8_t &red, uint8_t &green, uint8_t &blue) {
		uint32_t rgb = getPixel(index);
		red = (rgb >> 16) & 255;
		green = (rgb >> 8) & 255;
		blue = (rgb >> 0) & 255;
	}
	void getPixel(size_t index, uint8_t &white, uint8_t &red, uint8_t &green, uint8_t &blue) {
		uint32_t wrgb = getPixel(index);
		white = (wrgb >> 24) & 255;
		red = (wrgb >> 16) & 255;
		green = (wrgb >> 8) & 255;
		blue = (wrgb >> 0) & 255;
	}
protected:
	uint8_t analyze(size_t bitoffset, unsigned int numbits=8);
	float capture_to_nanoseconds;
private:
	// configuration, hopefully compiler will optimize these as known constants
	const uint8_t pin;
	const uint8_t format; // 0-5 RGB formats, 6-26 RGBW formats

	// how to access the hardware, maybe compiler will optimize these?
	IMXRT_FLEXPWM_t *flexpwm;
	uint8_t submodule;
	uint8_t trigger;
	uint8_t muxval;
	uint8_t selectinputval;
	volatile uint32_t *muxreg;
	volatile uint32_t *selectinputreg;

	// where to store the captured and analyzed data
	DMAChannel dma;
	void *buffer; // requires 33 bytes for each byte of LED data
	size_t maxbytes; // maximum number of LED bytes we can capture
	size_t decodedbitcount; // number of LED bytes fully decoded

	// detection of reset period
	size_t priorbitcount; // used by available() to detect when data still arriving
	elapsedMicros inactiveMicros; // microseconds since new data arrived

	// stats
	uint32_t resetMicros; // microseconds of reset before LED data started
};

