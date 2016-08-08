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

SIM: Simple Inversion Mutation
Article: (Development a new mutation operator to solve the Traveling Salesman Problem by aid of Genetic Algorithms)

Initial population: Nearest Neighbor tour construction heuristic
*************************************************************************/
// Created: 13 Nov 2015
// Last modified:

#ifndef SIM_H
#define SIM_H

#include "../GAPopulation.h"
#include "../GAIndividual.h"

class SIM : public GAPopulation<CodeVInt, GAIndividual<CodeVInt>>
{
public:
	SIM(ParamMap &v);
	~SIM(){}
	void selection(GAIndividual<CodeVInt> &indivl1, GAIndividual<CodeVInt> &indivl2);
	GAIndividual<CodeVInt> crossover(const GAIndividual<CodeVInt> &indivl1, const GAIndividual<CodeVInt> &indivl2);
	void mutation(GAIndividual<CodeVInt> &indivl);
	bool ifTerminating();
	ReturnFlag run_();

private:
	int m_num;
	int m_saveFre;
	int m_selectK;
	double m_PC, m_PM;
};

#endif //SIM_H