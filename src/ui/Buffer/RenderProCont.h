#ifndef RENDER_PROBLEMCONT_H
#define RENDER_PROBLEMCONT_H
#include "ofMain.h"
#include "../../imp/Utility/include.h"
#include "../../imp/Global/solution.h"

class BufferCont;
class BufferContMop;
struct Range{										// current range of each axis 
	double min, max, range;
	Range() :min(0), max(0), range(0){}
};
class RenderProCont{
	friend class BufferCont;
	friend class BufferContMop;
protected:	
	list<ofMesh> m_landscape;								//problem fitness landscape								
	int m_divNum=100;									
	vector<Range> m_rangeX;
	vector<Range> m_rangeObj;

	Range m_rangeXMap,m_rangeYMap,m_rangeZMap;
	vector<vector<float>> m_sample;					// store points sampled in problem fitness landscape
	
	list<ofMesh> m_optFound;
	vector<bool> m_valid;
	virtual void generateSample(const vector<vector<double>> &obj);
	list<ofMesh> m_obj, m_sol;
	list<vector<pair<double, double>>> m_gobjRange;
	vector<int> mv_selObj;    //the selected three objs to draw objective, default: 0,1,2
	double m_offsetObj = 0.1;

	list<ofMesh> m_freePeak;
	list<vector<ofPoint>> m_boxLabel;
protected: 
	void setMapRange();
	void setProRange();
	static int calculateSampleObj(int numTask,int taskid,RenderProCont & rend,vector<vector<double>> &obj);
	void startThread(vector<vector<double>> &obj);
	virtual void setupZ(const vector<vector<double>>&);
	virtual void updateSample(const vector<vector<double>>&);
	void setInfeasibleValue(vector<vector<double>>& obj);
public:
	virtual ~RenderProCont(){}
	void setup(int &, int defaultZobj, const vector<ofRectangle> &view, const vector<pair<double, double>>&curObjRange);
	void draw(int &);
	virtual void drawSolutionSpace(ofTrueTypeFont& font);
	virtual void drawObjectiveSpace();
	void updateLandscape(int &);
	virtual ofVec3f mapMainPoint(CodeVReal &s);
	void mapXtoScreen(const vector<double> &x, ofPoint &p,const float size);
	ofPoint  mapXtoScreen(const vector<double> &v, const float size);
	ofPoint RenderProCont::mapOtoScreen(const vector<double> &v, const vector<pair<double, double>>&, const float size);
	const vector<int> &getSelObj() const { return mv_selObj; }
	void setSelObj(const vector<int> & sel) {  mv_selObj=sel; }
	void updateObj(const vector<ofRectangle> &view, const vector<pair<double, double>>&curObjRange);
};


#endif