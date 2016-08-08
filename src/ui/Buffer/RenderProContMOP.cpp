#include "RenderProContMOP.h"

#include "OFGlobal.h"
#include "BufferCont.h"
extern unique_ptr<Scene> msp_buffer;
void RenderProContMOP::setupZ(const vector<vector<double>>& obj){
	RenderProCont::setupZ(obj); 
	int gsize = 0;
	if (Global::msp_global->mp_problem->isGlobalOptKnown()){
		//gsize = CAST_PROBLEM_CONT->getGOpt().getNumOpt();
	}
	
	m_pop.resize(obj.size()+gsize);
	for (decltype (obj.size()) i = 0; i < obj.size(); ++i){
		m_pop[i]->data().m_obj = obj[i];
	}
	if (gsize > 0){
		for (int i = 0, j = obj.size(); i < gsize; ++i,++j){
			m_pop[j]->data().m_obj=CAST_PROBLEM_CONT->getGOpt()[i].data().m_obj;
		}
	}
	m_pop.rank();
	m_rangeVZ.min = 0;
	m_rangeVZ.max = m_pop.getCurRank();
	m_rangeVZ.range = m_rangeVZ.max - m_rangeVZ.min;
	
}

void RenderProContMOP::generateSample(const vector<vector<double>> &obj){

	if (GET_NUM_DIM == 1){
		m_sample.resize(m_divNum);
		for (int i = 0; i<m_divNum; ++i){
			vector<float> v(2);
			v[0] = m_rangeXMap.min + m_rangeXMap.range*i / m_divNum; v[1] = 0;
			v.push_back(m_rangeZMap.min + m_rangeZMap.range*(m_rangeVZ.max - m_pop[toZ(0, i, m_divNum)]->rank()) / m_rangeVZ.range);
			m_sample[i]=v;
		}

	}
	else {
		m_sample.resize(m_divNum*m_divNum);
		
		for (int i = 0; i<m_divNum; ++i){
			for (int j = 0; j<m_divNum; ++j){
				vector<float> v(2);
				v[0] = m_rangeXMap.min + m_rangeXMap.range*i / m_divNum;
				v[1] = m_rangeYMap.min + m_rangeYMap.range*j / m_divNum;
				int idx = toZ(i, j, m_divNum);
				v.push_back(m_rangeZMap.min + m_rangeZMap.range*(m_rangeVZ.max - m_pop[idx]->rank()) / m_rangeVZ.range);
				m_sample[idx]=v;
			}
		}
	}

}

void RenderProContMOP::updateSample(const vector<vector<double>>&obj){
	if (GET_NUM_DIM == 1){
		for (int i = 0; i<m_divNum; ++i){	
			m_sample[i][2] = m_rangeZMap.min + m_rangeZMap.range*(m_rangeVZ.max - m_pop[toZ(0, i, m_divNum)]->rank()) / m_rangeVZ.range;
		}
	}
	else {
		for (int i = 0; i<m_divNum; ++i){
			for (int j = 0; j<m_divNum; ++j){
				int idx = toZ(i, j, m_divNum);			
				m_sample[idx][2] = m_rangeZMap.min + m_rangeZMap.range*(m_rangeVZ.max - m_pop[idx]->rank()) / m_rangeVZ.range;
			}
		}
	}
}
