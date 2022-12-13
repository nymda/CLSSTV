#include "textRendering.h"
#include <cstdarg>

namespace tr {

	//run init before dereferencing this
	SSTV::rgb* rgbFont = 0;

	vec2 fontImageSize = { 144, 101 };
	
	//monochrome font data, compressed so each byte contains 8 pixels
	//font from https://github.com/epto/epto-fonts
	const char compressedFontData[] = {
		0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x30, 0xB8, 0x33, 0xE3, 0xC7, 0x01, 0x0F, 0x1C, 0x60, 0x0C, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x30, 0x00, 0x30, 0x98, 0x31, 0xB3, 0x6D, 0x83, 0x19, 0x0C, 0x30, 0x18, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x30, 0xCC, 0x30, 0xB3, 0x61, 0x83, 0x19, 0x06, 0x18,
		0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x30, 0x00, 0xF8, 0xB7, 0xC1, 0x81, 0x19,
		0x00, 0x18, 0x30, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x30, 0x00, 0x30, 0xE3, 0x01,
		0x18, 0x0F, 0x00, 0x0C, 0x60, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x30, 0x00, 0x30,
		0x83, 0x03, 0x8E, 0x07, 0x00, 0x0C, 0x60, 0xFC, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x30,
		0x00, 0x30, 0x83, 0x87, 0xC3, 0x0C, 0x00, 0x0C, 0x60, 0xF0, 0xF8, 0x07, 0xE0, 0x1F, 0x00, 0x06,
		0x00, 0x30, 0x00, 0xF8, 0x87, 0xED, 0xC0, 0x38, 0x00, 0x0C, 0x60, 0xF0, 0xC0, 0x00, 0x00, 0x00,
		0x00, 0x06, 0x00, 0x00, 0x00, 0x30, 0x83, 0x0D, 0xCE, 0x18, 0x00, 0x0C, 0x60, 0x98, 0xC1, 0x00,
		0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x30, 0xB3, 0x0D, 0xDB, 0x18, 0x00, 0x18, 0x30, 0x00,
		0xC0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x30, 0x00, 0x30, 0xB3, 0x0D, 0xDB, 0x18, 0x00, 0x18,
		0x30, 0x00, 0x00, 0x80, 0x03, 0x00, 0x86, 0x01, 0x00, 0x30, 0x00, 0x30, 0xE3, 0x07, 0x8E, 0x37,
		0x00, 0x30, 0x18, 0x00, 0x00, 0x80, 0x01, 0x00, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01,
		0x00, 0x00, 0x00, 0x60, 0x0C, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x3C, 0x30, 0xF8, 0xF1, 0x03, 0xE4, 0x1F, 0x9F, 0x7F, 0x7E, 0xFC, 0x00,
		0x00, 0x00, 0x0C, 0xC0, 0x00, 0x3F, 0x66, 0x38, 0x0C, 0x1B, 0x06, 0x66, 0x80, 0x01, 0x60, 0xC3,
		0x86, 0x01, 0x00, 0x00, 0x06, 0x80, 0x81, 0x61, 0xC3, 0x3C, 0x00, 0x03, 0x06, 0x67, 0xC0, 0x00,
		0x60, 0xC3, 0x86, 0x01, 0x00, 0x00, 0x03, 0x00, 0x03, 0x60, 0xC3, 0x30, 0x00, 0x03, 0x86, 0x67,
		0xC0, 0x00, 0x30, 0xC3, 0x86, 0x61, 0xC0, 0x80, 0x01, 0x00, 0x06, 0x60, 0xC3, 0x30, 0x00, 0x03,
		0xC6, 0xE6, 0xC7, 0x00, 0x18, 0xC3, 0x86, 0x61, 0xC0, 0xC0, 0xE0, 0x1F, 0x0C, 0x60, 0xC3, 0x30,
		0x80, 0xC1, 0x63, 0x06, 0xCC, 0x1F, 0x18, 0x7E, 0xFC, 0x01, 0x00, 0x60, 0x00, 0x00, 0x18, 0x38,
		0xF3, 0x30, 0xC0, 0x00, 0x36, 0x06, 0xD8, 0x30, 0x0C, 0xC3, 0x80, 0x01, 0x00, 0x60, 0x00, 0x00,
		0x18, 0x0C, 0xDF, 0x30, 0x60, 0x00, 0xF6, 0x0F, 0xD8, 0x30, 0x0C, 0xC3, 0x80, 0x01, 0x00, 0xC0,
		0xE0, 0x1F, 0x0C, 0x0C, 0xC3, 0x30, 0x30, 0x00, 0x06, 0x06, 0xD8, 0x30, 0x06, 0xC3, 0x80, 0x01,
		0x00, 0x80, 0x01, 0x00, 0x06, 0x0C, 0xC3, 0x30, 0x18, 0x00, 0x06, 0x06, 0xD8, 0x30, 0x06, 0xC3,
		0x80, 0x61, 0xC0, 0x00, 0x03, 0x00, 0x03, 0x00, 0x66, 0x30, 0x0C, 0x18, 0x06, 0x66, 0xD8, 0x30,
		0x06, 0xC3, 0xC0, 0x60, 0xC0, 0x00, 0x06, 0x80, 0x01, 0x0C, 0x3C, 0x30, 0xFC, 0xF3, 0x03, 0xC6,
		0x8F, 0x1F, 0x06, 0x7E, 0x7C, 0x00, 0x60, 0x00, 0x0C, 0xC0, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x30, 0xFC, 0xE1, 0xF3, 0xE3, 0xCF, 0x1F, 0x3E, 0xC3,
		0x30, 0x00, 0x1B, 0x36, 0x60, 0xD8, 0x30, 0x1E, 0xC6, 0x30, 0x0C, 0x33, 0x36, 0x66, 0xC0, 0x00,
		0x63, 0xC3, 0x30, 0x00, 0x1B, 0x36, 0x60, 0xD8, 0x30, 0x33, 0xC3, 0x78, 0x0C, 0x1B, 0x30, 0x6C,
		0xC0, 0x80, 0x01, 0xC3, 0x30, 0x00, 0x1B, 0x33, 0x60, 0xD8, 0xB0, 0x61, 0xFB, 0x48, 0x0C, 0x1B,
		0x30, 0x6C, 0xC0, 0x80, 0x01, 0xC3, 0x30, 0x00, 0x9B, 0x31, 0xE0, 0xDC, 0xB1, 0x61, 0xDB, 0xCC,
		0x0C, 0x1B, 0x30, 0x6C, 0xC0, 0x80, 0x01, 0xC3, 0x30, 0x00, 0xDB, 0x30, 0xE0, 0xDF, 0xB3, 0x61,
		0xDB, 0xCC, 0xFC, 0x19, 0x30, 0xEC, 0xCF, 0x9F, 0x01, 0xFF, 0x30, 0x00, 0x7B, 0x30, 0x60, 0xDB,
		0xB6, 0x61, 0xDB, 0xCC, 0x0C, 0x1B, 0x30, 0x6C, 0xC0, 0x80, 0x79, 0xC3, 0x30, 0x00, 0x7B, 0x30,
		0x60, 0xDB, 0xBC, 0x61, 0xDB, 0xFC, 0x0C, 0x1B, 0x30, 0x6C, 0xC0, 0x80, 0x61, 0xC3, 0x30, 0x0C,
		0xDB, 0x30, 0x60, 0xD8, 0xB8, 0x61, 0x7B, 0x86, 0x0D, 0x1B, 0x30, 0x6C, 0xC0, 0x80, 0x61, 0xC3,
		0x30, 0x0C, 0x9B, 0x31, 0x60, 0xD8, 0xB0, 0x61, 0x03, 0x86, 0x0D, 0x1B, 0x30, 0x6C, 0xC0, 0x80,
		0x61, 0xC3, 0x30, 0x0C, 0x1B, 0x33, 0x60, 0xD8, 0xB0, 0x61, 0x06, 0x86, 0x0D, 0x33, 0x36, 0x66,
		0xC0, 0x00, 0x63, 0xC3, 0x30, 0x0C, 0x1B, 0x36, 0x60, 0xD8, 0x30, 0x33, 0x7C, 0x86, 0xFD, 0xE1,
		0xF3, 0xE3, 0xCF, 0x00, 0x3E, 0xC3, 0x30, 0xF8, 0x19, 0xF6, 0x6F, 0xD8, 0x30, 0x1E, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x78, 0xFC, 0xF1, 0xF1, 0x6F, 0xD8, 0xB0,
		0x61, 0xC3, 0x86, 0xFD, 0xE3, 0x33, 0xC0, 0x07, 0x06, 0x00, 0xC3, 0xCC, 0x0C, 0x1B, 0x83, 0x61,
		0xD8, 0xB0, 0x61, 0xC3, 0x86, 0x81, 0x61, 0x30, 0x00, 0x06, 0x0F, 0x00, 0xC3, 0x86, 0x0D, 0x1B,
		0x80, 0x61, 0xD8, 0xB0, 0x61, 0xC3, 0x86, 0x81, 0x61, 0x60, 0x00, 0x86, 0x19, 0x00, 0xC3, 0x86,
		0x0D, 0x1B, 0x80, 0x61, 0xD8, 0xB0, 0x61, 0x66, 0xCC, 0xC0, 0x60, 0x60, 0x00, 0xC6, 0x30, 0x00,
		0xC3, 0x86, 0x0D, 0x73, 0x80, 0x61, 0xD8, 0xB0, 0x61, 0x66, 0xCC, 0xC0, 0x60, 0xC0, 0x00, 0x06,
		0x00, 0x00, 0x63, 0x86, 0x8D, 0xC1, 0x81, 0x61, 0x98, 0x99, 0x61, 0x3C, 0x78, 0x60, 0x60, 0xC0,
		0x00, 0x06, 0x00, 0x00, 0x3B, 0x86, 0xED, 0x00, 0x83, 0x61, 0x98, 0x99, 0x6D, 0x3C, 0x30, 0x60,
		0x60, 0x80, 0x01, 0x06, 0x00, 0x00, 0x0F, 0x86, 0xBD, 0x01, 0x86, 0x61, 0x98, 0x99, 0x6D, 0x66,
		0x30, 0x30, 0x60, 0x80, 0x01, 0x06, 0x00, 0x00, 0x03, 0x86, 0x8D, 0x01, 0x86, 0x61, 0x18, 0x8F,
		0x7F, 0x66, 0x30, 0x30, 0x60, 0x00, 0x03, 0x06, 0x00, 0x00, 0x03, 0xBE, 0x0D, 0x1B, 0x86, 0x61,
		0x18, 0x8F, 0x73, 0xC3, 0x30, 0x18, 0x60, 0x00, 0x03, 0x06, 0x00, 0x00, 0x03, 0xEC, 0x0C, 0x1B,
		0x86, 0x61, 0x18, 0x86, 0x61, 0xC3, 0x30, 0x18, 0x60, 0x00, 0x06, 0x06, 0x00, 0x00, 0x03, 0x78,
		0x0C, 0xF3, 0x83, 0xC1, 0x0F, 0x86, 0x61, 0xC3, 0x30, 0xFC, 0xE3, 0x03, 0xC6, 0x07, 0x00, 0x00,
		0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x80, 0x7F, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x0C, 0x00, 0x00, 0x0C,
		0x00, 0x1F, 0x00, 0x03, 0x30, 0xC0, 0x18, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x18, 0x00, 0x0C, 0x00,
		0x00, 0x0C, 0x80, 0x31, 0x00, 0x03, 0x30, 0xC0, 0x18, 0x80, 0x01, 0x00, 0x00, 0x00, 0x30, 0x00,
		0x0C, 0x00, 0x00, 0x0C, 0x80, 0x31, 0x00, 0x03, 0x00, 0x00, 0x18, 0x80, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x0C, 0x00, 0x00, 0x0C, 0x80, 0x01, 0x00, 0x03, 0x00, 0x00, 0x18, 0x80, 0x01, 0x00,
		0x00, 0x00, 0x00, 0xF8, 0xFC, 0xE0, 0xC3, 0xCF, 0x8F, 0x01, 0x7E, 0x3F, 0x30, 0xC0, 0x18, 0x83,
		0xE1, 0xCC, 0x1E, 0x3F, 0x00, 0x80, 0x8D, 0x31, 0x66, 0x6C, 0xD8, 0x07, 0x63, 0x63, 0x30, 0xC0,
		0x98, 0x81, 0x61, 0xDB, 0xB3, 0x61, 0x00, 0xF8, 0x0D, 0x1B, 0x30, 0x6C, 0x98, 0x81, 0x61, 0xC3,
		0x30, 0xC0, 0xD8, 0x80, 0x61, 0xDB, 0xB0, 0x61, 0x00, 0x8C, 0x0D, 0x1B, 0x30, 0xEC, 0x8F, 0x81,
		0x61, 0xC3, 0x30, 0xC0, 0xF8, 0x80, 0x61, 0xDB, 0xB0, 0x61, 0x00, 0x86, 0x0D, 0x1B, 0x30, 0x6C,
		0x80, 0x81, 0x61, 0xC3, 0x30, 0xC0, 0xF8, 0x80, 0x61, 0xD8, 0xB0, 0x61, 0x00, 0x86, 0x0D, 0x1B,
		0x30, 0x6C, 0x80, 0x81, 0x61, 0xC3, 0x30, 0xC0, 0x98, 0x81, 0x61, 0xD8, 0xB0, 0x61, 0x00, 0x86,
		0x0D, 0x33, 0x36, 0x6C, 0x80, 0x01, 0x63, 0xC3, 0x30, 0xC0, 0x18, 0x83, 0x61, 0xD8, 0xB0, 0x61,
		0x00, 0xFC, 0xFD, 0xE1, 0xE3, 0xCF, 0x8F, 0x01, 0x7E, 0xC3, 0x30, 0xC0, 0x18, 0x86, 0x61, 0xD8,
		0x30, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0xCC, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x61, 0x00, 0x00, 0xCC,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00,
		0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x83, 0xC1, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x80, 0x01, 0x03, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x80, 0x01, 0x03,
		0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x80,
		0x01, 0x03, 0x00, 0x0E, 0x3F, 0xF8, 0xED, 0xF1, 0xE1, 0x6F, 0xD8, 0xB0, 0x61, 0xC3, 0x86, 0xFD,
		0xC3, 0x80, 0x01, 0x03, 0x00, 0x1B, 0x63, 0x8C, 0x1D, 0x1B, 0x83, 0x61, 0xD8, 0xB0, 0x61, 0x66,
		0x86, 0x81, 0x71, 0x80, 0x01, 0x8E, 0x9B, 0x31, 0xC3, 0x86, 0x0D, 0x70, 0x80, 0x61, 0xD8, 0xB0,
		0x61, 0x3C, 0x86, 0xC1, 0x70, 0x80, 0x01, 0xCE, 0x8E, 0x31, 0xC3, 0x86, 0x0D, 0xC0, 0x81, 0x61,
		0x98, 0x99, 0x6D, 0x18, 0x86, 0x61, 0xC0, 0x80, 0x01, 0x03, 0x80, 0x31, 0xC3, 0x86, 0x0D, 0x00,
		0x83, 0x61, 0x98, 0x99, 0x6D, 0x18, 0x86, 0x31, 0xC0, 0x80, 0x01, 0x03, 0x80, 0x31, 0xC3, 0x86,
		0x0D, 0x00, 0x83, 0x61, 0x18, 0x8F, 0x7F, 0x3C, 0x86, 0x19, 0xC0, 0x80, 0x01, 0x03, 0x80, 0x3F,
		0x63, 0x8C, 0x0D, 0x18, 0x83, 0x61, 0x18, 0x86, 0x61, 0x66, 0xCC, 0x0C, 0xC0, 0x80, 0x01, 0x03,
		0x00, 0x00, 0x3F, 0xF8, 0x0D, 0xF0, 0x01, 0xCF, 0x0F, 0x86, 0x61, 0xC3, 0x78, 0xFC, 0x83, 0x83,
		0xC1, 0x01, 0x00, 0x00, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};

