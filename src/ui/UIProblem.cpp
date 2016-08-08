#include "ofApp.h"
#include "./Buffer/OFGlobal.h"
#include "Problem/uiFreePeak_D_OnePeak.h"

#include <random>
extern unique_ptr<Scene> msp_buffer;

void ofApp::guiProEvent(ofxUIEventArgs &e){
	string name = e.widget->getName(); 	
	int kind = e.widget->getKind(); 	

	if(Global::g_arg[param_proName]=="DYN_CONT_MovingPeak"||Global::g_arg[param_proName]=="DYN_CONT_RotationDBG"||Global::g_arg[param_proName]=="DYN_CONT_CompositionDBG"){
		if(kind==OFX_UI_WIDGET_SLIDER_H){
			ofxUISlider * w=(ofxUISlider*) e.widget;
			float value=w->getValue();
			if(name=="CHANGHE RATIO"){
				Global::g_arg[param_changeRatio]=value;
			}else if(name=="SHIFT LENGTH"){
				Global::g_arg[param_shiftLength]=value;
			}		
		}else if(kind==OFX_UI_WIDGET_INTSLIDER_H){
			ofxUIIntSlider * w=(ofxUIIntSlider*) e.widget;
			int value=w->getValue();
			if(name=="NUMBER PEAKS"){
				Global::g_arg[param_numPeak]=value;
			}else if(name=="CHANGE TYPE"){
				Global::g_arg[param_changeType]=value;
				if(value==0){
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("NOISE"))->setLabel("NOISE:FALSE");
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("TIME-LINKAGE"))->setLabel("TIME-LINKAGE:FALSE");
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("PEAKS CHANGE"))->setLabel("PEAKS CHANGE:FALSE");
				}else if(value<=3){
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("NOISE"))->setLabel("NOISE:FALSE");
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("TIME-LINKAGE"))->setLabel("TIME-LINKAGE:FALSE");
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("PEAKS CHANGE"))->setLabel("PEAKS CHANGE:TRUE");
				}else if(value<=8){
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("NOISE"))->setLabel("NOISE:TRUE");
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("TIME-LINKAGE"))->setLabel("TIME-LINKAGE:FALSE");
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("PEAKS CHANGE"))->setLabel("PEAKS CHANGE:FALSE");
				}else{
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("NOISE"))->setLabel("NOISE:FALSE");
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("TIME-LINKAGE"))->setLabel("TIME-LINKAGE:TRUE");
					dynamic_cast<ofxUILabel*>(m_UIPro->getWidget("PEAKS CHANGE"))->setLabel("PEAKS CHANGE:FALSE");
				}
			}else if(name=="CHANGHE FREQUENCE"){
				if(kind==OFX_UI_WIDGET_INTSLIDER_H)
				Global::g_arg[param_changeFre]=value;
			}else if(name=="CDBGFID"){
				Global::g_arg[param_comDBGFunID]=value;
			}
		}
		
	}else if(Global::g_arg[param_proName]=="FUN_IBA"){
		if(kind==OFX_UI_WIDGET_INTSLIDER_H){
		ofxUIIntSlider * w=(ofxUIIntSlider*) e.widget;
		Global::g_arg[param_case]=w->getValue();
		}
	}else if(Global::g_arg[param_proName]=="FUN_Keane_Bump"){
		if(kind==OFX_UI_WIDGET_INTSLIDER_H){
		ofxUIIntSlider * w=(ofxUIIntSlider*) e.widget;
		Global::g_arg[param_solutionValidationMode]=w->getValue();
		}
	}else if(gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]).find("DTLZ")!=string::npos){
		if(kind==OFX_UI_WIDGET_INTSLIDER_H){
			ofxUIIntSlider * w=(ofxUIIntSlider*) e.widget;
			int value=w->getValue();
			if (name == "#No OBJ") Global::g_arg[param_numObj] = value;
			else if (name == "k=")	Global::g_arg[param_interTest1] = value;
		}
	}else if(Global::g_arg[param_proName]=="COMB_TSP"){
		if(kind==OFX_UI_WIDGET_LABELTOGGLE)
		{
			ofxUILabelToggle *lt=(ofxUILabelToggle*) e.widget;
			m_UITSPSet->setVisible(lt->getValue());
		}
		else if(kind==OFX_UI_WIDGET_INTSLIDER_H){
			ofxUIIntSlider * w=(ofxUIIntSlider*) e.widget;
			Global::g_arg[param_populationInitialMethod]=w->getValue();
		}
	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]).find("_FreePeak_D_") != string::npos){
		if (kind == OFX_UI_WIDGET_TOGGLE){
			ofxUIToggle *it = (ofxUIToggle*)e.widget;
			bool flag = it->getValue();
			m_UIPro->getWidget("CHANGE RATIO")->setVisible(false);
			if (name == "CHANGE NUM PEAK"){		
				if (flag == true){
					setFreePeak_D_OnePeakVisible(m_UIPro.get(), false);
					dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#PEAKS PER BOX"))->setValue(1);
				}
				else 		setFreePeak_D_OnePeakVisible(m_UIPro.get(), true);			
			}
			else if (name == "DETECTABALE"){
				m_UIPro->getWidget("CHANGE RATIO")->setVisible(true);
			}
		}
		
	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_OnePeak" || gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_C_OnePeak"
		|| gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_R_OnePeak"){
		
		if (kind == OFX_UI_WIDGET_LABELBUTTON){
			ofxUILabelButton *lb = (ofxUILabelButton *)e.widget;
			ofxUIRectangle *rec = lb->getRect();
			bool val = lb->getValue();
			if (name == "SETUP TRAPS"&&!val){
				if (g_fp.numTrap == 0) lb->setLabelVisible(false);
				else {
					g_curType = 2;
					lb->setLabelVisible(true);
					resetGUIFreePeak();
					m_UIFreePeak->setPosition(m_UIMain->getGlobalCanvasWidth() + m_UIPro->getGlobalCanvasWidth(), rec->y);
					m_UIFreePeak->setVisible(true);

				}
			}
			else if (name == "SETUP LOPS"&&!val){
				if (g_fp.numLOP == 0) lb->setLabelVisible(false);
				else {
					g_curType = 1;
					resetGUIFreePeak();
					m_UIFreePeak->setPosition(m_UIMain->getGlobalCanvasWidth() + m_UIPro->getGlobalCanvasWidth(), rec->y);
					lb->setLabelVisible(true);				
					m_UIFreePeak->setVisible(true);
				}	
			}
			else if (name == "SETUP GOPS"&&!val) {
				g_curType = 0; 
				resetGUIFreePeak();
				m_UIFreePeak->setPosition(m_UIMain->getGlobalCanvasWidth() + m_UIPro->getGlobalCanvasWidth(), rec->y);
				m_UIFreePeak->setVisible(true); 
			}			
		}
		else if (kind == OFX_UI_WIDGET_TEXTINPUT){
			if (name == "DATA DIR"){
				ofxUITextInput *ti = (ofxUITextInput *)e.widget;
				Global::g_arg[param_dataDirectory1] = ti->getTextString();
			}
		}
		else if (kind == OFX_UI_WIDGET_RADIO){
			ofxUIRadio *rad = (ofxUIRadio*)e.widget;
			int val = rad->getValue();
			if (name == "LOCATION"){
				if (val == 0) g_fp.center = true;
				else g_fp.center = false;
				if (g_fp.center)	Global::g_arg[param_peakCenter] = 1;
				else Global::g_arg[param_peakCenter] = 2;
			}
			else if (name == "DIVISION MODE"){
				if (val == 0) g_fp.equaldiv = false;
				else g_fp.equaldiv = true;
				if (g_fp.equaldiv) Global::g_arg[param_divisionMode] = 2;
				else Global::g_arg[param_divisionMode] = 1;
			}
		}
		else if (kind == OFX_UI_WIDGET_INTSLIDER_H){
			if (name == "TREE1" || name == "TREE2"){
				g_fp.numBox = g_fp.treeBox[0] + g_fp.treeBox[1];
				if (g_fp.numBox != g_fp.treeBox[0])				g_fp.numTree = 2;
				else g_fp.numTree = 1;
				ofxUIIntSlider* is = (ofxUIIntSlider*)m_UIFreePeak->getWidget("BASIN");
				is->setMax(g_fp.numBox - 1);
			}
		}
	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_M_OnePeak"){
		if (kind == OFX_UI_WIDGET_RADIO){
			ofxUIRadio *rad = (ofxUIRadio*)e.widget;
			int val = rad->getValue();
			if (val == 0){
				 m_UIPro->getWidget("RHO_H")->setVisible(true);
				 m_UIPro->getWidget("RHO_W")->setVisible(false);
			}
			else if (val == 1){
				m_UIPro->getWidget("RHO_H")->setVisible(false);
				m_UIPro->getWidget("RHO_W")->setVisible(true);
				g_fp.numGOP = 1;
			}
		}
	}
	
	if(kind==OFX_UI_WIDGET_LABELBUTTON){
		if(name=="OK"){
			updateProParam();
			m_UIPro->setVisible(false);
		}
	}

}

