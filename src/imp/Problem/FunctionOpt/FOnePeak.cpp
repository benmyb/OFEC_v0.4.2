#include "FOnePeak.h"

FOnePeak::FOnePeak(ParamMap &v) :Problem((v[param_proId]), (v[param_numDim]), (v[param_proName]), 1), \
BenchmarkFunction((v[param_proId]), (v[param_numDim]), (v[param_proName]), 1), m_location(m_numDim), m_transfLoc(m_numDim), m_vr(v[param_variableRelation]), \
m_dominoWeight(v[param_numDim]),m_irr(v[param_flagIrregular]),m_asy(v[param_flagAsymmetry]), m_shape(v[param_peakShape]) {
	setSearchRange(-100, 100);
	m_rotationFlag = v[param_flagRotation];
	
	initialize();
	if (v[param_peakCenter] == 1){
		setLocationAtCenter();
	}
	else if (v[param_peakCenter] == 2){
		setLocationRandom();
	}
	else if (v[param_peakCenter] == 3) {
		setLocationAtCorner();
	}
	if (v.find(param_noiseFlag) != v.end()) m_noisy = v[param_noiseFlag];
	loadConstraints();
	//difficulty();
}
FOnePeak::FOnePeak(const int rId, const int rDimNumber, string& rName, bool random, int shape, double h, vector<int> *transform, int label, vector<pair<bool, vector<double>>>* cstrt, bool noisy) :Problem(rId, rDimNumber, rName, 1), \
BenchmarkFunction(rId, rDimNumber, rName, 1), m_location(m_numDim), m_shape(shape), m_height(h), m_dominoWeight(rDimNumber), m_label(label), m_transfLoc(m_numDim),m_noisy(noisy){
	setSearchRange(-100, 100);
	setTransFlag(transform);
	setConstraints(cstrt);
	initialize();
	if (!random)	setLocationAtCenter();
	else setLocationRandom();
}
FOnePeak::FOnePeak(const int rId, const int rDimNumber, string& rName, const vector<double>&loc, int shape, double h, vector<int> *transform, int label, vector<pair<bool, vector<double>>>* cstrt, bool noisy) :Problem(rId, rDimNumber, rName, 1), \
BenchmarkFunction(rId, rDimNumber, rName, 1), m_location(m_numDim), m_shape(shape), m_height(h), m_dominoWeight(rDimNumber), m_label(label),m_transfLoc(m_numDim), m_noisy(noisy) {
	setSearchRange(-100, 100);
	setTransFlag(transform);
	setConstraints(cstrt);
	initialize();
	setLocation(loc);
}
void FOnePeak::setTransFlag(vector<int> *trans){
	if (trans){
		m_vr = (*trans)[0];
		m_rotationFlag = (*trans)[1];	
		m_irr = (*trans)[2];
		m_asy = (*trans)[3];		
	}
	else{
		m_rotationFlag = false;
		m_irr = false;
		m_asy = false;
		m_vr = Separable;
	}
}
void FOnePeak::initialize(){
	if (m_rotationFlag){
		setConditionNumber(2);
		loadRotation();
	}
	setOptType(MAX_OPT);
	setDisAccuracy(0.2);
	setAccuracy(1.e-6);
	m_globalOpt.setFlagLocTrue();
	m_globalOpt[0].data().m_obj[0] = m_height;
	m_originalGlobalOpt = m_globalOpt;
	if (m_shape == SH8 || m_shape == SH7 || m_shape == SH9 || m_shape == SH10)	initilizeStepFun();
	if (m_vr == FOnePeak::DistanceTransfm::Domino){
		for (auto&i : m_dominoWeight) i = pow(10, 3*Global::msp_global->mp_normalPro->Next());
	}else if (m_vr == FOnePeak::DistanceTransfm::PartSeparable) {
		m_noGroup = Global::msp_global->getRandInt(2, m_numDim, Program_Problem);
		m_group.resize(m_noGroup);
		vector<int> set(m_numDim-1),randIdx(m_numDim);
		for (int i = 0; i < m_numDim-1; ++i) {
			set[i] = i;
		}
		Global::msp_global->initializeRandomArray(randIdx, m_numDim, Program_Problem);
		vector<int> pick(m_noGroup);
		for (int i = 0; i < m_noGroup-1; ++i) {
			pick[i] = Global::msp_global->randPick(set,Program_Problem);
		}
		pick.back() = m_numDim - 1;
		std::sort(pick.begin(), pick.end());

		for (int i = 0,j=0; i < m_noGroup; ++i) {
			while (j <= pick[i]) {
				m_group[i].push_back(randIdx[j++]);
			}
		}
	}
	else if (m_vr == FOnePeak::DistanceTransfm::NonSeparable) {
		m_noGroup = 1;
		m_group.resize(m_noGroup);
		m_group[0].resize(m_numDim);
		Global::msp_global->initializeRandomArray(m_group[0], m_numDim, Program_Problem);
	}
	setObjSet();
}
void FOnePeak::initilizeStepFun(bool flag){	
	if (Global::g_arg.find(param_radius) != Global::g_arg.end()){
		setRadiusStepFun(Global::g_arg[param_radius],flag);
	}
	else{
		setRadiusStepFun(0.5,flag);
	}
}
void FOnePeak::setStepFun(){
	m_radius = m_ratio*m_nearestDis;
	if (m_shape == SH9)	m_foldHeight = -m_height / (sqrt(m_radius + 1));
	else if (m_shape == SH8) m_foldHeight = m_height*(-1 - m_theta*(m_k-1)) / (sqrt(m_radius + 1));
	else m_foldHeight = -m_height; 

	computeMinHeight();
}
void FOnePeak::setRadiusStepFun(double r, bool flag){
	m_ratio = r;	
	if (flag) setStepFun();
}

