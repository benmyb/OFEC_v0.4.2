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

#ifndef HMSOSWARM_H
#define HMSOSWARM_H

#include "../../MultiPopulationCont.h"
#include "HmSOSubSwarm.h"

class HmSO :public MultiPopulationCont<HmSOSubSwarm>, public Algorithm
{
public:
	HmSO(ParamMap &v);
	~HmSO(){}
	ReturnFlag evolve();
    ReturnFlag run_();
protected:
    int m_parentSize,m_childSize;
    double m_Rexcl; // radius of exlusion radius
    double m_Rconv; // threshold radius of conveged swarms
    double m_epsilon; //threshold for cbest and gbest
    double m_gbest;     // the fi
protected:
    void setDefaultPar();
    void checkConvergence();
    void checkCllision();
    void updateGBest();
    ReturnFlag responseChange();
};

#endif //HMSOSWARM_H