void ofApp::guiProAttriEvent(ofxUIEventArgs &e){
		string name = e.widget->getName(); 	
		int kind = e.widget->getKind();

		if(kind==OFX_UI_WIDGET_INTSLIDER_H){
			g_selZ=dynamic_cast<ofxUIIntSlider*>(m_UIProAttri->getWidget("OBJ to Pro"))->getValue();
		}else if(kind==OFX_UI_WIDGET_LABELTOGGLE){
			if(g_selObj.size()==0)
			{
				g_selObj.resize(3,-1);
				while(m_UIObjSet->getWidgets().size()>=1){
					vector<ofxUIWidget*> widgets=m_UIObjSet->getWidgets();
					m_UIObjSet->removeWidget(widgets[widgets.size()-1]);
				}
				vector<string> objs(Global::msp_global->mp_problem->getNumObj());
				for(int i=0;i<objs.size();i++)
					objs[i]=i+48;
				m_UIObjSet->addRadio("OBJ SET",objs,OFX_UI_ORIENTATION_VERTICAL);
				m_UIObjSet->autoSizeToFitWidgets();
				m_UIObjSet->setVisible(true);
			}
			else
			{
				while(m_UIObjSet->getWidgets().size()>=1){
					vector<ofxUIWidget*> widgets=m_UIObjSet->getWidgets();
					m_UIObjSet->removeWidget(widgets[widgets.size()-1]);
				}
				vector<string> objs(Global::msp_global->mp_problem->getNumObj());
				for(int i=0;i<objs.size();i++)
					objs[i]=i+48;
				for(int i=0;i<objs.size();i++)
				{
					for(int j=0;j<3;j++)
					{
						if(atoi(objs[i].c_str())==g_selObj[j])
						{
							objs.erase(objs.begin()+i);
							i--;
							break;
						}
					}
				}
				m_UIObjSet->addRadio("OBJ SET",objs,OFX_UI_ORIENTATION_VERTICAL);
				m_UIObjSet->autoSizeToFitWidgets();
				m_UIObjSet->setVisible(true);
			}
		}else if(kind==OFX_UI_WIDGET_LABELBUTTON){
			if (name == "OK"){
				bool update = false;
				if (BufferCont* buf = dynamic_cast<BufferCont*>(msp_buffer.get())){
					if (buf->getDefautZ() != g_selZ)
					{
						cout << "objZ: " << g_selZ << endl;
						update = true;
					}
					{
						unique_lock<mutex> lock(Scene::ms_mutexBuf);
						buf->updateSelObjAdZ(g_selObj, dynamic_cast<ofxUIIntSlider*>(m_UIProAttri->getWidget("OBJ to Pro"))->getValue());
					}
					if (update)
						buf->updateFitnessLandsacpe_();
					g_selObj.resize(0);
				}
			}
			else if(name=="reset")
			{
				if(g_selObj.size()==0)
					g_selObj.resize(3,-1);
				for(int i=0;i<g_selObj.size();i++)
					g_selObj[i]=-1;
				while(m_UIObjSet->getWidgets().size()>=1){
					vector<ofxUIWidget*> widgets=m_UIObjSet->getWidgets();
					m_UIObjSet->removeWidget(widgets[widgets.size()-1]);
				}
				vector<string> objs(Global::msp_global->mp_problem->getNumObj());
				for(int i=0;i<objs.size();i++)
					objs[i]=i+48;
				m_UIObjSet->addRadio("OBJ SET",objs,OFX_UI_ORIENTATION_VERTICAL);
				m_UIObjSet->autoSizeToFitWidgets();
				m_UIObjSet->setVisible(true);
			}
		}
}

void ofApp::guiObjSetEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName(); 
	int kind = e.widget->getKind(); 

	if(kind==OFX_UI_WIDGET_TOGGLE){
		if(dynamic_cast<ofxUILabelToggle*>(m_UIProAttri->getWidget("Fir OBJ to objec"))->getValue()){
			g_selObj[0]=atoi(name.c_str());
			dynamic_cast<ofxUILabelToggle*>(m_UIProAttri->getWidget("Fir OBJ to objec"))->setValue(false);
		}else if(dynamic_cast<ofxUILabelToggle*>(m_UIProAttri->getWidget("Sec OBJ to objec"))->getValue()){
			g_selObj[1]=atoi(name.c_str());
			dynamic_cast<ofxUILabelToggle*>(m_UIProAttri->getWidget("Sec OBJ to objec"))->setValue(false);
		}else if(dynamic_cast<ofxUILabelToggle*>(m_UIProAttri->getWidget("Thd OBJ to objec"))->getValue()){
			g_selObj[2]=atoi(name.c_str());
			dynamic_cast<ofxUILabelToggle*>(m_UIProAttri->getWidget("Thd OBJ to objec"))->setValue(false);
		}
		m_UIObjSet->setVisible(false);
	}
}

void ofApp::setupUIObjSet()
{
	m_UIObjSet.reset(new ofxUIScrollableCanvas(m_UIProAttri->getGlobalCanvasWidth()*1.5+m_UIMain->getGlobalCanvasWidth(),0,m_UIMain->getGlobalCanvasWidth()+4,200));
	m_UIObjSet->setColorBack(m_uiColor);
	m_UIObjSet->setScrollableDirections(false,true);
	vector<string> objs(Global::msp_global->mp_problem->getNumObj());
	for(int i=0;i<objs.size();i++)
		objs[i]=i+48;
	m_UIObjSet->addRadio("OBJ SET",objs,OFX_UI_ORIENTATION_VERTICAL);
	m_UIObjSet->autoSizeToFitWidgets();
	m_UIObjSet->setVisible(false);
	ofAddListener(m_UIObjSet->newGUIEvent,this,&ofApp::guiObjSetEvent);
}

void ofApp::setupUITSP() 
{
	m_UITSPSet.reset(new ofxUIScrollableCanvas(m_UIProSet->getGlobalCanvasWidth()+m_UIMain->getGlobalCanvasWidth(),0,m_UIMain->getGlobalCanvasWidth()+4,200));
	m_UITSPSet->setColorBack(m_uiColor);
	m_UITSPSet->setScrollableDirections(false,true);
	vector<string> fileList= ofgGet_filelist();
	m_UITSPSet->addRadio("TSP SET",fileList,OFX_UI_ORIENTATION_VERTICAL);
	m_UITSPSet->autoSizeToFitWidgets();
	m_UITSPSet->setVisible(false);
	ofAddListener(m_UITSPSet->newGUIEvent,this,&ofApp::guiTSPSetEvent);
}

void ofApp::guiTSPSetEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName(); 	
	int kind = e.widget->getKind();

	if(kind==OFX_UI_WIDGET_TOGGLE){
		cout<<name<<endl;
		Global::g_arg[param_dataFile1]=name;
		string str=Global::g_arg[param_dataFile1];
		int i;
		if((i=str.find(".tsp"))==string::npos) return;
		str.erase(i,4);
		for(i=str.size()-1;i>=0;i--)
		{
			if(str[i]>='0'&&str[i]<='9')
				continue;
			else break;
		}
		string s(str,i+1);
		Global::g_arg[param_numDim]=atoi(s.c_str());
		m_UITSPSet->setVisible(false);
		dynamic_cast<ofxUILabelToggle*>(m_UIPro->getWidget("File Name"))->setValue(false);
	}
}

