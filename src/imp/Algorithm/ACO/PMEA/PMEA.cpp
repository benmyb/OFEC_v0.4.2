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
*************************************************************************/
// Created: 7 Oct 2014
// Last modified:

#include "PMEA.h"
#include "../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

static mutex m_nearMutex;
static vector<set<int>> mv_candidate; //select a next city from the candidate list

PMEA::PMEA(ParamMap &v) :Population<CodeVInt,Ant>(int(v[param_popSize])),m_saveFre(1500),m_num(0),m_alpha(1),m_beta(5),mv_curPop(v[param_popSize]),\
	mv_flag(v[param_popSize],true),m_impRadio(0),m_preDiffs(0),mv_isEval(v[param_popSize],true),m_weight(v[param_popSize])
{
	mvv_edgesInfo.resize(v[param_numDim]);
	for(size_t i=0;i<mvv_edgesInfo.size();i++)
		mvv_edgesInfo[i].resize(v[param_numDim]);
	m_type=(string)v[param_case];
	if(m_type=="PMEA"){
		if(v.find(param_xoverProbability)!=v.end())		m_alpha=v[param_xoverProbability];
		else m_alpha=0.9;
		if(v.find(param_interTest3)!=v.end()) m_stra=(strategy) ((int) (v[param_interTest3]));
		else m_stra=Elism;
	}
	if(m_type=="ACO_PM"){
		if(v.find(param_interTest3)!=v.end()) m_stra=(strategy) ((int) (v[param_interTest3]));
		else m_stra=ALL;
	}	

	m_term.reset(new TermMean(v));
}

PMEA::~PMEA()
{
	mvv_edgesInfo.clear();
	mv_curPop.clear();
}

void PMEA::statisticEdgeInfo()
{
	int i,j;
	int e1,e2;
	int bestIdx=0, worstIdx=0, num=0;

	if(m_stra==Elism||m_iter==0){
		for(i=0;i<m_popsize;i++){
			if(m_pop[i]->self()>m_pop[bestIdx]->self()){
				bestIdx=i;
			}
			if(m_pop[i]->self()<m_pop[worstIdx]->self()){
				worstIdx=i;
			}
		}

		double disobj=m_pop[worstIdx]->getObjDistance_(m_pop[bestIdx]->data().m_obj)+1;
		for(i=0;i<m_popsize;i++){
			m_weight[i]=(m_pop[i]->getObjDistance_(m_pop[worstIdx]->data().m_obj)+1)/disobj;
			//m_weight[i]=2*(1.0/(1+exp(-4*m_weight[i])))-1; //be placed
			m_weight[i] = 1. / (1 + exp(-m_weight[i]));
		}	

		for(i=0;i<m_numDim;i++)
			fill(mvv_edgesInfo[i].begin(),mvv_edgesInfo[i].end(),0);
		
		for(i=0;i<m_popsize;i++){
			e1=m_pop[i]->data().m_x[0];
			for(j=0;j<m_numDim;j++){
				e2=m_pop[i]->data().m_x[(j+1)%m_numDim];
				mvv_edgesInfo[e1][e2]+=m_weight[i];
				mvv_edgesInfo[e2][e1]+=m_weight[i];
				e1=e2;
			}
		}

	}else{
		for(i=0;i<m_popsize;i++){
			if(mv_curPop[i].self()>mv_curPop[bestIdx].self()){
				bestIdx=i;
			}
			if(mv_curPop[i].self()<mv_curPop[worstIdx].self()){
				worstIdx=i;
			}
		}

		double disobj=mv_curPop[worstIdx].getObjDistance_(mv_curPop[bestIdx].data().m_obj)+1;
		for(i=0;i<m_popsize;i++){
			m_weight[i]=(mv_curPop[i].getObjDistance_(mv_curPop[worstIdx].data().m_obj)+1)/disobj;
			//m_weight[i]=2*(1.0/(1+exp(-4*m_weight[i])))-1; //
			m_weight[i] = 1. / (1 + exp(-m_weight[i]));
		}	

		for(i=0;i<m_numDim;i++)
			fill(mvv_edgesInfo[i].begin(),mvv_edgesInfo[i].end(),0);

		for(i=0;i<m_popsize;i++){
			e1=mv_curPop[i].data().m_x[0];
			for(j=0;j<m_numDim;j++){
				e2=mv_curPop[i].data().m_x[(j+1)%m_numDim];
				mvv_edgesInfo[e1][e2]+=m_weight[i];
				mvv_edgesInfo[e2][e1]+=m_weight[i];
				e1=e2;
			}
		}
	}

}

