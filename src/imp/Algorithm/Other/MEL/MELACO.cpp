#include "MELACO.h"
#include "../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#include "MELOperator_TSP.h"
#include "MELOperator_QAP.h"
#include "MELOperator_MKP.h"

#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

static mutex m_nearMutex;
vector<set<int>> MELACO::mv_candidate;

MELACO::MELACO(ParamMap &v):MEL<CodeVInt, Ant, Population<CodeVInt, Ant>>(v), m_num(0){

	if (Global::msp_global->mp_problem->isProTag(TSP)) {
		m_MELOperator.reset(new MELOperator_TSP<Ant>(m_numDim));
	}
	else if (Global::msp_global->mp_problem->isProTag(QAP)) {
		m_MELOperator.reset(new MELOperator_QAP<Ant>(m_numDim));
	}
	else if (Global::msp_global->mp_problem->isProTag(MKP)) {
		m_MELOperator.reset(new MELOperator_MKP<Ant>(m_numDim));
	}
}
void MELACO::resetAntsInfo()
{
	for (int i = 0; i<m_popsize; i++)
		m_curPop[i].resetData();
}


double MELACO::getLenOfNN()
{
	vector<int> candidate(m_numDim), result(m_numDim);
	TravellingSalesman *_ptr = dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get());
	const vector<vector<double>> cost = _ptr->getCost();
	int n = 0;
	for (int i = 0; i < candidate.size(); i++) {
		candidate[i] = i;
	}
	result[n++] = candidate[0];
	candidate[0] = candidate[m_numDim - 1];
	while (n < m_numDim) {
		int loc = 0;
		double min = cost[result[n - 1]][candidate[loc]];
		for (int m = 1; m < m_numDim - n; m++) {
			if (cost[result[n - 1]][candidate[m]] < min) {
				min = cost[result[n - 1]][candidate[m]];
				loc = m;
			}
		}
		result[n++] = candidate[loc];
		candidate[loc] = candidate[m_numDim - n];
	}
	double val = 0;
	for (int i = 0; i < m_numDim; i++) {
		val += cost[result[i]][result[(i + 1) % m_numDim]];
	}
	return val;
}

void MELACO::initialize_AS()
{
	ReturnFlag rf;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	int dim = m_pop[0]->getNumDim();
	vector<vector<double>> phero;
	phero.resize(dim);
	for (int i = 0; i < dim; i++)
		phero[i].resize(dim);
	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++)
			phero[i][j] = 1. / dim;
	for (int i = 1; i<dim; i++)
		for (int j = 0; j<m_popsize; j++)
			m_pop[j]->selectNextCity_Pro(phero, 5, 1);
	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}


void MELACO::initialize_ACS()
{
	ReturnFlag rf;
	double mt = getLenOfNN();
	mt = 1.0 / (m_numDim * mt);
	vector<vector<double>> phero;
	phero.resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		phero[i].resize(m_numDim);
	for (int i = 0; i<m_numDim; i++)
		for (int j = 0; j<m_numDim; j++)
			phero[i][j] = 1. / m_numDim;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	for (int i = 0; i < m_popsize; i++)
	{
		for (int j = 1; j < m_numDim; j++)
		{
			double q = Global::msp_global->mp_uniformAlg->Next();
			if (q <= 0.9)
				m_pop[i]->selectNextCity_Greedy(phero, 2);
			else
				m_pop[i]->selectNextCity_Pro(phero, 2);
			local_updatePheromeno(phero, mt, i);
		}
		local_updatePheromeno(phero, mt, i, true);
	}
	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}


void MELACO::initialize_MMAS()
{
	ReturnFlag rf;
	int length = 20;
	if (m_numDim <= 20)
		length = m_numDim - 1;
	double pheroMax = getLenOfNN();
	pheroMax = 1. / (0.02 * pheroMax);

	m_nearMutex.lock();

	if (mv_candidate.empty())
	{
		vector<vector<int>> temp;
		mv_candidate.resize(m_numDim);
		temp.resize(m_numDim);
		for (int i = 0; i < temp.size(); i++)
			temp[i].resize(length);
		dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get())->findNearbyCity(temp);
		for (int i = 0; i < temp.size(); i++)
		{
			for (int j = 0; j < length; j++)
			{
				mv_candidate[i].insert(temp[i][j]);
			}
		}
	}

	m_nearMutex.unlock();

	vector<vector<double>> phero;
	phero.resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		phero[i].resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		for (int j = 0; j < m_numDim; j++)
			phero[i][j] = pheroMax;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	for (int i = 1; i < m_numDim; i++)
		for (int j = 0; j < m_popsize; j++)
			m_pop[j]->selectNextCity_Pro(phero, mv_candidate, 2, 1);

	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}


ReturnFlag MELACO::run_()
{
	ReturnFlag rf = Return_Normal;
	int mode = Global::g_arg[param_populationInitialMethod];
	PopInitMethod popInitialMode = static_cast<PopInitMethod>(mode);
	if (popInitialMode == POP_INIT_AS)
		initialize_AS();
	else if (popInitialMode == POP_INIT_ACS)
		initialize_ACS();
	else if (popInitialMode == POP_INIT_MMAS)
		initialize_MMAS();
	else {
		rf = initialize(false, true, true);
		statisticDiffsAndImp();
#ifdef OFEC_CONSOLE
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif
	}

	initilizeMemory();
	initilizeCurPop();

	dynamic_cast<TermMean*>(m_term.get())->initialize(mean());

	while (!ifTerminating())
	{
#ifdef OFEC_DEMON
		for (int i = 0; i<this->getPopSize(); i++)
			updateBestArchive(this->m_pop[i]->self());
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		dynamic_cast<TermMean*>(m_term.get())->countSucIter(mean());

		m_MELOperator->createSolutions(m_pop, m_curPop, m_alpha);

		rf = updata();
		statisticDiffsAndImp();
#ifdef OFEC_CONSOLE
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif
		if (rf == Return_Terminate) break;
		m_iter++;
		updateMemory();
		resetAntsInfo();
	}

#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre, false);
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
#endif
	return Return_Terminate;
}

void MELACO::local_updatePheromeno(vector<vector<double>> &phero, double t, int ant_loc, bool isLastEdge)
{
	pair<int, int> edge;
	if (!isLastEdge)
		edge = m_pop[ant_loc]->getCurrentEdge();
	else
		edge = m_pop[ant_loc]->getLastEdge();
	if (edge.first == edge.second && edge.first == -1)
		throw myException("edge error in local_updatePheromeno function @ACS.cpp");
	phero[edge.first][edge.second] = (1 - 0.1)*phero[edge.first][edge.second] + 0.1*t;
	phero[edge.second][edge.first] = (1 - 0.1)*phero[edge.second][edge.first] + 0.1*t;  //symmetric
}


void MELACO::statisticDiffsAndImp()
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