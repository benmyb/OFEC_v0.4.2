#include "GSTM.h"
#include "../../../Global/global.h"
#include "../../../Problem/Combination/TSP/OptimalEdgeInfo.h"
#include "../../../Problem/Combination/TSP/TravellingSalesman.h"
#ifdef OFEC_DEMON
#include "../../../../ui/Buffer/Scene.h"
extern unique_ptr<Scene> msp_buffer;
extern bool g_algTermination;
#endif

GSTM::GSTM(ParamMap &v) : GAPopulation<CodeVInt, GAIndividual<CodeVInt>>(v[param_popSize], true), m_num(0), \
m_saveFre(1500), m_selectK(5), m_PL(0.2), m_NLMax(5), m_LMIN(2), m_PRC(0.5), m_PCP(0.8), m_PC(0.8), m_PM(0.2)
{
	m_LMAX = static_cast<int>(sqrt(m_numDim));
	m_name = (string)v[param_algName];

	m_term.reset(new TermMean(v));
}

void GSTM::selection(GAIndividual<CodeVInt> &indivl1, GAIndividual<CodeVInt> &indivl2)
{
	vector<int> arr(m_popsize);
	int i, j, local = -1;
	for (i = 0; i<m_popsize; i++)
		arr[i] = i;
	for (i = 0; i<2; i++)
	{
		double min = DBL_MAX;
		for (j = 0; j<m_selectK; j++)
		{
			int pos = Global::msp_global->getRandInt(0, m_popsize - 1 - j);
			if (min> m_pop[arr[pos]]->data().m_obj[0])
			{
				min = m_pop[arr[pos]]->data().m_obj[0];
				local = arr[pos];
			}
			arr[pos] = arr[m_popsize - 1 - j];
		}
		if (i == 0) indivl1 = *m_pop[local];
		if (i == 1) indivl2 = *m_pop[local];
	}
}

