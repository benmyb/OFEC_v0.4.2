#ifndef BUFFERCONTMOP_H
#define BUFFERCONTMOP_H

#include "BufferCont.h"

class BufferContMop :public BufferCont
{
public:
	BufferContMop(vector<ofRectangle> &view,vector<ofEasyCam> &cam, ofLight &light,ofTrueTypeFont  &font);
protected:
	void updateBuffer(const vector<Algorithm*> *pop = 0);
};

#endif //BUFFERCONTMOP_H