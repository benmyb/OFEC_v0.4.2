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
// Created: 18 Jan 2015
// Last modified:

/*Z. Zhan, J. Zhang, Y. Li, and H. S. Chung, adaptive particle swarm
optimization, IEEE Trans. Syst., Man, Cybern. B: Cybern., vol. 39,
pp. 1362-381, Dec. 2009.*/

#ifndef APSO_H
#define APSO_H

#include "AdaptParticle.h"
#include "../Swarm.h"

enum state{Exploration=1, Exploitation, Convergence, JumpingOut};
class APSO : public Swarm<CodeVReal,AdaptParticle>{
private:
	double m_MinDis,m_MaxDis,m_BestDis;
	double m_ef; //evolutionary factor
	int m_CurGen;
	state m_CurState,m_PreState;
public:
	APSO(ParamMap &v);
	void calculateEF();
	ReturnFlag updateAcceleration();
	ReturnFlag elitismLearning();
	ReturnFlag evolve();
	ReturnFlag run_();
	void increaseC1(double weight=1.);
	void decreaseC1(double weight=1.);
	void increaseC2(double weight=1.);
	void decreaseC2(double weight=1.);
	void updateW(double f);

};

#endif //APSO_H