void ofApp::setupUIPro(){
	//setup your problem parameters here

	while(m_UIPro->getWidgets().size()>1){
		vector<ofxUIWidget*> widgets=m_UIPro->getWidgets();
		m_UIPro->removeWidget(widgets[widgets.size()-1]);
	}
	m_UIPro->addSpacer();
	m_UIPro->addLabel(Global::g_arg[param_proName]);
	m_UIPro->addSpacer();
	if(Global::g_arg[param_proName]=="DYN_CONT_MovingPeak"||Global::g_arg[param_proName]=="DYN_CONT_RotationDBG"||Global::g_arg[param_proName]=="DYN_CONT_CompositionDBG"){
		m_UIPro->addIntSlider("NUMBER PEAKS",1,200,10);
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("CHANGE TYPE",0,DynamicProblem::msc_NumChangeTypes,0);
		m_UIPro->addSpacer();
		Global::g_arg[param_numDim]=2;
		m_UIPro->addSlider("CHANGHE RATIO",0,1,1);
		m_UIPro->addSpacer();
		m_UIPro->addSlider("SHIFT LENGTH",0,10,1);
		m_UIPro->addSpacer();
		//m_UIPro->addTextInput("CHANGHE FREQUENCE","2000");
		m_UIPro->addIntSlider("CHANGHE FREQUENCE",500,10000,2000);
		m_UIPro->addSpacer();
		if(Global::g_arg[param_proName]=="DYN_CONT_CompositionDBG")
		{
			m_UIPro->addIntSlider("CDBGFID",1,5,1);
			m_UIPro->addSpacer();
		}
		if(Global::g_arg[param_algName]=="ALG_DynDE")
			Global::g_arg[param_exlRadius]=0.0;
		ofxUILabel *label=m_UIPro->addLabel("NOISE");
		label->setLabel("NOISE:FALSE");
		label=m_UIPro->addLabel("TIME-LINKAGE");
		label->setLabel("TIME-LINKAGE:FALSE");
		label=m_UIPro->addLabel("PEAKS CHANGE");
		label->setLabel("PEAKS CHANGE:FALSE");
	}else if(gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]).find("FUN_")==0){
		Global::g_arg[param_numDim]=2;
		//if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]).find("FUN_MOP_") == 0) Global::g_arg[param_numDim] = 3;

		if(Global::g_arg[param_proName]=="FUN_IBA")		m_UIPro->addIntSlider("CASE",1,2,1);
		else if(Global::g_arg[param_proName]=="FUN_Keane_Bump")		m_UIPro->addIntSlider("VALI_MODE",0,3,2);
	}else if(Global::g_arg[param_proName]=="COMB_TSP"){
		m_UIPro->addLabelToggle("File Name",false);
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("PIM: ",4,5,4);
		Global::g_arg[param_populationInitialMethod]=4;
		setupUITSP();
	}	
	if(gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]).find("DTLZ")!=string::npos)
	{
		m_UIPro->addIntSlider("#No OBJ",2,15,3);
		m_UIPro->addIntSlider("k=", 1, 10, 5);
		//ofxUILabel *label=m_UIPro->addLabel("OBJ NUM: ");
		//m_UIPro->addLabel("3,5,8,10,15");
	}
	if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_D_OnePeak" || gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_D_R_OnePeak"
		|| gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_D_C_OnePeak"){
		m_UIPro->addIntSlider("#No BOX", 1, 200, 10);
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("CHANGE INTERVAL", 500, 10000, 1000);
		m_UIPro->addSpacer();
		m_UIPro->addSlider("SHIFT SEVERITY", 0.01, 1.0, 0.1);
		m_UIPro->addSpacer();
		m_UIPro->addToggle("NOISE", false);
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("#PEAKS PER BOX", 1, 5, 1);
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("#NUM MAX PEAK", 1, 200, 1);
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("TIME WINDOW", 1, 10, 1);
		m_UIPro->addSpacer();
		m_UIPro->addLabel("FEATURE1");
		m_UIPro->addToggle("CHANGE NUM PEAK",false);
		m_UIPro->addLabel("FEATURE2");
		vector<string> uf2 = {"TIME-LINKAGE","DETECTABALE","DEFAULT1"};
		ofxUIRadio *r=m_UIPro->addRadio("HR1", uf2, OFX_UI_ORIENTATION_VERTICAL);
		r->activateToggle("DEFAULT1");
		m_UIPro->addLabel("FEATURE3");
		vector<string> uf3 = { "BASIN CHANGE", "PREDICTABLE", "DEFAULT2" };
		r=m_UIPro->addRadio("HR2", uf3, OFX_UI_ORIENTATION_VERTICAL);
		r->activateToggle("DEFAULT2");
		m_UIPro->addSlider("CHANGE RATIO", 0.1,1,1);	
	}
	else	if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_OnePeak" ){
		
		m_UIPro->addTextInput("DATA DIR", "Data/GOP/")->setAutoClear(false);
		Global::g_arg[param_dataDirectory1] = "Data/GOP/";
		m_UIPro->addLabel("DATA DIRECTORY", OFX_UI_FONT_SMALL);
		m_UIPro->addSpacer();
		m_UIPro->addSlider("SPACE ALLOCATION", 0., 1., &g_fp.spaceRatio[0]);
		m_UIPro->addIntSlider("TREE1", 1, 20, &g_fp.treeBox[0]);
		m_UIPro->addIntSlider("TREE2", 0, 50, &g_fp.treeBox[1]);
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("NUM GOPS", 1, 20, &g_fp.numGOP);
		m_UIPro->addLabelButton("SETUP GOPS",false);
		
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("NUM LOPS", 0, 20, &g_fp.numLOP);
		ofxUILabelButton *lt= m_UIPro->addLabelButton("SETUP LOPS", false);
		if (g_fp.numLOP == 0) lt->setVisible(false);
		else lt->setVisible(true);

		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("NUM TRAPS", 0, 20, &g_fp.numTrap);
		lt = m_UIPro->addLabelButton("SETUP TRAPS", false);
		if (g_fp.numTrap == 0) lt->setVisible(false);
		else lt->setVisible(true);

		m_UIPro->addSpacer();
		vector<string> vs = { "RANDOM", "EQUAL" };
		ofxUIRadio *r = m_UIPro->addRadio("DIVISION MODE", vs, OFX_UI_ORIENTATION_VERTICAL);
		if (g_fp.equaldiv) 		r->activateToggle("EQUAL");
		else		r->activateToggle("RANDOM");
		m_UIPro->addSpacer();

		if (g_fp.equaldiv) Global::g_arg[param_divisionMode] = 2;
		else Global::g_arg[param_divisionMode] = 1;

		vector<string> vloc = { "CENTER", "RAND" };
		r = m_UIPro->addRadio("LOCATION", vloc, OFX_UI_ORIENTATION_VERTICAL);
		if (g_fp.center)	r->activateToggle("CENTER");
		else r->activateToggle("RAND");
		if (g_fp.center)	Global::g_arg[param_peakCenter] = 1;
		else Global::g_arg[param_peakCenter] = 2;
		m_UIPro->addSpacer();
	}
	else	if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_OnePeak"){
		
		if (Global::g_arg.find(param_peakShape) != Global::g_arg.end()) m_UIPro->addIntSlider("SHAPE", 1, 13, Global::g_arg[param_peakShape]);
		else m_UIPro->addIntSlider("SHAPE", 1, 13, 1);

		if (Global::g_arg.find(param_flagIrregular) != Global::g_arg.end()) m_UIPro->addToggle("IRREGULAR", Global::g_arg[param_flagIrregular]);
		else m_UIPro->addToggle("IRREGULAR", false);

		if (Global::g_arg.find(param_flagAsymmetry) != Global::g_arg.end()) m_UIPro->addToggle("ASYMMETRIC", Global::g_arg[param_flagAsymmetry]);
		else m_UIPro->addToggle("ASYMMETRIC", false);

		if (Global::g_arg.find(param_flagRotation) != Global::g_arg.end()) m_UIPro->addToggle("ROTATION", Global::g_arg[param_flagRotation]);
		else m_UIPro->addToggle("ROTATION", false);

		m_UIPro->addSpacer();		
		vector<string> vs = { "SEPERABLE", "NON-SEPERABLE","DOMINO","PAR-SEPERABLE","FLAT BORDER" };
		ofxUIRadio *r = m_UIPro->addRadio("VARIABLE RELATION", vs, OFX_UI_ORIENTATION_VERTICAL);
		if (Global::g_arg.find(param_variableRelation) != Global::g_arg.end()) 		r->activateToggle(vs[Global::g_arg[param_variableRelation] - 1]);
		else		r->activateToggle("SEPERABLE");
		m_UIPro->addSpacer();
		vs = { "CENTER", "RANDOM","CORNER" };
		r = m_UIPro->addRadio("PEAK LOCATION", vs, OFX_UI_ORIENTATION_VERTICAL);
		if (Global::g_arg.find(param_peakCenter) != Global::g_arg.end()) 		r->activateToggle(vs[Global::g_arg[param_peakCenter]-1]);
		else r->activateToggle("CENTER");

		m_UIPro->addSpacer();
		vs = { "SPHERE@PEAK","SPHERE@BORDER", "SINE"};
		for (int i = 0; i < vs.size(); ++i) {
			m_UIPro->addToggle(vs[i], false);
			if (i == 1) {
				m_UIPro->addBiLabelSlider("Value", "MIN", "MAX", 0., 1., 0.)->setID(10 + i);
			}
			else { 
				m_UIPro->addSlider("Value", 0, 1, 0.5)->setID(10 + i);
			}
		}

	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_M_OnePeak"){

		Global::g_arg[param_dataDirectory1] = "Data/MOP/";

		if (Global::g_arg.find(param_numObj) != Global::g_arg.end()) m_UIPro->addIntSlider("#No OBJ", 1, 5, Global::g_arg[param_numObj]);
		else		m_UIPro->addIntSlider("#No OBJ", 1, 5, 2);
		m_UIPro->addSlider("SPACE ALLOCATION", 0.01, 1., &g_fp.spaceRatio[0]);

		m_UIPro->addIntSlider("#No BOX", 1, 50, &g_fp.numBox);
		m_UIPro->addSpacer();
		m_UIPro->addIntSlider("#No PARETO BOX", 1, 10, &g_fp.numGOP);
		m_UIPro->addSpacer();

		vector<string> vs = { "JUMP", "WEB","COUNTABLE" };
		ofxUIRadio *r = m_UIPro->addRadio("PARETO GENERATION", vs, OFX_UI_ORIENTATION_VERTICAL);
		/*if (Global::g_arg.find(param_case) != Global::g_arg.end()) 		r->activateToggle(vs[Global::g_arg[param_case] - 1]);
		else r->activateToggle("JUMP");*/
		m_UIPro->addSpacer();
		if (Global::g_arg.find(param_radius) != Global::g_arg.end()) m_UIPro->addSlider("RADIUS", 0, 1, (double)Global::g_arg[param_radius]);
		else		m_UIPro->addSlider("RADIUS", 0, 1, 0.1);
		m_UIPro->addSpacer();
		m_UIPro->addSlider("RHO_W", 0, 1, &g_fp.rhow);		
		m_UIPro->addSpacer();
		m_UIPro->addSlider("RHO_H", 0, 1, &g_fp.rhoh);
		m_UIPro->addSpacer();
		
		if (Global::g_arg.find(param_interTest1) != Global::g_arg.end())m_UIPro->addIntSlider("CENTER PEAK", 1, 8, Global::g_arg[param_interTest1]);
		else m_UIPro->addIntSlider("CENTER PEAK", 1, 8, 1);
		m_UIPro->addSpacer();

		if (Global::g_arg.find(param_interTest2) != Global::g_arg.end())m_UIPro->addIntSlider("ONCIRCLE PEAK", 1, 7, Global::g_arg[param_interTest2]);
		else m_UIPro->addIntSlider("ONCIRCLE PEAK", 1, 7, 1);
		m_UIPro->addSpacer();
	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_D_M_OnePeak" ){

		if (Global::g_arg.find(param_numBox) != Global::g_arg.end()) m_UIPro->addIntSlider("#No BOX", 1, 50, Global::g_arg[param_numBox]);
		else m_UIPro->addIntSlider("#No BOX", 1, 50, 2);
		m_UIPro->addSpacer();
		
		if (Global::g_arg.find(param_numObj) != Global::g_arg.end()) m_UIPro->addIntSlider("#No OBJ", 1, 50, Global::g_arg[param_numObj]);
		else		m_UIPro->addIntSlider("#No OBJ", 1, 50, 2);
		m_UIPro->addSpacer();
		if (Global::g_arg.find(param_radius) != Global::g_arg.end()) m_UIPro->addSlider("RADIUS", 0, 1, (double)Global::g_arg[param_radius]);
		else		m_UIPro->addSlider("RADIUS", 0, 1, 0.1);
		m_UIPro->addSpacer();

		if (Global::g_arg.find(param_changeType) != Global::g_arg.end()) m_UIPro->addIntSlider("TYPE", 1, 7, Global::g_arg[param_changeType]);
		else m_UIPro->addIntSlider("TYPE", 1, 7, 1);
		m_UIPro->addSpacer();

		if (Global::g_arg.find(param_shiftLength) != Global::g_arg.end()) m_UIPro->addSlider("SHIFT SEVERITY", 0, 1, (double)Global::g_arg[param_shiftLength]);
		else		m_UIPro->addSlider("SHIFT SEVERITY", 0, 1, 0.1);
		m_UIPro->addSpacer();

		if (Global::g_arg.find(param_jumpHeight) != Global::g_arg.end()) m_UIPro->addSlider("JUMP SEVERITY", 0, 1, (double)Global::g_arg[param_jumpHeight]);
		else		m_UIPro->addSlider("JUMP SEVERITY", 0, 1, 0.1);
		m_UIPro->addSpacer();
		

		if (Global::g_arg.find(param_changeFre) != Global::g_arg.end()) m_UIPro->addIntSlider("CHANGE INTERVAL", 500, 5000, Global::g_arg[param_changeFre]);
		else m_UIPro->addIntSlider("CHANGE INTERVAL", 500, 5000, 1000);
		m_UIPro->addSpacer();


		if (Global::g_arg.find(param_interTest1) != Global::g_arg.end())m_UIPro->addIntSlider("CENTER PEAK", 1, 9, Global::g_arg[param_interTest1]);
		else m_UIPro->addIntSlider("CENTER PEAK", 1, 9, 1);
		m_UIPro->addSpacer();

		if (Global::g_arg.find(param_interTest2) != Global::g_arg.end())m_UIPro->addIntSlider("EDGE PEAK", 1, 7, Global::g_arg[param_interTest2]);
		else m_UIPro->addIntSlider("EDGE PEAK", 1, 7, 1);
		m_UIPro->addSpacer();

	}

	m_UIPro->addSpacer();
	m_UIPro->addLabelButton("OK",false);
	m_UIPro->autoSizeToFitWidgets();
}

