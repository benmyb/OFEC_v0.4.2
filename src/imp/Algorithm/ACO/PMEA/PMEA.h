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
// Created: 7 Oct 2014
// Last modified:

#ifndef PMEA_H
#define PMEA_H

#include "../../Population.h"
#include "../../Individual.h"
#include "../Ant.h"

//for symmetric TSP

class PMEA: public Population<CodeVInt,Ant>
{
protected:
	enum strategy { ALL, Elism };
	vector<vector<double> > mvv_edgesInfo;
	vector<Ant> mv_curPop;
	int m_saveFre;
	double m_alpha;
	double m_beta;
	int m_num;
	string m_type;
	
	//for test
	vector<bool> mv_flag;
	vector<bool> mv_isEval;
	int m_preDiffs, m_impRadio;
	strategy m_stra;
	ReturnFlag updata();
	void statisticDiffsAndImp(bool flag=true,bool enable=false);
	void setIsEval();
	//for test
	vector<double> m_weight;
public:
	PMEA(ParamMap &v);
	~PMEA();
	void statisticEdgeInfo();
	ReturnFlag selectBestPop();   //select the best individuals from the current and old population to create a new population
	void initializeSystem();
	void resetAntsInfo();
	bool ifTerminating();
	ReturnFlag run_();

private:
	void initialize_AS();
	void initialize_ACS();
	void local_updatePheromeno(vector<vector<double>> &phero, double t, int ant_loc, bool isLastEdge = false);
	void initialize_MMAS();
	double getLenOfNN();   //get the length of tour which is created based on nearest neighbor heuristic

private:
	void getCoordiate(vector<vector<double> > &coordinate);
	void keepTourCoordiate(const vector<vector<double> > &coordinate, const Solution<CodeVInt> &sol, string fileName);
	void keepMatrixCoordiate(const vector<vector<double> > &coordinate, string fileName);
	void keepMatrix(string filename, bool flag = false); //only for single thread run
	bool isConvergence(int val);
};

#endif