ReturnFlag PMEA::selectBestPop()
{
	int i,j,dim=m_pop[0]->getNumDim();
	ReturnFlag rf;
	for(i=0;i<m_popsize;i++)
	{
		mv_isEval[i]=true;
		rf=mv_curPop[i].evaluate();
		if(rf==Return_Terminate) return rf;
	}
	vector<bool> flag(m_popsize,false); //flag the selected individual in the  old population
	multimap<double,int> sort_indivls;
	for(i=0;i<m_popsize;i++)
		sort_indivls.insert(make_pair(m_pop[i]->data().m_obj[0],i));
	for(i=0;i<m_popsize;i++)
		sort_indivls.insert(make_pair(mv_curPop[i].data().m_obj[0],i+m_popsize));
	i=0;j=0;
	vector<int> curPopFlag;
	if(Global::msp_global->mp_problem->getOptType()==MIN_OPT)
	{
		for(multimap<double,int>::iterator iter=sort_indivls.begin();i<m_popsize;++iter,++i)
		{
			if(iter->second<m_popsize)
				flag[iter->second]=true;
			else
				curPopFlag.push_back(iter->second-m_popsize);
		}
		for(i=0;i<m_popsize;i++)
		{
			if(flag[i]) continue;
			else
			{
				for(int z=0;z<dim;z++)
					m_pop[i]->data().m_x[z]=mv_curPop[curPopFlag[j]].data().m_x[z];
				m_pop[i]->data().m_obj[0]=mv_curPop[curPopFlag[j]].data().m_obj[0];
				++j;
			}
		}
	}
	else
	{
		for(multimap<double,int>::iterator iter=sort_indivls.end();i<m_popsize;--iter,++i)
		{
			if(iter==sort_indivls.end()) 
			{
				--i;
				continue;
			}
			if(iter->second<m_popsize)
				flag[iter->second]=true;
			else
				curPopFlag.push_back(iter->second-m_popsize);
		}
		for(i=0;i<m_popsize;i++)
		{
			if(flag[i]) continue;
			else
			{
				for(int z=0;z<dim;z++)
					m_pop[i]->data().m_x[z]=mv_curPop[curPopFlag[j]].data().m_x[z];
				m_pop[i]->data().m_obj[0]=mv_curPop[curPopFlag[j]].data().m_obj[0];
				++j;
			}
		}
	}
	m_bestIdx[0]=0;
}

void PMEA::initializeSystem()
{
	for(int i=0;i<m_popsize;i++){
		mv_curPop[i].initialize(m_pop[i]->data().m_x[0]);
	}
}
	
void PMEA::resetAntsInfo()
{
	for(int i=0;i<m_popsize;i++)
		mv_curPop[i].resetData();
}

bool PMEA::ifTerminating()
{
	if (dynamic_cast<TermMean*>(m_term.get())->ifTerminating()) {
		if (m_iter == 0) return false;
		return true;
	}

	return false;
}

