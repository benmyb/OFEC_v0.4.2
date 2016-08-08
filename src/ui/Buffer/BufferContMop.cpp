#include "BufferContMop.h"
#include "OFGlobal.h"
#include "../../imp/Utility/CornerSort.h"
#include "../../imp/Algorithm/PopulationCont.h"
#include "../../imp/Problem/FunctionOpt/MOP/MOEA-F/F.h"
#include "RenderProContMOP.h"
#include "../../imp/Problem/FunctionOpt/FOnePeak.h"
#include "../../imp/Problem/FunctionOpt/FFreePeak.h"
#include "../../imp/Problem/DOP/FFreePeak_D_M_OnePeak.h"
#include "../../imp/Problem/FunctionOpt/MOP/FreePeak_M_OnePeak/FFreePeak_M_OnePeak.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_showDomination;
BufferContMop::BufferContMop(vector<ofRectangle> &view, vector<ofEasyCam> &cam, ofLight &light, ofTrueTypeFont  &font) :BufferCont(view, cam, light, font, new RenderProContMOP())
{
}

void BufferContMop::updateBuffer(const vector<Algorithm*> *pops ){
	BufferCont::updateBuffer(pops);

	if (g_algTermination) return;
	

	//for test
	vector<unsigned> rank1(m_indiColor.back().size());
	int j = 0;
	for (auto i = 0; i<pops->size(); ++i){
		for (auto k = 0; k<reinterpret_cast<Population<CodeVReal, Individual<CodeVReal>>*>((*pops)[i])->getPopSize(); ++k, ++j){
			if (g_algTermination) return;
			rank1[j] = (*reinterpret_cast<Population<CodeVReal, Individual<CodeVReal>>*>((*pops)[i]))[k]->rank()+1;
		}
	}
	//test end

/*
	double **p =new double* [m_indiColor.back().size()];
	for (auto i = 0; i < m_indiColor.back().size(); ++i){
	p[i] = m_buffer.back().at(i).data().m_obj.data();
	}
	vector<unsigned> rank(m_indiColor.back().size());
	int m = Global::msp_global->mp_problem->getNumObj();
	vector<int> comp(m);
	for (int i = 0; i < m; ++i){
		if (Global::msp_global->mp_problem->getOptType(i) == MIN_OPT)comp[i] = 0;
		else comp[i] = 1;
	}
	CornerSort::cornerSort(p, m, m_indiColor.back().size(), rank.data(),comp.data());
	if (!g_showPop){
		ofstream out("data.txt");
		for (auto i = 0; i < m_indiColor.back().size(); ++i){
			for (int j = 0; j < m; ++j)				out << p[i][j] << " ";
			out << endl;
		}
		out.close();
	}
	delete[]p;*/
	bool goptKnown = Global::msp_global->mp_problem->isGlobalOptKnown();
	for (auto i = 0; i < m_indiColor.back().size(); ++i){
		if (!g_showDomination || !goptKnown){
			if (rank1[i] == 1){ // non-dominated solution in different colors
				ofColor c(255, 0, 0);
				c.setHue(25 + 180. * i / m_indiColor.back().size());
				m_indiColor.back().at(i) = c;
			}
			else{// dominated solution in black
				m_indiColor.back().at(i).r = 0; m_indiColor.back().at(i).g = 0; m_indiColor.back().at(i).b = 0;
			}
		}
		else{			
			if (CAST_PROBLEM_CONT->isParetoSet(m_buffer.back()[i].data())){
				m_indiColor.back().at(i).r = 255; m_indiColor.back().at(i).g = 0; m_indiColor.back().at(i).b = 0;
			}
			else{// dominated solution in black
				m_indiColor.back().at(i).r = 0; m_indiColor.back().at(i).g = 0; m_indiColor.back().at(i).b = 0;
			}
		}
		
	}
	
}



