/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 16 Jan 2015
// Last modified:

#ifndef ESCA_H
#define ESCA_H

#include "../../DE/CRDE/CRDEPopulation.h"
#include "../../PSO/PSO_G/SwarmGBest.h"

class ESCA: public Algorithm
{
public:
	ESCA(ParamMap &v);
	~ESCA(){}
	void setPara(double rTheta, int rSigma);
	ReturnFlag run_();
protected:
	ReturnFlag evolve();
	void handleReturnFlag(ReturnFlag f);

private:
	SwarmGBest m_GBest;
	CRDEPopulation m_mCRDE,m_CRDE;
	double m_theta;
	int m_sigma;
};

#endif //ESCA_H