ReturnFlag PMEA::run_()
{
	int i,j,dim;
	ReturnFlag rf=Return_Normal;
	dim=mv_curPop[0].getNumDim();
	int mode = Global::g_arg[param_populationInitialMethod];
	PopInitMethod popInitialMode = static_cast<PopInitMethod>(mode);
	if (popInitialMode == POP_INIT_AS)
		initialize_AS();
	else if (popInitialMode == POP_INIT_ACS)
		initialize_ACS();
	else if (popInitialMode == POP_INIT_MMAS)
		initialize_MMAS();
	else rf=initialize(false,true,true);

	initializeSystem();
	statisticEdgeInfo();
	//keepMatrix("matrix",true);
	dynamic_cast<TermMean*>(m_term.get())->initialize(mean());
	statisticDiffsAndImp(false);
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif

	while(!ifTerminating())
	{
		dynamic_cast<TermMean*>(m_term.get())->countSucIter(mean());
		setIsEval();
		#ifdef OFEC_DEMON
			for(i=0;i<this->getPopSize();i++)
				updateBestArchive(this->m_pop[i]->self());
			vector<Algorithm*> vp;	
			vp.push_back(this);	
			msp_buffer->updateBuffer_(&vp);
		#endif
		for(j=0;j<m_popsize;j++){
			if (m_type == "PMEA")
				mv_curPop[j].initializeIteIndivl(m_pop[j]->self());
			for(i=1;i<dim;i++){
				if(m_type=="ACO_PM")
					mv_curPop[j].selectNextCity_Pro(mvv_edgesInfo,m_beta,m_alpha);
				else		mv_curPop[j].selectNextCity_GLMemory_TSP(*m_pop[j],mvv_edgesInfo,m_alpha);
			}
		}
		if(m_type=="ACO_PM")
			rf=selectBestPop();
		else
			rf=updata();
		statisticDiffsAndImp();
		#ifdef OFEC_CONSOLE
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(),Solution<CodeVInt>::getBestSolutionSoFar(),m_pop,m_num,m_popsize,m_saveFre); 		
		#endif
		//if(rf==Return_Terminate) break; ////////////////////////////////////
		m_iter++;
		statisticEdgeInfo();
		//keepMatrix("matrix");
		resetAntsInfo();
		//cout<<Global::msp_global->mp_problem->getEvaluations()<<endl;
	}

	#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(),Solution<CodeVInt>::getBestSolutionSoFar(),m_pop,m_num,m_popsize,m_saveFre,false);
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
	#endif
	//vector<vector<double> > coordinate;
	//getCoordiate(coordinate);
	//keepTourCoordiate(coordinate, dynamic_cast<TravellingSalesman*>(Global::msp_global.get()->mp_problem.get())->getGOpt()[0], "GlobalBest");
	//keepTourCoordiate(coordinate, Solution<CodeVInt>::getBestSolutionSoFar(), "Best");
	//keepMatrixCoordiate(coordinate, "MatrixCoordiate");
	//keepMatrix("matrix",true);
	return Return_Terminate;
}


ReturnFlag PMEA::updata()
{
	ReturnFlag rf=Return_Normal;
	m_impRadio=0;
	for(int i=0;i<m_popsize;i++){
		if(mv_curPop[i].getFlag()==true){
			mv_isEval[i]=true;
			rf=mv_curPop[i].evaluate();
			if(m_pop[i]->self()<mv_curPop[i].self()){
				m_pop[i]->self()=mv_curPop[i].self();
				if(m_pop[i]->self()>m_pop[m_bestIdx[0]]->self()){ 
					m_bestIdx.clear();
					m_bestIdx.push_back(i);
				}else if(m_pop[i]->self()==m_pop[m_bestIdx[0]]->self()){
					if(find(m_bestIdx.begin(),m_bestIdx.end(),i)!=m_bestIdx.end()) m_bestIdx.push_back(i);
				}
			}
			else if (m_pop[i]->self() == mv_curPop[i].self())
			{
				m_impRadio++; //算法改成了局部更新
				if (m_pop[i]->self()>m_pop[m_bestIdx[0]]->self()){
					m_bestIdx.clear();
					m_bestIdx.push_back(i);
				}
				else if (m_pop[i]->self() == m_pop[m_bestIdx[0]]->self()){
					if (find(m_bestIdx.begin(), m_bestIdx.end(), i) != m_bestIdx.end()) m_bestIdx.push_back(i);
				}
			}
			if(rf==Return_Terminate) break;;
		}else{
			mv_curPop[i].data().m_obj=m_pop[i]->data().m_obj;
		}
	}
	return rf;
}


void PMEA::statisticDiffsAndImp(bool flag,bool enable)
{
#ifdef OFEC_CONSOLE
	int numDim=mv_curPop[0].getNumDim(),diffs=0,num=0;
	if(enable){
		vector<vector<int> > edges;
		edges.resize(numDim);
		for(int i=0;i<numDim;i++)		edges[i].resize(numDim);
		for(int i=0;i<numDim;i++)
			for(int j=0;j<numDim;j++)
				edges[i][j]=0;
		for(int i=0;i<m_popsize;i++){
			for(int j=0;j<numDim;j++){
				int r,c;
				if(flag){
					r=mv_curPop[i].data().m_x[j];
					c=mv_curPop[i].data().m_x[(j+1)%numDim];
				}else{
					r=m_pop[i]->data().m_x[j];
					c=m_pop[i]->data().m_x[(j+1)%numDim];
				}
				edges[r][c]=1; edges[c][r]=1;
			}
		}
		for(int i=0;i<numDim;i++)
			for(int j=i+1;j<numDim;j++)
				if(edges[i][j])
					diffs++;

		if(!flag) {
			m_preDiffs=diffs;
			return;
		}
	}

	for(int i=0;i<m_popsize;i++)
		if(mv_isEval[i])
			num++;

	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	
	double dif=diffs-m_preDiffs;
	m_preDiffs=diffs;
	double impr=static_cast<double>(m_impRadio)/m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
	#endif
}


