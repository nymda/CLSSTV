#pragma once
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

//used in the sum (1500 + X * CFMultiplier) to convert a byte to the approximate SSTV complient colour / luminocity frequency
const double CFMultiplier = 3.1372549;

struct vec2 {
    int X;
    int Y;

    bool operator == (const vec2& rhs)
    {
        if (X == rhs.X && Y == rhs.Y) {
            return true;
        }
        return false;
    }

    bool operator != (const vec2& rhs)
    {
        return !(*this == rhs);
    }
};

namespace SSTV {
    int clampUC(int input);

    struct rgb {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        
        bool operator == (const rgb& rhs)
        {
            if (r == rhs.r && g == rhs.g && b == rhs.b) {
                return true;
            }
            return false;
        }

        bool operator != (const rgb& rhs)
        {
            return !(*this == rhs);
        }
        
        rgb(unsigned char R, unsigned char G, unsigned char B) {
            r = R;
            g = G;
            b = B;
        }

        rgb() {
            r = 0;
            g = 0;
            b = 0;
        }
    };

    struct yuv {
        unsigned char y;
        unsigned char u;
        unsigned char v;

        yuv(rgb rgb) {
            y = clampUC(((66 * (rgb.r) + 129 * (rgb.g) + 25 * (rgb.b) + 128) >> 8) + 16);
            u = clampUC(((-38 * (rgb.r) - 74 * (rgb.g) + 112 * (rgb.b) + 128) >> 8) + 128);
            v = clampUC(((112 * (rgb.r) - 94 * (rgb.g) - 18 * (rgb.b) + 128) >> 8) + 128);
        }
    };

    
    void addVoxTone();

    //adds the first 7 bytes and uses 8 for parity
    void addVisCode(char visCode);

    //adds all 8 bytes of the input, without calculating the parity bit
    void addVisCodeManual(char visCode);
}

