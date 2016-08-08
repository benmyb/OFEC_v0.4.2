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

Algorithm: Greedy Sub Tour Mutation (GSTM) algorithm
Article: (Development a new mutation operator to solve the Traveling Salesman Problem by aid of Genetic Algorithms)

Crossover: distance preserving crossover (DPX) (pro=0.8)
Article: (A Genetic Local Search Algorithm for Solving Symmetric and Asymmetric Traveling Salesman Problems)

Mutation: GSTM (pro=0.2)

Selection method: Tournament Selection (æ∫±Í»¸—°‘Ò) (k=5)

Initial population: Nearest Neighbor tour construction heuristic
*************************************************************************/
// Created: 13 Nov 2015
// Last modified:

#ifndef GSTM_H
#define GSTM_H

#include "../GAPopulation.h"
#include "../GAIndividual.h"

class GSTM : public GAPopulation<CodeVInt, GAIndividual<CodeVInt>>
{
public:
	GSTM(ParamMap &v);
	~GSTM(){}
	void selection(GAIndividual<CodeVInt> &indivl1, GAIndividual<CodeVInt> &indivl2);
	GAIndividual<CodeVInt> crossover(const GAIndividual<CodeVInt> &indivl1, const GAIndividual<CodeVInt> &indivl2);
	void mutation(GAIndividual<CodeVInt> &indivl);
	bool ifTerminating();
	ReturnFlag run_();

private:
	int m_num;
	int m_saveFre;
	int m_selectK;
	double m_PL;
	int m_NLMax;
	int m_LMIN, m_LMAX;
	double m_PRC, m_PCP;
	double m_PC, m_PM;
};

#endif //GSTM_H