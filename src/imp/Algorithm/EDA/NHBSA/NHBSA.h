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

Paper: Shigeyoshi Tsutsui, "Node Histogram vs. Edge Histogram: A Comparison of Probabilistic
Model-Building Genetic Algorithms in Permutation Domains".
2006 IEEE Congress on Evolutionary Computation.
*************************************************************************/
// Created: 12 April 2016
// Last modified:

#ifndef NHBSA_H
#define NHBSA_H
#include "../../Population.h"
#include "../../Individual.h"

class NHBSA :public Population<CodeVInt, Individual<CodeVInt>> {
	enum Stra { WO = 0, WT };
public:
	NHBSA(ParamMap &v);
	void setDefaultPara();
	void initializePro();
	void updatePro(const Individual<CodeVInt>& newIndivl, int index);
	ReturnFlag run_();
	bool ifTerminating();
	void statisticDiffsAndImp();
	void resetIndividualInfo();
	ReturnFlag sampleSolution();
	void chooseCity(int loc);
	vector<int> generateRandomPositionIndex();

private:
	vector<vector<double>> m_pro;
	double m_saveFre, m_bRatio;
	int m_num, m_n, m_impRadio;
	Stra m_stra;
	vector<bool> m_tabu;
	Individual<CodeVInt> m_newIndivl;
	vector<int> mv_cut;
};

#endif //NHBSA_H