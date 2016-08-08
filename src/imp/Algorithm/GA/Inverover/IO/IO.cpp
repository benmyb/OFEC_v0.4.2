#include "IO.h"
#include "../../../../Global/global.h"
#include "../../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#include "../../../../Problem/Combination/TSP/TravellingSalesman.h"
#ifdef OFEC_DEMON
#include "../../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

IO::IO(ParamMap &v) : GAPopulation<CodeVInt,GAIndividual<CodeVInt>>(v[param_popSize],true),m_num(0), m_saveFre(1500)
{
	m_name=(string)v[param_algName];

	m_term.reset(new TermMean(v));
}

void IO::inverse(int g,int gl,GAIndividual<CodeVInt> &indivl)
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

int IO::choseNextCity(vector<int> &a,const int &g,double p,int n,int &gl,vector<int> &visited)
{
	int i,j;
	int g1=g, gl1=gl;
	if(p<=0.02)
	{
		if(visited[g1]==0) 
		{
			for(i=0;i<m_numDim;i++)
				if(a[i]==g1)
				{
					j=i;
					break;
				}
			for(i=j;i<m_numDim-1;i++)
				a[i]=a[i+1];
			a[i]=g1;	
			visited[g1]=1;
			n++;
		} 
		if(n<m_numDim)
		{
			i= Global::msp_global->getRandInt(0,m_numDim-n);  
			gl=a[i];
		}
	}
	else
	{
		i=Global::msp_global->getRandInt(0,m_popsize);
		for(j=0;j<m_numDim;j++)
		{
			if(m_pop[i]->data().m_x[j]==g)
			{
				gl=m_pop[i]->data().m_x[(j+1)%m_numDim];
				break;
			}
		}
		gl1=gl;
		if(visited[gl1]==0) 
		{
			visited[gl1]=1;
			n++;
			for(i=0;i<m_numDim;i++)
				if(a[i]==gl1)
				{
					j=i;
					break;
				}
			for(i=j;i<m_numDim-1;i++)
				a[i]=a[i+1];
			a[i]=gl1;	
		}
	}
	return n;
}

bool IO::ifTerminating()
{
	if (dynamic_cast<TermMean*>(m_term.get())->ifTerminating(mean())) {
		if (m_iter == 0) return false;
		return true;
	}

	return false;
}

ReturnFlag IO::run_()
{
	int inverse_size0=0,i,k,pos,inverse_size1,n;
	double p;//a random number
	int g,gl;
	vector<int> visited(m_numDim);
	vector<int> arr(m_numDim);
	vector<int> a(m_numDim);
	GAIndividual<CodeVInt> indivl;

	Solution<CodeVInt> preBest(Solution<CodeVInt>::getBestSolutionSoFar());
	vector<bool> flag(m_popsize,false);
	/*	stringstream name;
	name<<m_name;
	name<<(int)Global::g_arg[param_populationInitialMethod];
	name<<".txt";
	ofstream mean(name.str().c_str());
	*/

	dynamic_cast<TermMean*>(m_term.get())->initialize(mean());

	while(!ifTerminating())
	{
		
		/*if(Global::msp_global->mp_problem->getEvaluations()/m_saveFre<m_num){
			mean<<diffEdges()<<" "<<Global::msp_global->mp_problem->getEvaluations()<<endl;			
		}*/

		#ifdef OFEC_DEMON
			for(i=0;i<this->getPopSize();i++)
				updateBestArchive(this->m_pop[i]->self());
			vector<Algorithm*> vp;	
			vp.push_back(this);	
			msp_buffer->updateBuffer_(&vp);
		#endif
		//cout<<Global::msp_global->mp_problem->getEvaluations()<<endl;
		for(k=0;k<m_popsize;k++)
		{
			inverse_size1=inverse_size0;
			indivl=*m_pop[k];
			i=Global::msp_global->getRandInt(0,m_numDim);
			gl=indivl.data()[i];
			pos=0;
			for(i=0;i<m_numDim;i++)
			{
				a[i]=i;
				visited[i]=0;
			}
			n=0;
			while(1)
			{ 
				g=gl;
				p=Global::msp_global->mp_uniformAlg->Next();
				n=choseNextCity(a,g,p,n,gl,visited);
				if(n==m_numDim) break;
				for(i=0;i<m_numDim;i++)
				{
					if(indivl.data()[i]==g&&(indivl.data()[(i-1+m_numDim)%m_numDim]==gl||indivl.data()[(i+1)%m_numDim]==gl))
					{
						pos=1;
						break;
					}
				}
				if(pos==1)
				{
					if(inverse_size0>inverse_size1)
					{
						indivl.evaluate();
						if(indivl>*m_pop[k]){
							*m_pop[k]=indivl;
							flag[k]=true;
						}
					}
					break;
				}
				if(g!=gl){inverse(g,gl,indivl);inverse_size0++;}
			}
		}
		
		#ifdef OFEC_CONSOLE
		double impRatio=count(flag.begin(),flag.end(),true)*1./m_popsize;
		fill(flag.begin(),flag.end(),false);
		/*int dif=preBest.getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
		if(dif!=0)		preBest=Solution<CodeVInt>::getBestSolutionSoFar(); */
		double tempdif = 0;
		for (int i = 0; i < m_popsize; i++)
			tempdif += m_pop[i]->self().getDistance(Solution<CodeVInt>::getBestSolutionSoFar());
		tempdif /= m_popsize;
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordiffAndImp(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations(), fabs(tempdif), impRatio);
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<GAIndividual<CodeVInt>>(Global::msp_global.get(),Solution<CodeVInt>::getBestSolutionSoFar(),m_pop,m_num,m_popsize,m_saveFre);	
#endif
		++m_iter;
		
	}
	//mean.close();
	#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<GAIndividual<CodeVInt>>(Global::msp_global.get(),Solution<CodeVInt>::getBestSolutionSoFar(),m_pop,m_num,m_popsize,m_saveFre,false);
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
	#endif
	return Return_Terminate;
}


double IO::diffEdges(){
	int numDim=Global::msp_global->mp_problem->getNumDim();
	double diffs=0;
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
			edges[m_pop[i]->data()[j]][m_pop[i]->data()[(j+1)%numDim]]=1;
			edges[m_pop[i]->data()[(j+1)%numDim]][m_pop[i]->data()[j]]=1;
		}
	}

		for(int i=0;i<numDim;i++)
			if(edges[CAST_TSP->getGOpt()[0].data().m_x[i]][CAST_TSP->getGOpt()[0].data().m_x[(i+1)%numDim]])
				diffs+=1;
		diffs/=numDim;

	return diffs;
}