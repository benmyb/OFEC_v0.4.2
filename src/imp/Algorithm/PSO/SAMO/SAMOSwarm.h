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
// Created: 15 OCT 2012
// Last modified:

/*
T. M. Blackwell, ¡°Particle swarm optimization in dynamic environments¡±,
in Evolutionary Computation in Dynamic and Uncertain Environments, pp. 29¨C49, 2007.
*/
#ifndef SAMOSWARM
#define SAMOSWARM

#include "../Swarm.h"
#include "../../MultiPopulationCont.h"
#include "../mQSO/MQSOSubSwarm.h"

class SAMOSwarm: public Algorithm, public MultiPopulationCont<MQSOSubSwarm>
{
    public:
        SAMOSwarm(ParamMap &v);
        ReturnFlag run_();

    protected:
        int m_M;        // the number of populations

        double m_Rexcl; // radius of exlusion radius
        double m_Rconv; // threshold radius of conveged swarms
		int m_Nexcess;	// maximum number of free swarms allowed
		int m_Nfree;	// number of free swarms
		
		// 11-07-2013
		double m_ConvFactor;

		void checkConvergenceAll();
        void exclude();
		void updateRadius();
		int findWorstFreeSwarm();
		void initialize();				
};

#endif // SAMOSWARM
