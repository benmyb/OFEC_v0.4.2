/*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 20 Jan 2015
// Last modified:

#ifndef HMSOSUBSWARM_H
#define HMSOSUBSWARM_H

#include "../Swarm.h"
#include "../Particle.h"

class HmSOSubSwarm :public Swarm<CodeVReal,Particle>
{
public:
	HmSOSubSwarm(const int popsize,const int id, bool mode);
	HmSOSubSwarm(const Solution<CodeVReal> & center, double radius,const int rPopsize,const int id,bool mode);
	~HmSOSubSwarm(){}
	ReturnFlag waken();
	void updateCurRadius(bool mode=false);	
protected:
	void setDefaultPar();	
	double m_Rsearch;
	ReturnFlag evolve(){ return Return_Normal;}
};

#endif //HMSOSUBSWARM_H