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
// Created: 21 July 2011
// Last modified:
#ifndef FWEIERSTRASS_H
#define FWEIERSTRASS_H
#include "BenchmarkFunction.h"

class FWeierstrass : public BenchmarkFunction
{
    public:
        FWeierstrass(ParamMap &v);
        FWeierstrass(const int rId, const int rDimNumber, string& rName);
        virtual ~FWeierstrass();
    protected:
        void initialize();
        void evaluate__(double const *x,vector<double>& obj);
    private:
};

#endif // FWEIERSTRASS_H
