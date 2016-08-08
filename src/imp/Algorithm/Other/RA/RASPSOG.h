/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 07 May 2016
#ifndef RASPSOG_H
#define RASPSOG_H
#include "../../PSO/PSO_G/SwarmGBest.h"
#include "RAI.h"
class RASPSOG :public RAI<SwarmGBest>{
protected:
	ReturnFlag evolve();
public:
	RASPSOG(ParamMap& v);
	ReturnFlag run_();
};
#endif // !RASPSOG_H

