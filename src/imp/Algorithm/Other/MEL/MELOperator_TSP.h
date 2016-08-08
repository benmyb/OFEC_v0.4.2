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
// Created: 20 Apr 2016
// Last modified:


#ifndef MELOPERATOR_TSP_H
#define MELOPERATOR_TSP_H

#include "MELOperator.h"

template<typename TypeIndi>
class MELOperator_TSP :public MELOperator<TypeIndi> {
public:
	MELOperator_TSP(int numDim);
	~MELOperator_TSP() {}
	void updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
	void updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
	//void updatePro(const vector<vector<int>> &pop, const vector<double>& weight);
	void createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha);
	ReturnFlag updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms);
protected:
	vector<vector<double> > mvv_pro;
};


template<typename TypeIndi>
MELOperator_TSP<TypeIndi>::MELOperator_TSP(int numDim) {
	mvv_pro.resize(numDim);
	for (int i = 0; i < numDim; i++)
		mvv_pro[i].resize(numDim);
}


template<typename TypeIndi>
void MELOperator_TSP<TypeIndi>::updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index) {
	int numDim = mvv_pro.size();
	int popsize = pop.size();
	for (int i = 0; i < numDim; ++i)
		for (int j = 0; j < numDim; ++j)
			mvv_pro[i][j] = 0;
	int e1, e2;
	if (index == nullptr) {
		for (int i = 0; i < popsize; ++i)
		{
			e1 = pop[i]->data().m_x[0];
			for (int j = 0; j < numDim; ++j)
			{
				e2 = pop[i]->data().m_x[(j + 1) % numDim];
				mvv_pro[e1][e2] += weight[i];
				mvv_pro[e2][e1] = mvv_pro[e1][e2];
				e1 = e2;
			}
		}
	}
	else {
		for (int i = 0; i < (*index).size(); ++i)
		{
			e1 = pop[(*index)[i]]->data().m_x[0];
			for (int j = 0; j < numDim; ++j)
			{
				e2 = pop[(*index)[i]]->data().m_x[(j + 1) % numDim];
				mvv_pro[e1][e2] += weight[(*index)[i]];
				mvv_pro[e2][e1] = mvv_pro[e1][e2];
				e1 = e2;
			}
		}
	}
}

template<typename TypeIndi>
void MELOperator_TSP<TypeIndi>::updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index) {
	int numDim = mvv_pro.size();
	int popsize = pop.size();
	for (int i = 0; i < numDim; ++i)
		for (int j = 0; j < numDim; ++j)
			mvv_pro[i][j] = 0;
	int e1, e2;
	if (index == nullptr) {
		for (int i = 0; i < popsize; ++i)
		{
			e1 = pop[i].data().m_x[0];
			for (int j = 0; j < numDim; ++j)
			{
				e2 = pop[i].data().m_x[(j + 1) % numDim];
				mvv_pro[e1][e2] += weight[i];
				mvv_pro[e2][e1] = mvv_pro[e1][e2];
				e1 = e2;
			}
		}
	}
	else {
		for (int i = 0; i < (*index).size(); ++i)
		{
			e1 = pop[(*index)[i]].data().m_x[0];
			for (int j = 0; j < numDim; ++j)
			{
				e2 = pop[(*index)[i]].data().m_x[(j + 1) % numDim];
				mvv_pro[e1][e2] += weight[(*index)[i]];
				mvv_pro[e2][e1] = mvv_pro[e1][e2];
				e1 = e2;
			}
		}
	}
}

/*template<typename TypeIndi>
void MELOperator_TSP<TypeIndi>::updatePro(const vector<vector<int>> &pop, const vector<double>& weight) {
	int numDim = mvv_pro.size();
	for (int i = 0; i < numDim; ++i)
		for (int j = 0; j < numDim; ++j)
			mvv_pro[i][j] = 0;
	int e1, e2;
	for (int i = 0; i < pop.size(); ++i)
	{
		e1 = pop[i][0];
		for (int j = 0; j < numDim; ++j)
		{
			e2 = pop[i][(j + 1) % numDim];
			mvv_pro[e1][e2] += weight[i];
			mvv_pro[e2][e1] += weight[i];
			e1 = e2;
		}
	}
}
*/

template<typename TypeIndi>
void MELOperator_TSP<TypeIndi>::createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha) {
	int popsize = pop.size();
	int numDim = mvv_pro.size();
	for (int j = 0; j<popsize; j++) {
		curPop[j].initializeIteIndivl(pop[j]->self());
		for (int i = 1; i<numDim; i++) {
			curPop[j].selectNext(*pop[j], mvv_pro, alpha);
		}
	}
}

template<typename TypeIndi>
ReturnFlag MELOperator_TSP<TypeIndi>::updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms) {
	ReturnFlag rf = Return_Normal;
	int popsize = pop.size();
	int numDim = mvv_pro.size();
	impRadio = 0;

	for (int i = 0; i < popsize; i++)
		if (pop[i]->getFlag() == true)
			impRadio++;

	for (int i = 0; i < popsize; i++)
	{
		if (curPop[i].getFlag() == true)
		{
			if (pop[i]->getFlag() == true)
				rf = pop[i]->evaluate();
			else
				rf = curPop[i].evaluate();
			if (rf == Return_Terminate) break;

			if (pop[i]->getFlag() == true && ms == MELACO::c)
				rf = curPop[i].evaluate();
		}
		else {
			curPop[i].data().m_obj = pop[i]->data().m_obj;
		}
	}

	return rf;
}
#endif //MELOPERATOR_TSP_H