void ofApp::updateProParam(){
	if(Global::g_arg[param_proName]=="DYN_CONT_MovingPeak"||Global::g_arg[param_proName]=="DYN_CONT_RotationDBG"||Global::g_arg[param_proName]=="DYN_CONT_CompositionDBG"){
		Global::g_arg[param_changeRatio]=dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("CHANGHE RATIO"))->getValue();
		Global::g_arg[param_shiftLength]=dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("SHIFT LENGTH"))->getValue();
		Global::g_arg[param_numPeak]=dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("NUMBER PEAKS"))->getValue();
		Global::g_arg[param_changeType]=dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("CHANGE TYPE"))->getValue();
		Global::g_arg[param_changeFre]=dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("CHANGHE FREQUENCE"))->getValue();
		if(Global::g_arg[param_proName]=="DYN_CONT_CompositionDBG")
			Global::g_arg[param_comDBGFunID]=dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("CDBGFID"))->getValue();
	}else if(Global::g_arg[param_proName]=="FUN_IBA")		Global::g_arg[param_case]=dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("CASE"))->getValue();
	else if(Global::g_arg[param_proName]=="FUN_Keane_Bump") Global::g_arg[param_solutionValidationMode]=dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("VALI_MODE"))->getValue();
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]).find("DTLZ") != string::npos){
			Global::g_arg[param_numObj] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#No OBJ"))->getValue();
			Global::g_arg[param_interTest1] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("k="))->getValue();
	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_D_OnePeak" ){
		Global::g_arg[param_numDim] = 2;
		Global::g_arg[param_numObj] = 1;
		Global::g_arg[param_numBox] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#No BOX"))->getValue();
		Global::g_arg[param_changeFre] =  dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("CHANGE INTERVAL"))->getValue();
		Global::g_arg[param_shiftLength] = (double) dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("SHIFT SEVERITY"))->getValue();
		Global::g_arg[param_flagNumPeakChange] = dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("CHANGE NUM PEAK"))->getValue();
		Global::g_arg[param_peaksPerBox] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#PEAKS PER BOX"))->getValue();
		Global::g_arg[param_numGOpt] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#NUM MAX PEAK"))->getValue();
		Global::g_arg[param_timeWindow] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("TIME WINDOW"))->getValue();  
		if (Global::g_arg[param_numGOpt] > Global::g_arg[param_numBox])Global::g_arg[param_numGOpt] = Global::g_arg[param_numBox];
		if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("TIME-LINKAGE"))->getValue()){
			Global::g_arg[param_changeType] = 0;
		}else	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("DETECTABALE"))->getValue()){
			Global::g_arg[param_changeType] = 1;
		}else 	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("DEFAULT1"))->getValue()){
			Global::g_arg[param_changeType] = 2;
		}
		if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("BASIN CHANGE"))->getValue()){
			Global::g_arg[param_changeType2] = 0;
		}else if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("PREDICTABLE"))->getValue()){
			Global::g_arg[param_changeType2] = 1;
			Global::g_arg[param_predicFlag] = true;
		}
		else 	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("DEFAULT2"))->getValue()){
			Global::g_arg[param_changeType2] = 2;
			Global::g_arg[param_predicFlag] = false;
		}
		 
		if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("NOISE"))->getValue())		Global::g_arg[param_noiseFlag] = true;
		else Global::g_arg[param_noiseFlag] = false;
		Global::g_arg[param_changeRatio] = dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("CHANGE RATIO"))->getValue();
		
	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_OnePeak"){
		
		if (g_fp.numBox != g_fp.numGOP + g_fp.numLOP + g_fp.numTrap){
			ofxUITextInput* ti = (ofxUITextInput*) m_UIErrInf->getWidget("MESSAGE");
			ti->setTextString("NUMBER¡¡OF PEAKS NOT MATCH");
			m_UIErrInf->setVisible(true);
			return;
		}

		Global::g_arg[param_numObj] = g_fp.numObj= 1;
		int dm = dynamic_cast<ofxUIRadio*>(m_UIPro->getWidget("DIVISION MODE"))->getValue() + 1;
		Global::g_arg[param_divisionMode] = dm;
			
		struct Peak{
			string shape;
			string basin;
			vector<int> transf;
			double height, minHeigh;
			bool noisy;
			vector<pair<bool, vector<double>>> constraint;
		};
		vector<Peak> conf(g_fp.numBox);
		
		vector<pair<int, int>> typeNum(3);
		typeNum[0].first = 2; typeNum[0].second = g_fp.numTrap;
		typeNum[1].first = 0; typeNum[1].second = g_fp.numGOP;
		typeNum[2].first = 1; typeNum[2].second = g_fp.numLOP;
		vector<string> gbox;
		int pidx = 0;
		default_random_engine gen;
		for (int i = 0; i < typeNum.size(); ++i){
			for (int j=0; j < typeNum[i].second; ++pidx,++j){
				if (g_onePeak[typeNum[i].first].basin == -1) conf[pidx].basin = "random";
				else if (g_onePeak[typeNum[i].first].basin == -2) conf[pidx].basin = "largest";
				else if (g_onePeak[typeNum[i].first].basin == -3) conf[pidx].basin = "smallest";
				else {
					stringstream t;
					t << g_onePeak[typeNum[i].first].basin;
					conf[pidx].basin = t.str();
				}
				if (i == 1) gbox.push_back(conf[pidx].basin);
				
				uniform_real_distribution<double> ur(g_onePeak[typeNum[i].first].height.first, g_onePeak[typeNum[i].first].height.second);
				
				conf[pidx].height = ur(gen);
				conf[pidx].minHeigh = g_onePeak[typeNum[i].first].minHeight;
				vector<string> shape;
				for (auto &sh : g_onePeak[typeNum[i].first].shape) shape.push_back(sh);
				uniform_int_distribution<int> ui(0, shape.size()-1);
				conf[pidx].shape = shape[ui(gen)];
				for (auto &tr : g_onePeak[typeNum[i].first].transform){
					if (tr >= 0)conf[pidx].transf.push_back(tr);
				}
				
				conf[pidx].noisy = 0;
				conf[pidx].constraint.push_back(make_pair(0, vector<double>()));
				conf[pidx].constraint.push_back(make_pair(0, vector<double>()));
				conf[pidx].constraint.push_back(make_pair(0, vector<double>()));

			}
		}
		

		stringstream ss;
		ss << "config_peak" << g_fp.numBox << ".conf";		
		string file1 = ss.str();
		Global::g_arg[param_dataFile1] = file1;	
		ss.str("");
		ss << Global::g_arg[param_workingDir] << Global::g_arg[param_dataDirectory1] << file1;
		cout << ss.str() << endl;
		ofstream out(ss.str().c_str());

		out << "#BOX \t OBJ " << endl;
		out << conf.size() / g_fp.numObj << " " << g_fp.numObj << endl;
		out << "#BASIN OF PARATO/GLOBAL OPT." << endl;

		for (auto i = 0; i < gbox.size() - 1; ++i) out << gbox[i] << ":";
		out << gbox.back() << endl;

		out << "#BASIN \t SHAPE \t HEIGHT \t MINHEIGHT \t TRANSFORMATION \t TRANSFORMATION([0-3]:[nor.,rot.,irr.,asy.])\tNOISE\t CONSTRAINTS(use:par)" << endl;
		
		for (int i = 0; i < g_fp.numBox; ++i){				
			out << conf[i].basin << " " << conf[i].shape << " " << conf[i].height << " " << conf[i].minHeigh << " ";
			for (auto j = 0; j < conf[i].transf.size(); ++j) {
				if (j < conf[i].transf.size() - 1) 	out << conf[i].transf[j] << ":";
				else out << conf[i].transf[j] << " ";
			}
			out << conf[i].noisy << " ";
			for (auto j = 0; j < conf[i].constraint.size(); ++j) {
				if (conf[i].constraint[j].first)	out << j + 1 << ":" << conf[i].constraint[j].first << ":";
				else out << j + 1 << ":" << conf[i].constraint[j].first << " ";
				for (auto k = 0; k < conf[i].constraint[j].second.size(); ++k) {
					if (k < conf[i].constraint[j].second.size() - 1) 	out << conf[i].constraint[j].second.at(k) << ":";
					else out << conf[i].constraint[j].second.at(k) << " ";
				}
			}
			out << endl;						
		}

		out << "#END" << endl;
		out.close();
		
		ss.str("");
		ss.clear();

		int numDim = Global::g_arg[param_numDim];
		ss << "location"  << "_dim" << numDim << "_peak" << g_fp.numBox << ".loc";
		Global::g_arg[param_dataFile3] = ss.str();
		ss.str("");
		ss.clear();
		ss << Global::g_arg[param_workingDir] << Global::g_arg[param_dataDirectory1] << Global::g_arg[param_dataFile3];
		
		uniform_real_distribution<double> udis(-100., 100.0);
		out.open(ss.str().c_str());

		out << "#PEAK \t DIM" << endl;
		out << g_fp.numBox << " " << numDim << endl;
		out << "#LOCATION" << endl;
		for (int i = 0; i < g_fp.numBox; ++i){
			for (int j = 0; j < numDim; ++j){
				if (g_fp.center)				out << 0<<" ";
				else out << udis(gen) << " ";;
			}
			out << endl;
		}
		out << "#END" << endl;
		out.close();
		

		ss.str("");
		ss.clear();	
		ss<< "division" << "_tree" << g_fp.numTree << "_node" << g_fp.numBox << ".div";

		Global::g_arg[param_dataFile2] = ss.str();
		
		string path = Global::g_arg[param_workingDir];
		path +=(string) Global::g_arg[param_dataDirectory1];
		path += ss.str();
		
		pair<double, double> range(-100, 100);
		vector<vector<pair<double, double>>> box(g_fp.numTree, vector<pair<double, double>>(numDim, range));
		
		out.open(path);
		if (g_fp.numTree==2)			box[0][0].second = box[1][0].first = -100 + 200 * g_fp.spaceRatio[0];
		out << g_fp.numTree << " " << g_fp.numBox << endl;
		for (int t = 0; t < g_fp.numTree; ++t){
			out << "Tree " << t + 1 << " " << g_fp.treeBox[t]-1 << endl;
			for (int d = 0; d < numDim; ++d) out << "[ " << box[t][d].first << " : " << box[t][d].second << " ] ";
			out << endl;

			if (!g_fp.equaldiv){
				default_random_engine gen;
				uniform_real_distribution<double> udis(0.0, 1.0);
				for (int n = 0; n < g_fp.treeBox[t] - 1; ++n){
					double x;
					for (int d = 0; d < numDim; ++d){
						x = box[t][d].first + (box[t][d].second - box[t][d].first)*udis(gen);
						out << x << " ";
					}
					out << endl;
				}
			}
			else{
				int div = 0;
				vector<pair<double, double>> ran(numDim, pair<double, double>(-100.0, 100.0));
				for (int i = 0; i < g_fp.treeBox[t] - 1; ++i){
					double r = 1. / (g_fp.treeBox[t] - i);
					div = i%numDim;
					for (int j = 0; j < numDim; ++j){
						double x;
						if (j == div){
							x = ran[j].first + (ran[j].second - ran[j].first)*r;
							ran[j].first = x;
						}
						else{
							x = (ran[j].first + ran[j].second) / 2;
						}
						out << x << " ";
					}
					out << endl;
				}
			}
		}
		out << "#END";
		out.close();
		
		Global::g_arg[param_numBox] = g_fp.numBox;
		
	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_OnePeak" ){

		Global::g_arg[param_numDim] = 2;
		Global::g_arg[param_peakShape] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("SHAPE"))->getValue();

		if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("CENTER"))->getValue()){
			Global::g_arg[param_peakCenter] = 1;
		}
		else	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("RANDOM"))->getValue()){
			Global::g_arg[param_peakCenter] = 2;
		}
		else	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("CORNER"))->getValue()) {
			Global::g_arg[param_peakCenter] = 3;
		}
		
		if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("SEPERABLE"))->getValue()){
			Global::g_arg[param_variableRelation] = 1;
		}
		else	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("NON-SEPERABLE"))->getValue()){
			Global::g_arg[param_variableRelation] = 2;
		}
		else	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("DOMINO"))->getValue()){
			Global::g_arg[param_variableRelation] = 3;
		}
		else	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("PAR-SEPERABLE"))->getValue()){
			Global::g_arg[param_variableRelation] = 4;
		}
		else	if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("FLAT BORDER"))->getValue()) {
			Global::g_arg[param_variableRelation] = 5;
		}
		

		if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("IRREGULAR"))->getValue())		Global::g_arg[param_flagIrregular] = true;
		else Global::g_arg[param_flagIrregular] = false;

		if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("ASYMMETRIC"))->getValue())		Global::g_arg[param_flagAsymmetry] = true;
		else Global::g_arg[param_flagAsymmetry] = false;

		if (dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget("ROTATION"))->getValue())		Global::g_arg[param_flagRotation] = true;
		else Global::g_arg[param_flagRotation] = false;

	
		vector<string> vs = { "SPHERE@PEAK","SPHERE@BORDER", "SINE"};;
		string s;
		stringstream ss;
		ss << "FUN_OnePeak" << ".cons";
		s = ss.str();
		s.insert(0, "Problem/FunctionOpt/Data_FPs/GOP/");
		s.insert(0, Global::g_arg[param_workingDir]);

		ofstream out(s);
		out << "#constraint index \tselection mask \t parameter value" << endl;

		for (int i = 0; i < vs.size(); ++i) {
			if (i == 1)out << i + 1 << " " << dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget(vs[i]))->getValue() << " " << dynamic_cast<ofxUIBiLabelSlider*>(m_UIPro->getWidget("Value", 10 + i))->getValue()<<endl;
			else		out << i + 1 << " " << dynamic_cast<ofxUIToggle*>(m_UIPro->getWidget(vs[i]))->getValue() << " " << dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("Value", 10 + i))->getValue()<<endl;
		}
		out << "#END";
		out.close();

	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_M_OnePeak"){

		//Global::g_arg[param_numDim] = 3;
		Global::g_arg[param_numBox] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#No BOX"))->getValue();
		Global::g_arg[param_numObj] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#No OBJ"))->getValue();
		Global::g_arg[param_peaksPerBox] = Global::g_arg[param_numObj];
		Global::g_arg[param_radius] = dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("RADIUS"))->getValue();
		int centerPeak = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("CENTER PEAK"))->getValue();
		int oncirclePeak = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("ONCIRCLE PEAK"))->getValue();
		Global::g_arg[param_case] = dynamic_cast<ofxUIRadio*>(m_UIPro->getWidget("PARETO GENERATION"))->getValue()+1;

		struct Peak{
			string shape;
			string basin;
			vector<int> transf;
			double height, minHeigh;
			bool noisy;
			vector<pair<bool, vector<double>>> constraint;
		};
		
		int numPeak = Global::g_arg[param_numBox] * Global::g_arg[param_numObj];
		int numObj = Global::g_arg[param_numObj];

		vector<Peak> conf(numPeak);

		g_fp.numBox = Global::g_arg[param_numBox];
		char buffer[100];
		int paretoWay = Global::g_arg[param_case];
		Global::g_arg[param_numParetoRegion] = g_fp.numGOP;

		default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
		uniform_real_distribution<double> udis(0.0, 1.0);

		vector<vector<double>> obj(g_fp.numGOP, vector<double>(numObj));
		for (int i = 0; i < g_fp.numGOP; ++i){
			for (int j = 0; j < numObj; ++j){
				if (j  == 0){
					obj[i][j] = 100 - i *20. / g_fp.numGOP;
				}
				else if(j==1) {
					obj[i][j] = 80 + (i+1) *20. / g_fp.numGOP;
				}
				else{
					obj[i][j] = 100;
				}
			}
		}
		if (g_fp.numGOP == 1){
			for (int i = 0; i < g_fp.numBox; ++i){
				for (int oidx = 0; oidx < numObj; oidx++){
					int p = i*numObj + oidx;
					conf[p].basin = itoa(i, buffer, 10);
					if (p%numObj == 0){
						if (p == 0 || paretoWay == 2) conf[p].height = 100;
						else conf[p].height = 80;// 100 * (1 - g_fp.rhoh * udis(gen));
						conf[p].shape = itoa(centerPeak, buffer, 10);
					}
					else{
						conf[p].height = 100;
						conf[p].shape = itoa(oncirclePeak, buffer, 10);
					}
					conf[p].minHeigh = 0;
					conf[p].transf.push_back(0);
					conf[p].noisy = 0;
					conf[p].constraint.push_back(make_pair(0, vector<double>()));
					conf[p].constraint.push_back(make_pair(0, vector<double>()));
					conf[p].constraint.push_back(make_pair(0, vector<double>()));
				}
			}
		}
		else{
			for (int i = 0; i < g_fp.numBox; ++i){
				for (int oidx = 0; oidx < numObj; oidx++){
					int p = i*numObj + oidx;
					conf[p].basin = itoa(i, buffer, 10);
					if (i<g_fp.numGOP){
						conf[p].height = obj[i][oidx];						
					}
					else{
						conf[p].height = 80 * udis(gen);
					}
					if (p%numObj == 0){
						conf[p].shape = itoa(centerPeak, buffer, 10);
					}
					else{
						conf[p].shape = itoa(oncirclePeak, buffer, 10);
					}
					conf[p].minHeigh = 0;
					conf[p].transf.push_back(0);
					conf[p].noisy = 0;
					conf[p].constraint.push_back(make_pair(0, vector<double>()));
					conf[p].constraint.push_back(make_pair(0, vector<double>()));
					conf[p].constraint.push_back(make_pair(0, vector<double>()));
				}
			}
		}
		

		stringstream ss;
		ss << "config_box" << g_fp.numBox << "_obj" << numObj << ".conf";
		string file1 = ss.str();
		Global::g_arg[param_dataFile1] = file1;
		ss.str("");
		ss << Global::g_arg[param_workingDir] << Global::g_arg[param_dataDirectory1] << file1;
		
		ofstream out(ss.str().c_str());
		
		
		

		out << "#BOX \t OBJ " << endl;
		out << conf.size() / numObj << " " << numObj << endl;
		out << "#BASIN OF PARATO/GLOBAL OPT." << endl;
		for (int i = 0; i < g_fp.numGOP - 1; ++i)	out << i << ":";
		out << g_fp.numGOP - 1 << endl;

		out << "#BASIN \t SHAPE \t HEIGHT \t MINHEIGHT \t TRANSFORMATION([0-3]:[nor.,rot.,irr.,asy.]) \t NOISE \t CONSTRAINTS(use:par)" << endl;

		for (int i = 0; i < numPeak; ++i){
			out << conf[i].basin << " " << conf[i].shape << " " << conf[i].height << " " << conf[i].minHeigh << " ";
			for (auto j = 0; j < conf[i].transf.size(); ++j) {
				if (j < conf[i].transf.size() - 1) 	out << conf[i].transf[j] << ":";
				else out << conf[i].transf[j] << " ";
			}
			out << conf[i].noisy << " ";
			for (auto j = 0; j < conf[i].constraint.size(); ++j) {
				if (conf[i].constraint[j].first)	out << j + 1 << ":" << conf[i].constraint[j].first << ":";
				else out << j + 1 << ":" << conf[i].constraint[j].first << " ";
				for (auto k = 0; k < conf[i].constraint[j].second.size(); ++k) {
					if (k < conf[i].constraint[j].second.size() - 1) 	out << conf[i].constraint[j].second.at(k) << ":";
					else out << conf[i].constraint[j].second.at(k) << " ";
				}
			}
			out << endl;
		}

		out << "#END" << endl;
		out.close();
		
		ss.str("");
		ss.clear();
		double radius = Global::g_arg[param_radius];
		int numDim = Global::g_arg[param_numDim];
		vector<vector<double>> peak(numObj, vector<double>(numDim, 0));

		for (int oi = 1; oi < numObj; ++oi){
			MyVector v(numDim);
			
			if (oi <= 2){
				for (int d = 0; d < numDim; ++d){
					if (d == 0){
						if (oi == 1)			v[d] = cos(OFEC_PI / 4);
						else v[d] = -cos(OFEC_PI / 4);
					}
					else if (d == 1)  v[d] = sin(OFEC_PI / 4);
					else v[d] = -1 + 2 * udis(gen);
				}				
			}else 
				v.randOnRadi(radius * 100, udis, gen);

			v.normalize();
			peak[oi] = v.data();
		}

		ss << "location" << "_dim" << numDim << "_box" << g_fp.numBox<<"_obj"<<numObj << ".loc";
		Global::g_arg[param_dataFile3] = ss.str();
		ss.str("");
		ss.clear();
		ss << Global::g_arg[param_workingDir] << Global::g_arg[param_dataDirectory1] << Global::g_arg[param_dataFile3];
		
		out.open(ss.str().c_str());

		out << "#BOX \t OBJ \t DIM" << endl;
		out << g_fp.numBox << " " << numObj << " " << numDim << endl;
		out << "#DIMENSION \t RADIUS" << endl;
	
		for (int pb = 0; pb < g_fp.numBox; ++pb){
			double r=0;
			if (paretoWay == 2) {
				if (pb == 0) r = radius;
				else	r = radius + g_fp.rhow*(1 - radius)*udis(gen);
			}
			for (int p = 0; p < numObj; ++p){	
				MyVector v(peak[p]);
				if (paretoWay == 1){
					v *= 100*radius;
					for (int d = 0; d < numDim; ++d){
						out << v[d] << " ";
					}
					if (p == 0){
						out << radius;						
					}else out << 0;
				}
				else{					
					
					v *= 100*r;					
					for (int d = 0; d < numDim; ++d){
						out << v[d] << " ";
					}
					if (p == 0){
						out << r;
					}
					else out << 0;
				}						
				out << endl;
			}
		}
		out << "#END";
		out.close();
		

		ss.str("");
		ss.clear();
		if (g_fp.spaceRatio[0] == 1.0)g_fp.numTree = 1;
		else	g_fp.numTree = 2;
		ss << "division_dim" <<numDim<< "_tree" << g_fp.numTree << "_node" << g_fp.numBox << ".div";

		Global::g_arg[param_dataFile2] = ss.str();

		string path = Global::g_arg[param_workingDir];
		path += (string)Global::g_arg[param_dataDirectory1];
		path += ss.str();

		pair<double, double> range(-100, 100);
		vector<vector<pair<double, double>>> box(g_fp.numTree, vector<pair<double, double>>(numDim, range));
		
		out.open(path);
		if (g_fp.numTree == 2){ 
			box[0][0].second = box[1][0].first = -100 + 200 * g_fp.spaceRatio[0]; 
			g_fp.treeBox[0] = 1;
			g_fp.treeBox[1] = g_fp.numBox - 1;
		}
		else{
			g_fp.treeBox[0] = g_fp.numBox;
			g_fp.treeBox[1] = 0;
		}
		out << g_fp.numTree << " " << g_fp.numBox << endl;
		
		
		for (int t = 0; t < g_fp.numTree; ++t){			
			out << "Tree " << t + 1 << " " << g_fp.treeBox[t] - 1 << endl;
			for (int d = 0; d < numDim; ++d) out << "[ " << box[t][d].first << " : " << box[t][d].second << " ] ";
			out << endl;
				
			default_random_engine gen;
			uniform_real_distribution<double> udis(0.0, 1.0);
			for (int n = 0; n < g_fp.treeBox[t] - 1; ++n){
				double x;
				for (int d = 0; d < numDim; ++d){
					x = box[t][d].first + (box[t][d].second - box[t][d].first)*udis(gen);
					out << x << " ";
				}
				out << endl;
			}
				
		}
		out << "#END";
		out.close();
	}
	else if (gGetProblemName(Global::msm_pro[Global::g_arg[param_proName]]) == "FUN_FreePeak_D_M_OnePeak"){
		Global::g_arg[param_numBox] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#No BOX"))->getValue();
		Global::g_arg[param_numObj] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("#No OBJ"))->getValue();
		Global::g_arg[param_peaksPerBox] = Global::g_arg[param_numObj];
		Global::g_arg[param_radius] = dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("RADIUS"))->getValue();
		Global::g_arg[param_changeType] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("TYPE"))->getValue();
		Global::g_arg[param_shiftLength] = (double) dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("SHIFT SEVERITY"))->getValue();
		Global::g_arg[param_jumpHeight] = (double) dynamic_cast<ofxUISlider*>(m_UIPro->getWidget("JUMP SEVERITY"))->getValue();
		Global::g_arg[param_changeFre] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("CHANGE INTERVAL"))->getValue();
		Global::g_arg[param_interTest1] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("CENTER PEAK"))->getValue();
		Global::g_arg[param_interTest2] = dynamic_cast<ofxUIIntSlider*>(m_UIPro->getWidget("EDGE PEAK"))->getValue();
	}
}

