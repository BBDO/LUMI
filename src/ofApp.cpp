//
//      Example app w/ OF Teensy/OCTO class
//      by Jason Walters @ BBDO ...
//      Original Processing/JS code by Paul Stoffregen/PJRC.COM
//
//      Last revision by Jason Walters on February 21th, 2014
//      Made with openFrameworks 0.80
//
//--------------------------------------------------------------

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofBackground(0, 0, 0);                      // default background to black / LEDs off
    ofDisableAntiAliasing();                    // we need our graphics sharp for the LEDs
    
    // SYSTEM SETTINGS
    //--------------------------------------
    stripWidth = 64;                            // pixel width of strip
    stripHeight = 2;                            // pixel height of strip
    stripsPerPort = 8;                          // total number of strips per port
    numPorts = 4;                               // total number of teensy ports?
    brightness = 0;                             // LED brightness
    fboRotate = 0;                              // rotate display
    fboFlip = false;                            // invert display
    waveSpeed = 0.05f;                          // animation speed
    drawModes = 0;                              // default is demo mode
    demoModes = 0;                              // default is draw white
    counterShape = 0;                           // init at zero
    hue = 0;                                    // init color at red
    
    // setup our teensys
    teensy.setup(stripWidth, stripHeight, stripsPerPort, numPorts);
    
    // configure out teensy boards
    // (portName, xOffset, yOffset, width%, height%, direction)
    teensy.serialConfigure("tty.usbmodem14761", 0, 50, 100, 25, 0);
    teensy.serialConfigure("tty.usbmodem14791", 0, 25, 100, 25, 0);
    teensy.serialConfigure("tty.usbmodem25", 0, 0, 100, 25, 0);
    teensy.serialConfigure("tty.usbmodem14751", 0, 75, 100, 25, 0);
    
    // allocate our pixels, fbo, and texture
    teensy.pixels1.allocate(stripWidth, stripHeight*stripsPerPort*numPorts, OF_PIXELS_RGB);
    fbo.allocate(stripWidth, stripHeight*stripsPerPort*numPorts, GL_RGB);
    tex.allocate(stripWidth, stripHeight*stripsPerPort*numPorts, GL_RGB);
    
    // osc -- listen on the given port
    //receiver.setup(PORT);
    
    // GIFS - dynamic loading
    //--------------------------------------
    dirGif.listDir("gifs/");
    gif.setup(dirGif.size());
    currentGif = 0;
    
    // VIDEOS - dynamic loading
    //--------------------------------------
    dirVid.listDir("videos/");
    dirVid.sort();
    //allocate the vector to have as many ofVidePlayer as files
	if( dirVid.size() ){
		vid.assign(dirVid.size(), ofVideoPlayer());
	}
    videoOn = false;
    currentVideo = 0;
    
    // IMAGES - dynamic loading
    //--------------------------------------
    dirImg.listDir("images/");
    dirImg.sort();
    //allocate the vector to have as many ofImages as files
	if( dirImg.size() ){
		img.assign(dirImg.size(), ofImage());
	}
    currentImage = 0;
}

//--------------------------------------------------------------
void ofApp::oscReceive()
{
    // check for waiting messages
	while(receiver.hasWaitingMessages())
    {
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);

        // pump our 2d array full of osc data
        pixelArray[(int)m.getArgAsFloat(1)][(int)m.getArgAsFloat(2)] = ofHexToInt(m.getArgAsString(0));        
    }
}

//--------------------------------------------------------------
void ofApp::update()
{
    //oscReceive();                             // receive our Osc messages
    teensy.update();                            // update our serial to teensy stuff
    updateFbo();                                // update our Fbo functions
    
    if (dirVid.size() > 0)
    {
        if (videoOn) vid[currentVideo].update();                  // update video when enabled
    }
}

//--------------------------------------------------------------
void ofApp::updateFbo()
{    
    fbo.begin();                                // begins the fbo
    ofClear(0,0,0);                             // refreshes fbo, removes artifacts
    
    ofSetColor(brightness);                     // brightness is controllable, don't burn your eyes out!
    tex.draw(0, 0);                             // draw our texture data into the FBO
    
    fbo.end();                                  // closes the fbo
    
    fbo.readToPixels(teensy.pixels1);           // send fbo pixels to teensy
}

