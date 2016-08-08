/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 20 Apr 2016
// Last modified:


#ifndef MELOPERATOR_H
#define MELOPERATOR_H

#include "../../ACO/Ant.h"
using namespace std;

template<typename TypeIndi>
class MELOperator {
protected:	
public:
	MELOperator() {}
	~MELOperator() {}
	virtual void updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index = nullptr) = 0;
	virtual void updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index = nullptr) = 0;
	//virtual void updatePro(const vector<vector<int>> &pop, const vector<double>& weight) = 0;
	virtual void createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha) = 0;
	virtual ReturnFlag updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms) = 0;
};

#endif //MELOPERATOR_H