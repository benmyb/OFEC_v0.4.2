#include"RASPSOG.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif
RASPSOG::RASPSOG(ParamMap& v) :RAI<SwarmGBest>(v) {
}

ReturnFlag RASPSOG::evolve() {
	if (this->m_popsize<1) return Return_Normal;
	ReturnFlag r_flag = Return_Normal;

	m_noCurActive = 0;
	m_flag[0] = false;

	for (int i = 0; i<this->m_popsize; i++) {
		if (Global::msp_global->mp_uniformAlg->Next()>m_pop[i]->fitness()) {
			m_pop[i]->setActive(false);
			m_curActive[i] = false;
			continue;
		}

		r_flag = this->m_pop[i]->move(neighborBest(i), m_W, m_C1, m_C2);//			
		if (this->m_pop[i]->self()>this->m_pop[i]->representative()) {
			this->m_pop[i]->representative() = this->m_pop[i]->self();
			m_pop[i]->setImpr(true);
			if (this->updateBestArchive(this->m_pop[i]->self())) {
				m_flag[0] = true;
			}
		}
		else	m_pop[i]->setImpr(false);
		m_pop[i]->setActive(true);
		m_noCurActive++;
		m_curActive[i] = true;

		if (r_flag != Return_Normal) break;
	}
	updateInfor();
	if (r_flag == Return_Normal) {
		this->m_iter++;
	}
	m_preActive = m_curActive;
	m_noPreActive = m_noCurActive;

	return r_flag;
}

ReturnFlag RASPSOG::run_()
{
	ReturnFlag rf = Return_Normal;

#ifdef OFEC_CONSOLE
	if (Global::msp_global->m_runId == 0) {
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if (mMultiModal::getPopInfor())
			mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
#endif // OFEC_CONSOLE

	while (!ifTerminating()) {
		g_mutexStream.lock();
		//cout<<Global::msp_global->m_runId<<" "<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_best[0]->obj(0)<<endl;
		g_mutexStream.unlock();
		rf = this->evolve();
		updatePerformance();

#ifdef OFEC_DEMON
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		if (rf != Return_Normal) handleReturnFlag(rf);

#ifdef OFEC_CONSOLE
		if (mMultiModal::getPopInfor()) {
			int peaksf;
			peaksf = CAST_PROBLEM_CONT->getGOpt().getNumGOptFound();
			mMultiModal::getPopInfor()->input(Global::msp_global.get(), Global::msp_global->mp_problem->getEvaluations(), \
				Global::msp_global->m_totalNumIndis, 1, peaksf, \
				CAST_PROBLEM_CONT->getGOpt().getNumOpt(), 0, 0, 0, 0, \
				0, 0, CAST_PROBLEM_CONT->getGOpt().isAllFound());
		}
#endif

		if (rf == Return_Terminate) break;
	}

	return rf;
}