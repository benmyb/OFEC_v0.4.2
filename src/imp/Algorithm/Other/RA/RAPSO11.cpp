#include "RAPSO11.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif
RASPSO11::RASPSO11(ParamMap& v) : RAI<SPSO11>(v){
}

ReturnFlag RASPSO11::run_() {
	ReturnFlag rf = Return_Normal;

#ifdef OFEC_CONSOLE
	if (Global::msp_global->m_runId == 0) {
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if (mMultiModal::getPopInfor())
			mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
#endif // OFEC_CONSOLE

	while (!ifTerminating()) {
		/*g_mutexStream.lock();
		cout<<Global::msp_global->m_runId<<" "<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_best[0]->obj(0)<<endl;
		g_mutexStream.unlock();*/
#ifdef OFEC_DEMON
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		rf = evolve();
		updatePerformance();

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
ReturnFlag RASPSO11::evolve() {

	//lbest model with an adaptive random topology

	ReturnFlag rf = Return_Normal;
	if (m_popsize<1) return rf;

	vector<int> rindex(m_popsize);
	Global::msp_global->initializeRandomArray(rindex, m_popsize);	//generate a permutation of particle index

	setNeibourhood();
	m_noCurActive = 0;
	m_impr = 0;
	m_flag[0] = false;
	for (int i = 0; i<m_popsize; i++) {

		if (Global::msp_global->mp_uniformAlg->Next()>m_pop[rindex[i]]->fitness()) {
			m_pop[rindex[i]]->setActive(false);
			m_curActive[rindex[i]] = false;
			continue;
		}

		Solution<CodeVReal> *l = &neighborBest(rindex[i]);

		if (l != &m_pop[rindex[i]]->pbest())
			rf = m_pop[rindex[i]]->move(m_W, m_C1, m_C2, l, true);
		else rf = m_pop[rindex[i]]->move(m_W, m_C1, m_C2, 0, true);


		if (m_pop[rindex[i]]->self()>(m_pop[rindex[i]]->pbest())) {
			m_pop[rindex[i]]->pbest() = m_pop[rindex[i]]->self();
			m_pop[rindex[i]]->setImpr(true);
			if (updateBestArchive(m_pop[rindex[i]]->self())) {
				m_impr++;
				m_flag[0] = true;
			}
		}else	m_pop[rindex[i]]->setImpr(false);

		m_pop[rindex[i]]->setActive(true);
		m_noCurActive++;
		m_curActive[rindex[i]] = true;

		handleReturnFlag(rf);
		HANDLE_RETURN_FLAG(rf)
	}
	updateInfor();

	updateCurRadius();
	m_iter++;

	m_preActive = m_curActive;
	m_noPreActive = m_noCurActive;

	return rf;
}
