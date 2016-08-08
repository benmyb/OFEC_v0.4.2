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

#ifndef ADAPTPARTICLE_H
#define ADAPTPARTICLE_H

#include "../Particle.h"

class AdaptParticle :public Particle
{
public:
	AdaptParticle();
	~AdaptParticle(){}
	ReturnFlag move(const Solution<CodeVReal> &lbest,double w, double c1, double c2);
};

#endif //ADAPTPARTICLE_H