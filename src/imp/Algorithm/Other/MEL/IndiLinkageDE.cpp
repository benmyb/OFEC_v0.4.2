#include "IndiLinkageDE.h"
#include "../../../Problem/ContinuousProblem.h"
#include "../../../Global/global.h"

IndiLinkageDE::IndiLinkageDE() :IndiLinkage<CodeVReal, DEIndividual>() {
}


ReturnFlag IndiLinkageDE::initialize(int rIdx, int rID, int rPopsize, bool mode) {
	ReturnFlag rf = Individual::initialize(rIdx, rID, rPopsize, mode);
	m_flag = true;
	m_pv = self();
	m_pu = self();
	return rf;
}

void IndiLinkageDE::mutate(const int gidx, double F, Solution<CodeVReal> *r1, Solution<CodeVReal> *r2, Solution<CodeVReal> *r3, Solution<CodeVReal> *r4, Solution<CodeVReal> *r5 ){
	double l, u;
	for (auto i :m_dec[gidx].var) {
		CAST_PROBLEM_CONT->getSearchRange(l, u, i);
		m_pv.data()[i] = (r1->data()[i]) + F*((r2->data()[i]) - (r3->data()[i]));
		if (r4&&r5) m_pv.data()[i] += F*((r4->data()[i]) - (r5->data()[i]));

		if ((m_pv.data()[i])>u) {
			m_pv.data()[i] = ((r1->data()[i]) + u) / 2;
		}
		else if ((m_pv.data()[i])<l) {
			m_pv.data()[i] = ((r1->data()[i]) + l) / 2;
		}
	}
}

void IndiLinkageDE::recombine(const int gidx, double CR) {

	int I = Global::msp_global->getRandInt(0, m_dec[gidx].var.size(), Program_Algorithm);
	I = m_dec[gidx].var[I];
	for (auto i: m_dec[gidx].var) {
		double p = Global::msp_global->mp_uniformAlg->Next();
		if (p <= CR || i == I)     m_pu.data()[i] = m_pv.data()[i];
		else m_pu.data()[i] = data()[i];
	}
}
ReturnFlag IndiLinkageDE::select(const int gidx) {
	ReturnFlag rf = m_pu.evaluate();
	if (m_pu > self()) {
		m_dec[gidx].progress = m_pu.data().getObjDistance(self().data().m_obj);
		self() = m_pu;
		m_dec[gidx].impr = true;
	}
	else {
		m_dec[gidx].progress = 0;
		m_dec[gidx].impr = false;
	}
	return rf;
}

/*Solution<CodeVReal>& IndiLinkageDE::trial() {
	return m_pu;
}*/