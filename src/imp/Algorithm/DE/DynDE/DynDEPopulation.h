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
// Created: 25 Sep 2014
// Last modified:

/*
Mendes, R., Mohais, A.: DynDE: a differential evolution for dynamic optimization problems. In: Proceedings
of the IEEE Congress on Evolutionary Computation (CEC¡¯05), pp. 2808¨C2815. IEEE (2005)
*/
#ifndef DYNDEPOPULATION_H
#define DYNDEPOPULATION_H

#include "DynDESubPop.h"
#include "../../MultiPopulationCont.h"
class DynDEPopulation: public DEPopulation<CodeVReal,DynDEIndividual>,public MultiPopulationCont<DynDESubPop>
{
	enum{f_reinit=0};
    public:
        DynDEPopulation(ParamMap &v);	
        virtual ~DynDEPopulation();
        void setDefault(ParamMap &v);		
		ReturnFlag run_();
		void exclude();
    public:
        int m_M;        // the number of populations
        double m_Rexcl; // radius of exlusion radius
        
};

#endif // DYNDEPOPULATION_H