// ADD ALL GRAPHICS DRAW MODES HERE -- INTO THE FBO !!!
//--------------------------------------------------------------
void ofApp::drawFbo()
{
    ofPushMatrix();
    
    if (!fboFlip)
    {
        // defaul position for our system.  x,y home (0,0) is top left corner.
        ofTranslate(stripWidth, stripHeight*stripsPerPort*numPorts);
        ofRotate(180);
        ofRotateZ(270);
        ofRotateY(180);
    }
    else
    {
        // inverted
        ofTranslate(stripWidth, 0);
        ofRotate(90);
    }
    
    ofPushMatrix();
    
    switch (fboRotate)
    {
        case 0:
            ofTranslate(0, 0);
            ofRotate(0);
            break;
            
        case 1:
            if (!fboFlip) {
                ofTranslate(64, 0);
                ofRotate(90);
                
            }
            else {
                ofTranslate(0, 64);
                ofRotate(-90);
                
            }
            break;
            
        case 2:
            ofTranslate(64, 64);
            ofRotate(180);
            break;
            
        case 3:
            if (!fboFlip) {
                ofTranslate(0, 64);
                ofRotate(270);
                
            }
            else {
                ofTranslate(64, 0);
                ofRotate(-270);
                
            }
            break;
            
        default:
            break;
    }
    
    // graphics draw mode
    switch (drawModes)
    {
        case 0:
            drawDemos();
            break;
        case 1:
            drawVideos();
            break;
        case 2:
            drawImages();
            break;
        case 3:
            gif.draw(currentGif, 255, waveSpeed, stripWidth, stripHeight*stripsPerPort*numPorts);
            break;
            
        default:
            break;
    }
    
    /*
     // displaying pixels from sermad's nodeJS
     for (int y = 0; y < 64; y++)
     {
         for (int x = 0; x < 128; x++)
         {
             ofSetHexColor(pixelArray[x][y]);
             ofRect(x, y, 1, 1);
             //cout << x << " " << y << " " << pixelArray[x][y] << endl;
         }
     }
     */
    
    ofPopMatrix();
    ofPopMatrix();
    
    tex.loadScreenData(699, 480, stripWidth, stripHeight*stripsPerPort*numPorts);
    tex.readToPixels(guiPixels);
}

// ONLY GUI DRAWS GO HERE
//--------------------------------------------------------------
void ofApp::draw()
{
    // draw our graphics on-screen so we can copy to texture.
    ofPushMatrix();
    ofTranslate(699, 480);
    drawFbo();
    ofPopMatrix();

    drawPanels();
    
    // brightness draw
    ofSetColor(255, 255, 255);
    ofDrawBitmapString("// PROJECT LUMI //", ofGetWidth()-250, 20);
    ofDrawBitmapString("Rotate (r) == " + ofToString(fboRotate), ofGetWidth()-250, 40);
    ofDrawBitmapString("Flip (f) == " + ofToString(fboFlip), ofGetWidth()-250, 60);
    ofDrawBitmapString("Bright (u/d) == " + ofToString(brightness), ofGetWidth()-250, 80);
    ofDrawBitmapString("Speed (l/r) == " + ofToString(waveSpeed), ofGetWidth()-250, 100);
    ofDrawBitmapString("Videos # == " + ofToString(dirVid.size()), ofGetWidth()-250, 120);
    ofDrawBitmapString("Images # == " + ofToString(dirImg.size()), ofGetWidth()-250, 140);
    ofDrawBitmapString("GIFs # == " + ofToString(dirGif.size()), ofGetWidth()-250, 160);
}

