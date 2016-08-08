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

Xia, Yong; Li, Changhe; Zeng, Sanyou. "Three New Heuristic Strategies For Solving Travelling Salesman Problem".
Advances in Swarm Intelligence (2014-01-01) 8794: 181-188 , January 01, 2014
*************************************************************************/
// Created: 28 Jan 2015
// Last modified:

#ifndef IOPOP_H
#define IOPOP_H

#include "../../GAPopulation.h"
#include "../../GAIndividual.h"

class IOPM : public GAPopulation<CodeVInt,GAIndividual<CodeVInt>>
{
public:
	enum strategy { ALL, Elism };
	IOPM(ParamMap &v);
	~IOPM(){}
	void inverse(int g,int gl,GAIndividual<CodeVInt> &indivl);
	bool ifTerminating();
	void populProMatrix();
	ReturnFlag run_();
	void updata(bool flag=true);
	double diffEdges();
protected:
	int m_num;
	int m_saveFre;
	vector<vector<double> > mvv_proMatrix;

	vector<bool> mv_flag;
	strategy m_staType;
	int m_preDiffs;
	vector<double> m_weight;
};

#endif //IOPOP_H