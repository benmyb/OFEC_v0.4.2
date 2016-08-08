#pragma once
#ifndef RADE_INDI_H
#define RADE_INDI_H
#include"../../DE/DEIndividual.h"
class RADEIndi:public DEIndividual {
public:
	ReturnFlag select() {
		m_impr = false;
		ReturnFlag rf = m_pu.evaluate();
		if (m_pu > self()) {
			self() = m_pu;
			m_impr = true;
		}
		return rf;
	}
};

#endif // !RADE_INDI_H

