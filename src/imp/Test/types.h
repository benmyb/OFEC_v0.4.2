#ifndef TYPES_ALG_PRO_H
#define TYPES_ALG_PRO_H
#include "../Utility/TypeList/Typelist.h"
#include "test.h"

typedef  FAMF<FAMFParticle,Swarm<CodeVReal,FAMFParticle>,FAMFPopPSO> ALG_FAMF_PSO;
typedef  FAMF<FAMFIndiDE,DEPopulation<CodeVReal,FAMFIndiDE>,FAMFPopDE> ALG_FAMF_DE;

typedef LOKI_TYPELIST_50(DynDEPopulation,SOSPopulation,FTSwarm,CPSOSwarm,CPSORSwarm,DynPopDEPopulation,AMSOSwarm,SPSO07,MQSOSwarm,SAMOSwarm,
		SpeciesSwarm,NFishSwarm,mNAFSASwarm,NSGAII_DExover2RealMu,NSGAII_SBXRealMu,NSGAIII_DExover2RealMu,NSGAIII_SBXRealMu,MOEAD_DExover2RealMu
		,MOEAD_SBXRealMu,Nrand1DEPopulation,ALG_FAMF_PSO,ALG_FAMF_DE,CRDEPopulation,SwarmGBest,ESCA,APSO,CPSOH,SLPSO,HmSO,IO,IOPM,AS,PMEA,LKH::LKHAlg,
		DEBest2, DERand1, ACS, MMAS, GSTM, SIM, MPL_E, MELACO, MIMIC, SPBBO, EHBSA, NHBSA, RADE, SPSO11,RASPSO11, RASPSOG) AlgList50;
typedef	Loki::TL::Append<AlgList50, LinkageDE_>::Result AlgList51;
typedef	Loki::TL::Append<AlgList51, CMAES>::Result AlgList;


typedef LOKI_TYPELIST_50(FSphere,FNoncont_Rastrigin,FModified_Rastrigin,FRastrigin,FWeierstrass,FGriewank,FAckley,FStep,FQuartic_Noisy,FScaffer_F6,
		FRosenbrock,FSchwefel_2_13,FSchwefel_2_22,FSchwefel_1_2,FSchwefel_2_21,FSchwefel_2_6,FSchwefel,FPenalized_1,FPenalized_2,HybridComp,FElliptic,
		FMAX_global1,FMAX_global2,FMAX_global3,FMAX_global4,FMAX_global5,FGear_Train,FParEst_FMSoundWaves,FSix_humpCamelBack,FWaves,FBraninRCOS,
		FShubert,FMichalewicz,FValleys,FFive_hills,FCenter_peak,FKeane_Bump,FBeasley,FHimmenblau,FModified_Shekel,FSzu,FIBA,FShaffer,FVincent,
		CompositionDBG,RotationDBG,MovingPeak,ZDT1,ZDT2,ZDT3) ProList50;
typedef	Loki::TL::Append<ProList50,ZDT4>::Result ProList51;
typedef	Loki::TL::Append<ProList51,ZDT6>::Result ProList52;
typedef	Loki::TL::Append<ProList52,DTLZ1>::Result ProList53;
typedef	Loki::TL::Append<ProList53,DTLZ2>::Result ProList54;
typedef	Loki::TL::Append<ProList54,DTLZ3>::Result ProList55;
typedef	Loki::TL::Append<ProList55,DTLZ4>::Result ProList56;
typedef	Loki::TL::Append<ProList56,F1>::Result ProList57;
typedef	Loki::TL::Append<ProList57,F2>::Result ProList58;
typedef	Loki::TL::Append<ProList58,F3>::Result ProList59;
typedef	Loki::TL::Append<ProList59,F4>::Result ProList60;
typedef	Loki::TL::Append<ProList60,F5>::Result ProList61;
typedef	Loki::TL::Append<ProList61,F6>::Result ProList62;
typedef	Loki::TL::Append<ProList62,F7>::Result ProList63;
typedef	Loki::TL::Append<ProList63,F8>::Result ProList64;
typedef	Loki::TL::Append<ProList64,F9>::Result ProList65;
typedef	Loki::TL::Append<ProList65, TravellingSalesman>::Result ProList66;
typedef	Loki::TL::Append<ProList66, QuadraticAssignment>::Result ProList67;
typedef	Loki::TL::Append<ProList67, MultidimensionalKnapsack>::Result ProList68;
typedef	Loki::TL::Append<ProList68, FOnePeak>::Result ProList69;
typedef	Loki::TL::Append<ProList69, FFreePeak_OnePeak>::Result	ProList70;
typedef	Loki::TL::Append<ProList70, FFreePeak_D_OnePeak>::Result ProList71;
typedef	Loki::TL::Append<ProList71, FFreePeak_M_OnePeak>::Result	ProList72;
typedef	Loki::TL::Append<ProList72, FreePeak_D_M_OnePeak>::Result	ProList;



#endif