	char fontMap[] = {
		' ', '!', '"', '#', '$', '%', '&', ' ', '(', ')', '*', '+', ',', '-', '.', '/',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
		'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', ' ', ']', '^', '_',
		' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
		'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', ' ',
	};
	
	void drawCharacter(SSTV::rgb* canvas, vec2 canvasSize, char c, vec2 pos) {
		int cmIndex = 0;
		for (char cm : fontMap) {
			if (cm == c) {
				vec2 map = { cmIndex % 16, cmIndex / 16 };
				vec2 mapPosExpanded = { (map.X * 8) + ((map.X * 8) / 8), (map.Y * 16) + ((map.Y * 16) / 16) };
				
				for (int y = 0; y < 16; y++) {
					for (int x = 0; x < 8; x++) {
						canvas[((pos.Y + y) * canvasSize.X) + (pos.X + x)] = rgbFont[((mapPosExpanded.Y + y) * fontImageSize.X) + (mapPosExpanded.X + x)];
					}
				}
				
				break;
			}
			cmIndex++;
		}
	}

	void drawSpacer(SSTV::rgb* canvas, vec2 canvasSize, int width, vec2 pos) {
		for (int y = 0; y < 16; y++) {
			for (int x = 0; x < width; x++) {
				canvas[((pos.Y + y) * canvasSize.X) + (pos.X + x)] = { 0, 0, 0 };
			}
		}
	}
	
