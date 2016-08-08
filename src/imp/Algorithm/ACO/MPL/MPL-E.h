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
// Created: 8 March 2016
// Last modified:

#ifndef MPL_E_H
#define MPL_E_H

#include "../../Population.h"
#include "../../Individual.h"
#include "../Ant.h"

//for symmetric TSP

class MPL_E : public Population<CodeVInt, Ant>
{
protected:
	vector<vector<double> > mvv_edgesInfo;
	int m_saveFre;
	int m_num;

	int m_impRadio;
	void statisticDiffsAndImp(bool flag = true);
	vector<double> m_weight;
public:
	MPL_E(ParamMap &v);
	~MPL_E();
	void statisticEdgeInfo();
	void initializeSystem();
	void resetAntsInfo();
	bool ifTerminating();
	ReturnFlag run_();
	ReturnFlag update();
};

#endif