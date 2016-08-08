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
// Last modified:
#ifndef RAED_H
#include "RADEIndi.h"
#include "RAI.h"
#include "../../DE/DEPopulation.h"
class RADE :public RAI<DEPopulation<CodeVReal, RADEIndi>> {
protected:
	int m_case;	
protected:
	ReturnFlag evolve();
	int select( vector<int> &candidate, bool bias);
public:
	RADE(ParamMap &v);
	ReturnFlag run_();
	void mutate(const int idx);
};

inline int RADE::select( vector<int> &candidate, bool bias) {
	// select one indiv based on fitness
	int idx = Global::msp_global->getRandInt(0, candidate.size());
	int idxp = candidate[idx];
	if (bias) {
		while (Global::msp_global->mp_uniformAlg->Next() > m_pop[idxp]->fitness()) {
			idx = Global::msp_global->getRandInt(0, candidate.size());
			idxp = candidate[idx];
		}
	}
	candidate.erase(candidate.begin() + idx);
	return idxp;
}
#endif // !RAED_H

