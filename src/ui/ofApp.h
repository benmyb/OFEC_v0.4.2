#pragma once

#include "ofMain.h"
#include "Buffer/BufferCont.h"
#include "Buffer/BufferContMop.h"
#include "ofxUI.h"

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
	void exit();
public:
	vector<ofEasyCam>& getCamera();
	ofLight& getLight();
	vector<ofRectangle>& getViewport();
	void terminate();
	ofApp();
	void saveScreen(string file, MyViewport v);
private:
	void setupUIMain();
	void guiEvent(ofxUIEventArgs &e);
	void guiAlgSetEvent(ofxUIEventArgs &e);
	void guiProSetEvent(ofxUIEventArgs &e);
	void guiProEvent(ofxUIEventArgs &e);
	void guiAlgEvent(ofxUIEventArgs &e);
	void guiProAttriEvent(ofxUIEventArgs &e);
	void guiTSPSetEvent(ofxUIEventArgs &e);
	void guiObjSetEvent(ofxUIEventArgs &e);
	void guiErrEvent(ofxUIEventArgs &e);
	void setupViewports();
	void drawViewportOutline();
	void disableMouse();
	void enableMouse();
	void startAlgorithm();
	void setupUIAlg();
	void setupUIPro();
	void setupUIProAttri();
	void setupUITSP();
	void setupUIObjSet();
	void updateAlgParam();
	void updateProParam();
	bool isMouseInput(int x,int y,unique_ptr<ofxUISuperCanvas>&);
	void cleanTempArg();
	void guiFreePeakEvent(ofxUIEventArgs &e);
	void setupUIFreePeak();
	void resetGUIFreePeak();
private:
	vector<ofRectangle> m_view;
	vector<ofEasyCam> m_cam;
	ofLight m_light;
	ofTrueTypeFont  m_font;
	bool m_pause,m_read2Draw,m_mouse;

	unique_ptr<ofxUISuperCanvas> m_UIMain,m_UIAlg,m_UIPro;
	unique_ptr<ofxUIScrollableCanvas> m_UIProSet, m_UIAlgSet, m_UITSPSet, m_UIObjSet;
	unique_ptr<ofxUISuperCanvas> m_UIProAttri,m_UIFreePeak,m_UIErrInf;
	ofxUIColor m_uiColor;
	float m_frameRate;

};
