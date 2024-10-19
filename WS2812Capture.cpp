#include <Arduino.h>
#include <WS2812Capture.h>

bool WS2812Capture::begin(void *buf, size_t bufsize)
{
	if (!flexpwm) return false;
	if (!buf || bufsize < 33) return false;
	buffer = buf;
	maxbytes = bufsize / 33; // 8 low counts + 8 high counts + 1 actual byte
	inactiveMicros = 0;
	priorbitcount = 0;
	decodedbitcount = 0;

	// configure FlexPWM timer
	flexpwm->MCTRL |= FLEXPWM_MCTRL_CLDOK(1<<submodule);
	flexpwm->SM[submodule].INIT = 0;
	flexpwm->SM[submodule].VAL0 = 0;
	flexpwm->SM[submodule].VAL1 = 0xFFFF;
	flexpwm->SM[submodule].VAL2 = 0;
	flexpwm->SM[submodule].VAL3 = 0;
	flexpwm->SM[submodule].VAL4 = 0;
	flexpwm->SM[submodule].VAL5 = 0;
	flexpwm->MCTRL |= FLEXPWM_MCTRL_LDOK(1<<submodule) | FLEXPWM_MCTRL_RUN(1<<submodule);

	// configure DMA to store FlexPWM timer CVAL2 and CVAL3 to buffer
	dma.begin();
	dma.TCD->SADDR = &(flexpwm->SM[submodule].CVAL2);
	dma.TCD->SOFF = 4;
	dma.TCD->ATTR = DMA_TCD_ATTR_SSIZE(1) | DMA_TCD_ATTR_DSIZE(1);
	dma.TCD->NBYTES_MLNO = DMA_TCD_NBYTES_MLOFFYES_NBYTES(4) |
		DMA_TCD_NBYTES_MLOFFYES_MLOFF(-8) | DMA_TCD_NBYTES_SMLOE;
	dma.TCD->SLAST = 0;
	dma.TCD->DADDR = buffer;
	dma.TCD->DOFF = 2;
	dma.TCD->CITER_ELINKNO = maxbytes * 8;
	dma.TCD->DLASTSGA = 0;
	dma.TCD->BITER_ELINKNO = maxbytes * 8;
	dma.TCD->CSR = DMA_TCD_CSR_DREQ;
	dma.triggerAtHardwareEvent(trigger);
	dma.enable();

	// confire FlexPWM timer to request DMA (page 3173, 3166)
	flexpwm->SM[submodule].CAPTCTRLA = FLEXPWM_SMCAPTCTRLA_ARMA |
		FLEXPWM_SMCAPTCTRLA_EDGA0(2/*rising*/) |
		FLEXPWM_SMCAPTCTRLA_EDGA1(1/*falling*/);
	flexpwm->SM[submodule].DMAEN = FLEXPWM_SMDMAEN_CAPTDE(1) | FLEXPWM_SMDMAEN_CA1DE;

	// configure pin for input
	// TODO: wait for pin to be low, maybe for minimum time so we don't begin inside an update?
	if (selectinputreg) *selectinputreg = selectinputval;
	*muxreg = muxval | 0x10;

	// TODO: zero stats
	
	capture_to_nanoseconds = 1.0e9 / (double)F_BUS_ACTUAL;
	return true;
}

uint8_t WS2812Capture::analyze(size_t bitoffset, unsigned int numbits /*up to 8 bits*/)
{
	uint8_t data=0;
	for (uint8_t mask=0x80; mask; mask = mask >> 1) {
		if (numbits == 0) break;
		float th = bitHighNanoseconds(bitoffset);
		float tl = bitLowNanoseconds(bitoffset);
		if (th > 450) data |= mask; // TODO: configurable threashold
		// TODO: compute stats... min, max, avd, stddev
		bitoffset++;
		numbits--;
	}
	return data;
}