void ofApp::setupUIProAttri()
{
	if(Global::msp_global->mp_problem->isProTag(MOP))
	{
		m_UIProAttri.reset(new ofxUISuperCanvas("Problem Attribute"));
		m_UIProAttri->setPosition(1.5*m_UIMain->getGlobalCanvasWidth(),0);
		m_UIProAttri->setColorBack(m_uiColor);
		m_UIProAttri->addSpacer();
		m_UIProAttri->addIntSlider("OBJ to Pro",0,Global::msp_global->mp_problem->getNumObj()-1,0);
		m_UIProAttri->addSpacer();
		if(Global::msp_global->mp_problem->getNumObj()>3)
		{
			m_UIProAttri->addLabelToggle("Fir OBJ to objec",false);
			m_UIProAttri->addLabelToggle("Sec OBJ to objec",false);
			m_UIProAttri->addLabelToggle("Thd OBJ to objec",false);
			m_UIProAttri->addSpacer();
			m_UIProAttri->addLabelButton("reset",false);
			setupUIObjSet();
		}
		m_UIProAttri->addLabelButton("OK",false);
		m_UIProAttri->addSpacer();
		m_UIProAttri->autoSizeToFitWidgets();
		m_UIProAttri->setVisible(true);
		m_UIProAttri->setMinified(true);
		ofAddListener(m_UIProAttri->newGUIEvent,this,&ofApp::guiProAttriEvent);
	}
}

