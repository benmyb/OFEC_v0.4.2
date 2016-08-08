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
// Created: 17 March 2016
// Last modified:

#ifndef MEL_H
#define MEL_H

#include "MELOperator.h"
template<typename ED, typename TypeIndi,typename TypePop>
class MEL :public TypePop {
public:
	enum UpdateScheme {bsf,hb,ci,c};
	// bsf : best so far of each individual
	// hb: all historical best solutions of each individual
	// ci: improved individuals in the best so far population
	// c: all individuals in the current population
protected:
	UpdateScheme m_ms = UpdateScheme::bsf;			//memory scheme

	vector<double> m_fitness;
	vector<double> m_weight;
	vector<double> m_obj;

	double m_memoryMaxObj, m_memoryMinObj, m_preMemoryMaxObj, m_preMemoryMinObj;
	double m_wt=1.e-3;			// weight threshold, individuals with weight less than m_wt are removed from memory

	vector<deque<int>> m_exMemory; // explicit memory: the index of all persnal best memory 
	
	vector<TypeIndi> m_hisIndi;		//historical individuals
	vector<TypeIndi> m_curPop;
	unique_ptr<MELOperator<TypeIndi> > m_MELOperator;
public:
	MEL(ParamMap &v);
	void initilizeMemory();
	bool ifTerminating();
	void updateMemory();
	void updateMemory_HB(const vector<int>& index);
	void updateMemory_BSF(const vector<int>& index);
	void updateMemory_CI(const vector<int>& index);
	void updateMemory_C();
	void initilizeCurPop();
protected:
	ReturnFlag updata();
	int m_impRadio, m_saveFre;
	double m_alpha;
};


template<typename ED, typename TypeIndi,typename TypePop>
MEL<ED, TypeIndi,TypePop>::MEL(ParamMap &v) :TypePop(int(v[param_popSize])), m_fitness(v[param_popSize])\
, m_weight(v[param_popSize]), m_obj(v[param_popSize]), m_exMemory(v[param_popSize]), m_impRadio(0), m_saveFre(1500){

	if(v.find(param_updateSchemeProbabilityLearning)!=v.end())	m_ms = (UpdateScheme)(int)v[param_updateSchemeProbabilityLearning];

	if (v.find(param_xoverProbability) != v.end())	m_alpha = v[param_xoverProbability];
	else m_alpha = 0.9;

	this->m_term.reset(new TermMean(v));

}

template<typename ED, typename TypeIndi,typename TypePop>
void MEL<ED, TypeIndi,TypePop>::initilizeCurPop()
{
	m_curPop.resize(this->m_popsize);
	for (int i = 0; i<this->m_popsize; i++) {
		m_curPop[i].initialize(this->m_pop[i]->data().m_x[0]);
	}
}

template<typename ED, typename TypeIndi,typename TypePop>
void MEL<ED, TypeIndi,TypePop>::initilizeMemory() {
	m_memoryMaxObj = m_memoryMinObj = (*this->m_pop[0]).data().m_obj[0];
	for (int i = 0; i < this->m_popsize; ++i) {
		double obj = this->m_pop[i]->data().m_obj[0];
		if (obj > m_memoryMaxObj) m_memoryMaxObj = obj;
		if (obj < m_memoryMinObj) m_memoryMinObj = obj;
	}
	//vector<int> indiv(this->m_numDim);
	double gap = m_memoryMaxObj - m_memoryMinObj + 1;
	for (int i = 0; i < this->m_popsize; ++i) {
		m_obj[i] = this->m_pop[i]->data().m_obj[0];
		if(Global::msp_global->mp_problem->getOptType()==MIN_OPT)	m_fitness[i] = (m_memoryMaxObj - m_obj[i] + 1) / gap;
		else m_fitness[i] = (m_obj[i]- m_memoryMinObj + 1) / gap;

		m_weight[i] = 1. / (1 + exp(-m_fitness[i]));
		m_exMemory[i].push_front(i);
		//for (int j = 0; j < this->m_numDim; j++)
		//	indiv[j] = this->m_pop[i]->data().m_x[j];
		//m_hisIndi.push_back(indiv);
		m_hisIndi.push_back(*this->m_pop[i]);
	}

	m_MELOperator->updatePro(this->m_pop, m_weight);

	m_preMemoryMaxObj = m_memoryMaxObj;
	m_preMemoryMinObj = m_memoryMinObj;
}

