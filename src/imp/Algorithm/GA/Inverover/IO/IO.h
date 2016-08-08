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

Tao, G. and Z. Michalewicz (1998). "Inver-over oprator for TSP." 
Parallel Problem Solving from Nature.
*************************************************************************/
// Created: 28 Jan 2015
// Last modified:

#ifndef IO_H
#define IO_H

#include "../../GAPopulation.h"
#include "../../GAIndividual.h"

class IO : public GAPopulation<CodeVInt,GAIndividual<CodeVInt>>
{
public:
	IO(ParamMap &v);
	~IO(){}
	void inverse(int g,int gl,GAIndividual<CodeVInt> &indivl);
	int choseNextCity(vector<int> &a,const int &g,double p,int n,int &gl,vector<int> &visited);
	bool ifTerminating();
	ReturnFlag run_();
	double diffEdges();
protected:
	int m_num;
	int m_saveFre;
};

#endif //IO_H