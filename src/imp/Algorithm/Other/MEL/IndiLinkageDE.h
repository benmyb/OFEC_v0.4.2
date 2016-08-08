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
// Created: 14 May 2016
// Last modified:
#ifndef IndiLinkageDE_H
#define IndiLinkageDE_H
#include "../../DE/DEIndividual.h"
#include"IndiLinkage.h"

class IndiLinkageDE: public IndiLinkage<CodeVReal, DEIndividual> {
public:
	using DEIndividual::mutate;
	using DEIndividual::recombine;
	using DEIndividual::select;
	using DEIndividual::initialize;
	
	IndiLinkageDE();
	ReturnFlag initialize(int rIdx, int rID, int rPopsize, bool mode = true);
	void mutate(const int gidx,double F, Solution<CodeVReal> *r1, Solution<CodeVReal> *r2, Solution<CodeVReal> *r3, Solution<CodeVReal> *r4 = 0, Solution<CodeVReal> *r5 = 0);
	void recombine(const int gidx, double CR);
	ReturnFlag select(const int gidx);
	//Solution<CodeVReal>& trial();
};
#endif // !IndiLinkageDE_H