void PMEA::setIsEval()
{
	for(int i=0;i<m_popsize;i++)
		mv_isEval[i]=false;
}


void PMEA::initialize_AS()
{
	ReturnFlag rf;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	int dim = m_pop[0]->getNumDim();
	vector<vector<double>> phero;
	phero.resize(dim);
	for (int i = 0; i < dim; i++)
		phero[i].resize(dim);
	for (int i = 0; i < dim; i++)
		for (int j = 0; j < dim; j++)
			phero[i][j] = 1. / dim;
	for (int i = 1; i<dim; i++)
		for (int j = 0; j<m_popsize; j++)
			m_pop[j]->selectNextCity_Pro(phero, 5, 1);
	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}


void PMEA::initialize_ACS()
{
	ReturnFlag rf;
	double mt = getLenOfNN();
	mt = 1.0 / (m_numDim * mt);
	vector<vector<double>> phero;
	phero.resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		phero[i].resize(m_numDim);
	for (int i = 0; i<m_numDim; i++)
		for (int j = 0; j<m_numDim; j++)
			phero[i][j] = 1. / m_numDim;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	for (int i = 0; i < m_popsize; i++)
	{
		for (int j = 1; j < m_numDim; j++)
		{
			double q = Global::msp_global->mp_uniformAlg->Next();
			if (q <= 0.9)
				m_pop[i]->selectNextCity_Greedy(phero, 2);
			else
				m_pop[i]->selectNextCity_Pro(phero, 2);
			local_updatePheromeno(phero, mt, i);
		}
		local_updatePheromeno(phero, mt, i, true);
	}
	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}

void PMEA::local_updatePheromeno(vector<vector<double>> &phero, double t, int ant_loc, bool isLastEdge)
{
	pair<int, int> edge;
	if (!isLastEdge)
		edge = m_pop[ant_loc]->getCurrentEdge();
	else
		edge = m_pop[ant_loc]->getLastEdge();
	if (edge.first == edge.second && edge.first == -1)
		throw myException("edge error in local_updatePheromeno function @ACS.cpp");
	phero[edge.first][edge.second] = (1 - 0.1)*phero[edge.first][edge.second] + 0.1*t;
	phero[edge.second][edge.first] = (1 - 0.1)*phero[edge.second][edge.first] + 0.1*t;  //symmetric
}

void PMEA::initialize_MMAS()
{
	ReturnFlag rf;
	int length = 20;
	if (m_numDim <= 20)
		length = m_numDim - 1;
	double pheroMax = getLenOfNN();
	pheroMax = 1. / (0.02 * pheroMax);

	m_nearMutex.lock();

	if (mv_candidate.empty())
	{
		vector<vector<int>> temp;
		mv_candidate.resize(m_numDim);
		temp.resize(m_numDim);
		for (int i = 0; i < temp.size(); i++)
			temp[i].resize(length);
		dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get())->findNearbyCity(temp);
		for (int i = 0; i < temp.size(); i++)
		{
			for (int j = 0; j < length; j++)
			{
				mv_candidate[i].insert(temp[i][j]);
			}
		}
	}

	m_nearMutex.unlock();

	vector<vector<double>> phero;
	phero.resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		phero[i].resize(m_numDim);
	for (int i = 0; i < m_numDim; i++)
		for (int j = 0; j < m_numDim; j++)
			phero[i][j] = pheroMax;
	for (int i = 0; i<m_popsize; i++)
		m_pop[i]->initialize(-1);
	for (int i = 1; i < m_numDim; i++)
		for (int j = 0; j < m_popsize; j++)
			m_pop[j]->selectNextCity_Pro(phero, mv_candidate, 2, 1);

	m_impRadio = 0;
	for (int i = 0; i<m_popsize; i++)
	{
		rf = m_pop[i]->evaluate();
		if (rf == Return_Terminate) break;
	}
#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<Ant>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	tempdif /= m_popsize;
	double impr = static_cast<double>(m_impRadio) / m_popsize;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impr);
#endif
}

