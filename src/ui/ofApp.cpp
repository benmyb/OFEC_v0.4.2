#include "ofApp.h"

#include "Buffer/OFGlobal.h"
#include "../imp/Test/test.h"
#include "../imp/Test/initialization.h"
#include "Buffer/BufferTSP.h"
#include "Buffer/RenderProCont.h" 

mutex g_mutex;
mutex g_mutexStream;

extern unique_ptr<Scene> msp_buffer;
extern bool g_showPop;
extern bool g_showGOP;
extern bool g_showDomination;
ofApp::ofApp():m_UIMain(nullptr),m_UIAlg(nullptr),m_UIPro(nullptr),m_UIProSet(nullptr),m_UIAlgSet(nullptr),m_UIProAttri(nullptr){

}

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofEnableDepthTest(); //Enable z-buffering
	ofEnableSmoothing();

	m_view.resize(3);
	m_cam.push_back(move(ofEasyCam()));
	m_cam.push_back(move(ofEasyCam()));
	m_cam.push_back(move(ofEasyCam()));
	disableMouse();
	
	setupViewports();

	 ofTrueTypeFont::setGlobalDpi(96);
    // load the font
    m_font.loadFont("sans-serif", 21,true,true);

	//default algorithm and problem
	Global::g_arg[param_algName] = "ALG_FAMF_PSO";
	Global::g_arg[param_proName] = "FUN_Shubert";
	//initialize the computing system 
	Global::g_arg[param_workingDir]=string("../src/imp/");
	Global::registerParamter();
	registerProNdAlg();
	m_pause=false;
	m_read2Draw=false;
	m_mouse=false;
	ofgsetup();

	
	m_uiColor.set(233,52,27);
	setupUIMain();

	m_UIPro.reset(new ofxUISuperCanvas("PROBLEM"));
	m_UIPro->setColorBack(m_uiColor);
	m_UIPro->autoSizeToFitWidgets();
	ofAddListener(m_UIPro->newGUIEvent,this,&ofApp::guiProEvent);
	m_UIPro->setMinified(true);
	m_UIPro->setVisible(false);

	m_UIAlg.reset(new ofxUISuperCanvas("ALGORITHM"));
	m_UIAlg->setColorBack(m_uiColor);
	m_UIAlg->autoSizeToFitWidgets();
	ofAddListener(m_UIAlg->newGUIEvent,this,&ofApp::guiAlgEvent);
	m_UIAlg->setVisible(false);
	m_UIAlg->setMinified(true);

	m_UIFreePeak.reset(new ofxUISuperCanvas("FREE PEAKS"));	
	m_UIFreePeak->setColorBack(m_uiColor);
	setupUIFreePeak();
	m_UIFreePeak->autoSizeToFitWidgets();
	ofAddListener(m_UIFreePeak->newGUIEvent, this, &ofApp::guiFreePeakEvent);	
	m_UIFreePeak->setVisible(false);

	m_UIErrInf.reset(new ofxUISuperCanvas("MESSAGEBOX"));
	m_UIErrInf->setPosition(ofGetWidth() - m_UIErrInf->getGlobalCanvasWidth(), 0);
	m_UIErrInf->setColorBack(ofxUIColor(0, 0, 255));
	m_UIErrInf->addSpacer();
	m_UIErrInf->addLabelButton("OK",false);
	m_UIErrInf->addTextInput("MESSAGE", "")->setAutoClear(false);
	ofAddListener(m_UIErrInf->newGUIEvent, this, &ofApp::guiErrEvent);
	m_UIErrInf->setVisible(false);

}

