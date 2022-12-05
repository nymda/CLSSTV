#pragma once
#include "SSTV.h"
#include "wav.h"

/*
    These are kind of a mess as BW8 and BW12 are not very well documented.
    Sources differ on what the timings should be and some report different versions of the 8s mode that use different timings.
    These arent perfect but can be decoded by both MMSSTV and RX-SSTV so id say its a win.
*/

void encodeBW(SSTV::rgb* rgbBuffer, double lineMS) {
    int jpg_width = 160;
    int jpg_height = 120;

    float mspp = lineMS / static_cast<float>(jpg_width);
    for (int y = 0; y < jpg_height; y++) {
        wav::addTone(1200, 7); //Sync pulse, no one is clear on how long this should be
        for (int x = 0; x < jpg_width; x++) {
            SSTV::rgb c = rgbBuffer[(y * jpg_width) + x];
            wav::addTone(1500 + (CFMultiplier * ((c.r + c.g + c.b) / 3)), mspp); //image data, average of RGB
        }
    }
}

void encodeBW8(SSTV::rgb* rgbBuffer) {
    SSTV::addVisCode(0x02);
    encodeBW(rgbBuffer, 60.f);
}

void encodeBW12(SSTV::rgb* rgbBuffer) {
    SSTV::addVisCode(0x03, 1); //VIS is non-standard and must be forced to have a parity of 1
    encodeBW(rgbBuffer, 93.f);
}