double PMEA::getLenOfNN()
{
	vector<int> candidate(m_numDim), result(m_numDim);
	TravellingSalesman *_ptr = dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get());
	const vector<vector<double>> cost = _ptr->getCost();
	int n = 0;
	for (int i = 0; i < candidate.size(); i++){
		candidate[i] = i;
	}
	result[n++] = candidate[0];
	candidate[0] = candidate[m_numDim - 1];
	while (n < m_numDim){
		int loc = 0;
		double min = cost[result[n - 1]][candidate[loc]];
		for (int m = 1; m < m_numDim - n; m++){
			if (cost[result[n - 1]][candidate[m]] < min){
				min = cost[result[n - 1]][candidate[m]];
				loc = m;
			}
		}
		result[n++] = candidate[loc];
		candidate[loc] = candidate[m_numDim - n];
	}
	double val = 0;
	for (int i = 0; i < m_numDim; i++){
		val += cost[result[i]][result[(i + 1) % m_numDim]];
	}
	return val;
}


void PMEA::getCoordiate(vector<vector<double> > &coordinate)
{
	size_t i;
	string Line;
	char *Keyword = 0;
	const char *Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";
	ostringstream oss1;
	ifstream infile;
	oss1 << Global::g_arg[param_workingDir] << "Problem/Combination/TSP/data/" << Global::g_arg[param_dataFile1];
	infile.open(oss1.str().c_str());
	if (!infile){
		throw myException("read travelling salesman data error");
	}
	char *savePtr;
	while (getline(infile, Line))
	{
		if (!(Keyword = gStrtok_r((char*)Line.c_str(), Delimiters, &savePtr)))
			continue;
		for (i = 0; i<strlen(Keyword); i++)
			Keyword[i] = toupper(Keyword[i]);
		if (!strcmp(Keyword, "NODE_COORD_SECTION"))
		{
			i = 0;
			vector<double> temp(2);
			while (infile >> Line)
			{
				infile >> temp[0];
				infile >> temp[1];
				coordinate.push_back(temp);
				++i;
				if (i == (size_t)m_numDim) break;
			}
			break;
		}
	}
	infile.close();
	infile.clear();
}

void PMEA::keepTourCoordiate(const vector<vector<double> > &coordinate, const Solution<CodeVInt> &sol, string fileName)
{
	ostringstream os;
	ofstream out;
	os << Global::g_arg[param_workingDir] << "Result/" << fileName << "_" << Global::g_arg[param_dataFile1] << ".txt";
	out.open(os.str().c_str());
	for (int i = 0; i < m_numDim; i++)
	{
		int loc = sol.data().m_x[i];
		out << coordinate[loc][0] << " " << coordinate[loc][1] << " " << loc << endl;
	}
	int loc = sol.data().m_x[0];
	out << coordinate[loc][0] << " " << coordinate[loc][1] << " " << " " << endl;
	out.close();
	out.clear();
}

void PMEA::keepMatrixCoordiate(const vector<vector<double> > &coordinate, string fileName)
{
	ostringstream os;
	ofstream out;
	os << Global::g_arg[param_workingDir] << "Result/" << fileName << "_" << Global::g_arg[param_dataFile1] << ".txt";
	out.open(os.str().c_str());
	statisticEdgeInfo();
	for (int i = 0; i < m_numDim; i++)
	{
		for (int j = 0; j < m_numDim; j++)
		{
			if (mvv_edgesInfo[i][j] != 0)
			{
				out << coordinate[i][0] << " " << coordinate[i][1] << endl;
				out << coordinate[j][0] << " " << coordinate[j][1] << endl << endl << endl;
			}
		}
	}
	out.close();
	out.clear();
}

void PMEA::keepMatrix(string fileName, bool flag)
{
	static int num = 0;
	int eval = Global::msp_global->mp_problem->getEvaluations();
	int val = static_cast<int>(log(eval));
	if (flag || val > num)
	{
		num = val;
		ostringstream os;
		ofstream out;
		os << Global::g_arg[param_workingDir] << "Result/" << fileName << num << "_" << Global::g_arg[param_dataFile1] << ".txt";
		out.open(os.str().c_str());
		vector<vector<double>> edges = mvv_edgesInfo;
		for (int i = 0; i < m_numDim; i++)
		{
			double sum = 0;
			for (int j = 0; j < m_numDim; j++)
				sum += edges[i][j];
			for (int j = 0; j < m_numDim; j++)
				edges[i][j] /= sum;
			for (int j = 0; j < m_numDim; j++)
				out << j << " " << i << " " << edges[i][j] <<endl;
			out << endl;
		}
		out.close();
		out.clear();
	}
}

bool PMEA::isConvergence(int val)
{
	double tempdif = 0;
	for (int i = 0; i < m_popsize; i++)
		tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	//cout << tempdif << endl;
	if (tempdif > val)
		return false;
	else
		return true;
}