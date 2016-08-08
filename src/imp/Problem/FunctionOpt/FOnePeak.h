/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 20 June 2015
// Last modified:
#ifndef FONEPEAK_H
#define FONEPEAK_H
#include "BenchmarkFunction.h"

class FOnePeak final: public BenchmarkFunction
{
public:
	enum Shape{ SH1 = 1, SH2, SH3, SH4, SH5, SH6, SH7, SH8,SH9,SH10,SH11,SH12,SH13 };
	enum DistanceTransfm{ Separable = 1, NonSeparable, PartSeparable, Domino, FlatBorder,NR_ITEMS_DT};
	enum VariableTransfm {Normal,Rotation,Irregularity,Asyemmetry, NR_ITEMS_VT};
	enum Constraint { C_SphereAtPeak, C_SphereAtBorder, C_Sine, NR_ITEMS_C };


	FOnePeak(ParamMap &v);
	FOnePeak(const int rId, const int rDimNumber, string& rName, bool random = false, int shape = 1, double h = 100, vector<int> *transform=0,int label=0,vector<pair<bool, vector<double>>>* cstrt=0,bool noisy=false);
	FOnePeak(const int rId, const int rDimNumber, string& rName, const vector<double>&loc, int shape = 1, double h = 100, vector<int> *transform = 0, int label = 0, vector<pair<bool, vector<double>>>* cstrt = 0, bool noisy = false);
	~FOnePeak(){}
	double height(){ return m_height; }
	double location(const int idx){ return m_location[idx]; }
	vector<double> &location(){ return m_location; }
	const vector<double> &location()const{ return m_location; }
	void setLocationAtCenter();
	void setLocationRandom();
	void setLocationAtCorner();
	void changeHeight(const double h);	
	void shiftLocation(const vector<double> &x);
	void shiftLocation();
	void setMemorySize(const int);
	void setMaxHeight(const double val);
	const double &preHeight(const int idx=0){ return m_preHeight[idx]; }
	const vector<double>&preLoc(const int idx = 0){ return m_preLoc[idx]; }
	size_t preLocSize(){ return m_preLoc.size(); }
	size_t preHeightSize(){ return m_preHeight.size(); }
	void setHeight(const double h = 100);
	double robustness(){ return m_robustness; }
	void setLocation(const vector<double> &x);
	void setFeasibleRadius(double r = 0.5);
	void setRadiusStepFun(double r,bool flag=true);
	double getFurestDis(){ return m_furestDis; }
	double getNearestDis(){ return m_nearestDis; }
	void setSdandardize(bool f){ m_standardize = f; }

	void setHeightSeverity(const double s){ m_heightSeverity = s; }
	void setShiftSeverity(const double s){ m_shiftSeverity = s; }
	double getHeightSeverity(){ return m_heightSeverity; }
	void copyChanges(const Problem * pro, const vector<int> *cd = nullptr, const vector<int> *co = nullptr);
	void difficulty(double rh=1);
	void setBasinRatio(double r);
	void setVariableRelation(int vr){ m_vr = vr; }
	double basinRatio(){ return m_basinRatio; }
	double mean(){ return m_mean; }
	double variance(){ return m_variance; }
	void setStdMinHeight(double minh){ m_stdMinHeight = minh; }
	vector<double> getPoint(double oval, const MyVector& dir, int ith=1);   //get point which has objective value of oval in a direction of dir
	double getDistance2Center(double oval, int ith = 1);
	void getContiSegment(const vector<double> pos, vector<pair<vector<double>, vector<double>>> &seg); // return segments of monotonically decrease in objecitve value between peak center and pos
	void getContiSegment(const vector<double> p1, const vector<double> p2, vector<pair<vector<double>, vector<double>>> &seg); //isosceles trangle