void FOnePeak::setLocationAtCenter(){ //default setup
	for (int i = 0; i < m_numDim; ++i){
		m_location[i] = (m_searchRange[i].m_lower + m_searchRange[i].m_upper) / 2;
	}
	m_transfLoc = m_location;
	transform(m_transfLoc.data());
	m_globalOpt[0].data().m_x = m_location;
	m_originalGlobalOpt[0].data().m_x = m_location;
	computeDisToBoarder();
	computeMinHeight();
}
void FOnePeak::setLocationRandom(){
	for (int i = 0; i < m_numDim; ++i){
		m_location[i] = m_searchRange[i].m_lower + (m_searchRange[i].m_upper - m_searchRange[i].m_lower)*Global::msp_global->mp_uniformPro->Next();
	}
	m_transfLoc = m_location;
	transform(m_transfLoc.data());
	m_globalOpt[0].data().m_x = m_location;
	m_originalGlobalOpt[0].data().m_x = m_location;
	computeDisToBoarder();
	computeMinHeight();
}
void FOnePeak::setLocationAtCorner() {
	for (int i = 0; i < m_numDim; ++i) {
		m_location[i] = m_searchRange[i].m_lower;
	}
	m_transfLoc = m_location;
	transform(m_transfLoc.data());
	m_globalOpt[0].data().m_x = m_location;
	m_originalGlobalOpt[0].data().m_x = m_location;
	computeDisToBoarder();
	computeMinHeight();
}
void FOnePeak::setLocation(const vector<double> &x){
	m_location = x;
	m_transfLoc = m_location;
	transform(m_transfLoc.data());
	m_globalOpt[0].data().m_x = m_location;
	m_originalGlobalOpt[0].data().m_x = m_location;
	computeDisToBoarder();
	computeMinHeight();
}
void FOnePeak::evaluate__(double const *x_, vector<double>& obj){
	double dummy = getTransDistance(x_);
	switch (m_shape)
	{
	case SH1: obj[0] = m_height -dummy;		break;	//linear
	case SH2: obj[0] = m_height*exp(-dummy); break;	//convex
	case SH3: obj[0] = m_height - sqrt(m_height)*sqrt(dummy); break;//covex
	case SH4: obj[0] = m_height / (dummy + 1); break;	//convex
	case SH5: obj[0] = m_height - dummy*dummy / m_height; break;//concave
	case SH6: obj[0] = m_height - exp(2 * sqrt(dummy / sqrt(m_numDim))) + 1; break;//concave	
	case SH7: //concave+linear
		if (dummy<m_radius)		obj[0] = m_height * cos(OFEC_PI*dummy / m_radius);
		else obj[0] = m_foldHeight - (dummy - m_radius);
		break;	
	case SH8:{
		if (dummy < m_radius){
			int domain = floor(m_k*dummy / m_radius);
			obj[0] = m_height*(cos(m_k * OFEC_PI*(dummy - domain*m_radius / m_k) / m_radius) - m_theta*domain) / (sqrt(dummy + 1));
		}
		else{
			obj[0] = m_foldHeight - (dummy - m_radius);
		}
	}	
		break;
	case SH9: //concave+convex+linear
		if (dummy <= m_radius){
			obj[0] = m_height*cos((2*m_k-1) * OFEC_PI*dummy / m_radius) / (sqrt(dummy+1));
		}
		else{
			obj[0] = m_foldHeight - (dummy - m_radius);
		}		
		break; 	
	case SH10:	//flat, nightmare when radius=0
		if (dummy <= m_radius)		obj[0] = m_height;
		else obj[0] = m_foldHeight ;
		break;	
	case SH11:	//netural+needle
		if (dummy <= 10)		obj[0] = m_height*exp(-dummy);
		else obj[0] = m_height*exp(-10);
		break;	
	case SH12:	//ripple
		obj[0] = m_height*cos((2 * m_k - 1) * OFEC_PI*dummy / m_furestDis) / sqrt(log(dummy + 1)+1);
		break;
	case SH13:	//netural+needle+ripple
		if (dummy <= 10)		obj[0] = m_height*exp(-dummy);
		else if (dummy<=50) obj[0] = m_height*exp(-10);
		else obj[0] = m_height*sin((2 * m_k - 1) * OFEC_PI*(dummy - 50) / (m_furestDis - 50)) / sqrt(log(dummy - 49) + 1) + m_height*exp(-10);
		break;
	}
	if (m_noisy) obj[0] -= fabs(Global::msp_global->mp_normalPro->Next());

	if (m_standardize) {
		obj[0] = m_stdMinHeight + (m_stdMaxHeight - m_stdMinHeight)*(obj[0] - m_minHeight) / (m_height - m_minHeight);
	}
}
void FOnePeak::setHeight(const double h){ 
	m_height = h; 
	m_globalOpt[0].data().m_obj[0] = m_height;
	m_originalGlobalOpt[0].data().m_obj[0] = m_height;
	if (m_shape == SH8 || m_shape == SH7 || m_shape == SH9 || m_shape == SH10)	setStepFun();
	computeMinHeight();
}