GAIndividual<CodeVInt> GSTM::crossover(const GAIndividual<CodeVInt> &indivl1, const GAIndividual<CodeVInt> &indivl2)
{
	if (indivl1 == indivl2)
		return indivl1;
	GAIndividual<CodeVInt> indivl;
	int i, j, n;
	vector<int> arr(m_numDim), flag(m_numDim+1);
	vector<vector<int>> matrix1(m_numDim), matrix2(m_numDim);
	for (i = 0; i < m_numDim; i++)
	{
		matrix1[i].resize(m_numDim);
		matrix2[i].resize(m_numDim);
	}
	map<int, vector<int> > frag;
	for (i = 0; i<m_numDim; i++)
		arr[i] = indivl1.data().m_x[i];
	for (i = 0; i<m_numDim + 1; i++)
		flag[i] = 0;
	for (i = 0; i<m_numDim; i++)
	{
		for (j = 0; j<m_numDim; j++)
		{
			matrix1[i][j] = 0;
			matrix2[i][j] = 0;
		}
	}
	for (i = 0; i<m_numDim; i++)
	{
		matrix1[arr[i]][arr[(i + 1) % m_numDim]] = 1;
		matrix1[arr[(i + 1) % m_numDim]][arr[i]] = 1;
		matrix2[indivl2.data().m_x[i]][indivl2.data().m_x[(i + 1) % m_numDim]] = 1;
		matrix2[indivl2.data().m_x[(i + 1) % m_numDim]][indivl2.data().m_x[i]] = 1;
	}
	for (i = 0; i<m_numDim; i++)
	{
		if (matrix2[arr[i]][arr[(i + 1) % m_numDim]] == 0)
			flag[i + 1] = 1;
	}
	flag[0] = flag[m_numDim];
	int m = 0;
	n = 0;
	j = 0;
	frag[n].push_back(arr[j++]);
	m++;
	while (flag[j] == 0 && m<m_numDim)
	{
		frag[n].push_back(arr[j++]);
		m++;
	}
	i = m_numDim;
	while (flag[i] == 0 && m<m_numDim)
	{
		frag[n].insert(frag[n].begin(), arr[i - 1]);
		i--;
		m++;
	}
	while (m<m_numDim)
	{
		if (flag[j] == 1) n++;
		frag[n].push_back(arr[j++]);
		m++;
		while (flag[j] == 0)
		{
			frag[n].push_back(arr[j++]);
			m++;
		}
	}
	vector<int> visited(m_numDim);
	map<int, int> mp;
	for (i = 0; i<m_numDim; i++)
		visited[i] = 0;
	m = 0;
	for (i = 0; i <= n; i++)
	{
		visited[m++] = frag[i].front();
		mp[visited[m - 1]] = i;
		if (frag[i].size()>1)
		{
			visited[m++] = frag[i].back();
			mp[visited[m - 1]] = i;
		}
	}
	int starts;
	i = Global::msp_global->getRandInt(0, m - 1);
	j = 0;
	arr[j++] = visited[i];
	starts = visited[i];
	if (frag[mp[visited[i]]].front() == starts)
	{
		for (size_t z = 1; z<frag[mp[visited[i]]].size(); z++)
			arr[j++] = frag[mp[visited[i]]][z];
		starts = frag[mp[visited[i]]].back();
		if (frag[mp[visited[i]]].size()>1)
		{
			int fg = 0;
			if (visited[m - 1] == visited[i]) fg = 1;
			for (int z = 0; z<m; z++)
			{
				if (visited[z] == frag[mp[visited[i]]].back())
				{
					visited[z] = visited[m - 1];
					m--;
					if (fg == 1) i = z;
					break;
				}
			}
		}
		visited[i] = visited[m - 1];
		m--;
	}
	else if (frag[mp[visited[i]]].back() == starts)
	{
		for (int z = frag[mp[visited[i]]].size() - 2; z >= 0; z--)
			arr[j++] = frag[mp[visited[i]]][z];
		starts = frag[mp[visited[i]]].front();
		if (frag[mp[visited[i]]].size()>1)
		{
			int fg = 0;
			if (visited[m - 1] == visited[i]) fg = 1;
			for (int z = 0; z<m; z++)
			{
				if (visited[z] == frag[mp[visited[i]]].front())
				{
					visited[z] = visited[m - 1];
					m--;
					if (fg == 1) i = z;
					break;
				}
			}
		}
		visited[i] = visited[m - 1];
		m--;
	}
	n--;
	for (i = 0; i <= n; i++)
	{
		double min = DBL_MAX;
		int temp;
		int pos = -1;
		TravellingSalesman * ptr = dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get());
		for (int z = 0; z<m; z++)
		{
			if (min>ptr->getCost()[starts][visited[z]] && matrix1[starts][visited[z]] == 0 && matrix2[starts][visited[z]] == 0)
			{
				min = ptr->getCost()[starts][visited[z]];
				temp = visited[z];
				pos = z;
			}
		}
		if (pos == -1)
		{
			temp = visited[0];
			pos = 0;
		}
		arr[j++] = temp;
		if (frag[mp[temp]].front() == temp)
		{
			for (size_t z = 1; z<frag[mp[temp]].size(); z++)
				arr[j++] = frag[mp[temp]][z];
			starts = frag[mp[temp]].back();
			if (frag[mp[temp]].size()>1)
			{
				int fg = 0;
				if (visited[m - 1] == visited[pos]) fg = 1;
				for (int z = 0; z<m; z++)
				{
					if (visited[z] == frag[mp[temp]].back())
					{
						visited[z] = visited[m - 1];
						m--;
						if (fg == 1) pos = z;
						break;
					}
				}
			}
			visited[pos] = visited[m - 1];
			m--;
		}
		else if (frag[mp[temp]].back() == temp)
		{
			for (int z = frag[mp[temp]].size() - 2; z >= 0; z--)
				arr[j++] = frag[mp[temp]][z];
			starts = frag[mp[temp]].front();
			if (frag[mp[temp]].size()>1)
			{
				int fg = 0;
				if (visited[m - 1] == visited[pos]) fg = 1;
				for (int z = 0; z<m; z++)
				{
					if (visited[z] == frag[mp[temp]].front())
					{
						visited[z] = visited[m - 1];
						m--;
						if (fg == 1) pos = z;
						break;
					}
				}
			}
			visited[pos] = visited[m - 1];
			m--;
		}
	}
	for (i = 0; i<m_numDim; i++)
		indivl.data().m_x[i] = arr[i];
	return indivl;
}

