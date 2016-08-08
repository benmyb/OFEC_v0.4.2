#include "IOPM.h"

#include "../../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#include "../../../../Problem/Combination/TSP/TravellingSalesman.h"
#ifdef OFEC_DEMON
#include "../../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

IOPM::IOPM(ParamMap &v) : GAPopulation<CodeVInt,GAIndividual<CodeVInt>>(v[param_popSize]),m_num(0),m_saveFre(1500),mv_flag(v[param_popSize],true),m_preDiffs(0),m_weight(v[param_popSize])
{
	m_flag[0]=true;
	mvv_proMatrix.resize(m_numDim);
	for(int i=0;i<m_numDim;i++)
		mvv_proMatrix[i].resize(m_numDim);
	m_staType=strategy ((int)v[param_case]);
	m_name=(string)v[param_algName];

	m_term.reset(new TermMean(v));
}

void IOPM::inverse(int g,int gl,GAIndividual<CodeVInt> &indivl)
{
	int i,j,pos1,pos2;
	int pos;
	for(i=0;i<m_numDim;i++)
	{
		if(indivl.data()[i]==g)
			pos1=(i+1)%m_numDim;
		if(indivl.data()[i]==gl)
			pos2=i;
	}
	if(pos2>pos1)
	{
		j=pos2-pos1+1;
		for(i=0;i<j/2;i++)
		{
			pos=indivl.data()[pos1];
			indivl.data()[pos1]=indivl.data()[pos2];
			indivl.data()[pos2]=pos;
			pos1++;
			pos2--;
		}
	}
	else if(pos2<pos1)
	{
		j=m_numDim-pos1+pos2+1;
		for(i=0;i<j/2;i++)
		{
			pos=indivl.data()[pos1];
			indivl.data()[pos1]=indivl.data()[pos2];
			indivl.data()[pos2]=pos;
			pos1=(pos1+1)%m_numDim;
			pos2=(pos2-1+m_numDim)%m_numDim;
		}
	}
}

bool IOPM::ifTerminating()
{
	if (dynamic_cast<TermMean*>(m_term.get())->ifTerminating(mean())) {
		if (m_iter == 0) return false;
		return true;
	}

	return false;
}

ReturnFlag IOPM::run_()
{
	int inverse_size0=0,i,k,pos,inverse_size1,coutt;
	double p;//a random number
	int g,gl;
	vector<int> visited(m_numDim);
	vector<int> arr(m_numDim);
	GAIndividual<CodeVInt> indivl;

	//updata(false);
	/*stringstream name;
	name<<m_name;
	name<<(int)Global::g_arg[param_populationInitialMethod];
	name<<".txt";
	ofstream mean(name.str().c_str());
	*/
	ReturnFlag rf = initialize(false, true, true);
	if (rf == Return_Normal) {
		dynamic_cast<TermMean*>(m_term.get())->initialize(mean());
		while (!ifTerminating())
		{
#ifdef OFEC_DEMON
			for(i=0;i<this->getPopSize();i++)
				updateBestArchive(this->m_pop[i]->self());
			vector<Algorithm*> vp;	
			vp.push_back(this);	
			msp_buffer->updateBuffer_(&vp);
#endif
			populProMatrix();

			/*if(Global::msp_global->mp_problem->getEvaluations()/m_saveFre<m_num){
				//double maxw=*max_element(m_weight.begin(),m_weight.end());
				//mean<<accumulate(m_weight.begin(),m_weight.end(),m_popsize*maxw,minus<double>())/m_popsize<<" "<<Global::msp_global->mp_problem->getEvaluations()<<endl;
				//std::stable_sort(m_weight.begin(),m_weight.end());
				mean<<diffEdges()<<" "<<Global::msp_global->mp_problem->getEvaluations()<<endl;
				}*/
			Solution<CodeVInt> preBest(Solution<CodeVInt>::getBestSolutionSoFar());
			for (k = 0; k < m_popsize; k++)
			{
				inverse_size1 = inverse_size0;
				indivl = *m_pop[k];
				i = Global::msp_global->getRandInt(0, m_numDim);
				gl = indivl.data()[i];
				pos = 0;
				coutt = 0;
				for (i = 0; i < m_numDim; i++)
					visited[i] = 0;
				while (1)
				{
					g = gl;
					p = Global::msp_global->mp_uniformAlg->Next();
					int temp = g;
					visited[temp] = 1;
					for (i = 0; i < m_numDim - 1; i++)
					{
						if (p < mvv_proMatrix[temp][0] && visited[0] == 0) { gl = 0; visited[0] = 1; coutt = 0; break; }
						else if (p >= mvv_proMatrix[temp][i] && p < mvv_proMatrix[temp][i + 1] && visited[i + 1] == 0) { gl = i + 1; visited[i + 1] = 1; coutt = 0; break; }
					}
					if (i == m_numDim - 1) coutt++;
					for (i = 0; i < m_numDim; i++)
					{
						if (indivl.data()[i] == g && (indivl.data()[(i - 1 + m_numDim) % m_numDim] == gl || indivl.data()[(i + 1) % m_numDim] == gl))
						{
							pos = 1;
							break;
						}
					}
					if (pos == 1 || coutt > 5)
					{
						if (inverse_size0 > inverse_size1)
						{
							rf = indivl.evaluate();
							if (rf == Return_Terminate) break;
							if (indivl > *m_pop[k])
							{
								*m_pop[k] = indivl;
								mv_flag[k] = true;
							}
						}
						break;
					}
					if (g != gl){ inverse(g, gl, indivl); inverse_size0++; }
				}
				if (rf == Return_Terminate) break;
			}
			if (rf == Return_Terminate) break;
			//updata();
			//cout << Global::msp_global->mp_problem->getEvaluations() << endl;
#ifdef OFEC_CONSOLE
			double impRatio = count(mv_flag.begin(), mv_flag.end(), true)*1. / m_popsize;
			/*int dif = preBest.getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
			if (dif != 0)		preBest = Solution<CodeVInt>::getBestSolutionSoFar(); */
			OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(diffEdges()), impRatio);
			OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<GAIndividual<CodeVInt>>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif
			m_iter++;
		}
	}
	//mean.close();

	#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<GAIndividual<CodeVInt>>(Global::msp_global.get(),Solution<CodeVInt>::getBestSolutionSoFar(),m_pop,m_num,m_popsize,m_saveFre,false);
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
	#endif
	return Return_Terminate;
}