	//text overrunning the edge of the provided canvas will be truncated
	void drawString(SSTV::rgb* canvas, vec2 canvasSize, vec2 pos, const char* fmt...) {
		
		//format string into buffer
		va_list lst;
		va_start(lst, fmt);
		char buffer[64];
		vsnprintf(buffer, 64, fmt, lst);

		//draw the background rectangle
		
		int offset = 0;
		int spacerWidth = 2;
		
		drawSpacer(canvas, canvasSize, spacerWidth, { pos.X, pos.Y });
		offset += spacerWidth;
		
		//draw the required characters with 1px between them
		for (int i = 0; i < strlen(buffer); i++) {
			if ((offset + 8) > canvasSize.X) {
				return;
			}
			
			drawCharacter(canvas, canvasSize, buffer[i], { pos.X + offset, pos.Y });
			offset += 8;
			drawSpacer(canvas, canvasSize, spacerWidth, { pos.X + offset, pos.Y });
			offset += spacerWidth;
		}

		drawSpacer(canvas, canvasSize, spacerWidth, { pos.X + offset, pos.Y });
		offset += spacerWidth;
	}
	
	SSTV::rgb* decompressFontData(char* compressed, int size) {
		SSTV::rgb* decompressed = new SSTV::rgb[size * 8];
		int decompressionIndex = 0;
		
		for (int p = 0; p < size; p++) {
			char* current = &compressed[p];

			for (int i = 0; i < 8; i++) {
				if (*current & (1 << i)) {
					decompressed[decompressionIndex++] = SSTV::rgb(0xFF, 0xFF, 0xFF);
				}
				else {
					decompressed[decompressionIndex++] = SSTV::rgb(0x00, 0x00, 0x00);
				}
			}
		}
		return decompressed;
	}
	
	void initFont() {
		rgbFont = decompressFontData((char*)tr::compressedFontData, sizeof(tr::compressedFontData));
	}
}

