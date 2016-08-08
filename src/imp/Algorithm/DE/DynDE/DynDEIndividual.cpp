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

#include "DynDEIndividual.h"
#include "../../../Global/global.h"
#include "../../../Problem/ContinuousProblem.h"

DynDEIndividual::DynDEIndividual():DEIndividual()
{
    //ctor
}

DynDEIndividual::~DynDEIndividual()
{
    //dtor
}
DynDEIndividual::DynDEIndividual(const DynDEIndividual & indi):DEIndividual(indi){
	m_mutStrategy=indi.m_mutStrategy;
	m_type=indi.m_type;
}
void DynDEIndividual::mutate(double m_F,Solution<CodeVReal> *r1,  Solution<CodeVReal> *r2,  Solution<CodeVReal> *r3, Solution<CodeVReal> *r4,  Solution<CodeVReal> *r5){
    double l,u;
	for(int i=0;i<GET_NUM_DIM;i++){
	    m_F=Global::msp_global->mp_uniformAlg->Next();
		CAST_PROBLEM_CONT->getSearchRange(l,u,i);
		m_pv.data()[i]=(r1->data()[i])+m_F*((r2->data()[i])-(r3->data()[i]));
		if(r4&&r5) m_pv.data()[i]=(m_pv.data()[i])+m_F*((r4->data()[i])-(r5->data()[i]));

		if((m_pv.data()[i])>u){
			m_pv.data()[i]=((r1->data()[i])+u)/2;
		} else if((m_pv.data()[i])<l) {
			m_pv.data()[i]=((r1->data()[i])+l)/2;
		}
	}
}
void DynDEIndividual::recombine(double m_CR){
	int I=Global::msp_global->getRandInt(0,GET_NUM_DIM);
    m_CR=Global::msp_global->mp_uniformAlg->Next();
	for(int i=0;i<GET_NUM_DIM;i++){
		double p=Global::msp_global->mp_uniformAlg->Next();
		if(p<=m_CR||i==I)     m_pu.data()[i]=m_pv.data()[i];
		else m_pu.data()[i]=data()[i];
	}
}
ReturnFlag DynDEIndividual::brownian(const Solution<CodeVReal> &best,double sigma){
    for(int i=0;i<GET_NUM_DIM;i++){
		data()[i]=(best.data()[i])+Global::msp_global->mp_normalAlg->NextNonStand(0,sigma); 
	}
	
	 self().validate();
	 
	 return self().evaluate();
}
ReturnFlag DynDEIndividual::quantum(const Solution<CodeVReal> &best,double rcloud){

    vector<double> x(GET_NUM_DIM,0);
    double dis=0;
    for( int i=0;i<GET_NUM_DIM;i++){
        x[i]=Global::msp_global->mp_normalAlg->Next();
        dis+=x[i]*x[i];
    }
    dis=sqrt(dis);
	double r=Global::msp_global->getRandFloat(0,rcloud);
    for( int i=0;i<GET_NUM_DIM;i++){
		data()[i]=(best.data()[i])+r*x[i]/dis;
    }
    x.clear();	
	self().validate();
	return self().evaluate();
}
ReturnFlag DynDEIndividual::entropy(double sigma){
	 for(int i=0;i<GET_NUM_DIM;i++){
		data()[i]=(data()[i])+Global::msp_global->mp_normalAlg->NextNonStand(0,sigma);
	}
	 
	 self().validate();
	 return self().evaluate();
}