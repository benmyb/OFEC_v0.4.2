#ifndef BUFFERCONT_H
#define BUFFERCONT_H
#include "Buffer.h"
#include "RenderProCont.h"
#include "RenderProContMOP.h"
class BufferCont:public Buffer<CodeVReal>{
protected:

	unique_ptr<RenderProCont> m_proCont;
	ofVbo m_vbo;
	ofShader m_shader;
	ofTexture m_texture;

	list<vector <ofVec3f>> m_mainPoint,m_objPoint,m_solPoint;
	list<vector <ofVec3f>> m_sizes;
	list<vector<ofFloatColor>> m_indiColor;
	list<vector<int>>m_popsId;

	int m_defaultZobj=0;     //the selected obj to draw soluionAdObj
	
public:
	template<typename T>
	BufferCont(vector<ofRectangle> &view, vector<ofEasyCam> &cam, ofLight &light, ofTrueTypeFont  &font, T* pro) :Buffer(view, cam, light, font), \
		m_proCont(pro){
		
		ofBackgroundHex(0xffffff);
		ofSetBackgroundAuto(true);
		m_cam[camMain].setDistance(1000);
		m_cam[camTop].setDistance(500);
		m_cam[camBot].setDistance(500);

		// load the texure
		ofDisableArbTex();
		ofLoadImage(m_texture, "dot.png");
				// load the shader
		#ifdef TARGET_OPENGLES
				m_shader.load("shaders_gles/shader");
		#else
				m_shader.load("shaders/shader");
		#endif
				
	}
	~BufferCont(){}
	void updateFitnessLandsacpe();
	void popupData();
	RenderProCont& getProCont() { return *m_proCont;}
	int getDefautZ() const { return m_defaultZobj; }
	void updateSelObjAdZ(const vector<int> &obj, int defaultZobj);
protected:
	
	void updateBuffer(const vector<Algorithm*> *pop=0);
	void updateBuffer(const vector<vector<Solution<CodeVReal>>*> *pop = 0);
	void drawSolution();
	void drawObjective();
	void drawSolutndObj();
	void drawProblem();
	void loadProblem();
	void updateInids();
	void startMappingThread();
	static int mappingThread(const vector<int> &tsk, BufferCont& buf);
	void drawPoint(ofRectangle &view, ofEasyCam & cam, vector <ofVec3f> &data);
};

#endif