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
// Created: 29 Oct. 2011
// Last modified: 09 Dec. 2014
/*
Parrott, D.; Xiaodong Li; , "Locating and tracking multiple dynamic optima
by a particle swarm model using speciation," Evolutionary Computation, IEEE Transactions
on , vol.10, no.4, pp.440-458, Aug. 2006.
*/
#ifndef SPECIESSWARM_H
#define SPECIESSWARM_H

#include "SpeciesParticle.h"
#include "../Swarm.h"


class SpeciesSwarm : public Swarm<CodeVReal,SpeciesParticle>
{
    public:
        SpeciesSwarm();

        SpeciesSwarm(const SpeciesSwarm& other);
        SpeciesSwarm(ParamMap& ); //int rPopsize,int pMax, double radius,bool mode=true
        SpeciesSwarm& operator=(const SpeciesSwarm& other);

        void findSeeds();
        bool removeConvergence();
        void checkOvercrowd();
		ReturnFlag evolve();
        ReturnFlag run_();
        void setPMax(int rPmax);
    private:
    	int m_numSeeds;
        vector<int> mv_seedList;
        int m_pMax;
};

#endif // SPECIESSWARM_H
