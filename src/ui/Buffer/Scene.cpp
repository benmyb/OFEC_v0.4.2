#include "Scene.h"
#include "../../imp/Utility/definition.h"
condition_variable Scene::ms_condPause;
bool Scene::ms_pause;
mutex  Scene::ms_mutexBuf;

Scene::Scene(vector<ofRectangle> &view, vector<ofEasyCam> &cam, ofLight &light, ofTrueTypeFont  &font) :m_view(view), m_cam(cam), m_light(light), m_font(font){

}
void Scene::pause(){
	{
		unique_lock<mutex> lock(ms_mutexBuf);
		while (ms_pause){
			//cout<<"sleeping ..."<<endl;
			ms_condPause.wait_for(lock, chrono::milliseconds(100));
			//ms_condPause.wait(lock);
			//cout<<"awaking ..."<<endl;
		}
	}
}
void Scene::resume(){
	ms_condPause.notify_one();
}

void Scene::setPause(bool flag){
	ms_pause = flag;
}