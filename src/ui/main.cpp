#include "ofMain.h"
#include "ofApp.h"

#include "../imp/Test/run.h"
#include "Buffer/Buffer.h"
#include "Buffer/OFGlobal.h"
#include "../imp/Algorithm/Clustering/FAMFDera.h"
unique_ptr<Scene>  msp_buffer=nullptr;
bool g_systemExit=false;


void drawEntry(){

	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofApp *app=new ofApp();
	ofRunApp(app);
}
void comptEntry(){
	
	Scene::setPause(true);
	Scene::pause();
	g_algExited=true;
	g_algRunning=false;
	while(!g_systemExit){
		g_algRunning=true;
		Run run(0,Global::g_arg);
		g_algExited=false;
		Scene::setPause(true);
		Scene::pause();
		msp_buffer->loadProblem_();
		run.go();
		g_algExited=true;
		g_algRunning=false;
		if(g_systemExit) break;

		FAMFDerating::ms_enableDerating=false;
		Scene::setPause(true); // wait for new algorithm initialization
		Scene::pause();	
	}

}
//========================================================================
int main( ){

	Global::g_arg[param_numRun]=1;
	thread threadDraw(&drawEntry);
	thread threadCompt(&comptEntry);

	threadCompt.join();
	threadDraw.join();

	cout<<"main exiting"<<endl;
}
