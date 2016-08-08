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
// Created: 07 May 2016
#ifndef RAI_H
#define RAI_H
#include "../../../Global/solution.h"

template<typename POP>
class RAI: public POP{
protected:
	vector<Solution<CodeVReal>> m_pb;								//personal historical best 
	vector<double> m_perfB, m_perfP, m_deltaB, m_deltaP;			// the current performance and previous of an indi
	int m_noPreActive, m_noCurActive;
	vector<bool> m_preActive, m_curActive;

	//for debuging
	struct Infor {
		double active; double obj;
		Infor(double act, double val) :active(act), obj(val) {}
	};
	vector<list<Infor>> m_infor;
	list<int>	m_usage;
	int m_gap = 10;
	vector<double> m_fit;
protected:
	void updatePerformance();
	void updateInfor();
	void outputInfor();
public:
	RAI(ParamMap &v);
};

template<typename POP>
RAI<POP>::RAI(ParamMap &v):POP(v[param_popSize]), m_perfP(this->m_popsize, 0), m_perfB(this->m_popsize, 0), m_deltaB(this->m_popsize),\
	m_deltaP(this->m_popsize), m_preActive(this->m_popsize), m_curActive(this->m_popsize), m_fit(this->m_popsize) {

	for (int i = 0; i < this->m_popsize; ++i) {
		m_pb.push_back(this->m_pop[i]->data());
		this->m_pop[i]->fitness() = 1;
		m_preActive[i] = m_curActive[i] = true;
	}
	m_noPreActive = m_noCurActive = this->m_popsize;

#ifdef OFEC_DEBUG_
	m_infor.resize(this->m_popsize);
#endif
}


template<typename POP>
void RAI<POP>::updatePerformance() {
	for (int i = 0; i < this->m_popsize; ++i) {
		m_deltaB[i] = this->m_pop[i]->data().m_obj[0] - this->m_best[0]->data().m_obj[0];
		m_deltaP[i] = this->m_pop[i]->data().m_obj[0] - m_pb[i].data().m_obj[0];
		if (Global::msp_global->mp_problem->getOptType() == MAX_OPT) {
			m_deltaB[i] *= -1;
			m_deltaP[i] *= -1;
		}
	}

	for (int i = 0; i < this->m_popsize; ++i) {
		m_perfP[i] = m_perfP[i] / 2 + m_deltaP[i];
		m_perfB[i] = m_perfB[i] / 2 + m_deltaB[i];
	}

	if (this->m_flag[0]) {
		gQuickSort(m_perfB, this->m_popsize, this->m_orderList);
		m_usage.push_back(1);
	}
	else {
		gQuickSort(m_perfP, this->m_popsize, this->m_orderList);
		m_usage.push_back(2);
	}

	for (int i = 0; i < this->m_popsize; ++i) {
		//this->m_pop[this->m_orderList[i]]->fitness() = 1 -  i* 1.0 /this->m_popsize;
		this->m_pop[this->m_orderList[i]]->fitness() = 1 / (1 + exp(-10 + 20.*i / this->m_popsize));
		m_fit[this->m_orderList[i]] = this->m_pop[this->m_orderList[i]]->fitness();
	}

	for (int i = 0; i < this->m_popsize; ++i) {
		if (this->m_pop[i]->isActive() && this->m_pop[i]->isImproved()) {
			if (this->m_pop[i]->self()>m_pb[i]) m_pb[i] = this->m_pop[i]->self();
		}
	}

}
template<typename POP>
void RAI<POP>::updateInfor() {
#ifdef OFEC_DEBUG_
	for (int i = 0; i<this->m_popsize; i++) {
		if (this->m_iter%m_gap == 0) {
			if (this->m_iter != 0) {
				m_infor[i].back().active /= m_gap;
				m_infor[i].back().obj /= m_gap;
			}
			if (m_curActive[i]) {
				m_infor[i].push_back(move(Infor(1, m_pb[i].data().m_obj[0])));
			}
			else {
				m_infor[i].push_back(move(Infor(0, m_pb[i].data().m_obj[0])));
			}
		}
		else {
			m_infor[i].back().active += m_curActive[i];
			m_infor[i].back().obj += m_pb[i].data().m_obj[0];
		}
	}
#endif
}

template<typename POP>
void RAI<POP>::outputInfor() {

#ifdef OFEC_DEBUG_
	int num = this->m_iter%m_gap + 1;
	for (int i = 0; i<this->m_popsize; i++) {
		m_infor[i].back().active /= num;
		m_infor[i].back().obj /= num;
	}

	string ss = Global::g_arg[param_workingDir];
	ss += "Result/";
	ss += mSingleObj::getSingleObj()->m_fileName.str();
	string ss1, ss2, ss3;
	ss1 = ss + "act.txt";
	ss2 = ss + "obj.txt";
	ss3 = ss + "sch.txt";
	ofstream out1(ss1.c_str()), out2(ss2.c_str()), out3(ss3.c_str());
	vector<typename list<RAI<POP>::Infor>::iterator> iter;
	for (auto i = m_infor.begin(); i != m_infor.end(); ++i) {
		iter.push_back(i->begin());
	}
	for (int j = 0; j <= this->m_iter / m_gap; j++) {
		out1 << j*m_gap << " ";
		out2 << j*m_gap << " ";
		for (auto i = 0; i <this->m_popsize; ++i) {
			out1 << (iter[i])->active << " ";
			out2 << (iter[i]++)->obj << " ";
		}
		out1 << endl;
		out2 << endl;
	}
	out1.close(); out2.close();
	int j = 0;
	for (auto i = m_usage.begin(); i != m_usage.end(); ++i) {
		out3 << j++ << " " << *i << endl;
	}

	out3.close();
#endif // OFEC_DEBUG_
}
#endif // !RAI_H
