/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 23 Nov 2014
// Last modified:
/*
M. du Plessis and A. Engelbrecht, ¡°Differential evolution for dynamic environments 
with unknown numbers of optima¡±, J. of Global Optim., 2012 (DOI: 10.1007/s10898-012-9864-9).
//WARNING: this algorithm does not consider the increasing number of populations, causing huge number of populations, 
which will slow down the run. 
*/
#ifndef DYNPOPDEPOPULATION_H
#define DYNPOPDEPOPULATION_H

#include "DynPopDESubPop.h"
#include "../../MultiPopulationCont.h"

class DynPopDEPopulation : public Algorithm,public MultiPopulationCont<DynPopDESubPop>{
public:
    DynPopDEPopulation(ParamMap &v);
    ~DynPopDEPopulation();
	ReturnFlag run_();
	bool isStagnation();
	void updateRadius();
private:      
	double m_exclRadius; // radius of exlusion radius
	int m_numNormal;    // the number of normal individuals of each swarm
	int m_numBrownian;       // the number of brownian individuals of each swarm
	int m_worstPop;		 
private:
	void exclude();
	int getHighestPerformancePop();
	void setDefault();
};

#endif // DYNPOPDEPOPULATION_H