void FOnePeak::setFeasibleRadius(double r){
	m_feasibleRadius = m_searchRange.getDomainSize()*r;
}
void FOnePeak::shiftLocation(const vector<double> &x){
	if (m_maxMemorysize > 0){
		m_preLoc.push_front(m_location);
		if (m_preLoc.size() > m_maxMemorysize) m_preLoc.pop_back();
	}
	m_location = x;
	SolutionValidation mode = VALIDATION_REMAP;
	validate_(m_location, &mode);
	m_globalOpt[0].data().m_x = m_location;
	m_originalGlobalOpt[0].data().m_x = m_location;
	computeDisToBoarder();
	computeMinHeight();
}
void FOnePeak::shiftLocation(){
	if (m_maxMemorysize > 0){
		m_preLoc.push_front(m_location);
		if (m_preLoc.size() > m_maxMemorysize) m_preLoc.pop_back();
	}
	
	MyVector v(m_numDim);
	v.randOnRadi(m_shiftSeverity*m_searchRange.getDomainSize(), Program_Problem);
	for (int i = 0; i < m_numDim; ++i){
		m_location[i] += v[i];
	}	
	SolutionValidation mode = VALIDATION_REMAP;
	validate_(m_location, &mode);
	m_globalOpt[0].data().m_x = m_location;
	m_originalGlobalOpt[0].data().m_x = m_location;
	computeDisToBoarder();
	computeMinHeight();
}

void FOnePeak::changeHeight(const double h){
	if (m_maxMemorysize > 0){
		m_preHeight.push_front(m_height);
		if (m_preHeight.size() > m_maxMemorysize) m_preHeight.pop_back();
	}
	setHeight(h);
	updateRobust();
	m_quality = m_height / m_maxHeight;
}
void FOnePeak::setMemorySize(const int val){
	m_maxMemorysize = val;
}
void FOnePeak::setMaxHeight(const double val){
	m_maxHeight = val;
	m_quality = m_height / m_maxHeight;
}
void  FOnePeak::updateRobust(){
	
	if (m_preHeight.size() > 0){
		double delta = 0;
		delta += fabs(m_height - m_preHeight[0]);
		
		for (auto i = m_preHeight.begin(); i != m_preHeight.end() - 1; ++i){
			delta += fabs(*i - *(i + 1));
		}	
		delta /= m_preHeight.size();
		m_robustness = delta;
	}
	else{
		m_robustness = 0;
	}
	
}

