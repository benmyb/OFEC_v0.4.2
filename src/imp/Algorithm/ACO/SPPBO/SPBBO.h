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

Paper: Ying-Chi Lin. "Simple Probabilistic Population-Based Optimization" 
IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, APRIL 2016
*************************************************************************/
// Created: 5 April 2016
// Last modified:

#ifndef SPBBO_H
#define SPBBO_H
#include "../../Population.h"
#include "../Ant.h"

class SPBBO :public Population<CodeVInt, Ant> {
	enum Stra { SPBBO1 = 0, SPBBO7 };

public:
	SPBBO(ParamMap &v);
	void setDefaultPara();
	ReturnFlag updateSelectPro();
	ReturnFlag run_();
	bool ifTerminating();
	void resetAntsInfo();
	void initializeSystem();

private:
	vector<vector<double>> m_pro;
	double m_saveFre;
	int m_num, m_impRadio;
	int m_k;
	double m_total, m_r, m_elite;
	int m_alpha, m_beta;
	Stra m_stra;

	list<Ant> m_gp;
	vector<Ant> m_pb;
	bool m_isHaveGlobalBest, m_isHavePersonBest;
	Solution<CodeVInt> m_globalBest; //the globally best tour from the beginning of the trial
};

#endif //SPBBO_H