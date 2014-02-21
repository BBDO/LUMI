
#include "gifLoader.h"

//--------------------------------------------------------------
void gifLoader::setup(int _size)
{
    sizeMult = .3f;
    frameScroller = 0.0f;
    
    for (int i = 1; i < _size+1; i++)
    {
        bool success = dcd.decode("gifs/" + ofToString(i) + ".gif");
        if (success) {
            gifs.push_back(dcd.getFile());
        }
    }
}

void gifLoader::draw(int _selector, int _brightness, float _speed, int _width, int _height)
{    
    //for (int i = 0; i < gifs.size(); i++) {
        
        frameScroller+=_speed;
        if (frameScroller > gifs[_selector].getNumFrames()) {
            frameScroller = 0.0;
        }
                
        int gifW = _width;//gifs[i].getWidth();
        int gifH = _height;//gifs[i].getHeight();
    
    ofSetColor(_brightness, _brightness, _brightness);
    gifs[_selector].drawFrame(frameScroller, 0, 0, gifW, gifH);
    //}
}
