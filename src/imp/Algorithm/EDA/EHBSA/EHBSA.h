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

Paper: Shigeyoshi Tsutsui, "Probabilistic Model-Building Genetic Algorithms
in Permutation Representation Domain
Using Edge Histogram".
J.J. Merelo Guerv¨®s et al. (Eds.): PPSN VII, LNCS 2439, pp. 224¨C233, 2002.

Note: the implemented algorithm just suits for the symmetrical problem. For
asymmetrical problem, you can see the paper "Node Histogram vs. Edge Histogram: A Comparison of Probabilistic
Model-Building Genetic Algorithms in Permutation Domains".
*************************************************************************/
// Created: 11 April 2016
// Last modified:

#ifndef EHBSA_H
#define EHBSA_H
#include "../../Population.h"
#include "../../Individual.h"

class EHBSA :public Population<CodeVInt, Individual<CodeVInt>> {
	enum Stra { WO = 0, WT};
public:
	EHBSA(ParamMap &v);
	void setDefaultPara();
	void initializePro();
	void updatePro(const Individual<CodeVInt>& newIndivl, int index);
	ReturnFlag run_();
	bool ifTerminating();
	void statisticDiffsAndImp();
	void resetIndividualInfo();
	ReturnFlag sampleSolution();
	void chooseNextCity(int loc);

private:
	vector<vector<double>> m_pro;
	double m_saveFre, m_bRatio;
	int m_num, m_n, m_impRadio;
	Stra m_stra;
	vector<bool> m_tabu;
	Individual<CodeVInt> m_newIndivl;
	vector<int> mv_cut;
};

#endif //EHBSA_H