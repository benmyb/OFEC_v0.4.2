#include "ofApp.h"

void ofApp::guiAlgEvent(ofxUIEventArgs &e){
	string name = e.widget->getName(); 	
	int kind = e.widget->getKind(); 

	if(kind==OFX_UI_WIDGET_INTSLIDER_H){
		if(name=="POPSIZE"){
			Global::g_arg[param_popSize]=dynamic_cast<ofxUIIntSlider*>(e.widget)->getValue();
		}
	}else if(kind==OFX_UI_WIDGET_LABELBUTTON){
		if(name=="OK"){
			updateAlgParam();
			m_UIAlg->setVisible(false);
		}
	}else if(kind==OFX_UI_WIDGET_DOUBLESLIDER_H){
		if(name=="ConvFactor"){
			Global::g_arg[param_convFactor]=dynamic_cast<ofxUIDoubleSlider*>(e.widget)->getValue()/10.;
		}
	}

}

void ofApp::updateAlgParam(){
	string algName=Global::g_arg[param_algName];
	Global::g_arg[param_popSize]=dynamic_cast<ofxUIIntSlider*>(m_UIAlg->getWidget("POPSIZE"))->getValue();
	if (algName == "ALG_NSGAII_SBXRM"&&Global::g_arg[param_popSize] % 2 != 0) Global::g_arg[param_popSize] += 1;
	if(algName.find("ALG_FAMF_")!=string::npos){
		Global::g_arg[param_convFactor]=dynamic_cast<ofxUIDoubleSlider*>(m_UIAlg->getWidget("ConvFactor"))->getValue()/10.;
	}

}
void ofApp::setupUIAlg(){
	//configure your algorithm parameters here
	while(m_UIAlg->getWidgets().size()>1){
		vector<ofxUIWidget*> widgets=m_UIAlg->getWidgets();
		m_UIAlg->removeWidget(widgets[widgets.size()-1]);
	}
	string algName=Global::g_arg[param_algName];
	m_UIAlg->addSpacer();
	m_UIAlg->addLabel(algName);
	m_UIAlg->addSpacer();

	if (Global::g_arg.find(param_popSize) != Global::g_arg.end()) m_UIAlg->addIntSlider("POPSIZE", 21, 800, Global::g_arg[param_popSize]);
	else{
		if (algName.find("ALG_MOEAD_") != string::npos)
			m_UIAlg->addIntSlider("POPSIZE", 21, 800, 21);
		else m_UIAlg->addIntSlider("POPSIZE", 2, 500, 10);
	}

	if(algName.find("ALG_FAMF_")!=string::npos){
		m_UIAlg->addDoubleSlider("ConvFactor",0,1,0.05);
	}
	//if(algName=="ALG_NSGAII_SBXRM")
	//	m_UIAlg->addLabel("popSize must be even");
	m_UIAlg->addSpacer();
	m_UIAlg->addLabelButton("OK",false);
	m_UIAlg->autoSizeToFitWidgets();

}