#ifndef RENDERPROCONTMOP_H
#define RENDERPROCONTMOP_H

#include "RenderProCont.h"
#include "../../imp/Algorithm/Population.h"
class RenderProContMOP : public RenderProCont {
protected:

	void setupZ(const vector<vector<double>>&);
	void generateSample(const vector<vector<double>> &obj);
	void updateSample(const vector<vector<double>>&);
public:
	ofVec3f mapMainPoint(CodeVReal &s);
	void updateLandscape(int &);

protected:
	Population<CodeVReal, Individual<CodeVReal>> m_pop;
	Range m_rangeVZ;
};

inline ofVec3f RenderProContMOP::mapMainPoint(CodeVReal &s){
	double x, y, z;
	if (GET_NUM_DIM == 1){
		x = m_rangeXMap.min + m_rangeXMap.range*(s.m_x[0] - m_rangeX[0].min) / m_rangeX[0].range;
		y = 0;
	}
	else {
		x = m_rangeXMap.min + m_rangeXMap.range*(s.m_x[0] - m_rangeX[0].min) / m_rangeX[0].range;
		y = m_rangeYMap.min + m_rangeYMap.range*(s.m_x[1] - m_rangeX[1].min) / m_rangeX[1].range;
	}

	double rank = m_pop.rank(s);
	z = m_rangeZMap.min + m_rangeZMap.range*(m_rangeVZ.max - rank) / m_rangeVZ.range + 5;

	if (z>m_rangeZMap.max) z = m_rangeZMap.max;
	if (z<m_rangeZMap.min) z = m_rangeZMap.min;
	return ofVec3f(x, y, z);
}
#endif