	inline double getTransDistance(double const *x_);
	int shape(){ return m_shape; }
	void setShape(int shape);
	double getConstraintValue(const VirtualEncoding &s, vector<double> &val);
	bool isValid(const VirtualEncoding  &s);
protected:
	void updateRobust();
	void initialize();
	void evaluate__(double const *x, vector<double>& obj);
	void computeMinHeight();
	void computeDisToBoarder();
	void initilizeStepFun(bool flag=false);
	void setStepFun();
	void irregularize(double *x);
	void asyemmetricalize(double *x);
	void setTransFlag(vector<int> *trans);
	bool loadRotation();
	void transform(double * x);
	void setConstraints(vector<pair<bool, vector<double>>>* cstrt=0);
	void setFeasibleBorderCenter(bool flag);
	bool loadConstraints();
private:
	//basics of onepeak
	double m_height=100;
	vector<double> m_location;
	bool m_randomLoc=false;
	int m_shape = 1;
	double m_furestDis;		// the longest distance from location to boarder points 
	double m_nearestDis;	// the shortest distance from location to boundary points
	bool m_standardize=false;		// standardize objective value between height and zero
	int m_vr = Separable;
	double m_basinRatio = 1;	//the ratio of the area of the basin of attraction to the area of the whole space
	double m_mean, m_variance;	//the mean and variance of objective values of 10,000 random points 
	vector<double> m_dominoWeight;
	bool m_irr=false, m_asy=false;
	double &m_stdMaxHeight = m_height, m_stdMinHeight = 0;
	int m_label;		// 
	vector<double> m_transfLoc;

	//onepeak in dynamic environments
	double m_robustness;
	deque<double> m_preHeight;
	deque<vector<double>> m_preLoc;
	int m_maxMemorysize=0;
	double m_shiftSeverity, m_heightSeverity;

	//onepeak in multi-modal optimization
	double m_quality;
	double m_maxHeight,m_minHeight;

	//parameters for step functions
	double m_radius;
	double m_foldHeight;
	double m_ratio = 1;
	int m_k = 3;
	double m_theta=2.5;

	//partially separable 
	int m_noGroup;
	vector<vector<int>> m_group;

	//June 28,2016
	bool m_noisy = false;

	bool m_cstr[Constraint::NR_ITEMS_C] = {0,0,0};
	bool m_minFeasibleArea4Border = true;
	double m_feasibleRadius=50,m_sineCutoff=0.5,m_feasibleRadiusBorder;
	vector<double> m_feasibleBorderCenter;

};

inline double FOnePeak::getTransDistance(double const *x_){
	double val, dummy = 0;
	switch (m_vr)
	{
	case FOnePeak::DistanceTransfm::Separable: //Sphere
		for (int j = 0; j<m_numDim; ++j){
			val = (x_[j] - m_transfLoc[j]);
			dummy += val*val;
		}
		dummy = sqrt(dummy);
		break;
		/*case FOnePeak::DistanceTransfm::NonSeparable://Schwefel 1.2
		for (int j = 0; j<m_numDim; ++j){
			val = 0;
			for (int i = 0; i <= j; ++i){
				val += (x_[i] - m_transfLoc[i]);
			}
			dummy += val*val;
		}
		dummy = sqrt(dummy);
		break;*/
	case FOnePeak::DistanceTransfm::Domino: //weighted Sphere
		for (int j = 0; j<m_numDim; ++j){
			val = (x_[j] - m_transfLoc[j]);
			dummy += m_dominoWeight[j] * val*val;
		}
		dummy = sqrt(dummy);
		break;
	case FOnePeak::DistanceTransfm::NonSeparable: case FOnePeak::DistanceTransfm::PartSeparable:
		/*for (int i = 0; i < m_noGroup; ++i) {
			val = 1;
			for (auto&j : m_group[i]) {
				val *= pow((x_[j] - m_transfLoc[j])*(x_[j] - m_transfLoc[j]) + 1, 1./log(1.+m_group[i].size()));				
			}
			dummy += val - 1;
		}
		for (int i = 0; i < m_noGroup; ++i) {
			val = 0;
			for (auto&j : m_group[i]) {
				val += (x_[j] - m_transfLoc[j])*(x_[j] - m_transfLoc[j]);
			}
			dummy += val*val;
		}*/
		for (int i = 0; i < m_noGroup; ++i) {			
			for (int j = 0; j<m_group[i].size(); ++j) {
				val = 0;
				for (int k = 0; k <= j; ++k) {
					val += (x_[m_group[i][k]] - m_transfLoc[m_group[i][k]]);
				}
				dummy += val*val;
			}
		}
		dummy = sqrt(dummy);
		break;
	case FOnePeak::DistanceTransfm::FlatBorder: 
		dummy = 1;
		for (int j = 0; j < m_numDim; ++j) {
			val = x_[j] - m_transfLoc[j];
			if (val >= 0) val = 1 - val / (m_searchRange[j].m_upper - m_location[j]);
			else val = 1 + val / (m_location[j] - m_searchRange[j].m_lower);
			dummy *= val;
		}
		dummy = (1 - dummy) * m_searchRange.getDomainSize();
		break;
	}
	return dummy;
}
#endif