template<typename ED, typename TypeIndi,typename TypePop>
void MEL<ED, TypeIndi,TypePop>::updateMemory() {

	vector<int> index;  //the updated individual in the best so far
	m_memoryMinObj = m_preMemoryMinObj;
	for (int i = 0; i < this->m_popsize; ++i)
	{
		if (this->m_pop[i]->getFlag() == true)
		{
			index.push_back(i);
			if (m_memoryMinObj > this->m_pop[i]->data().m_obj[0])
				m_memoryMinObj = this->m_pop[i]->data().m_obj[0];
		}
	}

	if (m_ms != MEL::c)
		if (index.empty())
			return;

	switch (m_ms)
	{
	case MEL::bsf:
		updateMemory_BSF(index);
		break;
	case MEL::hb:
		updateMemory_HB(index);
		break;
	case MEL::ci:
		updateMemory_CI(index);
		break;
	case MEL::c:
		updateMemory_C();
		break;
	default:
		break;
	}

	for (int i = 0; i < this->m_popsize; ++i)
		this->m_pop[i]->setFlag(false);
}

template<typename ED, typename TypeIndi,typename TypePop>
void MEL<ED, TypeIndi,TypePop>::updateMemory_HB(const vector<int>& index)
{
	if (m_memoryMinObj < m_preMemoryMinObj)
		m_preMemoryMinObj = m_memoryMinObj;

	double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
	if (m_memoryMinObj < m_preMemoryMinObj)
	{
		for (int i = 0; i < m_weight.size(); ++i)
		{
			m_fitness[i] = (m_preMemoryMaxObj - m_obj[i] + 1) / gap;
			m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
		}
	}
	vector<double> weight(index.size()), fitness(index.size());
	for (int i = 0; i < weight.size(); ++i)
	{
		fitness[i] = (m_preMemoryMaxObj - this->m_pop[index[i]]->data().m_obj[0]) / gap;
		weight[i] = 1 / (1 + exp(-fitness[i]));
	}
	int z = 0; //改进个体按顺序1，2，...出现
	for (int i = 0; i < m_exMemory.size(); ++i)
	{
		int exMemorySize = m_exMemory[i].size();
		for (int j = 0; j < exMemorySize; ++j)
		{
			if (this->m_pop[i]->getFlag() == true)
			{
				if (m_memoryMinObj < m_preMemoryMinObj)
					m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] / (pow((j + 2), 2));
				else
					m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] * pow((j + 1), 2) / pow((j + 2), 2);
				if (j == m_exMemory[i].size() - 1)
				{
					if (m_weight[m_exMemory[i][j]] / weight[z] < m_wt) {
						int pos = m_exMemory[i][j];
						m_weight[pos] = weight[z];
						m_obj[pos] = this->m_pop[i]->data().m_obj[0];
						m_fitness[pos] = fitness[z];
						m_exMemory[i].pop_back();
						m_exMemory[i].push_front(pos);
						for (int e1 = 0; e1 < this->m_numDim; e1++)
							//m_hisIndi[pos][e1] = this->m_pop[i]->data().m_x[e1];
							m_hisIndi[pos].data().m_x[e1] = this->m_pop[i]->data().m_x[e1];
					}
					else {
						m_obj.push_back(this->m_pop[i]->data().m_obj[0]);
						m_fitness.push_back(fitness[z]);
						m_weight.push_back(weight[z]);
						m_exMemory[i].push_front(m_weight.size() - 1);
						//vector<int> tempIndiv(this->m_numDim);
						//for (int e1 = 0; e1 < this->m_numDim; e1++)
						//	tempIndiv[e1] = this->m_pop[i]->data().m_x[e1];
						//m_hisIndi.push_back(tempIndiv);
						m_hisIndi.push_back(*this->m_pop[i]);
					}
					z++;
				}
			}
			else
			{
				if (m_memoryMinObj < m_preMemoryMinObj)
					m_weight[m_exMemory[i][j]] = m_weight[m_exMemory[i][j]] / pow((j + 1), 2);
			}
		}
	}

	m_MELOperator->updatePro(m_hisIndi, m_weight);

	vector<double>::iterator maxObj = max_element(m_obj.begin(), m_obj.end());
	m_preMemoryMaxObj = *maxObj;
}