// PANEL VIEW GUI
//--------------------------------------------------------------
void ofApp::drawPanels()
{
//    /*
    ofPushMatrix();
    ofTranslate(319, 520, 0);
    ofScale(5, 5);
    ofRotate(90);
    ofRotateZ(180);
    ofRotateX(180);
    drawFbo();
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(638, 520, 0);
    ofScale(5, 5);
    ofRotate(90);
    ofRotateZ(180);
    ofRotateX(180);
    drawFbo();
    ofPopMatrix();
//     */
    
    ofColor colors;
    ofPushMatrix();
    ofTranslate(1, 190, 0);
    ofRotate(90);
    ofRotateZ(180);
    ofSetRectMode(OF_RECTMODE_CENTER);
    for (int y = 0; y < stripHeight*stripsPerPort*numPorts; y++)
    {
        for (int x = 0; x < stripWidth; x++)
        {
            ofPushMatrix();
            colors = guiPixels.getColor(x, y);//teensy.pixels1.getColor(x, y);
            ofSetColor(colors);
            ofTranslate(x*2, y*2 + (y/16*4)); //sections in groups
            ofRect(x, y, 2, 2);
            ofPopMatrix();
        }
    }
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofPopMatrix();
}

//--------------------------------------------------------------
// DEMO DRAW FUNCTIONS BELOW !!! //
//--------------------------------------------------------------
void ofApp::drawDemos()
{
    switch (demoModes) {
        case 0:
            drawDebug();
            break;
        case 1:
            drawRainbowH();
            break;
        case 2:
            drawRainbowV();
            break;
        case 3:
            drawWaves();
            break;
            
        default:
            break;
    }
}

// debugger
//--------------------------------------------------------------
void ofApp::drawDebug()
{
    // white debug...
    for (int i = 0; i < (stripHeight*stripsPerPort*numPorts); i++)
    {
        ofColor color;
        ofSetColor(ofColor::fromHsb(i*10, 0, 255));
        ofRect(0, i, stripWidth, 1);
    }
}

// rainbow - horizontal
//--------------------------------------------------------------
void ofApp::drawRainbowH()
{
    // vertical strips of rainbow goodness
    for (int i = 0; i < stripWidth; i++)
    {
        int huemap = ofMap(i, 0, stripWidth-1, 0, 255);
        ofSetColor(ofColor::fromHsb(huemap, 255, 255));
        ofRect(i, 0, 1, (stripHeight*stripsPerPort*numPorts));
    }
}

// rainbow - vertical
//--------------------------------------------------------------
void ofApp::drawRainbowV()
{
    // horizontal strips of rainbow goodness
    for (int i = 0; i < (stripHeight*stripsPerPort*numPorts); i++)
    {
        int huemap = ofMap(i, 0, (stripHeight*stripsPerPort*numPorts)-1, 0, 255);
        ofSetColor(ofColor::fromHsb(huemap, 255, 255));
        ofRect(0, i, stripWidth, 1);
    }
}

// wave animation
//--------------------------------------------------------------
void ofApp::drawWaves()
{
    // sin wave
    counterShape = counterShape + waveSpeed;
    
    // color scroller
    hue++;
    if (hue > 255) hue = 0;
    
    // back layer
    float k = 0.0;
    for(int i = 0; i < stripWidth; i+=5)
    {
        ofSetColor(ofColor::fromHsb(hue, 255, 255));
        ofRect(i, (stripHeight*stripsPerPort*numPorts), 5, -(stripHeight*stripsPerPort*numPorts)/4 * (sin(counterShape-k)+1.0) - (stripHeight*stripsPerPort*numPorts)/4);
        k+=0.5;
    }
    
    // front layer
    float kk = 0.0;
    for(int i = 0; i < stripWidth; i+=5)
    {
        ofSetColor(ofColor::fromHsb(hue, 255, 255*0.25));
        ofRect(i, (stripHeight*stripsPerPort*numPorts)+2, 5, -(stripHeight*stripsPerPort*numPorts)/4 * (sin(counterShape-kk)+1.0) - (stripHeight*stripsPerPort*numPorts)/4);
        kk+=0.5;
    }
}

//--------------------------------------------------------------
void ofApp::drawVideos()
{
    //Play videos
    ofSetColor(255);
    if (dirVid.size() > 0){
        vid[currentVideo].draw(0, 0, stripWidth, stripHeight*stripsPerPort*numPorts);
    }
}