void ofApp::setupUIMain(){
	
	m_UIMain.reset(new ofxUISuperCanvas("EALIB MAIN MENU"));

	m_UIMain->setColorBack(m_uiColor);
	m_UIMain->addSpacer();
	m_UIMain->addFPSSlider("FPS",1000);
	m_UIMain->addSpacer();
	m_UIMain->addIntSlider("DRAW SPEED(f/s)",0,100,&g_drawSpeed);
	m_UIMain->addSpacer();
	
	m_UIMain->addToggle("ENABLE MOUSE",&m_mouse);
	m_UIMain->addToggle("FULL SCREEN",false);

	m_UIMain->addSpacer();
	ofxUILabelToggle *lt=m_UIMain->addLabelToggle("ALGORITHM",false);
	ofxUIRectangle *rect=lt->getRect();
	m_UIAlgSet.reset(new ofxUIScrollableCanvas(rect->x+m_UIMain->getGlobalCanvasWidth(),rect->y,m_UIMain->getGlobalCanvasWidth()+4,200));
	m_UIAlgSet->setColorBack(m_uiColor);
	m_UIAlgSet->setScrollableDirections(false,true);
	vector<string> name;
	for(auto &i:Global::msm_alg){
		name.push_back(i.first);
	}
	ofxUIRadio *radio= m_UIAlgSet->addRadio("ALGORITHM SET",name,OFX_UI_ORIENTATION_VERTICAL);
	radio->activateToggle(gGetAlgorithmName(Global::msm_alg["ALG_FAMF_PSO"]));
	m_UIAlgSet->setVisible(false);
	m_UIAlgSet->autoSizeToFitWidgets();
	ofAddListener(m_UIAlgSet->newGUIEvent,this,&ofApp::guiAlgSetEvent);
	
	m_UIMain->addSpacer();

	lt=m_UIMain->addLabelToggle("PROBLEM",false);
	rect=lt->getRect();
	m_UIProSet.reset(new ofxUIScrollableCanvas(rect->x+m_UIMain->getGlobalCanvasWidth(),rect->y,m_UIMain->getGlobalCanvasWidth()+4,200));
	m_UIProSet->setColorBack(m_uiColor);
	m_UIProSet->setScrollableDirections(false,true);
	name.clear();
	for(auto &i:Global::msm_alg4pro){
		if(i.first.first=="ALG_FAMF_PSO") name.push_back(i.first.second);
	}
	radio=m_UIProSet->addRadio("PROBLEM SET",name,OFX_UI_ORIENTATION_VERTICAL);
	radio->activateToggle(gGetProblemName(Global::msm_pro["FUN_Shubert"]));
	m_UIProSet->setVisible(false);
	m_UIProSet->autoSizeToFitWidgets();
	ofAddListener(m_UIProSet->newGUIEvent,this,&ofApp::guiProSetEvent);
	
	m_UIMain->addSpacer();
	m_UIMain->addToggle("PAUSE", &m_pause); 
	m_UIMain->addToggle("SHOW POP", &g_showPop);
	m_UIMain->addToggle("SHOW OPT", &g_showGOP);
	m_UIMain->addToggle("SHOW DOMINATION", &g_showDomination);
	m_UIMain->addLabelButton("OK",false);

	m_UIMain->autoSizeToFitWidgets();
	ofAddListener(m_UIMain->newGUIEvent,this,&ofApp::guiEvent);

	//m_UIMain->loadSettings("guiSettings.xml");
}
void ofApp::disableMouse(){
	for(auto &i:m_cam) i.disableMouseInput();
	m_mouse=false;
}
bool  ofApp::isMouseInput(int x,int y,unique_ptr<ofxUISuperCanvas> &canvas){
	ofxUIRectangle *rect=canvas->getRect();
	if(!canvas->isVisible()) return false;
	if(x>=rect->x&&x<=rect->x+rect->width&&y>=rect->y&&y<=rect->y+rect->height) return true;
	else return false;
}
void ofApp::enableMouse(){
	for(auto &i:m_cam) i.enableMouseInput();
	m_mouse=true;
}
void ofApp::guiProSetEvent(ofxUIEventArgs &e){
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 

	if(kind==OFX_UI_WIDGET_TOGGLE){
		cout<<name<<endl;
		Global::g_arg[param_proName]=name;
		cleanTempArg();
		setupUIPro();
		m_UIPro->setPosition(m_UIMain->getGlobalCanvasWidth(),0);
		m_UIProSet->setVisible(false);
		m_UIPro->setVisible(true);
		m_UIPro->setMinified(false);
		cout<<m_UIPro->isMinified()<<endl;
		dynamic_cast<ofxUILabelToggle*>(m_UIMain->getWidget("PROBLEM"))->setValue(false);
	}

}