void FOnePeak::computeMinHeight(){
	double dummy = m_furestDis;
	switch (m_shape)
	{
	case SH1: m_minHeight = m_height - dummy; break;
	case SH2: m_minHeight = m_height*exp(-dummy); break;
	case SH3: m_minHeight = m_height - sqrt(m_height)*sqrt(dummy); break;
	case SH4: m_minHeight = m_height / (dummy + 1); break;
	case SH5: m_minHeight = m_height - dummy*dummy / m_height; break;
	case SH6: m_minHeight = m_height - exp(2*sqrt(dummy/sqrt(m_numDim))) + 1; break;
	case SH7: case SH8:
		m_minHeight = m_foldHeight - (dummy - m_radius);
		break;
	case SH9:{
		double minInR = -m_height / (sqrt(m_radius / (2 * m_k - 1) + 1));
		dummy = m_foldHeight - (dummy - m_radius);
		m_minHeight = minInR<dummy ? minInR : dummy;
	}
		   break; 
	case SH10:
		m_minHeight = m_foldHeight;
		break;
	case SH11:
		m_minHeight = m_height*exp(-10);
		break;
	case SH12:
		m_minHeight = -m_height / sqrt(log(m_furestDis/(2*m_k-1) + 1) + 1);
		break;
	case SH13:
		m_minHeight = -m_height / sqrt(log(1.5*(m_furestDis - 50) / (2 * m_k - 1) +1) + 1) + m_height*exp(-10);
		break;
	}
}

void FOnePeak::computeDisToBoarder(){
	
	vector<double> fpoint(m_numDim);
	m_nearestDis = DBL_MAX;
	for (int i = 0; i < m_numDim; ++i) {
		double l, u;
		m_searchRange.getSearchRange(l, u, i);
		if (u - m_location[i] > m_location[i] - l) {
			fpoint[i] = u;
			if (m_nearestDis > m_location[i] - l) m_nearestDis = m_location[i] - l;
		}
		else {
			fpoint[i] = l;
			if (m_nearestDis > u - m_location[i])m_nearestDis = u - m_location[i];
		}
	}

	transform(fpoint.data());
	m_furestDis = getTransDistance(fpoint.data());

	if (m_shape == SH8 || m_shape == SH7 || m_shape == SH9 || m_shape == SH10)	setStepFun();
	
	
}

void FOnePeak::difficulty(double rh){
	
	vector<double> sample(10000);
	static bool flag=false;
	static vector<CodeVReal> points(10000, CodeVReal(m_numDim, m_numObj));
	for (int i = 0; i < 10000; ++i){		
		if (!flag){
			for (auto &j : points[i].m_x){
				j = -100 + Global::msp_global->mp_uniformPro->Next() * 200;
			}
		}		
		evaluate__(points[i].m_x.data(), points[i].m_obj);
		sample[i] = points[i].m_obj[0];
	}
	flag = true;
	double max, min;
	max = *max_element(sample.begin(), sample.end());
	min = *min_element(sample.begin(), sample.end());
	for_each(sample.begin(), sample.end(), [&](double &n){n = rh*(n - min) / (max - min); });
	
	m_mean = accumulate(sample.begin(), sample.end(), 0.0);
	m_mean /= sample.size();

	vector<double> diff(sample.size());
	std::transform(sample.begin(), sample.end(), diff.begin(), std::bind2nd(std::minus<double>(), m_mean));
	m_variance = inner_product(diff.begin(), diff.end(), diff.begin(), 0.0) / (diff.size()-1);
	
}
void FOnePeak::setBasinRatio(double r){
	m_basinRatio = r;
}
void FOnePeak::copyChanges(const Problem * pro, const vector<int> *cd, const vector<int> *co){

	const FOnePeak *op = dynamic_cast<const FOnePeak *>(pro);

	if (cd){
		for (int j = 0; j < m_numDim; ++j){
			m_location[j] = op->m_location[(*cd)[j]];
			m_transfLoc[j] = op->m_transfLoc[(*cd)[j]];
		}
	}
	else{
		copy(op->m_location.begin(), op->m_location.end(), m_location.begin());
		copy(op->m_transfLoc.begin(), op->m_transfLoc.end(), m_transfLoc.begin());
	}
	
	computeDisToBoarder();
	computeMinHeight();

	m_preHeight=op->m_preHeight;
	m_preLoc=op->m_preLoc;		// take care of # of dimensions
	m_height = op->m_height;
	m_shape = op->m_shape;
	m_standardize = op->m_standardize;
	m_robustness=op->m_robustness;
	m_maxMemorysize = op->m_maxMemorysize;
	m_shiftSeverity = op->m_shiftSeverity;
	m_heightSeverity=op->m_heightSeverity;
	m_quality=op->m_quality;
	m_maxHeight = op->m_maxHeight;
	m_radius=op->m_radius;
	m_foldHeight=op->m_foldHeight;
	m_ratio =op->m_ratio;
	m_basinRatio = op->m_basinRatio;
	m_dominoWeight = op->m_dominoWeight;
	m_k = op->m_k;
	m_theta = op->m_theta;

	m_globalOpt[0].data().m_x = m_location;
	m_globalOpt[0].data().m_obj[0] = m_height;
	m_originalGlobalOpt = m_globalOpt;

	m_stdMaxHeight = op->m_stdMaxHeight;
	m_stdMinHeight = op->m_stdMinHeight;
	m_label = op->m_label;

	m_noGroup=op->m_noGroup;
	m_group=op->m_group;

	m_noisy = op->m_noisy;

	for (int i = 0; i < Constraint::NR_ITEMS_C; ++i) {
		m_cstr[i] = op->m_cstr[i];
	}
	
	m_minFeasibleArea4Border = op->m_minFeasibleArea4Border;
	m_feasibleRadius = op->m_feasibleRadius;
	m_sineCutoff = op->m_sineCutoff;
	m_feasibleRadiusBorder=op->m_feasibleRadiusBorder;
	m_feasibleBorderCenter=op->m_feasibleBorderCenter;
}