void ofApp::setupUIFreePeak(){

	m_UIFreePeak->setPosition(m_UIMain->getGlobalCanvasWidth() + m_UIPro->getGlobalCanvasWidth(), 0);
	m_UIFreePeak->addSpacer();
	vector<pair<string, string>> shape;
	for (auto i = 1; i <= 12; ++i){
		stringstream ss1, ss2;
		if (i != 13){
			ss1 << "SH" << i;
			ss2 << "shape/s" << i << "on.png";
		}
		shape.push_back(move(pair<string, string>(ss1.str(), ss2.str())));
	}
	m_UIFreePeak->setGlobalButtonDimension(24);
	for (auto i = 0; i < shape.size(); ++i){
		m_UIFreePeak->addImageToggle(shape[i].first, shape[i].second, false);
		if (i != 5 && i != 11) m_UIFreePeak->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
		else m_UIFreePeak->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
	}
	
	m_UIFreePeak->setGlobalButtonDimension(16);
	m_UIFreePeak->addLabel("SHAPE");
	m_UIFreePeak->addSpacer();
	m_UIFreePeak->addRangeSlider("HEIGHT", 1, 100, &g_curPeak.height.first, &g_curPeak.height.second);
	m_UIFreePeak->addSlider("MIN HEIGHT", 0, 100, &g_curPeak.minHeight);

	ofxUIIntSlider* it=m_UIFreePeak->addIntSlider("BASIN", -3, g_fp.numBox-1, &g_curPeak.basin);

	m_UIFreePeak->addLabel("s=-3 l=-2 r=-1");
	m_UIFreePeak->addSpacer();
	
	vector<string> vs = { "SEPERABLE", "NON-SEPERABLE", "DOMINO","PAR-SEPERABLE","FLAT BORDER" };
	ofxUIRadio *r = m_UIFreePeak->addRadio("VARIABLE RELATION", vs, OFX_UI_ORIENTATION_VERTICAL);
	if (Global::g_arg.find(param_variableRelation) != Global::g_arg.end()) 		r->activateToggle(vs[Global::g_arg[param_variableRelation] - 1]);
	else		r->activateToggle("SEPERABLE");

	m_UIFreePeak->addSpacer();
	if (Global::g_arg.find(param_flagRotation) != Global::g_arg.end())		m_UIFreePeak->addToggle("ROTATION", Global::g_arg[param_flagRotation]);
	else m_UIFreePeak->addToggle("ROTATION", false);
	
	if (Global::g_arg.find(param_flagIrregular) != Global::g_arg.end())		m_UIFreePeak->addToggle("IRREGULARITY", Global::g_arg[param_flagIrregular]);
	else m_UIFreePeak->addToggle("IRREGULARITY", false);

	if (Global::g_arg.find(param_flagAsymmetry) != Global::g_arg.end())		m_UIFreePeak->addToggle("ASSYMETRY", Global::g_arg[param_flagAsymmetry]);
	else m_UIFreePeak->addToggle("ASSYMETRY", false);


	m_UIFreePeak->addSpacer();
	m_UIFreePeak->addLabelButton("OK", false);
}

