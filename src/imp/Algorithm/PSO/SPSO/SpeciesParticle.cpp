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
// Last modified:
#include "SpeciesParticle.h"
#include "../../../Global/global.h"

SpeciesParticle::SpeciesParticle():Particle(),m_lbestIdx(-1),m_seedship(false),m_speciesship(false)
{
    //ctor
}

SpeciesParticle::~SpeciesParticle()
{
    //dtor
}

SpeciesParticle::SpeciesParticle( const SpeciesParticle& other)
{
    //copy ctor
     m_lbestIdx=other.m_lbestIdx;
    m_seedship=other.m_seedship;
    m_speciesship=other.m_speciesship;
}

SpeciesParticle& SpeciesParticle::operator=(const SpeciesParticle& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    Particle::operator=(rhs);
    m_lbestIdx=rhs.m_lbestIdx;
    m_seedship=rhs.m_seedship;
    m_speciesship=rhs.m_speciesship;
    return *this;
}
ReturnFlag SpeciesParticle::move( const Solution<CodeVReal> &lbest,double w, double c1, double c2){

    double u,l;
	for( int j=0;j<GET_NUM_DIM;j++){
		CAST_PROBLEM_CONT->getSearchRange(l,u,j);
		double x=data()[j];
		m_vel[j]=w*(m_vel[j]+c1*Global::msp_global->mp_uniformAlg->Next()*((m_pbest.data()[j])-x)+c2*Global::msp_global->mp_uniformAlg->Next()*((lbest.data()[j])-x));

		if(m_vel[j]>m_vMax[j].m_max)	m_vel[j]=m_vMax[j].m_max;
		else if(m_vel[j]<m_vMax[j].m_min)		m_vel[j]=m_vMax[j].m_min;

		data()[j]=x+m_vel[j];
    }
    self().validate();
	return self().evaluate();
}