void FOnePeak::irregularize(double *x){
	double c1, c2, x_;
	for (int i = 0; i < m_numDim; ++i){
		if (x[i]>0){
			c1 = 10;	c2 = 7.9;
		}else {
			c1 = 5.5;	c2 = 3.1;
		}
		if (x[i] != 0){
			x_ = log(fabs(x[i]));
		}
		else x_ = 0;
		x[i] = gSign(x[i])*exp(x_ + 0.049*(sin(c1*x_) + sin(c2*x_)));
	}
}
void FOnePeak::asyemmetricalize(double *x){
	double belta = 0.2;
	if (m_numDim == 1) return;
	
	for (int i = 0; i < m_numDim; ++i){
		if (x[i]>0){
			x[i] = pow(x[i], 1 + belta*i*sqrt(x[i]) / (m_numDim - 1));
		}
	}
		
}

bool FOnePeak::loadRotation(){
	string s;
	stringstream ss;
	ss << m_numDim << "Dim.rot";
	s = ss.str();
	s.insert(0, m_name);

	s.insert(0, "Problem/FunctionOpt/Data/");
	s.insert(0, Global::g_arg[param_workingDir]);
	ifstream in;
	in.open(s.data());
	if (in.fail()){		
		m_conditionNumber = 10;
		mp_rotationMatrix->randomize(Program_Problem);
		mp_rotationMatrix->generateRotationMatrix(m_conditionNumber, Program_Problem);
		
		ofstream out(s.c_str());
		mp_rotationMatrix->Print(out);
		out.close();
	}
	else{
		mp_rotationMatrix->Read_Data(in);
	}
	in.close();

	return true;
}
void FOnePeak::transform(double * x){

	double *x_ = new double[m_numDim];	
	for (int i = 0; i < m_numDim; ++i) x_[i] = x[i] - m_location[i];
	
	if (m_irr)		irregularize(x_);
	if (m_asy)		asyemmetricalize(x_);
	if (m_rotationFlag){
		for (int i = 0; i<m_numDim; i++) {
			x[i] = 0;

			for (int j = 0; j < m_numDim; j++) {
				x[i] += (*mp_rotationMatrix)[j][i] * x_[j];
			}
		}
	}
	else copy(x_, x_ + m_numDim, x);

	delete[] x_;
}

vector<double> FOnePeak::getPoint(double oval, const MyVector& dir, int ith){

	vector<double> p;
	MyVector v(dir);
	double dummy = getDistance2Center(oval,ith);
	if (dummy >= 0){
		p.resize(m_numDim);
		v.normalize();
		v *= dummy;
		copy(v.data().begin(), v.data().end(), p.begin());
	}
	return move(p);
}

