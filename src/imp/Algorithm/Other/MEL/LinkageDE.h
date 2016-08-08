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
#ifndef LinkageDE_H
#define LinkageDE_H

#include "../../DE/DEPopulation.h"
#include "IndiLinkageDE.h"
#include "MEL.h"

class LinkageDE :public DEPopulation<CodeVReal, IndiLinkageDE> {
public:
	LinkageDE(int size);
	
	ReturnFlag evolve();
	void mutate(const int idx, const int gidx);
protected:
	int m_mutationScheme;
};

class LinkageDE_ :public MEL<CodeVReal, IndiLinkageDE, LinkageDE> {
public:
	LinkageDE_(ParamMap &v);
	ReturnFlag run_();
};
#endif // !LinkageDE_H

