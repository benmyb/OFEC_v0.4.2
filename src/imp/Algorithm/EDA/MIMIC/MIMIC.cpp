#include "MIMIC.h"
#include "../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

MIMIC::MIMIC(ParamMap &v) :Population<CodeVInt, Ant>(int(v[param_popSize]),true), m_pro(m_numDim), m_saveFre(1500), m_num(0)
{
	setDefaultPara();
	for (int i = 0; i < m_numDim; ++i)
		m_pro[i].resize(m_numDim);
	m_index.resize(m_len);
	m_p.resize(m_numDim);

	m_term.reset(new TermMean(v));
}

void MIMIC::setDefaultPara()
{
	m_a = 0.8;
	m_len = 10;
}

void MIMIC::selectIndividuals()
{
	vector<bool> flag(m_popsize, false);
	int min, index;
	for (int i = 0; i < m_index.size(); i++)
	{
		min = INT_MAX; 
		index = -1;
		for (int j = 0; j < m_popsize; j++)
		{
			if (m_pop[j]->data().m_obj[0] < min && flag[j] == false){
				min = m_pop[j]->data().m_obj[0];
				index = j;
			}

		}
		m_index[i] = index;
		flag[index] = true;
	}
}

void MIMIC::statisticFrequency()
{
	for (int i = 0; i < m_numDim; i++)
	{
		m_p[i] = 0;
		for (int j = 0; j < m_numDim; j++)
			m_pro[i][j] = 0;
	}

	int e1, e2;
	for (int i = 0; i < m_len; i++)
	{
		e1 = m_pop[m_index[i]]->data().m_x[0];
		m_p[e1] += 1;
	}

	for (int i = 0; i < m_len; i++)
	{
		e1 = m_pop[m_index[i]]->data().m_x[0];
		for (int j = 0; j < m_numDim; j++){
			e2 = m_pop[m_index[i]]->data().m_x[(j + 1) % m_numDim];
			m_pro[e1][e2] += 1;
			m_pro[e2][e1] += 1;
			e1 = e2;
		}
	}

	for (int i = 0; i < m_numDim; i++){
		m_p[i] /= m_len;
		for (int j = 0; j < m_numDim; j++)
			m_pro[i][j] /= m_len;
	}
}

void MIMIC::modifyModel()
{
	int num = 0;
	for (int i = 0; i < m_numDim; i++)
		if (m_p[i] == 0)
			num++;
	if (num != 0){
		for (int i = 0; i < m_numDim; i++)
		{
			if (m_p[i] == 0)
				m_p[i] = m_a / num;
			else
				m_p[i] = (1 - m_a) / m_p[i];
			if (i > 0) m_p[i] += m_p[i - 1];
		}
	}

	num = 0;
	for (int i = 0; i < m_numDim; i++){
		for (int j = 0; j < m_numDim; j++){
			if (i != j&&m_pro[i][j] == 0){
				num++;
			}
		}
	}
	for (int i = 0; i < m_numDim; i++){
		for (int j = 0; j < m_numDim; j++){
			if (i != j&&m_pro[i][j] == 0){
				m_pro[i][j] = m_a / num;
			}
			else if (m_pro[i][j] != 0){
				m_pro[i][j] = (1 - m_a)*m_pro[i][j];
			}
		}
	}
}


bool MIMIC::ifTerminating()
{
	if (dynamic_cast<TermMean*>(m_term.get())->ifTerminating(mean())) {
		if (m_iter == 0) return false;
		return true;
	}

	return false;
}


ReturnFlag MIMIC::run_()
{
	int i, j;
	ReturnFlag rf = Return_Normal;

	dynamic_cast<TermMean*>(m_term.get())->initialize(mean());

	while (!ifTerminating())
	{
#ifdef OFEC_DEMON
		for (i = 0; i<this->getPopSize(); i++)
			updateBestArchive(this->m_pop[i]->self());
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		selectIndividuals();
		statisticFrequency();
		modifyModel();
		for (j = 0; j<m_popsize; j++){
			double p = Global::msp_global->mp_uniformAlg->Next()*m_p[m_p.size()-1];
			if (p < m_p[0]){
				m_pop[j]->initialize(0);
			}
			for (i = 1; i < m_numDim; i++){
				if (p > m_p[i - 1] && p < m_p[i])
				{
					m_pop[j]->initialize(i);
					break;
				}
			}
			for (i = 1; i<m_numDim; i++){
				m_pop[j]->selectNextCity_Pro(m_pro,0,1);
			}
		}
		for (i = 0; i < m_popsize; i++){
			rf = m_pop[i]->evaluate();
			if (rf == Return_Terminate) break;
		}
#ifdef OFEC_CONSOLE
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif
		if (rf == Return_Terminate) break;
		m_iter++;
		resetAntsInfo();
	}

#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre, false);
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
#endif
	return Return_Terminate;
}

void MIMIC::resetAntsInfo()
{
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->resetData(false);
}


