#include "LinkageDE.h"
#include "MELOperator_Linkage.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

LinkageDE::LinkageDE(int size) :DEPopulation<CodeVReal, IndiLinkageDE>(size), m_mutationScheme(1){
	setMutationStrategy((DEMutationStratgy)m_mutationScheme);
}

void LinkageDE::mutate(const int idx,const int gidx) {
	vector<int> ridx;
	switch (m_mutStrategy) {
	case DE_rand_1:
		select(idx, 3, ridx);
		this->m_pop[idx]->mutate(gidx,m_F, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
		break;
	case DE_best_1:
		select(idx, 2, ridx);
		this->m_pop[idx]->mutate(gidx, m_F, &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
		break;
	case DE_targetToBest_1:
		select(idx, 2, ridx);
		this->m_pop[idx]->mutate(gidx, m_F, &this->m_pop[idx]->self(), &this->m_best[0]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self());
		break;
	case DE_best_2:
		select(idx, 4, ridx);
		this->m_pop[idx]->mutate(gidx, m_F, &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self());
		break;
	case DE_rand_2:
		select(idx, 5, ridx);
		this->m_pop[idx]->mutate(gidx, m_F, &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self(), &this->m_pop[ridx[3]]->self(), &this->m_pop[ridx[4]]->self());
		break;
	case DE_randToBest_1:
		select(idx, 3, ridx);
		this->m_pop[idx]->mutate(gidx, m_F, &this->m_pop[ridx[0]]->self(), &this->m_best[0]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
		break;
	case DE_targetToRand_1:
		select(idx, 3, ridx);
		this->m_pop[idx]->mutate(gidx, m_F, &this->m_pop[idx]->self(), &this->m_pop[ridx[0]]->self(), &this->m_pop[idx]->self(), &this->m_pop[ridx[1]]->self(), &this->m_pop[ridx[2]]->self());
		break;
	}
}

ReturnFlag LinkageDE::evolve() {
	if (m_popsize<5) {
		throw myException("the population size cannot be smaller than 5@DEPopulation<ED,TypeDEIndi>::evolve()");
	}
	ReturnFlag r_flag = Return_Normal;
	for (int i = 0; i<this->m_popsize; i++) {
		for (int j = 0; j < m_pop[i]->numDecomposer(); ++j) {
			mutate(i,j);
			m_pop[i]->recombine(j,m_CR);
			r_flag = this->m_pop[i]->select(j);
			this->updateBestArchive(this->m_pop[i]->self());
			if (r_flag != Return_Normal) break;
		}
		if (r_flag != Return_Normal) break;
	}
	
	if (r_flag == Return_Normal) {
		this->m_center = *this->m_best[0];
		this->updateCurRadius();
		this->m_iter++;
	}
	return r_flag;
}


LinkageDE_::LinkageDE_(ParamMap &v):MEL<CodeVReal, IndiLinkageDE, LinkageDE>(v) {
	m_mutationScheme = (int)v[param_mutationSchemeDE];
	setMutationStrategy((DEMutationStratgy)m_mutationScheme);
	if (Global::msp_global->mp_problem->isProTag(CONT)) {
		m_MELOperator.reset(new MELOperator_Linkage<IndiLinkageDE>(m_numDim));
	}
}
ReturnFlag LinkageDE_::run_() {
	ReturnFlag rf = Return_Normal;

#ifdef OFEC_CONSOLE
	if (Global::msp_global->m_runId == 0) {
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if (mMultiModal::getPopInfor())
			mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
#endif // OFEC_CONSOLE

	initilizeMemory();
	initilizeCurPop();

	while (!ifTerminating()) {
		/*g_mutexStream.lock();
		cout<<Global::msp_global->m_runId<<" "<<Global::msp_global->mp_problem->getEvaluations()<<" "<<m_best[0]->obj(0)<<endl;
		g_mutexStream.unlock();*/
		rf = this->evolve();
#ifdef OFEC_DEMON
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		if (rf != Return_Normal) handleReturnFlag(rf);

		updateMemory();

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