void ofApp::guiFreePeakEvent(ofxUIEventArgs &e){
	string name = e.widget->getName();
	int kind = e.widget->getKind();

	if (kind == OFX_UI_WIDGET_LABELBUTTON){
		ofxUILabelButton* lb = (ofxUILabelButton*)e.widget;
		bool val = lb->getValue();
		if (name == "OK"&&val==0){
			g_onePeak[g_curType] = g_curPeak;
			m_UIFreePeak->setVisible(false);
		}
	}
	else if (kind == OFX_UI_WIDGET_RADIO){
		ofxUIRadio *rad = (ofxUIRadio*)e.widget;
		int val = rad->getValue();
		if (name == "VARIABLE RELATION"){
			Global::g_arg[param_variableRelation] = val +1;
			g_curPeak.transform[0] = val;
		}
		
		
	}
	else if (kind == OFX_UI_WIDGET_TOGGLE){
		ofxUIToggle *t = (ofxUIToggle*)e.widget;
		bool val = t->getValue();
		if (name == "ASSYMETRY"){
			Global::g_arg[param_flagAsymmetry] = val;
			g_curPeak.transform[3] = 7;
		}
		else if (name == "IRREGULARITY"){
			Global::g_arg[param_flagIrregular] = val;
			g_curPeak.transform[2] = 6;
		}
		else if (name == "ROTATION"){
			Global::g_arg[param_flagRotation] = val;
			g_curPeak.transform[1] = 5;
		}
	}
	else if (kind == OFX_UI_WIDGET_IMAGETOGGLE){
		ofxUIImageToggle *it = (ofxUIImageToggle*)e.widget;
		bool val = it->getValue();
		if (it->getValue())		g_curPeak.shape.insert(name.substr(2, name.size()));
		else g_curPeak.shape.erase(name.substr(2, name.size()));
	}		
}