size_t WS2812Capture::available()
{
	// get the number of bits received so far by DMA
	size_t bitcount = ((uint32_t)(dma.TCD->DADDR) - (uint32_t)buffer) >> 2;
	// if new data has arrived
	if (bitcount > priorbitcount) {
		// handle begin of new LED update
		if (priorbitcount == 0) {
			resetMicros = inactiveMicros;
			decodedbitcount = 0;
		}
		// analyze any full bytes received so far
		uint8_t *databuf = (uint8_t *)((uint32_t)buffer + maxbytes * 16);
		databuf += decodedbitcount / 8;
		while (bitcount - decodedbitcount > 8) {
			*databuf++ = analyze(decodedbitcount, 8);
			decodedbitcount += 8;
		}
		priorbitcount = bitcount;
		inactiveMicros = 0;
		return 0;
	}
	// if inactive after data received, assume end of LED frame
	if (bitcount > 0 && inactiveMicros >= 12) { // TODO: configurable threshold
		// finish analyzing any data we received but didn't already analyze
		uint8_t *databuf = (uint8_t *)((uint32_t)buffer + maxbytes * 16);
		databuf += decodedbitcount / 8;
		while (bitcount - decodedbitcount) {
			size_t bits = bitcount - decodedbitcount;
			if (bits > 8) bits = 8;
			*databuf++ = analyze(decodedbitcount, bits);
			decodedbitcount += bits;
		}
		// reset the DMA capture to begin watching for next LED update
		dma.disable();
		dma.TCD->DADDR = buffer;
		dma.TCD->CITER_ELINKNO = maxbytes * 8;
		dma.TCD->BITER_ELINKNO = maxbytes * 8;
		dma.enable();
		priorbitcount = 0;
		return bitcount;
	}
	return 0;
}

uint32_t WS2812Capture::getPixel(size_t index)
{
	if (format < 6) {
		const uint8_t *p = (uint8_t *)((uint32_t)buffer + maxbytes * 16 + index * 3);
		uint8_t r, g, b;
		switch (format) {
			case WS2812_RGB: r = *p++; g = *p++; b = *p++; break;
			case WS2812_RBG: r = *p++; b = *p++; g = *p++; break;
			case WS2812_BRG: b = *p++; r = *p++; g = *p++; break;
			case WS2812_BGR: b = *p++; g = *p++; r = *p++; break;
			case WS2812_GBR: g = *p++; b = *p++; r = *p++; break;
			default:
			case WS2812_GRB: g = *p++; r = *p++; b = *p++; break;
		}
		return (r << 16) | (g << 8) | b;
	}
	if (format < 30) {
		const uint8_t *p = (uint8_t *)((uint32_t)buffer + maxbytes * 16 + index * 4);
		uint8_t w, r, g, b;
		switch (format) {
			case WS2812_RGBW: r = *p++; g = *p++; b = *p++; w = *p++; break;
			case WS2812_RBGW: r = *p++; b = *p++; g = *p++; w = *p++; break;
			case WS2812_GRBW: g = *p++; r = *p++; b = *p++; w = *p++; break;
			case WS2812_GBRW: g = *p++; b = *p++; r = *p++; w = *p++; break;
			case WS2812_BRGW: b = *p++; r = *p++; g = *p++; w = *p++; break;
			case WS2812_BGRW: b = *p++; g = *p++; r = *p++; w = *p++; break;
			case WS2812_WRGB: w = *p++; r = *p++; g = *p++; b = *p++; break;
			case WS2812_WRBG: w = *p++; r = *p++; b = *p++; g = *p++; break;
			case WS2812_WGRB: w = *p++; g = *p++; r = *p++; b = *p++; break;
			case WS2812_WGBR: w = *p++; g = *p++; b = *p++; r = *p++; break;
			case WS2812_WBRG: w = *p++; b = *p++; r = *p++; g = *p++; break;
			case WS2812_WBGR: w = *p++; b = *p++; g = *p++; r = *p++; break;
			case WS2812_RWGB: r = *p++; w = *p++; g = *p++; b = *p++; break;
			case WS2812_RWBG: r = *p++; w = *p++; b = *p++; g = *p++; break;
			case WS2812_GWRB: g = *p++; w = *p++; r = *p++; b = *p++; break;
			case WS2812_GWBR: g = *p++; w = *p++; b = *p++; r = *p++; break;
			case WS2812_BWRG: b = *p++; w = *p++; r = *p++; g = *p++; break;
			case WS2812_BWGR: b = *p++; w = *p++; g = *p++; r = *p++; break;
			case WS2812_RGWB: r = *p++; g = *p++; w = *p++; b = *p++; break;
			case WS2812_RBWG: r = *p++; b = *p++; w = *p++; g = *p++; break;
			case WS2812_GRWB: g = *p++; r = *p++; w = *p++; b = *p++; break;
			case WS2812_GBWR: g = *p++; b = *p++; w = *p++; r = *p++; break;
			case WS2812_BRWG: b = *p++; r = *p++; w = *p++; g = *p++; break;
			case WS2812_BGWR: b = *p++; g = *p++; w = *p++; r = *p++; break;
		}
		return (w << 24) | (r << 16) | (g << 8) | b;
	}
	return 0;
}


void WS2812Capture::end()
{
	dma.disable();
	flexpwm->SM[submodule].DMAEN = 0;
	// TODO: restore FlexPWM settings
	// TODO: de-allocate DMA channel
	// TODO: pinMode to restore pin to GPIO
}