void Reconnection(GAIndividual<CodeVInt> &indivl, int starts, int ends)
{
	vector<int> T, subTour;
	size_t i;
	int startPos;
	int numDim = indivl.getNumDim();
	for (int j = 0; j<numDim; j++)
	{
		if (indivl.data().m_x[j] == starts)
		{
			startPos = j;
			break;
		}
	}
	while (indivl.data().m_x[startPos] != ends)
	{
		subTour.push_back(indivl.data().m_x[startPos]);
		startPos = (startPos + 1) % numDim;
	}
	subTour.push_back(indivl.data().m_x[startPos]);
	startPos = (startPos + 1) % numDim;
	while (indivl.data().m_x[startPos] != starts)
	{
		T.push_back(indivl.data().m_x[startPos]);
		startPos = (startPos + 1) % numDim;
	}
	double len1 = 0, len2 = 0;
	TravellingSalesman *ptr = dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get());
	for (i = 0; i<T.size(); i++)
		len1 += ptr->getCost()[T[i]][T[(i + 1) % T.size()]];
	for (i = 0; i<subTour.size(); i++)
		len2 += ptr->getCost()[subTour[i]][subTour[(i + 1) % subTour.size()]];
	bool flag;
	size_t pos;
	double min = DBL_MAX;
	for (i = 0; i<T.size(); i++)
	{
		double temp;
		temp = len1 + len2 + ptr->getCost()[T[i]][starts] + ptr->getCost()[ends][T[(i + 1) % T.size()]] - ptr->getCost()[T[i]][T[(i + 1) % T.size()]] - ptr->getCost()[starts][ends];
		if (min>temp)
		{
			min = temp;
			pos = i;
			flag = true;
		}
		temp = len1 + len2 + ptr->getCost()[T[i]][ends] + ptr->getCost()[starts][T[(i + 1) % T.size()]] - ptr->getCost()[T[i]][T[(i + 1) % T.size()]] - ptr->getCost()[starts][ends];
		if (min>temp)
		{
			min = temp;
			pos = i;
			flag = false;
		}
	}
	int j = 0;
	for (i = 0; i<T.size(); i++)
	{
		if (i == pos)
		{
			indivl.data().m_x[j++] = T[i];
			if (flag == true)
			{
				for (size_t z = 0; z<subTour.size(); z++)
					indivl.data().m_x[j++] = subTour[z];
			}
			else if (flag == false)
			{
				for (int z = subTour.size() - 1; z >= 0; z--)
					indivl.data().m_x[j++] = subTour[z];
			}
		}
		else
			indivl.data().m_x[j++] = T[i];
	}
}

void DistortionMethod(GAIndividual<CodeVInt> &indivl, int starts, int ends, double pl)
{
	vector<int> subTour;
	int startPos, i;
	int numDim = indivl.getNumDim();
	for (i = 0; i<numDim; i++)
	{
		if (indivl.data().m_x[i] == starts)
		{
			startPos = i;
			break;
		}
	}
	int loc = startPos;
	while (indivl.data().m_x[startPos] != ends)
	{
		subTour.push_back(indivl.data().m_x[startPos]);
		startPos = (startPos + 1) % numDim;
	}
	subTour.push_back(indivl.data().m_x[startPos]);
	while (!subTour.empty())
	{
		double rnd = Global::msp_global->mp_uniformAlg->Next();
		if (rnd <= pl)
		{
			int temp = Global::msp_global->getRandInt(0, subTour.size() - 1);
			indivl.data().m_x[loc] = subTour[temp];
			loc = (loc + 1) % numDim;
			subTour.erase(subTour.begin() + temp);
		}
		else
		{
			indivl.data().m_x[loc] = subTour.back();
			loc = (loc + 1) % numDim;
			subTour.erase(subTour.end() - 1);
		}
	}
}

