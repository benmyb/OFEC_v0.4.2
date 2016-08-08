#ifndef SCENE_H
#define SCENE_H

#include "ofMain.h"
#include "../../imp/Algorithm/Population.h"

class Scene{
protected:
	condition_variable m_condSolutBufFull, m_condSolutBufEmpty;
	condition_variable m_condProBufFull, m_condProBufEmpty;
	// reference to ofApp
	vector<ofRectangle> &m_view;
	vector<ofEasyCam> &m_cam;
	ofLight &m_light;
	ofTrueTypeFont  &m_font;
public:
	static condition_variable ms_condPause;
	static bool ms_pause;
	static mutex ms_mutexBuf;
	static void pause();
	static void resume();
	static void setPause(bool flag);
public:
	Scene(vector<ofRectangle> &view, vector<ofEasyCam> &cam, ofLight &light, ofTrueTypeFont  &font);
	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	virtual ~Scene(){}
	
	virtual void updateBuffer_(const vector<Algorithm*> *pop = 0) {};
	virtual void updateBuffer_(const vector< vector<Solution<CodeVReal>> * > *pop = 0) {};
	virtual void updateBuffer_(const vector< vector<Solution<CodeVInt>> * > *pop = 0) {};
	virtual void loadProblem_()=0;
	virtual void updateFitnessLandsacpe_()=0;
	virtual bool isReady2Draw()=0;
	virtual void drawBuffer()=0;
	virtual void setReady2Draw(bool)=0;
};

#endif