//--------------------------------------------------------------
void ofApp::drawImages()
{
    if (dirImg.size() > 0) {
        ofSetColor(255);
        img[currentImage].draw(0, 0);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch (key)
    {
        //-----------------------------------------------
        case OF_KEY_UP:
            brightness += 2;
            if (brightness > 255) brightness = 255;
            break;
            
        case OF_KEY_DOWN:
            brightness -= 2;
            if (brightness < 0) brightness = 0;
            break;
            
        case OF_KEY_RIGHT:
            waveSpeed += 0.01f;
            if (waveSpeed > 0.99f) waveSpeed = 0.99f;
            break;
            
        case OF_KEY_LEFT:
            waveSpeed -= 0.01f;
            if (waveSpeed < 0.05f) waveSpeed = 0.05f;
            break;
            
        case 'r':
            fboRotate++;                            // rotates 90 degrees
            if (fboRotate > 3) {
                fboRotate = 0;
            }
            break;
            
        case 'f':                                   // flips the screen
            fboFlip = !fboFlip;
            break;
            
        
        //-----------------------------------------------
        case 'v':
            drawModes = 1;                          // video mode
            if (!videoOn) videoOn = true;           // enables video
            if (vid[currentVideo].isLoaded() == false) {
                vid[currentVideo].loadMovie(dirVid.getPath(currentVideo));
                vid[currentVideo].play();           // plays the video
            }
            else {
                if (vid[currentVideo].isPlaying()) {
                    vid[currentVideo].stop();       // stops/pauses the video
                }
                else {
                    vid[currentVideo].play();       // plays the video
                }
            }
            break;
        
        //-----------------------------------------------
        case 'i':
            drawModes = 2;                          // image mode
            videoOn = false;                        // disables video
            if (vid[currentVideo].isPlaying()) vid[currentVideo].stop();        // stops/pauses the video
            vid[currentVideo].setFrame(0);          // restart video at first frame
            img[currentImage].loadImage(dirImg.getPath(currentImage));
            break;
        
        //-----------------------------------------------
        case 'g':
            drawModes = 3;                          // gif mode
            videoOn = false;                        // disables video
            if (vid[currentVideo].isPlaying()) vid[currentVideo].stop(); // stops/pauses the video
            vid[currentVideo].setFrame(0);          // restart video at first frame
            waveSpeed = 0.35f;                      // increase speed for gifs
            break;
            
        case '=':
            if (drawModes == 1) {
                vid[currentVideo].stop();
            }
            
            if (drawModes == 2) {
                currentImage++;
                if (currentImage > dirImg.size()-1) currentImage = 0;
                img[currentImage].loadImage(dirImg.getPath(currentImage));
            }
            
            if (drawModes == 3) {
                currentGif++;
                if (currentGif > dirGif.size()-1) currentGif = 0;
                waveSpeed = 0.35f;                  // increase speed for gifs
            }
            break;
            
        case '-':
            if (drawModes == 1) {
                vid[currentVideo].stop();
            }
            
            if (drawModes == 2) {
                currentImage--;
                if (currentImage < 0) currentImage = dirImg.size()-1;
                img[currentImage].loadImage(dirImg.getPath(currentImage));
            }
            
            if (drawModes == 3) {
                currentGif--;
                if (currentGif < 0) currentGif = dirGif.size()-1;
                waveSpeed = 0.35f;                  // increase speed for gifs
            }
            break;
            
        //-----------------------------------------------
        case 'd':
            videoOn = false;                        // disables video
            if (vid[currentVideo].isPlaying()) vid[currentVideo].stop();  // stops/pauses the video
            vid[currentVideo].setFrame(0);                        // restart video at first frame
            
            demoModes++;
            if (drawModes != 0) drawModes = 0;      // switch the draw mode to display demo mode.
            if (demoModes > 3) demoModes = 0;       // tap through the demo modes on each press.
            if (demoModes == 3) waveSpeed = 0.05f;  // if wave animation, drop speed down
            break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    switch (key) {
        case '=':
            if (drawModes == 1)
            {
                currentVideo++;
                if (currentVideo > dirVid.size()-1) currentVideo = 0;
                vid[currentVideo].loadMovie(dirVid.getPath(currentVideo));
                vid[currentVideo].play();
            }                       // restart video at first frame
            break;
            
        case '-':
            if (drawModes == 1) {
                currentVideo--;
                if (currentVideo < 0) currentVideo = dirVid.size()-1;
                vid[currentVideo].loadMovie(dirVid.getPath(currentVideo));
                vid[currentVideo].play();
            }
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
