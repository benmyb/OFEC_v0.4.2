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
// Created: 15 Nov 2011
// Last modified:

#ifndef DYNDEINDIVIDUAL_H
#define DYNDEINDIVIDUAL_H

#include "../DEIndividual.h"
class DynDESubPop;
class DynPopDESubPop;
class DynDEIndividual : public DEIndividual
{	
	friend class DynDESubPop;
	friend class DynPopDESubPop;
public:
	enum IndividualType{TYPE_DE=0,TYPE_ENTROPY_DE,TYPE_QUANTUM,TYPE_BROWNIAN};
        DynDEIndividual();
		DynDEIndividual(const DynDEIndividual & indi);

        ~DynDEIndividual();
        void setRandomMutationScheme();
        void setMutationScheme(DEMutationStratgy s);
        void mutate(double F, Solution<CodeVReal> *r1,  Solution<CodeVReal> *r2,  Solution<CodeVReal> *r3, Solution<CodeVReal> *r4=0,  Solution<CodeVReal> *r5=0);
        void recombine(double CR);
        ReturnFlag brownian(const Solution<CodeVReal> &best,double sigma);
        ReturnFlag quantum(const Solution<CodeVReal> &best,double r);
		ReturnFlag entropy(double sigma);
private:
        DEMutationStratgy m_mutStrategy;
        IndividualType m_type;
};

#endif // DYNDEINDIVIDUAL_H