double FOnePeak::getDistance2Center(double oval,int ith){
	bool flag = m_irr || m_asy || m_rotationFlag || m_vr != FOnePeak::DistanceTransfm::Separable;
	if (flag) return -1;
	if (m_standardize){
		oval = (oval - m_stdMinHeight)*(m_height - m_minHeight) / (m_stdMaxHeight - m_stdMinHeight) + m_minHeight;
	}  

	double dummy = -1;
	switch (m_shape){
	case SH1: dummy = m_height - oval; 		break;
	case SH2: dummy = -log(oval / m_height); break;
	case SH3: dummy = pow((m_height - oval) / sqrt(m_height), 2.); break;
	case SH4: dummy = (m_height / oval) - 1; break;
	case SH5:  dummy = sqrt((m_height - oval)*m_height); break;
	case SH6: dummy = pow(log(m_height - oval + 1) / 2, 2.)*sqrt(m_numDim); break;
	case SH7:
		if (oval >= m_foldHeight){
			dummy = m_radius*acos(oval / m_height) / OFEC_PI;
		}
		else{
			dummy = m_foldHeight - oval + m_radius;
		}
		break;
	case SH8:
		if (oval > m_foldHeight){
			double d1, d2, val, d3;
			d1 = (ith - 1)*m_radius / m_k, d2 = ith*m_radius / m_k;
			do{
				d3 = (d1 + d2) / 2;
				val = m_height*(cos(m_k * OFEC_PI*(d3 - floor(m_k*d3 / m_radius)) / m_radius) - m_theta*floor(m_k*d3 / m_radius)) / (sqrt(d3 + 1));
				if (val > oval){
					d1 = d3;
				}
				else{
					d2 = d3;
				}
			} while (fabs(val - oval)>1.e-10);
			dummy = d3;
		}
		else{
			dummy = m_foldHeight - oval + m_radius;
		}
		break;
	case SH9:{		
		double d1 = 0, d2 = m_radius / (2 * m_k - 1), val, d3;
		d1 = (ith - 1)*m_radius / (2 * m_k - 1), d2 = ith*m_radius / (2 * m_k - 1);
		do{
			d3 = (d1 + d2) / 2;
			val = m_height*cos((2 * m_k - 1) * OFEC_PI*d3 / m_radius) / (sqrt(d3 + 1));
			if (val > oval){
				d1 = d3;
			}
			else{
				d2 = d3;
			}
		} while (fabs(val - oval)>1.e-10);
		dummy = d3;		
	}
		break;
	default:
		break;
	}
	
	return dummy;
}

void FOnePeak::getContiSegment(const vector<double> pos, vector<pair<vector<double>, vector<double>>> &seg){
	seg.clear();
	CodeVReal x(m_numDim, m_numObj);
	switch (m_shape){
	case SH1: 	case SH2:	case SH3:	case SH4: 	case SH5: 	case SH6: case SH7:
		seg.push_back(make_pair(m_location, pos));
		break;
	case SH8:{
		double dummy = getTransDistance(pos.data());
		double step = m_radius / m_k;
		MyVector vn(m_numDim);
		for (int d = 0; d < m_numDim; ++d) vn[d] = pos[d] - m_location[d];
		vn.normalize();
		for (int i = 1; i <=m_k; ++i){
			MyVector v1(vn), v2(vn);
			v1 = vn*(step*(i - 1) + 1.0e-10);
			if (dummy > step*i + 1.0e-10){
				v2 = vn*(step*i - 1.0e-10);
				seg.push_back(make_pair(v1.data(), v2.data()));
			}
			else{								
				seg.push_back(make_pair(v1.data(), pos));
				break;
			}	
		}
		
	}
		break;
	case SH9:{	
		double dummy = getTransDistance(pos.data());
		int i;
		MyVector vn(m_numDim);
		for (int d = 0; d < m_numDim; ++d) vn[d] = pos[d] - m_location[d];
		vn.normalize();
		vector<MyVector> vv;
		
		for (i = 1; i <= m_k; ++i){			
			if (i == 1){
				vv.push_back(MyVector(m_location));
			}
			else{
				MyVector v(vn);
				v *= m_radius*(i - 1) * 2 / (2 * m_k - 1);
				vv.push_back(v);
			}
			if (i < m_k){
				double d = m_radius*i * 2 / (2 * m_k - 1);
				MyVector v(vn);
				v *= d;
				x.m_x = v.data();
				evaluate_(x, false, Program_Problem, false);
				vv.push_back(move(MyVector(getPoint(x.m_obj[0], vn,i*2-1))));
			}
			else{
				double d = m_radius*(i * 2-1) / (2 * m_k - 1);
				MyVector v(vn);
				v *= d;
				vv.push_back(v);
			}
		}

		for (i = 1; i <= m_k; ++i){
			double dis = m_radius*(2 * i - 1) / (2 * m_k - 1);
			if (dummy >= dis) {
				seg.push_back(move(make_pair(vv[2 * i - 2].data(), vv[2 * i - 1].data())));
			}
			else break;
		}
		if (i <= m_k){
			int is = (2 * m_k - 1)*dummy / m_radius;
			if (is % 2 == 0)	seg.push_back(move(make_pair(vv[2 * i - 2].data(), pos)));
			else{
				MyVector mid(m_numDim);
				for (int d = 0; d < m_numDim; ++d) mid[d] = (vv[is + 1][d] + vv[is][d]) / 2;
				double dis = getTransDistance(mid.data().data());

				if (dis<=dummy)	seg.push_back(move(make_pair(pos, vv[is + 1].data())));
				else{
					MyVector x1(pos), x2(vv[is + 1]),x3,x4;
					do{
						mid = x1.getPointBetween(x2, 0.5);
						x.m_x = mid.data();
						evaluate_(x, false, Program_Problem, false);
						x3.data()=getPoint(x.m_obj[0], mid, is);
						x4=mid.getPointBetween(x3, 0.5);
						dis = getTransDistance(x4.data().data());
						if (dis <= dummy){
							x1 = mid;
						}
						else{
							x2 = mid;
						}
					} while (fabs(dis-dummy)>1.0e-10);
					seg.back().second = x3.data();
					seg.push_back(move(make_pair(pos, mid.data())));
				}
			}
		}
		else seg.back().second = pos;
	}
		break;
	default:
		break;
	}
}

