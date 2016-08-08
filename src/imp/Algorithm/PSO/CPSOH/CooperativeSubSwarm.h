/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 18 Jan 2015
// Last modified:

#ifndef COOPERATIVESUBSWARM_H
#define COOPERATIVESUBSWARM_H

#include "CooperativeParticle.h"
#include "../Swarm.h"

class CPSOH;
class CooperativeSubSwarm :public Swarm<CodeVReal,CooperativeParticle>
{
	friend class CPSOH;
public:
	CooperativeSubSwarm(int popsize,int ID,int popNum,bool mode);
	CooperativeSubSwarm();
	~CooperativeSubSwarm(){}	
protected:
	ReturnFlag evolve(Solution<CodeVReal> & msp_gbest);
	vector<int> mv_members;
	int m_numbers;

};


#endif //COOPERATIVESUBSWARM_H