void ofApp::guiAlgSetEvent(ofxUIEventArgs &e){
	string name = e.widget->getName(); 	
	int kind = e.widget->getKind(); 	

	if(kind==OFX_UI_WIDGET_TOGGLE){
		cout<<name<<endl;
		Global::g_arg[param_algName]=name;
		cleanTempArg();
		setupUIAlg();
		m_UIAlg->setPosition(m_UIMain->getGlobalCanvasWidth(),0);	
		m_UIAlgSet->setVisible(false);
		m_UIAlg->setVisible(true);
		m_UIAlg->setMinified(false);
		dynamic_cast<ofxUILabelToggle*>(m_UIMain->getWidget("ALGORITHM"))->setValue(false);

		// find out problems the algorithm is able to solve
		while(m_UIProSet->getWidgets().size()>1){
			vector<ofxUIWidget*> widgets=m_UIProSet->getWidgets();
			m_UIProSet->removeWidget(widgets[widgets.size()-1]);
		}
		vector<string> vname;
		for(auto &i:Global::msm_alg4pro){
			if(i.first.first==name)			vname.push_back(i.first.second);
		}
		m_UIProSet->addRadio("PROBLEM SET",vname,OFX_UI_ORIENTATION_VERTICAL);
		if(name.find("ALG_MOEAD_")!=string::npos||name.find("ALG_NSGAII")!=string::npos)
			m_UIProSet->addLabel("please select ZDT*");
		m_UIProSet->autoSizeToFitWidgets();
	}
}


void ofApp::guiEvent(ofxUIEventArgs &e){
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 


	if(kind == OFX_UI_WIDGET_TOGGLE){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
		if(name=="ENABLE MOUSE"){
			if(toggle->getValue())	enableMouse(); 
			else   disableMouse();
		}else if(name=="PAUSE"){
			Scene::setPause(toggle->getValue());
		}else if(name=="FULL SCREEN"){
			ofToggleFullscreen();
		}
		
	}else if(kind==OFX_UI_WIDGET_LABELTOGGLE){
		ofxUILabelToggle *lt=(ofxUILabelToggle*) e.widget;
		if(name=="ALGORITHM"){	
			m_UIAlgSet->setVisible(lt->getValue());
		}else if(name=="PROBLEM"){		
			m_UIProSet->setVisible(lt->getValue());
		}
	}else if(kind==OFX_UI_WIDGET_LABELBUTTON){
		ofxUILabelButton *lb=(ofxUILabelButton*) e.widget;
		if(name=="OK"&&lb->getValue()==0){
			//m_UIMain->toggleVisible();
			startAlgorithm();
		}
	}
	else if (kind == OFX_UI_WIDGET_INTSLIDER_H){
		ofxUISlider *lb = (ofxUISlider*)e.widget;
		if (name == "DRAW SPEED(f/s)"){
			ofSetFrameRate(g_drawSpeed);
		}
	}
	
	
}
//--------------------------------------------------------------
void ofApp::update(){
	
}


//--------------------------------------------------------------
void ofApp::draw(){
	if(m_read2Draw&& msp_buffer->isReady2Draw()){
		msp_buffer->drawBuffer();	
	}

	drawViewportOutline();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key){		
		case 'g':case 'G':		// start algorithm 		
			startAlgorithm();
			break;
		case ' ':		//suspend or resume algorithm thread
			m_pause=!m_pause;
			Scene::setPause(m_pause);
			break;
		case 'f': case 'F':
			ofToggleFullscreen();
			break;
		case 't': case 'T':
			m_UIMain->toggleVisible();
			if (m_UIProAttri.get())		m_UIProAttri->setVisible(m_UIMain->isVisible());
			break;
		case 's':case 'S':
			ofSaveScreen("screen.png");
			break;
		case OF_KEY_F1: //save the fitness landscape
			saveScreen("landscape.png", viewMain);
			break;
		case OF_KEY_F2: //save the objective space
			saveScreen("objective.png", viewTop);
			break;
		case OF_KEY_F3: //save the solution space
			saveScreen("solution.png", viewBot);
			break;
		case OF_KEY_ESC:
			terminate();
			break;
	}
}

