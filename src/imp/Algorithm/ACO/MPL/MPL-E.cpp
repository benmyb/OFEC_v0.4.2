/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 7 Oct 2014
// Last modified:

#include "MPL-E.h"
#include "../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

MPL_E::MPL_E(ParamMap &v) :Population<CodeVInt, Ant>(int(v[param_popSize])), m_saveFre(1500), m_num(0), m_impRadio(0), m_weight(v[param_popSize])
{
	mvv_edgesInfo.resize(v[param_numDim]);
	for (size_t i = 0; i<mvv_edgesInfo.size(); i++)
		mvv_edgesInfo[i].resize(v[param_numDim]);

	m_term.reset(new TermMean(v));
}

MPL_E::~MPL_E()
{
	mvv_edgesInfo.clear();
}

void MPL_E::statisticEdgeInfo()
{
	int i, j;
	int e1, e2;
	int bestIdx = 0, worstIdx = 0, num = 0;

	for (i = 0; i<m_popsize; i++){
		if (m_pop[i]->self()>m_pop[bestIdx]->self()){
			bestIdx = i;
		}
		if (m_pop[i]->self()<m_pop[worstIdx]->self()){
			worstIdx = i;
		}
	}

	double disobj = m_pop[worstIdx]->getObjDistance_(m_pop[bestIdx]->data().m_obj) + 1;
	for (i = 0; i<m_popsize; i++){
		m_weight[i] = (m_pop[i]->getObjDistance_(m_pop[worstIdx]->data().m_obj) + 1) / disobj;
		m_weight[i] = 1. / (1 + exp(-m_weight[i]));
	}

	for (i = 0; i<m_numDim; i++)
		fill(mvv_edgesInfo[i].begin(), mvv_edgesInfo[i].end(), 0);

	for (i = 0; i<m_popsize; i++){
		e1 = m_pop[i]->data().m_x[0];
		for (j = 0; j<m_numDim; j++){
			e2 = m_pop[i]->data().m_x[(j + 1) % m_numDim];
			mvv_edgesInfo[e1][e2] += m_weight[i];
			mvv_edgesInfo[e2][e1] += m_weight[i];
			e1 = e2;
		}
	}
}


void MPL_E::initializeSystem()
{
	for (int i = 0; i<m_popsize; i++){
		m_pop[i]->initialize(m_pop[i]->data().m_x[0]);
	}
}

void MPL_E::resetAntsInfo()
{
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->resetData();
}

bool MPL_E::ifTerminating()
{
	if (dynamic_cast<TermMean*>(m_term.get())->ifTerminating()) {
		if (m_iter == 0) return false;
		return true;
	}

	return false;
}

ReturnFlag MPL_E::run_()
{
	int i, j, dim;
	ReturnFlag rf = Return_Normal;
	dim = m_numDim;
	rf = initialize(false, true, true);

	initializeSystem();
	statisticEdgeInfo();
	dynamic_cast<TermMean*>(m_term.get())->initialize(mean());
	statisticDiffsAndImp(false);
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif

	while (!ifTerminating())
	{
		//cout << Global::msp_global->mp_problem->getEvaluations() << " " << Solution<CodeVInt>::getBestSolutionSoFar().data().m_obj[0] << endl;
#ifdef OFEC_DEMON
		for (i = 0; i<this->getPopSize(); i++)
			updateBestArchive(this->m_pop[i]->self());
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		dynamic_cast<TermMean*>(m_term.get())->countSucIter(mean());
		for (j = 0; j<m_popsize; j++){
			for (i = 1; i<dim; i++){
				m_pop[j]->selectNextCity_Pro(mvv_edgesInfo, 0, 1);
			}
		}
		rf = update();
		statisticDiffsAndImp();
#ifdef OFEC_CONSOLE
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif
		if(rf==Return_Terminate) break; 
		m_iter++;
		statisticEdgeInfo();
		resetAntsInfo();
	} 

#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre, false);
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
#endif
	return Return_Terminate;
}


ReturnFlag MPL_E::update()
{
	ReturnFlag rf;
	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		double temp = m_pop[i]->data().m_obj[0];
		rf = m_pop[i]->evaluate();
		if (temp > m_pop[i]->data().m_obj[0])
			m_impRadio++;
		if (rf == Return_Terminate) break;
	}
	return rf;
}

void MPL_E::statisticDiffsAndImp(bool flag)
{
#ifdef OFEC_CONSOLE
	int numDim = m_numDim;
	if (!flag)
		m_impRadio = 0;

	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;

	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}