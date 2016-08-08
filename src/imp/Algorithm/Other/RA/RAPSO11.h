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
#ifndef RAPSO11_H
#define RAPSO11_H
#include"RAI.h"
#include "../../PSO/SPSO11/SPSO11.h"

class RASPSO11:public RAI<SPSO11> {
protected:
	ReturnFlag evolve();
public:
	RASPSO11(ParamMap& v);
	ReturnFlag run_();
};
#endif // !RAPSO11

