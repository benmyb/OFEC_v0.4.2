#include "ZDT2.h"

ZDT2::ZDT2(ParamMap &v):Problem((v[param_proId]), (v[param_numDim]),(v[param_proName]),2),ZDT(v)
{
  generateAdLoadPF();
}

void ZDT2::evaluate__(double const *x,vector<double>& obj)
{
	double g = 0;
	for(int n=1;n<m_numDim;n++)
		g=g+x[n];
	g = 1 + 9*g/(m_numDim-1);
	obj[0] = x[0];
	obj[1] = g*(1 - pow(obj[0]/g,2));
}

