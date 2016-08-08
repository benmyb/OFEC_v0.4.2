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
// Created:  29 Oct. 2011
// Last modified: 09 Dec. 2014
#ifndef SPECIESPARTICLE_H
#define SPECIESPARTICLE_H

#include "../Particle.h"

class SpeciesSwarm;
class SpeciesParticle : public Particle
{
	friend class SpeciesSwarm;
    public:
        SpeciesParticle();
        ~SpeciesParticle();
        SpeciesParticle( const SpeciesParticle& );
        SpeciesParticle& operator=( const SpeciesParticle& );
        ReturnFlag move( const Solution<CodeVReal> & lbest ,double w, double c1, double c2);
    private:
        int m_lbestIdx;
        bool m_seedship;
        bool m_speciesship;
};

#endif // SPECIESPARTICLE_H
