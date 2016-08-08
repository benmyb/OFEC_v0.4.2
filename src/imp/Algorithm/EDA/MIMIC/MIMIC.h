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

Paper: "�Ľ���MIMIC�㷨�������������", �Ʊ��飬 ��������������
*************************************************************************/
// Created: 26 March 2016
// Last modified:

#ifndef MIMIC_H
#define MIMIC_H
#include "../../Population.h"
#include "../../ACO/Ant.h"

class MIMIC :public Population<CodeVInt, Ant> {

public:
	MIMIC(ParamMap &v);
	void setDefaultPara();
	void selectIndividuals();
	void statisticFrequency();
	void modifyModel();
	ReturnFlag run_();
	bool ifTerminating();
	void resetAntsInfo();

private:
	vector<int> m_index;
	vector<vector<double>> m_pro;
	vector<double> m_p;
	double m_a, m_saveFre;
	int m_num, m_len;
};

#endif //MIMIC_H