void inverse(int g, int gl, GAIndividual<CodeVInt> &indivl)
{
	int i, j, pos1, pos2;
	int pos;
	int numDim = indivl.getNumDim();
	for (i = 0; i<numDim; i++)
	{
		if (indivl.data().m_x[i] == g)
			pos1 = (i + 1) % numDim;
		if (indivl.data().m_x[i] == gl)
			pos2 = i;
	}
	if (pos2>pos1)
	{
		j = pos2 - pos1 + 1;
		for (i = 0; i<j / 2; i++)
		{
			pos = indivl.data().m_x[pos1];
			indivl.data().m_x[pos1] = indivl.data().m_x[pos2];
			indivl.data().m_x[pos2] = pos;
			pos1++;
			pos2--;
		}
	}
	else if (pos2<pos1)
	{
		j = numDim - pos1 + pos2 + 1;
		for (i = 0; i<j / 2; i++)
		{
			pos = indivl.data().m_x[pos1];
			indivl.data().m_x[pos1] = indivl.data().m_x[pos2];
			indivl.data().m_x[pos2] = pos;
			pos1 = (pos1 + 1) % numDim;
			pos2 = (pos2 - 1 + numDim) % numDim;
		}
	}
}

void Rotation(GAIndividual<CodeVInt> &indivl, int starts, int ends, int nlMax)
{
	int i, j, z;
	int numDim = indivl.getNumDim();
	vector<int> NL(nlMax), visited(numDim);
	GAIndividual<CodeVInt> indivll[2];
	int point[2] = { starts, ends };
	TravellingSalesman *ptr = dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get());
	for (i = 0; i<2; i++)
	{
		for (j = 0; j<numDim; j++)
			visited[j] = 0;
		for (j = 0; j<nlMax; j++)
		{
			double min = DBL_MAX;
			int pos;
			for (z = 0; z<numDim; z++)
			{
				if (min>ptr->getCost()[point[i]][z] && z != point[i] && visited[z] == 0)
				{
					min = ptr->getCost()[point[i]][z];
					pos = z;
				}
			}
			NL[j] = pos;
			visited[pos] = 1;
		}
		int flag, temp;
		j = 0;
		do
		{
			flag = 0;
			temp = Global::msp_global->getRandInt(0, nlMax - 1 - j);
			for (z = 0; z<numDim; z++)
			{
				if (indivl.data().m_x[z] == point[i] && (indivl.data().m_x[(z + 1) % numDim] == NL[temp] || indivl.data().m_x[(z - 1 + numDim) % numDim] == NL[temp]))
				{
					flag = 1;
					break;
				}
			}
			if (flag)
			{
				NL[temp] = NL[nlMax - 1 - j];
				j++;
			}
		} while (flag);
		indivll[i] = indivl;
		inverse(point[i], NL[temp], indivll[i]);
		indivll[i].evaluate();
	}
	if (indivll[0] > indivll[1])
		indivl = indivll[0];
	else
		indivl = indivll[1];
}