void FOnePeak::getContiSegment(const vector<double> p1, const vector<double> p2, vector<pair<vector<double>, vector<double>>> &seg){
	seg.clear();
	CodeVReal x(m_numDim, m_numObj);
	switch (m_shape){
	case SH1: 	case SH2:	case SH3:	case SH4: 	case SH5: 	case SH6: case SH7:{
		seg.push_back(make_pair(p1, p2));
		}	
		break;
	case SH8:{
		MyVector mid(m_numDim), v1(m_numDim), v2(m_numDim), vn1(p1), vn2(p2);
		for (int d = 0; d < m_numDim; ++d) mid[d] = (p1[d] + p2[d])/2;
		double step = m_radius / m_k;
		double dummy = getTransDistance(mid.data().data());
		deque<MyVector> lseg;

		vn1 -= mid;	vn2 -= mid;
		vn1.normalize();	vn2.normalize();
		
		for (int i = 1; i < m_k; i++){
			if (dummy <= step*i){
				v1 = vn1;	v2 = vn2;
				double dis2mid = sqrt(pow(step*i, 2) - dummy*dummy);
				v1 *= dis2mid;	v2 *= dis2mid;
				v1 += mid;		v2 += mid;
				lseg.push_front(v1);
				lseg.push_back(v2);
			}
		}

		lseg.push_front(move(MyVector(p1)));
		lseg.push_back(move(MyVector(p2)));

		int midx = lseg.size() / 2-1;
		for (int i = 0; i < lseg.size()-1; ++i){
			MyVector dir1(lseg[i]), dir2(lseg[i + 1]);
			dir1 -= m_location;
			dir2 -= m_location;
			if (i == midx){
				dir1 *= 1 - 1.e-10;
				dir2 *= 1 - 1.e-10;
			}
			else if(i<midx){
				dir1 *= 1 - 1.e-10;
				dir2 *= 1 + 1.e-10;
			}
			else{
				dir1 *= 1 + 1.e-10;
				dir2 *= 1 - 1.e-10;
			}
			dir1 += m_location;
			dir2 += m_location;
			seg.push_back(make_pair(dir1.data(), dir2.data()));
		}	
	}
		break;
	
	default:
		break;
	}
}

void FOnePeak::setShape(int shape) {
	if (m_shape != shape) {
		m_shape = shape;		
		if (m_shape == SH8 || m_shape == SH7 || m_shape == SH9 || m_shape == SH10)	initilizeStepFun(true);
		computeMinHeight();
	}
}
void FOnePeak::setConstraints(vector<pair<bool, vector<double>>>* cstrt) {
	if (cstrt) {
		for (int i = 0; i < Constraint::NR_ITEMS_C; ++i) {
			m_cstr[i] = (*cstrt)[i].first;
		}
		if (m_cstr[C_SphereAtPeak]) m_feasibleRadius = (*cstrt)[0].second.at(0) * m_searchRange.getDomainSize();
		if (m_cstr[C_SphereAtBorder]) {
			setFeasibleBorderCenter((int)(*cstrt)[1].second.at(0));
		}
		if (m_cstr[C_Sine]) m_sineCutoff = (*cstrt)[2].second.at(0);
	}
	else{
		if (m_cstr[C_SphereAtPeak]) m_feasibleRadius = 0.5*m_searchRange.getDomainSize();
		if (m_cstr[C_SphereAtBorder]) {
			setFeasibleBorderCenter(true);
		}
		if (m_cstr[C_Sine]) m_sineCutoff = 0.5;
	}
}

