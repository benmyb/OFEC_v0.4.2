/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@google.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 14 May 2016
// Last modified:
#ifndef MEL_OPERATOR_LINKAGE
#define MEL_OPERATOR_LINKAGE
#include "MELOperator.h"

template<typename TypeIndi>
class MELOperator_Linkage :public MELOperator<TypeIndi> {
public:
	MELOperator_Linkage(int numDim);
	~MELOperator_Linkage() {}
	void updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
	void updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index = nullptr);
	//void updatePro(const vector<vector<int>> &pop, const vector<double>& weight);
	void createSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, double alpha){}
	ReturnFlag updateSolutions(vector<unique_ptr<TypeIndi>> &pop, vector<TypeIndi> &curPop, int& impRadio, int ms) {
		return Return_Normal;
	}
protected:
	vector<vector<double> > mvv_pro;
};

template<typename TypeIndi>
MELOperator_Linkage<TypeIndi>::MELOperator_Linkage(int numDim):mvv_pro(numDim,vector<double>(numDim)){

}

template<typename TypeIndi>
void MELOperator_Linkage<TypeIndi>::updatePro( vector<unique_ptr<TypeIndi>> &pop, const vector<double>& weight, const vector<int> *index) {

	int popsize = pop.size();
	for (auto &i: mvv_pro) for (auto &j:i)	j= 0;

	int v1, v2;
	if (index == nullptr) {
		for (int i = 0; i < popsize; ++i){
			pop[i]->linkageWeight(weight[i]);
		}
		for (int i = 0; i < popsize; ++i) {
			for (int j = 0; j < pop[i]->numDecomposer(); ++j) {	
				double w = pop[i]->weight(j);
				for (int k = 0; k < pop[i]->decomposer(j).size(); ++k) {
					v1 = pop[i]->decomposer(j)[k];
					if (pop[i]->decomposer(j).size() == 1) {
						mvv_pro[v1][v1] = w;
					}
					else {
						for (int m = k + 1; m < pop[i]->decomposer(j).size(); ++m) {
							v2 = pop[i]->decomposer(j)[m];
							mvv_pro[v1][v2] = mvv_pro[v2][v1] = w;
						}
					}					
				}
			}
		}
	}
	else {
		for (auto i:(*index)){
			pop[i]->linkageWeight(weight[i]);
		}
		for (auto i : (*index)) {
			for (int j = 0; j < pop[i]->numDecomposer(); ++j) {
				double w = pop[i]->weight(j);
				for (int k = 0; k < pop[i]->decomposer(j).size(); ++k) {
					v1 = pop[i]->decomposer(j)[k];
					for (int m = k + 1; m < pop[i]->decomposer(j).size(); ++m) {
						v2 = pop[i]->decomposer(j)[m];
						mvv_pro[v1][v2] = mvv_pro[v2][v1] = w;
					}
				}
			}
		}
	}
}

template<typename TypeIndi>
void MELOperator_Linkage<TypeIndi>::updatePro( vector<TypeIndi> &pop, const vector<double>& weight, const vector<int> *index) {
	int popsize = pop.size();
	for (auto &i : mvv_pro) for (auto &j : i)	j = 0;

	int v1, v2;
	if (index == nullptr) {
		for (int i = 0; i < popsize; ++i) {
			pop[i].linkageWeight(weight[i]);
		}
		for (int i = 0; i < popsize; ++i) {
			for (int j = 0; j < pop[i].numDecomposer(); ++j) {
				double w = pop[i].weight(j);
				for (int k = 0; k < pop[i].decomposer(j).size(); ++k) {
					v1 = pop[i].decomposer(j)[k];
					for (int m = k + 1; m < pop[i].decomposer(j).size(); ++m) {
						v2 = pop[i].decomposer(j)[m];
						mvv_pro[v1][v2] = mvv_pro[v2][v1] = w;
					}
				}
			}
		}
	}
	else {
		for (auto i : (*index)) {
			pop[i].linkageWeight(weight[i]);
		}
		for (auto i : (*index)) {
			for (int j = 0; j < pop[i].numDecomposer(); ++j) {
				double w = pop[i].weight(j);
				for (int k = 0; k < pop[i].decomposer(j).size(); ++k) {
					v1 = pop[i].decomposer(j)[k];
					for (int m = k + 1; m < pop[i].decomposer(j).size(); ++m) {
						v2 = pop[i].decomposer(j)[m];
						mvv_pro[v1][v2] = mvv_pro[v2][v1] = w;
					}
				}
			}
		}
	}
}

/*
template<typename TypeIndi>
void MELOperator_Linkage<TypeIndi>::updatePro(const vector<vector<int>> &pop, const vector<double>& weight) {
	int numDim = mvv_pro.size();
	int popsize = pop.size();
	for (auto &i : mvv_pro) for (auto &j : i)	j = 0;

	double v1, v2;
	
	for (int i = 0; i < popsize; ++i) {
		pop[i]->linkageWeight(weight[i]);
	}
	for (int i = 0; i < popsize; ++i) {
		for (int j = 0; j < pop[i]->numDecomposer(); ++j) {
			double w = pop[i]->weight(j);
			for (int k = 0; k < pop[i]->decomposer(j).size(); ++k) {
				v1 = pop[i]->decomposer(j)[k];
				for (int m = k + 1; m < pop[i]->decomposer(j).size(); ++m) {
					v2 = pop[i]->decomposer(j)[m];
					mvv_pro[v1][v2] = mvv_pro[v2][v1] = w;
				}
			}
		}
	}

}*/
#endif // !MEL_OPERATOR_LINKAGE
