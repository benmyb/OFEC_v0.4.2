#include "SPBBO.h"

#include "../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif


SPBBO::SPBBO(ParamMap &v) :Population<CodeVInt, Ant>(int(v[param_popSize])), m_pro(m_numDim), m_saveFre(1500), m_num(0), \
m_impRadio(0), m_isHaveGlobalBest(false), m_globalBest(), m_isHavePersonBest(false)
{
	m_pro.resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		m_pro[i].resize(m_numDim);
	m_stra = (Stra)(int)v[param_case];
	setDefaultPara();
	m_pb.resize(m_popsize);

	m_term.reset(new TermMean(v));
}


void SPBBO::setDefaultPara(){
	m_k = 5;
	m_r = 1. / (m_numDim - 1);
	if (m_stra == SPBBO1){
		m_total = 12;
		m_elite = 0.1;
	}
	else if (m_stra == SPBBO7){
		m_total = 48;
		m_elite = 1;
	}
	m_alpha = 1;
	m_beta = 5;
}

ReturnFlag SPBBO::updateSelectPro(){
	ReturnFlag rf;
	m_impRadio = 0;
	for (int i = 0; i < m_popsize; i++){
		double temp = m_pop[i]->data().m_obj[0];
		rf = m_pop[i]->evaluate();
		if (rf == Return_Invalid){
			throw myException("new solution error in SPBBO::updateSelectPro()");
		}
		if (temp > m_pop[i]->data().m_obj[0]){
			m_impRadio++;
		}
		if (rf == Return_Terminate) break;
	}

#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif
	if (rf == Return_Terminate) return rf;

	if (m_stra == SPBBO1){
		vector<int> bestIdx = findBest();
		if (!m_isHaveGlobalBest || m_globalBest < m_pop[bestIdx[0]]->representative()){
			m_globalBest = m_pop[bestIdx[0]]->representative(); //deep copy
			m_isHaveGlobalBest = true;
		}

		if (m_gp.size() < m_k){
			m_gp.push_back(*m_pop[bestIdx[0]]);
		}
		else{
			m_gp.pop_front();
			m_gp.push_back(*m_pop[bestIdx[0]]);
		}

		for (int i = 0; i < m_numDim; i++){
			for (int j = 0; j < m_numDim; j++){
				if (i == j)
					m_pro[i][j] = 0;
				else
					m_pro[i][j] = m_r;
			}
		}
			
		int e1, e2;
		double total = m_total / m_gp.size();
		for (list<Ant>::iterator it = m_gp.begin(); it != m_gp.end();it++){
			e1 = (*it).data().m_x[0];
			for (int i = 0; i < m_numDim; i++){
				e2 = (*it).data().m_x[(i + 1) % m_numDim];
				m_pro[e1][e2] += total;
				m_pro[e2][e1] = m_pro[e1][e2];
				e1 = e2;
			}
		}

		e1 = m_globalBest.data().m_x[0];
		for (int i = 0; i < m_numDim; i++){
			e2 = m_globalBest.data().m_x[(i + 1) % m_numDim];
			m_pro[e1][e2] += m_elite;
			m_pro[e2][e1] = m_pro[e1][e2];
			e1 = e2;
		}
	}
	else if (m_stra == SPBBO7){
		vector<int> bestIdx = findBest();
		if (!m_isHaveGlobalBest || m_globalBest < m_pop[bestIdx[0]]->representative()){
			m_globalBest = m_pop[bestIdx[0]]->representative(); //deep copy
			m_isHaveGlobalBest = true;
		}

		if (!m_isHavePersonBest){
			for (int i = 0; i < m_popsize; i++){
				m_pb[i] = *m_pop[i];
			}
			m_isHavePersonBest = true;
		}
		else{
			for (int i = 0; i < m_popsize; i++){
				if (m_pb[i] < *m_pop[i]){
					m_pb[i] = *m_pop[i];
				}
			}
		}

		for (int i = 0; i < m_numDim; i++){
			for (int j = 0; j < m_numDim; j++){
				if (i == j)
					m_pro[i][j] = 0;
				else
					m_pro[i][j] = m_r;
			}
		}

		double total = m_total / 2;
		total = total / m_popsize;

		int e1, e2;
		for (int i = 0; i < m_popsize; i++){
			e1 = m_pop[i]->data().m_x[0];
			for (int j = 0; j < m_numDim; j++){
				e2 = m_pop[i]->data().m_x[(j + 1) % m_numDim];
				m_pro[e1][e2] += total;
				m_pro[e2][e1] = m_pro[e1][e2];
				e1 = e2;
			}
		}

		for (int i = 0; i < m_popsize; i++){
			e1 = m_pb[i].data().m_x[0];
			for (int j = 0; j < m_numDim; j++){
				e2 = m_pb[i].data().m_x[(j + 1) % m_numDim];
				m_pro[e1][e2] += total;
				m_pro[e2][e1] = m_pro[e1][e2];
				e1 = e2;
			}
		}

		e1 = m_globalBest.data().m_x[0];
		for (int i = 0; i < m_numDim; i++){
			e2 = m_globalBest.data().m_x[(i + 1) % m_numDim];
			m_pro[e1][e2] += m_elite;
			m_pro[e2][e1] = m_pro[e1][e2];
			e1 = e2;
		}
	}
	return rf;
}


bool SPBBO::ifTerminating()
{
	if (dynamic_cast<TermMean*>(m_term.get())->ifTerminating()) {
		if (m_iter == 0) return false;
		return true;
	}

	return false;
}

void SPBBO::initializeSystem()
{
	for (int i = 0; i < m_numDim; i++){
		for (int j = 0; j < m_numDim; j++){
			if (i == j)
				m_pro[i][j] = 0;
			else
				m_pro[i][j] = m_r;
		}
	}

	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
}


void SPBBO::resetAntsInfo()
{
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->resetData();
}


ReturnFlag SPBBO::run_()
{
	int i, j;
	ReturnFlag rf = Return_Normal;
	initializeSystem();
	dynamic_cast<TermMean*>(m_term.get())->initialize(DBL_MAX);
	while (!ifTerminating())
	{
#ifdef OFEC_DEMON
		for (i = 0; i<this->getPopSize(); i++)
			updateBestArchive(this->m_pop[i]->self());
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		for (i = 0; i < m_popsize; i++)
			for (j = 1; j < m_numDim; j++)
			{
				m_pop[i]->selectNextCity_Pro(m_pro, m_beta, m_alpha);
			}

		rf = updateSelectPro();
		++m_iter;
		dynamic_cast<TermMean*>(m_term.get())->countSucIter(mean());
#ifdef OFEC_CONSOLE
		double tempdif = 0;
		for (int i = 0; i < m_popsize; i++)
			tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
		tempdif /= m_popsize;
		double impr = static_cast<double>(m_impRadio) / m_popsize;
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif

		if (rf == Return_Terminate) break;
		resetAntsInfo();		
		//getStopCount();  //must be here
		//cout<<" "<<Global::msp_global->mp_problem->getBestSolutionSoFar().getObjDistance(CAST_TSP->getGOpt()[0].data().m_obj)<<" "<<m_stopCount<<endl;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre, false);

	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
#endif
	return Return_Terminate;
}