void GSTM::mutation(GAIndividual<CodeVInt> &indivl)
{
	int starts, ends;
	vector<int> arr(m_numDim), tour(m_numDim);
	int i, j, z;
	int len = 0;
	for (i = 0; i<m_numDim; i++)
	{
		arr[i] = i;
		tour[i] = indivl.data().m_x[i];
	}
	starts = Global::msp_global->getRandInt(0, m_numDim - 1);
	arr[starts] = arr[m_numDim - 1];
	i = 0;
	for (z = 0; z<m_numDim; z++)
	{
		if (tour[z] == starts)
		{
			j = z;
			break;
		}
	}
	do
	{
		int temp = Global::msp_global->getRandInt(0, m_numDim - 2 - i);
		ends = arr[temp];
		arr[temp] = arr[m_numDim - 2 - i];
		i++;
		for (int t = 0; t<m_numDim; t++)
		{
			if (tour[t] == ends)
			{
				z = t;
				break;
			}
		}
		len = (z - j + m_numDim) % m_numDim;
	} while (len<m_LMIN || len>m_LMAX);
	double rnd = Global::msp_global->mp_uniformAlg->Next();
	if (rnd <= m_PRC)
		Reconnection(indivl, starts, ends);
	else
	{
		double rnd1 = Global::msp_global->mp_uniformAlg->Next();
		if (rnd1 <= m_PCP)
			DistortionMethod(indivl, starts, ends, m_PL);
		else
			Rotation(indivl, starts, ends, m_NLMax);
	}
}

bool GSTM::ifTerminating()
{
	if (dynamic_cast<TermMean*>(m_term.get())->ifTerminating(mean())) {
		if (m_iter == 0) return false;
		return true;
	}

	return false;
}

ReturnFlag GSTM::run_()
{
	GAPopulation<CodeVInt, GAIndividual<CodeVInt>> subPopul(m_popsize);
	GAIndividual<CodeVInt> ia, ib;
	int i, n;
	double p;
	int flag, flag1;
	double bestlen = DBL_MAX;
	vector<int> arr(m_numDim);

	dynamic_cast<TermMean*>(m_term.get())->initialize(mean());
	while (!ifTerminating())
	{

		/*if(Global::msp_global->mp_problem->getEvaluations()/m_saveFre<m_num){
		mean<<diffEdges()<<" "<<Global::msp_global->mp_problem->getEvaluations()<<endl;
		}*/

#ifdef OFEC_DEMON
		for (i = 0; i<this->getPopSize(); i++)
			updateBestArchive(this->m_pop[i]->self());
		vector<Algorithm*> vp;
		vp.push_back(this);
		msp_buffer->updateBuffer_(&vp);
#endif
		//cout<<Global::msp_global->mp_problem->getEvaluations()<<endl;

		n = 0;
		while (n<m_popsize)
		{
			flag = 0;
			flag1 = 0;
			selection(ia, ib);
			p = Global::msp_global->mp_uniformAlg->Next();
			if (p <= m_PC)
			{
				*subPopul.getPop()[n++] = crossover(ia, ib);
				flag = 1;
			}
			if (flag == 0)
			{
				if (n<m_popsize - 1)
				{
					*subPopul.getPop()[n++] = ia;
					*subPopul.getPop()[n++] = ib;
				}
				else
				{
					*subPopul.getPop()[n++] = ia;
					flag1 = 1;
				}
			}
			p = Global::msp_global->mp_uniformAlg->Next();
			if (p <= m_PM)
			{
				if (flag == 1 || flag1 == 1) mutation(*subPopul.getPop()[n - 1]);
				else
				{
					mutation(*subPopul.getPop()[n - 2]);
					mutation(*subPopul.getPop()[n - 1]);
				}
			}
		}
		for (i = 0; i<m_popsize; i++)
			subPopul.getPop()[i]->evaluate();
		*(static_cast<GAPopulation<CodeVInt, GAIndividual<CodeVInt>>*>(this)) = subPopul;

#ifdef OFEC_CONSOLE
		OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<GAIndividual<CodeVInt>>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre);
#endif
		m_iter++;
	}

#ifdef OFEC_CONSOLE
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordEdgeInfo<GAIndividual<CodeVInt>>(Global::msp_global.get(), Solution<CodeVInt>::getBestSolutionSoFar(), m_pop, m_num, m_popsize, m_saveFre, false);
	OptimalEdgeInfo::getOptimalEdgeInfo()->recordLastInfo(Global::msp_global->m_runId, Global::msp_global->mp_problem->getEvaluations());
#endif
	return Return_Terminate;
}