void ofApp::resetGUIFreePeak(){
	vector<pair<string, string>> shape;
	for (auto i = 1; i <= 12; ++i){
		stringstream ss1, ss2;	
		ss1 << "SH" << i;
		ss2 << "shape/s" << i << "on.png";	
		shape.push_back(move(pair<string, string>(ss1.str(), ss2.str())));
	}
	for (auto i = 0; i < shape.size(); ++i){
		ofxUIImageToggle *it = (ofxUIImageToggle *)m_UIFreePeak->getWidget(shape[i].first);
		it->setValue(false);
	}
	float lh, uh;
	if (g_curType == 0)	lh=uh = 100;
	else lh = uh = 95;
	ofxUIRangeSlider *rs = (ofxUIRangeSlider *)m_UIFreePeak->getWidget("HEIGHT");
	rs->setValueLow(lh);
	rs->setValueHigh(uh);

	g_curPeak.minHeight = 0;
	g_curPeak.basin = 0;
	g_curPeak.shape.clear();
	for (auto&i : g_curPeak.transform) i = -1;
	g_curPeak.transform[0] = 0;
	ofxUIRadio *r = (ofxUIRadio *)m_UIFreePeak->getWidget("VARIABLE RELATION");
	r->activateToggle("SEPERABLE");

	ofxUIToggle*t = (ofxUIToggle*)m_UIFreePeak->getWidget("ROTATION");
	t->setValue(false);

	t = (ofxUIToggle*)m_UIFreePeak->getWidget("IRREGULARITY");
	t->setValue(false);
	t = (ofxUIToggle*)m_UIFreePeak->getWidget("ASSYMETRY");
	t->setValue(false);
}