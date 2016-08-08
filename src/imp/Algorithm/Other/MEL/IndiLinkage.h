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
#ifndef INDILINKAGE_H
#define INDILINKAGE_H

template<typename ED,typename TypeIndi>
class IndiLinkage :public TypeIndi {
public:
	struct Linkage {
		vector<int> var;	//linkaged desicion variables
		double progress;		//difference of objective value
		double weight;
		bool impr;
	};
	IndiLinkage() :TypeIndi() {	
		initializeDecomposer();
	}
	int numDecomposer() {
		return m_dec.size();
	}
	const vector<int>& decomposer(int i) const {
		return m_dec[i].var;
	}
	double weight(int i) const {
		return m_dec[i].weight;
	}
	void initializeDecomposer() {
		int numDim = Global::msp_global->mp_problem->getNumDim();
		vector<int> cand(numDim);
		for (int i = 0; i < numDim; ++i) {
			cand[i] = i;
		}

		vector<vector<int>> res;
		randGroup(cand,res);
		m_dec.resize(res.size());

		for (auto i = 0; i < m_dec.size();++i) {
			m_dec[i].var = res[i];
			m_dec[i].progress = 0;
			m_dec[i].impr = false;
			m_dec[i].weight = 0;
		}
	}

	void sample(const vector<vector<double>> &pbi) {
		int numDim = Global::msp_global->mp_problem->getNumDim();
		int counter = numDim;
		vector<bool> flag(numDim, false);	
		vector<vector<int>> result;
		for (int i = 0; i < m_dec.size(); ++i) {
			if (m_dec[i].impr||Global::msp_global->mp_uniformAlg->Next()<m_belta) {
				for (auto j : m_dec[i].var) {
					flag[j] = true;
				}
				counter -= m_dec[i].var.size();
				result.push_back(move(m_dec[i]));
			}
		}
		vector<double> accPro(numDim, 0);
		while (counter > 0) {			
			int firstFalse=0;
			while (flag[firstFalse]&& firstFalse<numDim) ++firstFalse;
			
			for (int i = 0; i < numDim; ++i) {
				if (!flag[i]) {
					accPro[i] = pbi[firstFalse][i];
				}
				else accPro[i] = 0;
				if(i>0) accPro[i] += accPro[i - 1];
			}

			vector<int> gr;
			while (accPro[numDim - 1] > 0) {				
				double p = Global::msp_global->mp_uniformAlg->Next()*accPro[numDim - 1];
				vector<double>::iterator it = lower_bound(accPro.begin(), accPro.end(), p);
				int idx = int(it - accPro.begin());
				if (flag[idx]) break;
				gr.push_back(idx);
				flag[idx] = true;

				for (int i = idx; i < numDim; ++i) {
					accPro[i] -= pbi[firstFalse][idx];
				}
			}
			if (gr.size() > 0) {
				gr.push_back(firstFalse);
				flag[firstFalse] = true;
				counter-=gr.size();
				result.push_back(move(gr));
			}
			else {//random group the remaining dimensions
				vector<int> cand;
				for (int i = 0; i < numDim; ++i) {
					if (!flag[i]) cand.push_back(i);
				}
				vector<vector<int>> remain;
				randGroup(cand, remain);
				for (auto&i : remain) {
					result.push_back(move(i));
				}
				break;
			}						
		}

		m_dec.resize(result.size());
		for (auto i = 0; i < m_dec.size(); ++i) {
			m_dec[i].var = result[i];
			m_dec[i].progess = 0;
			m_dec[i].impr = false;
			m_dec[i].weight = 0;
		}
	}
	void linkageWeight(double w) {
		double min = m_dec[0].progress, max = m_dec[0].progress;
		
		for (auto &i : m_dec) {
			if (max < i.progress) max = i.progress;
			if (min > i.progress) min = i.progress;
		}
		if (max > 0) {
			for (auto &i : m_dec) {
				i.weight = w*i.progress / max;
			}
		}
		else {
			for (auto &i : m_dec) {
				i.weight = w;
			}			
		}
		
	}
protected:
	void randGroup(const vector<int> &candidate, vector<vector<int>>&result) {
		int noGroup = Global::msp_global->getRandInt(1, candidate.size() + 1);
		result.resize(noGroup);
		vector<int> set(candidate.size() - 1), randIdx(candidate.size());
		for (int i = 0; i < set.size(); ++i) {
			set[i] = i;
		}
		for (int i = 0; i < candidate.size(); ++i) {
			randIdx[i] = i;
		}
		Global::msp_global->randomize(randIdx);
		vector<int> pick(noGroup);
		for (int i = 0; i < noGroup - 1; ++i) {
			pick[i] = Global::msp_global->randPick(set);
		}
		pick.back() = candidate.size() - 1;
		std::sort(pick.begin(), pick.end());

		for (int i = 0, j = 0; i < noGroup; ++i) {
			while (j <= pick[i]) {
				result[i].push_back(randIdx[j++]);
			}
		}
	}
protected:
	vector<Linkage> m_dec;			//decomposer of desicion variables
	double m_belta = 0.5;			//the probability of regrouping of unimproved groups
};
#endif // !INDILINKAGE_H
