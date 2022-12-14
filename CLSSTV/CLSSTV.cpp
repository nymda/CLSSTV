/*
 * This file is part of CLSSTV (https://github.com/nymda/CLSSTV).
 * Copyright (c) 2022 github/nymda
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <Windows.h>
#include <commdlg.h>

#include "SSTV.h"
#include "wav.h"
#include "textRendering.h"
#include "modes.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define VERSION "1.10"

//gets the substring after the last '/' character
const char* getFilenameFromPath(const char* path) {
	const char* filename = path;
	for (int x = strlen(path); x > 0; x--) {
		if (path[x] == '\/') {
			filename = &path[x + 1];
		}
	}
	return filename;
}

//-1: Resize failed, 0: Resize not required, 1: Resize successful
int resizeNN(SSTV::rgb** input, vec2 inputSize, vec2 newSize) {
	//dont need to do anything if its already the right size, return the origional to save memory
	if (inputSize == newSize) { return 0; }
	
	printf_s("[Resizing: %ix%i ==> %ix%i]\n", inputSize.X, inputSize.Y, newSize.X, newSize.Y);
	
	SSTV::rgb* output = new SSTV::rgb[newSize.Y * newSize.X];
	if (!output) { return -1; }

	//calc scale values
	float xScale = (float)newSize.X / (float)inputSize.X;
	float yScale = (float)newSize.Y / (float)inputSize.Y;

	for (int y = 0; y < newSize.Y; y++) {
		for (int x = 0; x < newSize.X; x++) {
			//get the nearest pixel in the input image using the x and y scale values
			int writeIndex = y * newSize.X + x;
			int readIndex = (int)(y / yScale) * inputSize.X + (int)(x / xScale);

			//set the pixel to the closest value, avoid any over/underflows. VS still complains about the possibility.
			if (writeIndex <= (newSize.Y * newSize.X) && readIndex <= (inputSize.X * inputSize.Y) && writeIndex >= 0 && readIndex >= 0) {
				output[writeIndex] = (*input)[readIndex];
			}
		}
	}

	*input = output;	
	return 1;
}

void printHelp() {
	printf_s("[CLSSTV USAGE]\n");
	printf_s("EXAMPLE: CLSSTV.exe -M [MODE] -I [INPUT] -O [OUTPUT] -P [DEVICE]\n\n");
	
	printf_s("Required:\n");
	printf_s(" -M: Encode mode\n");
	printf_s(" -I: Input path  (JPG / PNG / BMP / GIF)\n");
	printf_s("Require either:\n");
	printf_s(" -O: Output path (WAV)\n");
	printf_s(" -P: Play generated audio, use with or instead of -O\n");
	printf_s("Optional:\n");
	printf_s(" -SR: Set the sample rate (8khz default)\n");
	printf_s(" -CS: Callsign to draw on the top right, no spaces\n");
	printf_s("Help:\n");
	printf_s(" -L: List encode modes\n");
	printf_s(" -D: List playback devices\n");
	printf_s(" -H: Show this help text\n");
}

void printEncCodes() {
	printf_s("[CLSSTV MODES]\n");
	printf_s(" %-8s | %-16s | %-9s\n", "CODE", "DESCRIPTION", "IMG SIZE");
	printf_s(" %-8s | %-16s | %-9s\n", "", "", "");
	for (encMode& line : modes) {
		if (line.size == vec2{0, 0}) {
			continue;
		}
		printf_s(" %-8s | %-16s | %i x %i\n", line.code, line.desc, line.size.X, line.size.Y);
	}
}

int main(int argc, char* argv[])
{	
	//print help texts if required
	if (strcmp(argv[1], "-L") == 0) {
		printEncCodes();
		return 0;
	}
	
	if (strcmp(argv[1], "-H") == 0) {
		printHelp();
		return 0;
	}

	if (strcmp(argv[1], "-D") == 0) {
		wav::WASAPIListDevices();
		return 0;
	}
	
	//program info
	printf_s("[CLSSTV R%s 2022]\n", VERSION);
	
	//output file pointer
	FILE* ofptr = 0;

	//init drawing system
	tr::initFont();

	//process other arguments
	encMode* selectedEncMode = 0;
	bool validEncMode = false;
	
	char* inputPath = 0;
	char* outputPath = 0;
	char* callsign = 0;
	
	bool playback = false;
	int playbackDevice = -1;

	bool setSampleRate = false;
	int sampleRate = 8000;
	int standardSampleRates[] = { 8000, 11025, 16000, 22050, 32000, 44100, 48000, 96000 };
	bool usingStandardSampleRate = false;
	
	for (int i = 0; i < argc; i++) {
		//find -I switch and input argument
		if (strcmp(argv[i], "-I") == 0 && i + 1 <= argc && !inputPath) {
			inputPath = argv[i + 1];
		}
		
		//find -O switch and output argument
		if (strcmp(argv[i], "-O") == 0 && i + 1 <= argc && !outputPath) {
			outputPath = argv[i + 1];
		}

		//find -M switch and encode mode
		if (strcmp(argv[i], "-M") == 0 && i + 1 <= argc && !validEncMode) {
			for (encMode& em : modes) {
				if (strcmp(argv[i + 1], em.code) == 0) {
					selectedEncMode = &em;
					validEncMode = true;
				}
			}
		}
		
		//find -P switch and device
		if (strcmp(argv[i], "-P") == 0 && i + 1 <= argc && !playback) {
			playbackDevice = strtol(argv[i + 1], NULL, 10);
			playback = true;
		}

		//find -SR switch and sample rate
		if (strcmp(argv[i], "-SR") == 0 && i + 1 <= argc && !setSampleRate) {		
			sampleRate = strtol(argv[i + 1], NULL, 10);
			
			if (sampleRate <= 0) {
				printf_s("[Invalid sample rate, using default]\n");
				sampleRate = 8000;
			}
			
			for (int sr : standardSampleRates) {
				if (sr == sampleRate) {
					usingStandardSampleRate = true;
				}
			}

			if (!usingStandardSampleRate) {
				printf_s("[Non-standard sample rate set, expect possible issues]\n");
			}
			
			setSampleRate = true;
		}

		//find -CS switch and output argument
		if (strcmp(argv[i], "-CS") == 0 && i + 1 <= argc && !callsign) {
			callsign = argv[i + 1];
		}
	}

	//validation stuff
	if (!inputPath || !(playback || outputPath) || !validEncMode) {
		printHelp();
		return 0;
	}

	//make sure the encode mode is valid
	if (!selectedEncMode) {
		printf_s("[ERR] Invalid encode method, see -M");
		return 0;
	}
	
	//init wav system
	if (!wav::init(sampleRate)) {
		printf_s("[ERR] Could not allocate WAV memory\n");
		return 0;
	}
	
	//begin encode
	printf_s("[Beginning %s generation @ %iHz]\n", selectedEncMode->code, wav::header.sampleRate);

	//read input image
	vec2 imgSize = { 0, 0 };
	int imgChannels = 4;
	SSTV::rgb* rgbBuffer = nullptr;

	//stbi will load most image types, dont need to determin which load function to use anymore
	rgbBuffer = (SSTV::rgb*)stbi_load(inputPath, &imgSize.X, &imgSize.Y, &imgChannels, 3);
	
	if (!rgbBuffer || imgSize.X <= 0 || imgSize.Y <= 0) {
		printf_s("[ERR] Could not read source file\n");
		return 0;
	}
	
	//open output file
	if (outputPath) {
		int openErrNo = fopen_s(&ofptr, outputPath, "wb");
		if (openErrNo != 0) {
			char errBuffer[256] = {};
			strerror_s(errBuffer, openErrNo);
			printf_s("[ERR] Could not open output file (%s)\n", errBuffer);
			return 0;
		}
	}

	//resize if required
	if (resizeNN(&rgbBuffer, imgSize, selectedEncMode->size) < 0) {
		printf_s("[ERR] Error occured during resizing\n");
		return 0;
	}

	//draw overlay	
	if (callsign) {
		tr::bindToCanvas(rgbBuffer, selectedEncMode->size);
		tr::setTextOrigin({ 0, 0 });
		tr::drawString(tr::white, 1, "%s", callsign);
	}

	//add 500ms header
	wav::addTone(0, 500.f);
	
	//add VOX tone
	SSTV::addVoxTone();

	//call actual encode function
	selectedEncMode->ec(rgbBuffer);
	
	//add 500ms footer
	wav::addTone(0, 500.f);
	
	//save file if requested and print info
	if (outputPath) {
		if (wav::save(ofptr) <= 0) {
			char errBuffer[256] = {};
			strerror_s(errBuffer, errno);
			printf_s("[ERR] Issue opening output file (%s)\n", errBuffer);
			return 0;
		}
		printf_s("[Encode complete, wrote %i bytes to %s]\n", wav::header.fileSize, getFilenameFromPath(outputPath));
	}
	else {
		printf_s("[Encode complete, storing %i bytes]\n", wav::header.fileSize);
	}	

	printf_s(" Expected time: %f MS\n", wav::expectedDurationMS);
	printf_s(" Actual time  : %f MS\n", wav::actualDurationMS);
	printf_s(" Added: %i, Skipped: %i\n", wav::balance_AddedSamples, wav::balance_SkippedSamples);

	//playback the file if requested
	bool first = true;
	
	while (playback) {
		if (!first) { //ignore quitting possibility on the first playback loop
			if ((GetConsoleWindow() != GetForegroundWindow())) { continue; } //not focussed, dont do anything
			if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { playback = false; } //escape pressed, quit out of the loop
		}

		if (GetAsyncKeyState('R') & 0x8000 || first) { //initial play, or replay
			first = false;
			wav::beginPlayback(playbackDevice);
			printf_s("\n[End of playback. Press R to reply or ESC to exit.]\n");
		}
	}
}