template<typename ED, typename TypeIndi,typename TypePop>
void MEL<ED, TypeIndi,TypePop>::updateMemory_BSF(const vector<int>& index)
{
	m_memoryMaxObj = this->m_pop[0]->data().m_obj[0];
	for (int i = 1; i < this->m_popsize; ++i) {
		if (m_memoryMaxObj < this->m_pop[i]->data().m_obj[0])
			m_memoryMaxObj = this->m_pop[i]->data().m_obj[0];
	}

	if ((m_memoryMinObj < m_preMemoryMinObj) || (m_memoryMaxObj != m_preMemoryMaxObj))
	{
		m_preMemoryMinObj = m_memoryMinObj;
		m_preMemoryMaxObj = m_memoryMaxObj;

		double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
		for (int i = 0; i < this->m_popsize; i++) {
			m_fitness[i] = (m_preMemoryMaxObj - this->m_pop[i]->data().m_obj[0] + 1) / gap;
			m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
		}
	}
	else {
		double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
		for (int i = 0; i < index.size(); ++i)
		{
			m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_pop[index[i]]->data().m_obj[0] + 1) / gap;
			m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
		}
	}

	m_MELOperator->updatePro(this->m_pop, m_weight);
}

template<typename ED, typename TypeIndi,typename TypePop>
void MEL<ED, TypeIndi,TypePop>::updateMemory_CI(const vector<int>& index)
{
	m_preMemoryMaxObj = this->m_pop[index[0]]->data().m_obj[0];
	m_preMemoryMinObj = this->m_pop[index[0]]->data().m_obj[0];
	for (int i = 1; i < index.size(); ++i)
	{
		if (m_preMemoryMaxObj < this->m_pop[index[i]]->data().m_obj[0])
			m_preMemoryMaxObj = this->m_pop[index[i]]->data().m_obj[0];
		if (m_preMemoryMinObj > this->m_pop[index[i]]->data().m_obj[0])
			m_preMemoryMinObj = this->m_pop[index[i]]->data().m_obj[0];
	}
	double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
	for (int i = 0; i < index.size(); ++i)
	{
		m_fitness[index[i]] = (m_preMemoryMaxObj - this->m_pop[index[i]]->data().m_obj[0] + 1) / gap;
		m_weight[index[i]] = 1 / (1 + exp(-m_fitness[index[i]]));
	}

	m_MELOperator->updatePro(this->m_pop, m_weight, &index);
}

template<typename ED, typename TypeIndi,typename TypePop>
void MEL<ED, TypeIndi,TypePop>::updateMemory_C()
{
	m_preMemoryMaxObj = m_curPop[0].data().m_obj[0];
	m_preMemoryMinObj = m_curPop[0].data().m_obj[0];
	for (int i = 1; i < this->m_popsize; ++i)
	{
		if (m_preMemoryMaxObj < m_curPop[i].data().m_obj[0])
			m_preMemoryMaxObj = m_curPop[i].data().m_obj[0];
		if (m_preMemoryMinObj > m_curPop[i].data().m_obj[0])
			m_preMemoryMinObj = m_curPop[i].data().m_obj[0];
	}
	double gap = m_preMemoryMaxObj - m_preMemoryMinObj + 1;
	for (int i = 0; i < this->m_popsize; ++i)
	{
		m_fitness[i] = (m_preMemoryMaxObj - m_curPop[i].data().m_obj[0] + 1) / gap;
		m_weight[i] = 1 / (1 + exp(-m_fitness[i]));
	}

	m_MELOperator->updatePro(m_curPop, m_weight);
}

template<typename ED, typename TypeIndi,typename TypePop>
bool MEL<ED, TypeIndi,TypePop>::ifTerminating()
{
	if (dynamic_cast<TermMean*>(this->m_term.get())->ifTerminating()) {
		if (this->m_iter == 0) return false;
		return true;
	}

	return false;
}

template<typename ED, typename TypeIndi,typename TypePop>
ReturnFlag MEL<ED, TypeIndi,TypePop>::updata()
{
	ReturnFlag rf = Return_Normal;
	rf = m_MELOperator->updateSolutions(this->m_pop, m_curPop, m_impRadio, m_ms);
	return rf;
}

#endif // !MEL_H

