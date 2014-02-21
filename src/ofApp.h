#pragma once

#include "ofxTeensyOcto.h"
#include "ofMain.h"
#include "ofxOsc.h"
#include "gifLoader.h"

// listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20

#define WIDTH 128
#define HEIGHT 64

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    // LED - Teensy stuff
    //-----------------------------
    ofxTeensyOcto teensy;
    int stripWidth;
    int stripHeight;
    int stripsPerPort;
    int numPorts;
    int brightness;
    float waveSpeed;
    
    // FBO stuff
    //-----------------------------
    ofFbo fbo;
    void updateFbo();
    void drawFbo();
    int fboRotate;
    bool fboFlip;
    ofTexture tex;
    ofPixels guiPixels;
    
    // gui views
    //-----------------------------
    void drawPanels();
    
    // Graphic functions
    //-----------------------------
    void drawDemos();
    void drawDebug();
    void drawRainbowH();
    void drawRainbowV();
    void drawWaves();
    void drawVideos();
    void drawImages();
    int drawModes;
    int demoModes;
    float counterShape;
    int hue;
    
    // Video player
    //-----------------------------
    ofDirectory dirVid;
    vector<ofVideoPlayer> vid;
    int currentVideo;
    bool videoOn;
    
    // Image player
    //-----------------------------
    ofDirectory dirImg;
    vector<ofImage> img;
    int currentImage;
    
    // GIF player
    //-----------------------------
    ofDirectory dirGif;
    gifLoader gif;
    int currentGif;
    
    // OSC data
    //-----------------------------
    ofxOscReceiver receiver;
    void oscReceive();
    float data1;
    float data2;
    string data3;
    int pixelArray[128][64];
		
};