void ofApp::saveScreen(string file, MyViewport v){
	ofImage img;
	img.allocate(m_view[v].width, m_view[v].height, OF_IMAGE_COLOR);
	img.grabScreen(m_view[v].x, m_view[v].y, m_view[v].width, m_view[v].height);
	img.saveImage(file);
}
void ofApp::terminate(){
	g_algTermination=true;
	m_read2Draw=false;
	g_systemExit=true;
	Scene::setPause(false);
	while(!g_algExited){
		this_thread::sleep_for(chrono::milliseconds(100));
	}

}
void ofApp::exit(){
	msp_buffer.reset();
	m_UIMain.reset();
	m_UIProSet.reset();
	m_UIAlgSet.reset();
	m_UIProAttri.reset();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if(button==0&&(isMouseInput(x,y,m_UIMain)|| isMouseInput(x,y,m_UIPro)|| isMouseInput(x,y,m_UIAlg))){
		disableMouse();
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
		
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	setupViewports();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::setupViewports(){
	//call here whenever we resize the window
	//--
	// Define viewports

	float xOffset = ofGetWidth() / 3;
	float yOffset = ofGetHeight() / 2;

	m_view[viewMain].x = 0;
	m_view[viewMain].y = 0;
	m_view[viewMain].width = xOffset * 2;
	m_view[viewMain].height = ofGetHeight();

	m_view[viewTop].x=2*xOffset;
	m_view[viewTop].y=0;
	m_view[viewTop].width=xOffset;
	m_view[viewTop].height=yOffset;

	m_view[viewBot].x=2*xOffset;
	m_view[viewBot].y=yOffset;
	m_view[viewBot].width=xOffset;
	m_view[viewBot].height=yOffset;

}

//--------------------------------------------------------------
void ofApp::drawViewportOutline(){

	ofPushStyle();
	glDepthFunc(GL_ALWAYS); // draw on top of everything
	// draw outlines on views
	ofSetLineWidth(5);
	ofNoFill();
	ofSetColor(0,0,0);
	//
	ofRect(m_view[viewMain]);
	ofRect(m_view[viewTop]);
	ofRect(m_view[viewBot]);
	// restore the GL depth function
	glDepthFunc(GL_LESS);
	ofPopStyle();
}
vector<ofEasyCam >& ofApp::getCamera(){
	return m_cam;
}

ofLight& ofApp::getLight(){
	return m_light;
}

vector<ofRectangle>& ofApp::getViewport(){
	return m_view;
}
void ofApp::cleanTempArg(){
	if (Global::g_arg.find(param_interTest1) != Global::g_arg.end())	Global::g_arg.erase(Global::g_arg.find(param_interTest1));
	if (Global::g_arg.find(param_interTest2) != Global::g_arg.end())	Global::g_arg.erase(Global::g_arg.find(param_interTest2));
	if (Global::g_arg.find(param_interTest3) != Global::g_arg.end())	Global::g_arg.erase(Global::g_arg.find(param_interTest3));
	if (Global::g_arg.find(param_interTest4) != Global::g_arg.end())	Global::g_arg.erase(Global::g_arg.find(param_interTest4));
}
void ofApp::startAlgorithm(){
	// terminate the running algorithm thread
	g_algTermination=true;
	m_read2Draw=false;
	if(msp_buffer!=nullptr) 	msp_buffer->setReady2Draw(false);
			
	//wait for algorithm exiting
	while(g_algRunning&&!g_algExited){
		this_thread::sleep_for(chrono::milliseconds(100));
	}
	
	// setup algorithm here
	ofgSetGlobalParameters();
	setAddtParameters();
	if(msp_buffer!=nullptr) msp_buffer.reset();
			
	g_algTermination=false;
	Scene::setPause(false);
	Scene::resume();
	while(g_algExited)
		this_thread::sleep_for(chrono::milliseconds(100));
	if(m_UIProAttri!=nullptr) 
	{
		m_UIProAttri.reset();
		m_UIObjSet.reset();
	}
	setupUIProAttri();
	if(Global::msp_global->mp_problem->isProTag(MOP)){
		Global::g_arg[param_numDim]=2;	
		msp_buffer=unique_ptr<Scene>(new BufferContMop(m_view,m_cam,m_light,m_font));
	}else if(Global::msp_global->mp_problem->isProTag(TSP)){
		msp_buffer=unique_ptr<Scene>(new BufferTSP(m_view,m_cam,m_light,m_font,Global::msp_global->mp_problem->getNumDim()));
	}else{
		msp_buffer = unique_ptr<Scene>(new BufferCont(m_view, m_cam, m_light, m_font, new RenderProCont()));
	}
	Scene::setPause(false);
	Scene::resume();
	m_read2Draw=true;
}
void ofApp::guiErrEvent(ofxUIEventArgs &e){
	string name = e.widget->getName();
	int kind = e.widget->getKind();

	if (kind == OFX_UI_WIDGET_LABELBUTTON){
		if (name == "OK"){
			m_UIErrInf->setVisible(false);
		}
	}
}
