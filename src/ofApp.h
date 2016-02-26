#pragma once

#include "ofMain.h"
//#include "ofxWonweiOutline.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxPrint.h"
#include "ofxXmlSettings.h"
#include "ofxPrint.h"

#include "imatrix.h"
#include "ETF.h"
#include "fdog.h"
#include "myvec.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void imageProcess();
//        void drawWithNormals (const ofPolyline& polyline);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    // XML save trace
    ofxXmlSettings XML;
    bool saveContours;
    int lastTagNumber;
    int numContours;
    int numPts;
    
    // ofImage bild;
    ofImage processed;
    ofImage srcImg;
    
    ofxPrint print;
    
    
    imatrix img;
    //imatrix input;
    ETF e;
    unsigned char *bild_pix;
  
    ofxPanel gui;
    ofParameter<int>  black;
    ofParameter<float>  sig1;
    ofParameter<float>  sig2;
    ofParameter<float>  tau;
    ofParameter<float>  thresh;
    
    
    //line/contour stuff
    //    ofxWonweiOutline    outline;
    ofxCvColorImage cvImgColor;
    ofxCvGrayscaleImage cvImgGrayscale;
    ofxCvContourFinder contourFinder;
    vector<ofPolyline> polylines, smoothed;
    int threshold;
    
    


};