void IOPM::populProMatrix()
{
	int i,j;
	double sum=0;
	double Min=0xfffffff,Max=-0xfffffff;

	if(m_staType==ALL) 
	{
		for(i=0;i<m_popsize;i++)
			mv_flag[i]=true;
	}
	j=0;
	for(i=0;i<m_popsize;i++)
	{
		if(!mv_flag[i]) continue;
		if(m_pop[i]->obj(0)>Max)
			Max=m_pop[i]->obj(0);
		if(m_pop[i]->obj(0)<Min)
			Min=m_pop[i]->obj(0);
		j++;
	}
	if(j==0) return;

	for(i=0;i<m_popsize;i++)
	{
		if(!mv_flag[i]) continue;
		if(Global::msp_global->mp_problem->getOptType()==MIN_OPT)
			m_weight[i]=static_cast<double>((Max-m_pop[i]->obj(0)+1)/(Max-Min+1));
		else
			m_weight[i]=static_cast<double>((m_pop[i]->obj(0)-Min+1)/(Max-Min+1));
		//m_weight[i]=2*(1.0/(1+exp(-4*m_weight[i])))-1;
		m_weight[i] = 1. / (1 + exp(-m_weight[i]));
		sum+=m_weight[i];
	}	
	sum=2*sum;
	for(i=0;i<m_numDim;i++)
		for(j=0;j<m_numDim;j++)
			mvv_proMatrix[i][j]=0;
	for(i=0;i<m_popsize;i++)
	{
		if(!mv_flag[i]) continue;
		for(j=0;j<m_numDim;j++)
		{
			int temp=m_pop[i]->data()[j];
			int tempp=m_pop[i]->data()[(j+1)%m_numDim];
			mvv_proMatrix[temp][tempp]+=m_weight[i];
			mvv_proMatrix[tempp][temp]+=m_weight[i];
		}
	}
	for(i=0;i<m_numDim;i++)
	{
		for(j=i+1;j<m_numDim;j++)
		{
			mvv_proMatrix[i][j]=mvv_proMatrix[i][j]/sum;
			mvv_proMatrix[j][i]=mvv_proMatrix[i][j];
		}
	}
	for(i=0;i<m_numDim;i++)
		for(j=1;j<m_numDim;j++)
			mvv_proMatrix[i][j]+=mvv_proMatrix[i][j-1];

	for(i=0;i<m_popsize;i++)
		mv_flag[i]=false;
}

void IOPM::updata(bool flag)
{
	int numDim=Global::msp_global->mp_problem->getNumDim(),imps=0,diffs=0;
	vector<vector<int> > edges;
	edges.resize(numDim);
	for(int i=0;i<numDim;i++)
		edges[i].resize(numDim);
	for(int i=0;i<numDim;i++)
		for(int j=0;j<numDim;j++)
			edges[i][j]=0;
	for(int i=0;i<m_popsize;i++)
	{
		for(int j=0;j<numDim;j++)
		{
			int r=m_pop[i]->data().m_x[j];
			int c=m_pop[i]->data().m_x[(j+1)%numDim];
			edges[r][c]=1; edges[c][r]=1;
		}
	}
	for(int i=0;i<numDim;i++)
		for(int j=i+1;j<numDim;j++)
			if(edges[i][j])
				diffs++;

	for(int i=0;i<m_popsize;i++)
		if(mv_flag[i])
			imps++;

	if(!flag) imps=0;

	#ifdef OFEC_CONSOLE
	double dif=diffs-m_preDiffs;
	m_preDiffs=diffs;
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(),fabs(dif),static_cast<double>(imps)/m_popsize);
	#endif
}

double IOPM::diffEdges(){
	//int numDim=Global::msp_global->mp_problem->getNumDim();
	double diffs=0;
	/*vector<vector<int> > edges;
	edges.resize(numDim);
	for(int i=0;i<numDim;i++)
		edges[i].resize(numDim);
	for(int i=0;i<numDim;i++)
		for(int j=0;j<numDim;j++)
			edges[i][j]=0;
	for(int i=0;i<m_popsize;i++)
	{
		for(int j=0;j<numDim;j++)
		{
			edges[int(m_pop[i]->data()[j])][int(m_pop[i]->data()[(j+1)%numDim])]=1;
			edges[int(m_pop[i]->data()[(j+1)%numDim])][int(m_pop[i]->data()[j])]=1;
		}
	}

		for(int i=0;i<numDim;i++)
			if(edges[int(CAST_TSP->getGOpt()()[0].m_x[i])][int(CAST_TSP->getGOpt()()[0].m_x[(i+1)%numDim])])
				diffs+=1;
	
		diffs/=numDim;*/
	for(int i=0;i<m_popsize;i++){
		diffs+=m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
	}

	return diffs/m_popsize;
}