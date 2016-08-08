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

/*F. van den Bergh, A. P. Engelbrecht, Cooperative approach to particle
swarm optimization,?IEEE Trans. Evol. Comput., vol. 8, pp. 225?39,
Jun. 2004.*/

#ifndef CPSOH_H
#define CPSOH_H

#include "CooperativeParticle.h"
#include "CooperativeSubSwarm.h"
#include "../../MultiPopulationCont.h"
#include "../../PSO/PSO_G/SwarmGBest.h"

class CPSOH : public MultiPopulationCont<CooperativeSubSwarm>, public SwarmGBest
{
public:
	CPSOH(ParamMap &v);
	~CPSOH();
	CPSOH &operator=( CPSOH & rs);
	ReturnFlag run_();
	ReturnFlag evolve();
	ReturnFlag updateGlobalBest();
protected:
	unique_ptr<Solution<CodeVReal>> msp_gbest;
};


#endif //CPSOH_H