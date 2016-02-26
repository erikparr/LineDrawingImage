#include "ofApp.h"
int imgW,imgH;
int numDrawlines;
bool processNewImage;
//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetFrameRate(1);
    imgW = 933;
    imgH = 498;
    //////////////////////
    //////////////PROCESS NEW IMAGE? (DO THIS FIRST) /////////////
    processNewImage = false;
    //////////////////////
    //////////////////////

    if (processNewImage)
        srcImg.loadImage("electron1.jpg"); // input image here
    else
        srcImg.loadImage("screen.png"); // processed line drawing image here
    
//    srcImg.loadImage("pflanz.jpg");
    srcImg.resize(imgW, imgH);
    srcImg.setUseTexture(true);
    srcImg.setImageType(OF_IMAGE_COLOR);
    bild_pix = new unsigned char[(int)srcImg.getWidth()*(int)srcImg.getHeight()*3];
//    w=srcImg.getWidth();
//    h = srcImg.getHeight();
    img.init(imgW,imgH);
    
    numDrawlines = 1000;
    
    
    //setup contour detectn
    cvImgColor.allocate(imgW,imgH);
    cvImgGrayscale.allocate(imgW,imgH);

//
    gui.setup("panel");
    gui.add(black.set( "black", -226, -255, 255));
    gui.add(sig1.set( "sigma 1",1.54, 0.01, 2.0));
    gui.add(sig2.set( "sigma 2",2.5, 0.01, 10.0));
    gui.add(tau.set( "Tau",0.999, 0.8, 1.0));
    gui.add(thresh.set( "threshold",0.76, -1.0, 1.0));
    processed.allocate(imgW, imgH, OF_IMAGE_COLOR);
    gui.loadFromFile("settings.xml");
//
//    if(XML.loadFile("contours.xml"))
//        cout<<"xml loaded"<< endl;

}

//--------------------------------------------------------------
void ofApp::update(){
    polylines.clear();
    smoothed.clear();
    processed.update();
    unsigned char * pixels = srcImg.getPixels();
    for(int x=0; x<imgW; x++) {
        for(int y=0; y<imgH; y++) {
            int p = y*imgW + x;
            img[x][y] = pixels[p*3] - black;
        }
    }
    e.init(imgW, imgH);
    e.set(img);
    e.Smooth(4, 2);
    GetFDoG(img, e, sig1, sig2, tau);
    GrayThresholding(img, thresh);
//
//    // assign bild_pix char array from clamped img array vals
    for(int x=0; x<imgW; x++) {
        for(int y=0; y<imgH; y++) {
            
            int p = y*imgW + x;
            bild_pix[p*3] = CLAMP(img[x][y], 0, 255);
            bild_pix[p*3+1] = CLAMP(img[x][y], 0, 255);
            bild_pix[p*3+2] = CLAMP(img[x][y], 0, 255);
        }
    }
    //set ofImage from bild_pix pixels (this should have hand-drawn image effect)
    processed.setFromPixels(bild_pix, imgW, imgH, OF_IMAGE_COLOR);

    //countour finding
    cvImgColor.setFromPixels(srcImg.getPixels(), imgW, imgH);
    cvImgGrayscale.setFromColorImage(cvImgColor);
    cvImgGrayscale.threshold(threshold, ofGetKeyPressed());
    contourFinder.findContours(cvImgGrayscale, 4, imgW * imgH, numDrawlines, true, false);

    
    for(unsigned int i = 0; i < contourFinder.blobs.size(); i++) {
        ofPolyline cur;
        // add all the current vertices to cur polyline
        cur.addVertices(contourFinder.blobs[i].pts);
        cur.setClosed(true);
        // add the cur polyline to all these vector<ofPolyline>
        polylines.push_back(cur);
        smoothed.push_back(cur.getSmoothed(16));

    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(processNewImage){
    processed.draw(0, 0);
    ofSaveScreen("screen.png");
    ofExit();
    }else{
    if(polylines.size()>0){
        print.begin();
//        cout<<"drawing "<<polylines.size()<<" contours"<<endl;
    for(unsigned int i = 0; i < polylines.size(); i++) {
        ofNoFill();
        ofPolyline polyline = polylines[i];
        ofSetColor(255-2*i, 120+(i*0.3), (-i*0.01)+100);
        ofSetLineWidth(1.0);
        
            for(int j=0; j< (int) polyline.size()-1; j++ ) {
                bool repeatNext = false;
        
                const ofPoint& cur = polyline[j];
                const ofPoint& next = repeatNext ? polyline[0] : polyline[j + 1];
        
                float angle = atan2f(next.y - cur.y, next.x - cur.x) * RAD_TO_DEG;
                float distance = cur.distance(next);
        
//                if(repeatNext) {
//                    ofSetColor(255, 0, 255);
//                }
                ofPushMatrix();
                //        if (i==0) {
                //            ofLine(0, 0, ofGetWidth(), ofGetHeight());
                //
                //        }
                ofTranslate(cur.x, cur.y, 0);
                ofRotate(angle);
                //		ofLine(0, 0, 0, distance);
                ofLine(0, 0, distance, 0);
                ofPopMatrix();
            }

        
//        drawWithNormals(smoothed[i]);
    }
//        ofSetColor(255, 255, 255);
        print.end();

    }
    }
//    gui.draw();
}

//--------------------------------------------------------------
//void drawWithNormals (const ofPolyline& polyline) {
//    for(int i=0; i< (int) polyline.size()-1; i++ ) {
//        bool repeatNext = false;
//        
//        const ofPoint& cur = polyline[i];
//        const ofPoint& next = repeatNext ? polyline[0] : polyline[i + 1];
//        
//        float angle = atan2f(next.y - cur.y, next.x - cur.x) * RAD_TO_DEG;
//        float distance = cur.distance(next);
//        
//        if(repeatNext) {
//            ofSetColor(255, 0, 255);
//        }
//        ofPushMatrix();
//        //        if (i==0) {
//        //            ofLine(0, 0, ofGetWidth(), ofGetHeight());
//        //
//        //        }
//        ofTranslate(cur.x, cur.y, 0);
//        ofRotate(angle);
//        //		ofLine(0, 0, 0, distance);
//        ofLine(0, 0, distance, 0);
//        ofPopMatrix();
//    }
//}
//
//--------------------------------------------------------------x
void ofApp::imageProcess(){
    polylines.clear();
    smoothed.clear();
    unsigned char * pixels = srcImg.getPixels();
//
    // assign "imatrix" from pixel char array
    for(int w=0; w<imgW ; w++) {
        for(int h=0; h<imgH; h++) {
            
            int p = h*imgW + w;
            img[w][h] = pixels[p*3] - black;
        }
    }
//    e.init(vidW, vidH);
//    e.set(img);
//    e.Smooth(4, 2);
//    GetFDoG(img, e, sigma1, sigma2, tau);
//    GrayThresholding(img, thres);
//    
//    // assign bild_pix char array from clamped img array vals
//    for(int w=0; w<vidW; w++) {
//        for(int h=0; h<vidH; h++) {
//            
//            int p = h*vidW + w;
//            bild_pix[p*3] = CLAMP(img[w][h], 0, 255);
//            bild_pix[p*3+1] = CLAMP(img[w][h], 0, 255);
//            bild_pix[p*3+2] = CLAMP(img[w][h], 0, 255);
//        }
//    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
