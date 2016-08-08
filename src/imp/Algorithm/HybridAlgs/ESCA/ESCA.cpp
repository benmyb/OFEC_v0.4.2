#include "ESCA.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
#endif

ESCA::ESCA(ParamMap &v) : Algorithm(-1,string()), m_GBest(v), m_mCRDE(v), m_CRDE(v)
{
	m_sigma=10;
	m_theta=1.;
}

void ESCA::setPara(double rTheta, int rSigma){
	m_theta=rTheta;
	m_sigma=rSigma;
}


void ESCA::handleReturnFlag(ReturnFlag f)
{
	m_GBest.handleReturnFlag(f);
	m_mCRDE.handleReturnFlag(f);
	m_CRDE.handleReturnFlag(f);
}

ReturnFlag ESCA::evolve()
{
	ReturnFlag rf=Return_Normal;
	Solution<CodeVReal> x;
	x=m_mCRDE.getBest()[0]->self();

	if(!(*m_mCRDE.getBest()[0]==*m_GBest.getBest()[0]) && m_mCRDE.getBest()[0]->getDistance(*m_GBest.getBest()[0])<m_theta){
		// reinitialize the swarm population by replacement all indis from mCRDE
		for(int i=0;i<m_GBest.getPopSize();i++){
			rf=m_GBest.getPop()[i]->initialize(&m_mCRDE.getPop()[i]->self()); 
			handleReturnFlag(rf);
			HANDLE_RETURN_FLAG(rf)
		}
		if(rf==Return_Terminate) return rf;
		m_GBest.getBest()[0]->self()=m_mCRDE.getBest()[0]->self(); 
	}
	
	rf=m_mCRDE.evolve();
	handleReturnFlag(rf);
	if(rf==Return_Terminate) return rf;

	rf=m_GBest.evolve();
	handleReturnFlag(rf);
	if(rf==Return_Terminate) return rf;

	if(m_mCRDE.getEvoNum()%m_sigma==0){
		if(m_mCRDE.getBest()[0]->getDistance(*m_CRDE.getBest()[0])<m_theta)
			m_mCRDE=m_CRDE;
		rf=m_CRDE.evolve();
		handleReturnFlag(rf);
		if(rf==Return_Terminate) return rf;
	}

	double xfit=x.obj(0);

	if(!dynamic_cast<DynamicProblem *>(Global::msp_global->mp_problem.get())->predictChange(1)){
		//the detection will cost one evalution, which may trigger the enviromental change. In such case, change detection does not work
		x.evaluate();
	}

	if(xfit!=x.obj(0))
	{
		for(int i=0;i<m_GBest.getPopSize();i++)
		{
			rf=m_GBest.getPop()[i]->initialize(&m_mCRDE.getPop()[i]->self());
			handleReturnFlag(rf);
			HANDLE_RETURN_FLAG(rf)
		}
		if(rf==Return_Terminate) return rf;
		m_GBest.getBest()[0]->self()=m_mCRDE.getBest()[0]->self();
	}

	if(*m_GBest.getBest()[0]>*m_CRDE.getBest()[0]){
		int idx1,idx2;
		m_CRDE.findNearestPair(idx1,idx2,1);
		m_CRDE.getPop()[idx1]->self()=m_GBest.getBest()[0]->self();
		m_CRDE.getBest()[0]->self()=m_GBest.getBest()[0]->self();
	}
	else 
		m_GBest.getBest()[0]->self()=m_CRDE.getBest()[0]->self();

	if(*m_GBest.getBest()[0]>*m_mCRDE.getBest()[0])
		m_mCRDE.getBest()[0]->self()=m_GBest.getBest()[0]->self();

	return rf;
}

ReturnFlag ESCA::run_()
{
	ReturnFlag rf=Return_Normal;

	#ifdef OFEC_CONSOLE
	if(Global::msp_global->m_runId==0){
		mSingleObj::getSingleObj()->setProgrOutputFlag(true);
		if(mMultiModal::getPopInfor())
		mMultiModal::getPopInfor()->setOutProgFlag(true);
	}
	#endif // OFEC_CONSOLE

	while(!ifTerminating())
	{
		rf=evolve();

		#ifdef OFEC_CONSOLE
		if(mMultiModal::getPopInfor()){
			int peaksf;
			if (CAST_PROBLEM_DYN_CONT) {
				if(IS_PROBLEM_NAME(Global::ms_curProId,"DYN_CONT_MovingPeak")||IS_PROBLEM_NAME(Global::ms_curProId,"DYN_CONT_RotationDBG")){
					peaksf=CAST_PROBLEM_DYN_CONT->getPeaksFound();

				}else if(Global::msp_global->mp_problem->m_name.find("FUN_")!=string::npos){
					peaksf=CAST_PROBLEM_CONT->getGOpt().getNumGOptFound();
				}
				mMultiModal::getPopInfor()->input(Global::msp_global.get(), Global::msp_global->mp_problem->getEvaluations(),\
					Global::msp_global->m_totalNumIndis,3,peaksf,\
					CAST_PROBLEM_DYN_CONT->getNumofVisablePeaks(),0,0,0,0,\
					CAST_PROBLEM_DYN_CONT->getPeaksTracedQaulity(),0,CAST_PROBLEM_DYN_CONT->isGOptTracked());
			}
			else if (CAST_PROBLEM_DYN_ONEPEAK) {
				FFreePeak_D_OnePeak*pro = dynamic_cast<FFreePeak_D_OnePeak*>(Global::msp_global->mp_problem.get());
				int peaksf = pro->getPT();
				mMultiModal::getPopInfor()->input(Global::msp_global.get(), Global::msp_global->mp_problem->getEvaluations(), \
					Global::msp_global->m_totalNumIndis, 3, peaksf, \
					pro->getNumPeak(), 0, 0, 0, 0, 0, 0, pro->isAllGOptTraced());
			}
		}
			
		#endif
		#ifdef OFEC_DEMON
		vector<Algorithm*> vp;
		vp.push_back(&m_GBest);
		vp.push_back(&m_CRDE);
		vp.push_back(&m_mCRDE);
		msp_buffer->updateBuffer_(&vp);
		#endif
		if(rf==Return_Terminate) break;
	}
	return rf;
}


