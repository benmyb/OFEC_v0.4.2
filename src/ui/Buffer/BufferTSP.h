#ifndef _BUFFERTSP_H
#define _BUFFERTSP_H

#include "Buffer.h"

class BufferTSP : public Buffer<CodeVInt>
{
public:
	BufferTSP(vector<ofRectangle> &view,vector<ofEasyCam> &cam, ofLight &light,ofTrueTypeFont  &font,int numDim);
	~BufferTSP(){}
protected:
	void updateBuffer(const vector<Algorithm*> *pops=0);
	void drawObjective();
	void drawProblem();
	void drawSolutndObj(){};
	void drawSolution(){};
	void initialize(int numDim);
	ofMesh m_meshOpt, m_mesh;
};

#endif //_BUFFERTSP_H