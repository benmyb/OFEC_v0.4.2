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
// Created: 31 December 2014
// Last modified:

#ifndef ZDT2_H
#define ZDT2_H

#include "ZDT.h"

class ZDT2 :public ZDT
{
public:
	ZDT2(ParamMap &v);
	~ZDT2(){};
protected:
	void evaluate__(double const *x,vector<double>& obj);
};

#endif //ZDT2_H