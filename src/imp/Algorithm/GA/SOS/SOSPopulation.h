/*************************************************************************
* Project: Library of Evolutionary Algorithms
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
// Created: 30 Sep 2014
// Last modified:
/*
Branke, J.; Kau, T.; l Schmidt & Schmeck, H. (2000), A multi-population approach 
to dynamic optimization problems, in 'Fourth International Conference on Adaptive 
Computing in Design and Manufacture (ACDM 2000)' .
*/
//NOTE: many parameter setting not provided, e.g., the restrictions below on generation of child populations, 

//minimum and maximum number of individuals relative to overall amount
//minimum and maximum diameter of the subspaces relative to the size of the total search space
//minimum fitness of new forking populations relative to current overall best individual
//minimum fitness of existing forking populations relative to current overall best individual.

// the number of total populations not well controlled 

#ifndef SOSPOPULATION_H
#define SOSPOPULATION_H

#include "SOSSubPop.h"
#include "../../MultiPopulationCont.h"

class SOSPopulation: public GAPopulation<CodeVReal,GAIndividual<CodeVReal>>,public MultiPopulationCont<SOSSubPop>
{
    public:
       
		SOSPopulation(ParamMap &v);
		virtual ~SOSPopulation();
        void computeDesiredPopSize();
        void defaultParSet(int);
        ReturnFlag adjustPopulation();
        int removeOverlapping();
        ReturnFlag run_();

    public:
        int m_minSubSize;
        double m_ratioToBestFit;
        double m_alpha;
        int m_forkingIter;
        double m_range;
 
};

#endif // SOSPOPULATION_H
