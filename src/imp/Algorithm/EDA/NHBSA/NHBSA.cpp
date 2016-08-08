#include "NHBSA.h"
#include "../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

NHBSA::NHBSA(ParamMap &v) :Population<CodeVInt, Individual<CodeVInt>>(int(v[param_popSize]), true), m_pro(m_numDim), m_saveFre(1500), m_num(0),
m_tabu(m_numDim, false), m_newIndivl(), m_impRadio(0)
{
	setDefaultPara();
	for (int i = 0; i < m_numDim; ++i)
		m_pro[i].resize(m_numDim);
	m_stra = (Stra)(int)v[param_case];

	m_term.reset(new TermMean(v));
}

void NHBSA::setDefaultPara(){
	m_bRatio = 0.0002;
	m_n = m_numDim / 15;
	if (m_n < 2) m_n = 2;
	m_bRatio = (m_popsize * m_bRatio) / m_numDim;
	mv_cut.resize(m_n);
}

void NHBSA::initializePro(){
	int i, j;
	for (i = 0; i < m_numDim; i++){
		for (j = 0; j < m_numDim; j++){
			m_pro[i][j] = m_bRatio;
		}
	}

	int node;
	for (i = 0; i < m_popsize; i++){
		for (j = 0; j < m_numDim; j++){
			node = m_pop[i]->data().m_x[j];
			m_pro[j][node] += 1;
		}
	}
}

void NHBSA::updatePro(const Individual<CodeVInt>& newIndivl, int index){
	int node;
	for (int i = 0; i < m_numDim; i++){
		node = m_pop[index]->data().m_x[i];
		m_pro[i][node] -= 1;
	}

	for (int i = 0; i < m_numDim; i++){
		node = newIndivl.data().m_x[i];
		m_pro[i][node] += 1;
	}
}

vector<int> NHBSA::generateRandomPositionIndex(){
	vector<int> temp(m_numDim), index(m_numDim);
	for (int i = 0; i < m_numDim; i++){
		temp[i] = i;
	}
	int pos;
	for (int i = 0; i < m_numDim; i++){
		pos = Global::msp_global->getRandInt(0, m_numDim - i);
		index[i] = temp[pos];
		temp[pos] = temp[m_numDim - 1 - i];
	}
	return index;
}

void NHBSA::resetIndividualInfo(){
	for (int i = 0; i < m_numDim; i++)
		m_tabu[i] = false;
}

ReturnFlag NHBSA::sampleSolution(){
	ReturnFlag rf;
	vector<int> index = generateRandomPositionIndex();
	if (m_stra == WO){
		for (int i = 0; i < m_numDim; i++)
			chooseCity(index[i]);
		rf = m_newIndivl.evaluate();
		if (rf == Return_Invalid){
			throw myException("new solution error in NHBSA::sampleSolution()");
		}

		int pos = Global::msp_global->getRandInt(0, m_popsize);

		if (m_newIndivl > *m_pop[pos]){
			updatePro(m_newIndivl, pos);
			*m_pop[pos] = m_newIndivl;
			m_impRadio++;
		}
	}
	else if (m_stra == WT){
		int pos = Global::msp_global->getRandInt(0, m_popsize);
		vector<int> cutPoint(m_numDim);
		set<int> points;
		for (int i = 0; i < m_numDim - 1; i++)
			cutPoint[i] = i;
		for (int i = 0; i < m_n; i++){
			int loc = Global::msp_global->getRandInt(0, m_numDim - i);
			points.insert(cutPoint[loc]);
			cutPoint[loc] = cutPoint[m_numDim - 1 - i];
		}
		int n = 0;
		for (set<int>::iterator iter = points.begin(); iter != points.end(); iter++){
			mv_cut[n++] = *iter;
		}

		int len = 0;
		n = Global::msp_global->getRandInt(0, m_n);
		for (int i = mv_cut[n]; i != mv_cut[(n + 1) % m_n]; i = (i + 1) % m_numDim){
			len++;
		}

		for (int i = 0; i < m_numDim - len; i++){
			m_newIndivl.data().m_x[index[i]] = m_pop[pos]->data().m_x[index[i]];
			m_tabu[m_newIndivl.data().m_x[index[i]]] = true;
		}

		for (int i = m_numDim - len; i < m_numDim; i++){
			chooseCity(index[i]);
		}

		rf = m_newIndivl.evaluate();
		if (rf == Return_Invalid){
			throw myException("new solution error in NHBSA::sampleSolution()");
		}

		if (m_newIndivl > *m_pop[pos]){
			updatePro(m_newIndivl, pos);
			*m_pop[pos] = m_newIndivl;
			m_impRadio++;
		}
	}

	return rf;
}


void NHBSA::chooseCity(int loc){
	vector<double> pro(m_numDim);
	for (int i = 0; i < m_numDim; i++){
		if (m_tabu[i])
			pro[i] = 0;
		else
			pro[i] = m_pro[loc][i];

		if (i > 0){
			pro[i] += pro[i - 1];
		}
	}

	double p = Global::msp_global->getRandFloat(0, pro[m_numDim - 1]);
	vector<double>::iterator it = lower_bound(pro.begin(), pro.end(), p);
	m_newIndivl.data().m_x[loc] = int(it - pro.begin());
	m_tabu[int(it - pro.begin())] = true;
}

bool NHBSA::ifTerminating()
{
	if (dynamic_cast<TermMean*>(m_term.get())->ifTerminating(mean())) {
		if (m_iter == 0) return false;
		return true;
	}

	return false;
}

ReturnFlag NHBSA::run_()
{
	int i;
	ReturnFlag rf = Return_Normal;

#ifdef OFEC_CONSOLE
	statisticDiffsAndImp();
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Individual<CodeVInt>>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif

	dynamic_cast<TermMean*>(m_term.get())->initialize(mean());

	initializePro();

	while (!ifTerminating())
	{
#ifdef OFEC_DEMON
		for (i = 0; i<this->getPopSize(); i++)
			updateBestArchive(this->m_pop[i]->self());
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		m_impRadio = 0;
		for (i = 0; i < m_popsize; i++){
			rf = sampleSolution();
			if (rf == Return_Terminate) break;
			resetIndividualInfo();
		}

#ifdef OFEC_CONSOLE
		statisticDiffsAndImp();
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Individual<CodeVInt>>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif
		m_iter++;
		if (rf == Return_Terminate) break;
	}

#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Individual<CodeVInt>>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre, false);
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
#endif
	return Return_Terminate;
}


void NHBSA::statisticDiffsAndImp()
{
#ifdef OFEC_CONSOLE
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}
