#include "RADE.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif
RADE::RADE(ParamMap &v):RAI< DEPopulation<CodeVReal, RADEIndi> >(v), m_case(v[param_case]){
	
	this->setMutationStrategy((DEMutationStratgy)m_case);
	
}

ReturnFlag RADE::run_() {
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
		cout << Global::msp_global->m_runId << " " << Global::msp_global->mp_problem->getEvaluations() << " " << m_best[0]->obj(0) << endl;
		g_mutexStream.unlock();*/
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

	outputInfor();

	return rf;
}

ReturnFlag RADE::evolve() {
	if (this->m_popsize<5) {
		throw myException("the population size cannot be smaller than 5@DEPopulation<ED,TypeDEIndi>::evolve()");
	}
	m_noCurActive = 0;
	ReturnFlag r_flag = Return_Normal;
	
	for (int i = 0,k=0; i<this->m_popsize; i++) {
		if (Global::msp_global->mp_uniformAlg->Next()>m_pop[i]->fitness()) {
			m_pop[i]->setActive(false);
			m_curActive[i] = false;
			continue;
		}
		mutate(i);
		this->m_pop[i]->recombine(m_CR);
		m_pop[i]->setActive(true);
		m_noCurActive++;
		m_curActive[i] = true;
	}
	updateInfor();
	m_flag[0] = false;
	bool flag = false;
	for (int i = 0; i<this->m_popsize; i++) {
		if (!m_pop[i]->isActive()) continue;
		r_flag = this->m_pop[i]->select();
		flag = this->updateBestArchive(this->m_pop[i]->self());
		if (flag&&!m_flag[0]) 
			m_flag[0] = flag;
		if (r_flag != Return_Normal) break;
	}
	if (r_flag == Return_Normal) {
		this->m_iter++;
	}
	m_preActive = m_curActive;
	m_noPreActive = m_noCurActive;
	return r_flag;
}

void RADE::mutate(const int idx) {
	int r[5];
	
	vector<int> candidate;
	for (int i = 0; i < this->m_popsize; i++) {
		if (idx != i&&m_preActive[i] ) candidate.push_back(i);		
	}
	int i = 0;
	while (candidate.size() < 5&&i<this->m_popsize) {	
		if (idx != m_orderList[i]&&!m_preActive[m_orderList[i]]) candidate.push_back(m_orderList[i]);
		++i;
	}
	switch (m_mutStrategy) {
	case DE_rand_1:
		r[0] = select(candidate,true);
		r[1] = select(candidate, true);
		r[2] = select(candidate, false);
		this->m_pop[idx]->mutate(m_F, &this->m_pop[r[0]]->self(), &this->m_pop[r[1]]->self(), &this->m_pop[r[2]]->self());
		break;
	case DE_best_1:
		r[0] = select(candidate, true);
		r[1] = select(candidate, false);
		this->m_pop[idx]->mutate(m_F, &this->m_best[0]->self(), &this->m_pop[r[0]]->self(), &this->m_pop[r[1]]->self());
		break;
	case DE_targetToBest_1:
		r[0] = select(candidate, true);
		r[1] = select(candidate, false);
		this->m_pop[idx]->mutate(m_F, &this->m_pop[idx]->self(), &this->m_best[0]->self(), &this->m_pop[idx]->self(), &this->m_pop[r[0]]->self(), &this->m_pop[r[1]]->self());
		break;
	case DE_best_2:
		r[0] = select(candidate, true);
		r[1] = select(candidate, false);
		r[2] = select(candidate, true);
		r[3] = select(candidate, false);
		this->m_pop[idx]->mutate(m_F, &this->m_best[0]->self(), &this->m_pop[r[0]]->self(), &this->m_pop[r[1]]->self(), &this->m_pop[r[2]]->self(), &this->m_pop[r[3]]->self());
		break;
	case DE_rand_2:
		r[0] = select(candidate, true);
		r[1] = select(candidate, true);
		r[2] = select(candidate, false);
		r[3] = select(candidate, true);
		r[4] = select(candidate, false);
		this->m_pop[idx]->mutate(m_F, &this->m_pop[r[0]]->self(), &this->m_pop[r[1]]->self(), &this->m_pop[r[2]]->self(), &this->m_pop[r[3]]->self(), &this->m_pop[r[4]]->self());
		break;
	case DE_randToBest_1:
		r[0] = select(candidate, true);
		r[1] = select(candidate, true);
		r[2] = select(candidate, false);
		this->m_pop[idx]->mutate(m_F, &this->m_pop[r[0]]->self(), &this->m_best[0]->self(), &this->m_pop[r[0]]->self(), &this->m_pop[r[1]]->self(), &this->m_pop[r[2]]->self());
		break;
	case DE_targetToRand_1:
		r[0] = select(candidate, true);
		r[1] = select(candidate, true);
		r[2] = select(candidate, false);
		this->m_pop[idx]->mutate(m_F, &this->m_pop[idx]->self(), &this->m_pop[r[0]]->self(), &this->m_pop[idx]->self(), &this->m_pop[r[1]]->self(), &this->m_pop[r[2]]->self());
		break;
	}
}
