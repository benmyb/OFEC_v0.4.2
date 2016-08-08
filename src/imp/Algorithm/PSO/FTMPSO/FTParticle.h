#ifndef FTPARTICLE_H
#define FTPARTICLE_H

#include "../Particle.h"
class FTParticle: public Particle{
	friend class FTSubSwarm;
public:
	ReturnFlag move(  const Solution<CodeVReal> &,double w, double c1, double c2);
};

#endif