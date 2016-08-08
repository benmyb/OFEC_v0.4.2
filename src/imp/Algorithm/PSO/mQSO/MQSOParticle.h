#ifndef MQSOPARTICLE_H
#define MQSOPARTICLE_H

#include "../Particle.h"

class SAMOSwarm;
class MQSOSwarm;
class MQSOSubSwarm;
class MQSOParticle : public Particle
{
	enum ParticleType{PARTICLE_NEUTRAL,PARTICLE_QUANTUM,PARTICLE_CHARGED};
	friend class MQSOSubSwarm;
	friend class MQSOSwarm;
	friend class SAMOSwarm;
    public:
        MQSOParticle();
        ~MQSOParticle();
        MQSOParticle(const MQSOParticle& other);

        void increaseDimension();
        void decreaseDimension();
        ReturnFlag move(const Solution<CodeVReal> &gbest,double w, double c1, double c2,double r);
        void setType(ParticleType t);
    private:
        ParticleType m_type;
        vector<double> mv_repulse;
};

#endif // MQSOPARTICLE_H