double FOnePeak::getConstraintValue(const VirtualEncoding &s, vector<double> &val) {

	const CodeVReal &ss = dynamic_cast< const CodeVReal&>(s);

	val.resize(Constraint::NR_ITEMS_C);
	for_each(val.begin(), val.end(), [](double &r) {r = 0; });
	MyVector point(ss.m_x);
	if (m_cstr[C_SphereAtPeak]) {
		val[0] = point.getDis(m_location) - m_feasibleRadius;
	}
	
	if (m_cstr[C_SphereAtBorder]) {
		val[1] = point.getDis(m_feasibleBorderCenter)-m_feasibleRadiusBorder;
	}
	if (m_cstr[C_Sine]) {
		val[2] = fabs(sin(log(1 + pow(point.getDis(m_location), 3)))) - m_sineCutoff;
	}
	return accumulate(val.begin(), val.end(), 0.0);
}

bool FOnePeak::isValid(const VirtualEncoding  &s) {
	if (ContinuousProblem::isValid(s) == false) return false;
	const CodeVReal &ss = dynamic_cast< const CodeVReal&>(s);
	MyVector point(ss.m_x);
	if (m_cstr[C_SphereAtPeak]) {
		if (point.getDis(m_location) - m_feasibleRadius > 0) return false;
	}

	if (m_cstr[C_SphereAtBorder]) {
		if (point.getDis(m_feasibleBorderCenter) - m_feasibleRadiusBorder > 0) return false;
	}
	if (m_cstr[C_Sine]) {
		
		if (fabs(sin(log(1+ pow(point.getDis(m_location),3)))) - m_sineCutoff > 0) return false;
	}
	return true;
}

void FOnePeak::setFeasibleBorderCenter(bool flag) {
	m_minFeasibleArea4Border = flag;
	m_feasibleBorderCenter.resize(m_numDim);
	int idx, dis;
	double loc;
	if (m_minFeasibleArea4Border) {
		double tdis = 200, value;
		for (int i = 0; i < m_numDim; ++i) {
			dis = m_searchRange[i].m_upper - m_location[i] < m_location[i] - m_searchRange[i].m_lower ? (value = m_searchRange[i].m_upper, m_searchRange[i].m_upper - m_location[i]) : (value = m_searchRange[i].m_lower, m_location[i] - m_searchRange[i].m_lower);
			if (dis < tdis) {
				tdis = dis; idx = i; loc = value; m_feasibleRadiusBorder = dis;
			}
		}
	}
	else {
		double tdis = 0, value;
		for (int i = 0; i < m_numDim; ++i) {
			dis = m_searchRange[i].m_upper - m_location[i] > m_location[i] - m_searchRange[i].m_lower ? (value = m_searchRange[i].m_upper, m_searchRange[i].m_upper - m_location[i]) : (value = m_searchRange[i].m_lower, m_location[i] - m_searchRange[i].m_lower);
			if (dis > tdis) {
				tdis = dis; idx = i; loc = value; m_feasibleRadiusBorder = dis;
			}
		}
	}
	for (int i = 0; i < m_numDim; ++i) {
		if (i != idx) m_feasibleBorderCenter[i] = m_location[i];
		else m_feasibleBorderCenter[i] = loc;
	}
	//transform(m_feasibleBorderCenter.data());
}

bool FOnePeak::loadConstraints() {
	string s;
	stringstream ss;
	ss << m_name << ".cons";
	s = ss.str();
	s.insert(0, "Problem/FunctionOpt/Data/");
	s.insert(0, Global::g_arg[param_workingDir]);
	ifstream in;
	in.open(s.data());
	if (in.fail()) {
		ofstream out(s);
		out << "#constraint index \tselection mask \t parameter value" << endl;
		out << "1 " << m_cstr[C_SphereAtPeak] << " 0.5" << endl;
		out << "2 " << m_cstr[C_SphereAtBorder] << " 1" << endl;
		out << "3 " << m_cstr[C_Sine] << " 0.5" << endl;
		out << "#END";
		out.close();
	}
	else {
		while (!in.eof()) {
			getline(in, s);
			if (s == "#END") break;
			if(s[0] == '#') continue;
			vector<string> col(3);
			stringstream ss(s);
			for (int i = 0; i < col.size(); ++i) ss >> col[i];
			vector<TypeVar> value;
			gGetStringValue(col[0], value);
			int idx = value[0]-1;
			gGetStringValue(col[1], value);
			m_cstr[idx] = (int)value[0];
			
			if (m_cstr[idx]) {
				gGetStringValue(col[2], value);
				switch (idx + 1)
				{
				case 1:
					m_feasibleRadius = ((double)value[0]) * m_searchRange.getDomainSize();
					break;
				case 2:
					setFeasibleBorderCenter((int)value[0]);
					break;
				case 3:
					m_sineCutoff =value[0];
					break;
				}
			}
		}
	}
	in.close();

	return true;
}