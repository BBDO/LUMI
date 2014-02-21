#pragma once

#include "ofMain.h"
#include "ofxGifDecoder.h"
#include "ofxGifFile.h"

class gifLoader
{
public:
    
    // teensy functions
    void setup(int _size);
    void draw(int _selector, int _brightness, float _speed, int _width, int _height);
    
    ofxGifDecoder dcd;
    vector <ofxGifFile> gifs;
    float sizeMult;
    
    float frameScroller;
};
