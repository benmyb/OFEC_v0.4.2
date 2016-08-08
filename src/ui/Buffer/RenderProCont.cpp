#include "RenderProCont.h"
#include "OFGlobal.h"
#include "BufferCont.h"
#include <thread>
#include "../../imp/Algorithm/Clustering/FAMFDera.h"
#include "../../imp/Problem/FunctionOpt/FOnePeak.h"
#include "../../imp/Problem/FunctionOpt/FFreePeak.h"
#include "../../imp/Problem/DOP/FFreePeak_D_M_OnePeak.h"
#include "../../imp/Problem/FunctionOpt/MOP/FreePeak_M_OnePeak/FFreePeak_M_OnePeak.h"
extern mutex g_mutex;
extern unique_ptr<Scene> msp_buffer;
extern bool g_showGOP;
void RenderProCont::setMapRange(){

	m_rangeXMap.min=m_rangeYMap.min=-300;
	m_rangeXMap.max=m_rangeYMap.max=300;
	m_rangeZMap.min=0; 
	m_rangeZMap.max=200;
	m_rangeXMap.range=m_rangeXMap.max-m_rangeXMap.min;
	m_rangeYMap.range=m_rangeYMap.max-m_rangeYMap.min;
	m_rangeZMap.range=m_rangeZMap.max-m_rangeZMap.min;
}
void RenderProCont::setProRange(){
	int num = GET_NUM_DIM;
	m_rangeX.resize(num);
	for (int i = 0; i < num; ++i){
		CAST_PROBLEM_CONT->getSearchRange(m_rangeX[i].min, m_rangeX[i].max, i);
		m_rangeX[i].range = fabs(m_rangeX[i].max - m_rangeX[i].min);
	}

}
void RenderProCont::setupZ(const vector<vector<double>> &obj){
	int num = GET_NUM_OBJ;
	m_rangeObj.resize(num);
	for (int i = 0; i<num; ++i){
		auto it = obj.begin();
		m_rangeObj[i].min = (*it)[i];
		m_rangeObj[i].max = (*it)[i];
		for (; it != obj.end(); ++it){
			if ((*it)[i]<m_rangeObj[i].min) m_rangeObj[i].min = (*it)[i];
			if ((*it)[i]>m_rangeObj[i].max) m_rangeObj[i].max = (*it)[i];
		}
		m_rangeObj[i].range = m_rangeObj[i].max - m_rangeObj[i].min;
	}
	const vector<pair<double, double>> &objrange = Global::msp_global->mp_problem->getObjRange();

	if (Global::msp_global->mp_problem->getOptType() == MAX_OPT){	
		if (Global::msp_global->mp_problem->isGlobalOptKnown()){
			for (int i = 0; i < num; ++i){
				m_rangeObj[i].max = objrange[i].second;
				m_rangeObj[i].range = m_rangeObj[i].max - m_rangeObj[i].min;
			}		
		}	
	}
	else {
		if (Global::msp_global->mp_problem->isGlobalOptKnown()){
			for (int i = 0; i < num; ++i){
				m_rangeObj[i].min = objrange[i].first;
				m_rangeObj[i].range = m_rangeObj[i].max - m_rangeObj[i].min;
			}
		}
	}

}
void RenderProCont::setup(int &size, int defaultZobj, const vector<ofRectangle> &view, const vector<pair<double, double>>&curObjRange){
	mv_selObj.resize(3);
	for (int i = 0; i<3; i++)		mv_selObj[i] = i;
	
	setMapRange();
	setProRange();
	
	vector<vector<double>> obj;
	ofMesh mesh;
	if(GET_NUM_DIM==1)mesh.setMode(OF_PRIMITIVE_LINES);
	else mesh.setMode(OF_PRIMITIVE_TRIANGLES);

	if(GET_NUM_DIM==1) m_valid.resize(m_divNum);
	else m_valid.resize(m_divNum*m_divNum);

	startThread(obj);	
	if (g_algTermination) return;
	setInfeasibleValue(obj);
	setupZ(obj);
	generateSample(obj);

	obj.clear();
	int num = count(m_valid.begin(), m_valid.end(), true);
	vector<pair<size_t, double>> color(num);
	int i = 0;
	for(auto it=m_sample.begin();it!=m_sample.end();++it){		
		if(m_valid[it-m_sample.begin()]){		
			color[i++] = make_pair(it - m_sample.begin(), (*it)[2 + defaultZobj] / m_rangeZMap.range);
		}
		
	}
	sort(color.begin(), color.end(), [](const pair<size_t, double>&c1, const pair<size_t, double>&c2)->bool{return c1.second < c2.second; });
	vector<double> scolor(m_sample.size());
	size_t darkRed = 1;
	if (Global::msp_global->mp_problem->getNumObj() == 1)darkRed = color.size()*0.1;
	else{
		//while (darkRed < color.size() && (color.end() - darkRed)->second == (color.end() - darkRed - 1)->second) ++darkRed;
		while (darkRed < color.size() && (color.end() - darkRed)->second == 1.0) ++darkRed;
	}
	for (auto i = 0; i < color.size(); ++i){
		if (i>color.size() - darkRed) scolor[color[i].first] = -1.0*(i + darkRed - color.size() ) / (double)(darkRed);
		else	scolor[color[i].first] = 170.*(1.0 - i  / (double)(color.size() - darkRed));
	}
	color.clear();
	for (auto it = m_sample.begin(); it != m_sample.end(); ++it){
		mesh.addVertex(ofVec3f((*it)[0], (*it)[1], (*it)[2 + defaultZobj]));
		if (m_valid[it - m_sample.begin()]){
			ofColor c(255, 0, 0);
			if (scolor[it - m_sample.begin()]>=0)			c.setHue(scolor[it - m_sample.begin()]);
			else c.r = 150+105 *(1+ scolor[it - m_sample.begin()]);
			mesh.addColor(c);
		}
		else mesh.addColor(ofColor(0, 0, 0));
	}
	scolor.clear();
	

	if(GET_NUM_DIM==1){
		for(int i=0;i<m_divNum-1;++i){
			mesh.addIndex(i);
			mesh.addIndex(i+1);
		}
	}else{
		for (int y=0; y<m_divNum-1; y++) {
			for (int x=0; x<m_divNum-1; x++) {
				int i1 = x + m_divNum * y;
				int i2 = x+1 + m_divNum * y;
				int i3 = x + m_divNum * (y+1);
				int i4 = x+1 + m_divNum * (y+1);
				mesh.addTriangle( i1, i2, i3 );
				mesh.addTriangle( i2, i4, i3 );
			}
		}
	}
	
	ofgSetNormals( mesh );
		
	m_landscape.push_back(move(mesh));
	size++;
	
	if (Global::msp_global->mp_problem->isGlobalOptKnown()){
		ofMesh omesh, smesh;
		omesh.setMode(OF_PRIMITIVE_POINTS);
		smesh.setMode(OF_PRIMITIVE_POINTS);
		ContinuousProblem*pro = dynamic_cast<ContinuousProblem*>(Global::msp_global->mp_problem.get());
		float sizeTop = view[viewTop].width<view[viewTop].height ? view[viewTop].width : view[viewTop].height;
		float sizeBot = view[camBot].width <view[camBot].height ? view[camBot].width : view[camBot].height;
		int numObj = GET_NUM_OBJ;
		vector<pair<double, double>> goptrange(numObj, pair<double, double>(numeric_limits<double>::max(), numeric_limits<double>::min()));
		for (int i = 0; i < pro->getGOpt().getNumOpt(); ++i){			
			omesh.addVertex(mapOtoScreen(pro->getGOpt()[i].data().m_obj, curObjRange, sizeTop));
			smesh.addVertex(mapXtoScreen(pro->getGOpt()[i].data().m_x, sizeBot));
			for (int j = 0; j < numObj; ++j){
				if (pro->getGOpt()[i].data().m_obj[j] < goptrange[j].first) goptrange[j].first = pro->getGOpt()[i].data().m_obj[j];
				if (pro->getGOpt()[i].data().m_obj[j] > goptrange[j].second) goptrange[j].second = pro->getGOpt()[i].data().m_obj[j];
			}
		}
		m_obj.push_back(move(omesh));
		m_sol.push_back(move(smesh));
		m_gobjRange.push_back(move(goptrange));
	}


	if (Global::ms_curProId == Global::msm_pro["FUN_FreePeak_M_OnePeak"] || Global::ms_curProId == Global::msm_pro["FUN_FreePeak_D_M_OnePeak"]){

		vector<int> psr;
		vector<bool> ispsr;
		if (Global::ms_curProId == Global::msm_pro["FUN_FreePeak_M_OnePeak"]){
			psr = dynamic_cast<FFreePeak_M_OnePeak*>(Global::msp_global->mp_problem.get())->getParetoRegion();
			ispsr = dynamic_cast<FFreePeak_M_OnePeak*>(Global::msp_global->mp_problem.get())->getisPSR();
		}

		else if (Global::ms_curProId == Global::msm_pro["FUN_FreePeak_D_M_OnePeak"]){
			psr = dynamic_cast<FreePeak_D_M_OnePeak*>(Global::msp_global->mp_problem.get())->getParetoRegion();
			ispsr = dynamic_cast<FreePeak_D_M_OnePeak*>(Global::msp_global->mp_problem.get())->getisPSR();
		}

		int numBox = dynamic_cast<FreePeak*>(Global::msp_global->mp_problem.get())->getNumBox();
		float sizeBot = view[camBot].width <view[camBot].height ? view[camBot].width : view[camBot].height;
		vector<vector<ofPoint>> reg(numBox);
		
		ofMesh peakMesh;
		peakMesh.setMode(OF_PRIMITIVE_POINTS);
		vector<ofPoint> label;
		
		for (int t = 0; t < dynamic_cast<FreePeak*>(Global::msp_global->mp_problem.get())->getNumTree(); ++t){
			KDTreeSpace::PartitioningKDTree<double>* tree = dynamic_cast<FreePeak*>(Global::msp_global->mp_problem.get())->getTree(t);
			for (int i = 0; i < tree->region.size(); ++i){
				// draw box id			
				vector<double> lowerleft(Global::msp_global->mp_problem.get()->getNumDim());
				for (int j = 0; j < Global::msp_global->mp_problem.get()->getNumDim(); ++j) lowerleft[j] = tree->region[i].box[j].first;
				label.push_back(mapXtoScreen(lowerleft, sizeBot));
			}
		}
		
		m_boxLabel.push_back(move(label));
		const vector<CodeVReal> peak = dynamic_cast<FreePeak*>(Global::msp_global->mp_problem.get())->getPeak();
		int numObj = Global::msp_global->mp_problem->getNumObj();
		for (auto i = 0; i<peak.size(); ++i){			
			peakMesh.addVertex(move(mapXtoScreen(peak[i].m_x, sizeBot)));			
			int j = i / numObj;
			if (i%numObj == 0)	peakMesh.addColor(ofColor(255, 0, 0));
			else if (ispsr[j]) peakMesh.addColor(ofColor(0, 0, 255));
			else peakMesh.addColor(ofColor(0, 0, 0));

		}
		m_freePeak.push_back(move(peakMesh));
	}


	cout<<"problem loading is finishing"<<endl;
}

void  RenderProCont::drawSolutionSpace(ofTrueTypeFont& font){
	if (m_sol.size()>0){
		ofSetColor(255, 0, 0);
		if (Global::msp_global->mp_problem->getNumObj() == 1)	glPointSize(5);
		else glPointSize(1);

		glEnable(GL_POINT_SMOOTH);
		if (g_showGOP)		m_sol.front().draw();
		if (m_sol.size() > 1) m_sol.pop_front();
	}
	if (Global::ms_curProId == Global::msm_pro["FUN_FreePeak_M_OnePeak2"] || Global::ms_curProId == Global::msm_pro["FUN_FreePeak_D_M_OnePeak"]){
		glPointSize(10);
		glDisable(GL_POINT_SMOOTH);
		m_freePeak.front().draw();
		for (int i = 0; i < m_boxLabel.front().size();++i){
			stringstream label;
			label << "B" << i + 1;
			font.drawString(label.str(), m_boxLabel.front().at(i).x, m_boxLabel.front().at(i).y);
		}
		if (m_freePeak.size()>1) m_freePeak.pop_front();
		if (m_boxLabel.size() > 1) m_boxLabel.pop_front();
	}
}
void  RenderProCont::drawObjectiveSpace(){
	if (m_obj.size() == 0) return;
	ofSetColor(255, 0, 0);
	glPointSize(5);
	glEnable(GL_POINT_SMOOTH);
	{
		unique_lock<mutex> lock(g_mutex);
		if (g_showGOP)		m_obj.front().draw();
	}
	if (m_obj.size() > 1){
		m_obj.pop_front();
	}
	if (m_gobjRange.size() > 1){
		m_gobjRange.pop_front();
	}
}

void RenderProCont::updateObj(const vector<ofRectangle> &view, const vector<pair<double, double>>&curObjRange){
	ContinuousProblem*pro = dynamic_cast<ContinuousProblem*>(Global::msp_global->mp_problem.get());
	float sizeTop = view[viewTop].width<view[viewTop].height ? view[viewTop].width : view[viewTop].height;	
	for (int i = 0; i < pro->getGOpt().getNumOpt(); ++i){
		m_obj.back().setVertex(i,mapOtoScreen(pro->getGOpt()[i].data().m_obj, curObjRange, sizeTop));		
	}
}
void RenderProCont::draw(int &size){
	
	m_landscape.front().draw();
	
	if(m_optFound.size()>0){
		//ofDisableLighting(); //Disable lighting
		m_optFound.front().draw(); //Draw lines
		//ofEnableLighting(); //Enable lighting
	}
	if(m_landscape.size()>1){
		m_landscape.pop_front();
		size--;
	}
	if(m_optFound.size()>1){
		m_optFound.pop_front();
	}
}

void RenderProCont::updateSample(const vector<vector<double>>& obj){
	if (GET_NUM_DIM == 1){
		for (int i = 0; i<m_divNum; ++i){
			vector<float> v;
			for (int j = 0; j<Global::msp_global->mp_problem->getNumObj(); ++j){
				if (Global::msp_global->mp_problem->getOptType() == MIN_OPT){
					v.push_back(m_rangeZMap.min + m_rangeZMap.range*(m_rangeObj[j].max - obj[toZ(0, i, m_divNum)][j]) / m_rangeObj[j].range);
				}
				else{
					v.push_back(m_rangeZMap.min + m_rangeZMap.range*(obj[toZ(0, i, m_divNum)][j] - m_rangeObj[j].min) / m_rangeObj[j].range);
				}
				if (g_algTermination) return;
			}
			copy(v.begin(), v.end(), m_sample[i].begin() + 2);
		}
	}
	else {
		for (int i = 0; i<m_divNum; ++i){
			if (g_algTermination) return;
			for (int j = 0; j<m_divNum; ++j){
				if (g_algTermination) return;
				vector<float> v;
				int idx = toZ(i, j, m_divNum);
				for (int k = 0; k<Global::msp_global->mp_problem->getNumObj(); ++k){
					if (Global::msp_global->mp_problem->getOptType() == MIN_OPT){
						v.push_back(m_rangeZMap.min + m_rangeZMap.range*(m_rangeObj[k].max - obj[idx][k]) / m_rangeObj[k].range);
					}
					else{
						v.push_back(m_rangeZMap.min + m_rangeZMap.range*(obj[idx][k] - m_rangeObj[k].min) / m_rangeObj[k].range);
					}
				}
				copy(v.begin(), v.end(), m_sample[idx].begin() + 2);
			}
		}
	}
}

void RenderProCont::updateLandscape(int &size){
	int defaultZobj = dynamic_cast<BufferCont*>(msp_buffer.get())->getDefautZ();
	setProRange();
	vector<vector<double>> obj;
	startThread(obj);	

	if (g_algTermination) return;
	setInfeasibleValue(obj);
	setupZ(obj);
	updateSample(obj);
	
	ofMesh mesh=m_landscape.front();

	int num = count(m_valid.begin(), m_valid.end(), true);
	vector<pair<size_t, double>> color(num);
	int i = 0;
	for (auto it = m_sample.begin(); it != m_sample.end(); ++it){
		if (m_valid[it - m_sample.begin()]){
			color[i++] = make_pair(it - m_sample.begin(), (*it)[2 + defaultZobj] / m_rangeZMap.range);
		}

	}
	sort(color.begin(), color.end(), [](const pair<size_t, double>&c1, const pair<size_t, double>&c2)->bool{return c1.second < c2.second; });
	vector<double> scolor(m_sample.size());
	size_t darkRed = 1;
	if (Global::msp_global->mp_problem->getNumObj() == 1)darkRed=color.size()*0.1;
	else{
		while (darkRed < color.size() && (color.end() - darkRed)->second == (color.end() - darkRed - 1)->second) ++darkRed;
	}
	for (auto i = 0; i < color.size(); ++i){
		if (i>color.size() - darkRed) scolor[color[i].first] = -1.0*(i + darkRed - color.size()) / (double)(darkRed);
		else	scolor[color[i].first] = 170.*(1.0 - i / (double)(color.size() - darkRed));
	}
	color.clear();

	vector<ofColor> saved(m_sample.size());
	for (auto it = m_sample.begin(); it != m_sample.end(); ++it){
		if (m_valid[it - m_sample.begin()]){
			ofColor c(255, 0, 0);
			if (scolor[it - m_sample.begin()] >= 0)			c.setHue(scolor[it - m_sample.begin()]);
			else c.r = 150 + 105 * (1 + scolor[it - m_sample.begin()]);
			saved[it - m_sample.begin()] = c;
		}
		else {
			saved[it - m_sample.begin()].r = saved[it - m_sample.begin()].g = saved[it - m_sample.begin()].b = 0;
		};
	}
	scolor.clear();


	if(GET_NUM_DIM==1){
		for (int x=0; x<m_divNum; x++) {
				if(g_algTermination) return;
				int idx=toZ(0,x,m_divNum);
				ofVec3f p=mesh.getVertex(idx);
				p.z=m_sample[idx][2+defaultZobj];
				mesh.setVertex(idx,p);
				mesh.setColor(idx,saved[idx]);
		}
	}else{
		for (int y=0; y<m_divNum; y++) {
			if(g_algTermination) return;
			for (int x=0; x<m_divNum; x++) {
				if(g_algTermination) return;
				int idx=toZ(y,x,m_divNum);
				ofVec3f p=mesh.getVertex(idx);
				p.z=m_sample[idx][2+defaultZobj];
				mesh.setVertex(idx,p);
				mesh.setColor(idx, saved[idx]);
			}
		}
		ofgSetNormals( mesh ); //Update the normals
	}
	
	{
		unique_lock<mutex> lock1(Scene::ms_mutexBuf);
		unique_lock<mutex> lock2(g_mutex);
		if(FAMFDerating::ms_enableDerating)
		{
			if(FAMFDerating::msp_opt->getNumGOptFound()>0){
				ofMesh mesh_opt;
				mesh_opt.setMode(OF_PRIMITIVE_LINES);
				int num=0;
				for(int i=0;i<FAMFDerating::msp_opt->getNumGOptFound();++i){
					mesh_opt.addVertex(mapMainPoint((*FAMFDerating::msp_opt)[i].data()));
					mesh_opt.addColor(ofFloatColor(0,0,255));
					for(int j=0;j<(*FAMFDerating::msp_opt)[i].size();++j){
						Solution<CodeVReal> x(GET_NUM_DIM, GET_NUM_OBJ);
						MyVector v=(*FAMFDerating::msp_opt)[i].getBest();
						v+=(*FAMFDerating::msp_opt)[i][j];
						copy(v.begin(),v.end(),x.data().m_x.begin());
						Global::msp_global->mp_problem->evaluate(x,false,Program_Problem);
						mesh_opt.addVertex(mapMainPoint(x.data()));
						mesh_opt.addColor(ofFloatColor(0,0,255));
					}
					for(int j=0;j<(*FAMFDerating::msp_opt)[i].size();++j){
						mesh_opt.addIndex(num); mesh_opt.addIndex(num+j+1);
					}
					num+=(*FAMFDerating::msp_opt)[i].size()+1;
				}
				m_optFound.push_back(move(mesh_opt));
			}
		}
	}

	{
		
		unique_lock<mutex> lock(Scene::ms_mutexBuf);
		m_landscape.push_back(move(mesh));
		size++;
		
	}
}

ofVec3f RenderProCont::mapMainPoint(CodeVReal &s){
	double x, y, z;
	if(GET_NUM_DIM==1){		
		x=m_rangeXMap.min+m_rangeXMap.range*(s.m_x[0]-m_rangeX[0].min)/m_rangeX[0].range;
		y=0;
	}else {
		x=m_rangeXMap.min+m_rangeXMap.range*(s.m_x[0]-m_rangeX[0].min)/m_rangeX[0].range;
		y=m_rangeYMap.min+m_rangeYMap.range*(s.m_x[1]-m_rangeX[1].min)/m_rangeX[1].range;
	}
	int defaultZobj = dynamic_cast<BufferCont*>(msp_buffer.get())->getDefautZ();
	if(Global::msp_global->mp_problem->getOptType()==MIN_OPT){
		z=m_rangeZMap.min+ m_rangeZMap.range*(m_rangeObj[defaultZobj].max-s.m_obj[defaultZobj])/m_rangeObj[defaultZobj].range+5;		
	}else{
		z=m_rangeZMap.min+ m_rangeZMap.range*(s.m_obj[defaultZobj]-m_rangeObj[defaultZobj].min)/m_rangeObj[defaultZobj].range+5;	
	}
	if(z>m_rangeZMap.max) z=m_rangeZMap.max;
	if(z<m_rangeZMap.min) z=m_rangeZMap.min;
	 
	 return ofVec3f(x,y,z);
}
int RenderProCont::calculateSampleObj(int numTask,int taskid,RenderProCont & rend,vector<vector<double>> &obj){
	
	Solution<CodeVReal> x(GET_NUM_DIM,Global::msp_global->mp_problem->getNumObj());
	if(GET_NUM_DIM==1){
		for(int i=taskid*rend.m_divNum/numTask;i<(taskid+1)*rend.m_divNum/numTask;++i){		
			x.data().m_x[0]=rend.m_rangeX[0].min+i*rend.m_rangeX[0].range/rend.m_divNum;
			rend.m_valid[i] = Return_Invalid !=Global::msp_global->mp_problem->evaluate(x, false, Program_Problem);
			if (FAMFDerating::ms_enableDerating) FAMFDerating::derateFitness(x);
			obj[i]=x.data().m_obj;
			if(g_algTermination) return 1;
		}
	}
	else {
		int jobs=rend.m_divNum*rend.m_divNum/numTask;
		for(int k=taskid*jobs;k<(taskid+1)*jobs;++k){
			int i=k/rend.m_divNum,j=k%rend.m_divNum;
			x.data().m_x[0]=rend.m_rangeX[0].min+i*rend.m_rangeX[0].range/rend.m_divNum;
			x.data().m_x[1]=rend.m_rangeX[1].min+j*rend.m_rangeX[1].range/rend.m_divNum;
			
			rend.m_valid[k] = Return_Invalid != Global::msp_global->mp_problem->evaluate(x, false, Program_Problem);
			if (FAMFDerating::ms_enableDerating) FAMFDerating::derateFitness(x);

			obj[k]=x.data().m_obj;
			if(g_algTermination) return 1;
		}
	}
	return 0;
}
void RenderProCont::startThread(vector<vector<double>> &obj){


	int numTask =  std::thread::hardware_concurrency();	
	if(GET_NUM_DIM==1){
		obj.resize(m_divNum);
		while(numTask>0&&m_divNum%numTask!=0) --numTask;
	}else{
		obj.resize(m_divNum*m_divNum);
		while(numTask>0&&m_divNum*m_divNum%numTask!=0) --numTask;
	}
	vector<thread>  thrd;
		
	for(int i=0;i<numTask;i++){
		thrd.push_back(thread(RenderProCont::calculateSampleObj, numTask, i, ref(*this), ref(obj)));		
	}
	
	for (auto &i : thrd)  i.join();
	
}

void RenderProCont::generateSample(const vector<vector<double>> &obj){

	if (GET_NUM_DIM == 1){
		m_sample.resize(m_divNum);
		for (int i = 0; i<m_divNum; ++i){
			vector<float> v(2);
			v[0] = m_rangeXMap.min + m_rangeXMap.range*i / m_divNum; v[1] = 0;
			for (int j = 0; j<Global::msp_global->mp_problem->getNumObj(); ++j){
				if (Global::msp_global->mp_problem->getOptType() == MIN_OPT){
					v.push_back(m_rangeZMap.min + m_rangeZMap.range*(m_rangeObj[j].max - obj[toZ(0, i, m_divNum)][j]) / m_rangeObj[j].range);
				}
				else{
					v.push_back(m_rangeZMap.min + m_rangeZMap.range*(obj[toZ(0, i, m_divNum)][j] - m_rangeObj[j].min) / m_rangeObj[j].range);
				}
			}
			m_sample[i]=v;
		}

	}
	else{
		m_sample.resize(m_divNum*m_divNum);
		for (int i = 0; i<m_divNum; ++i){
			for (int j = 0; j<m_divNum; ++j){
				vector<float> v(2);
				v[0] = m_rangeXMap.min + m_rangeXMap.range*i / m_divNum;
				v[1] = m_rangeYMap.min + m_rangeYMap.range*j / m_divNum;
				int idx = toZ(i, j, m_divNum);
				for (int k = 0; k<Global::msp_global->mp_problem->getNumObj(); ++k){
					if (Global::msp_global->mp_problem->getOptType() == MIN_OPT){
						v.push_back(m_rangeZMap.min + m_rangeZMap.range*(m_rangeObj[k].max - obj[idx][k]) / m_rangeObj[k].range);
					}
					else{
						v.push_back(m_rangeZMap.min + m_rangeZMap.range*(obj[idx][k] - m_rangeObj[k].min) / m_rangeObj[k].range);
					}
				}
				m_sample[idx]=v;
			}
		}
	}

}

void RenderProCont::mapXtoScreen(const vector<double> &v, ofPoint &p,const float size){
	
	double x = 0, y = 0, z = 0;
	if (GET_NUM_DIM >= 1){
		x = -size / 2 + size*(v[0] - m_rangeX[0].min) / m_rangeX[0].range;
		if (x>size / 2) x = size / 2;
		if (x < -size / 2) x = -size / 2;
	}

	if (GET_NUM_DIM >= 2){
		y = -size / 2 + size*(v[1] - m_rangeX[1].min) / m_rangeX[1].range;
		if (y > size / 2) y = size / 2;
		if (y < -size / 2) y = -size / 2;
	}

	if (GET_NUM_DIM >= 3){
		z = -size / 2 + size*(v[2] - m_rangeX[2].min) / m_rangeX[2].range;
		if (z > size / 2) z = size / 2;
		if (z < -size / 2) z = -size / 2;
	}
	p.x = x; p.y = y; p.z = z;
}

ofPoint  RenderProCont::mapXtoScreen(const vector<double> &v,  const float size){

	double x = 0, y = 0, z = 0;
	if (GET_NUM_DIM >= 1){
		x = -size / 2 + size*(v[0] - m_rangeX[0].min) / m_rangeX[0].range;
		if (x>size / 2) x = size / 2;
		if (x < -size / 2) x = -size / 2;
	}

	if (GET_NUM_DIM >= 2){
		y = -size / 2 + size*(v[1] - m_rangeX[1].min) / m_rangeX[1].range;
		if (y > size / 2) y = size / 2;
		if (y < -size / 2) y = -size / 2;
	}

	if (GET_NUM_DIM >= 3){
		z = -size / 2 + size*(v[2] - m_rangeX[2].min) / m_rangeX[2].range;
		if (z > size / 2) z = size / 2;
		if (z < -size / 2) z = -size / 2;
	}
	return ofPoint(x, y, z);
}

ofPoint RenderProCont::mapOtoScreen(const vector<double> &v, const vector<pair<double, double>>&curObjRange,const float size){
	int objs = Global::msp_global->mp_problem->getNumObj()<3 ? Global::msp_global->mp_problem->getNumObj() : 3;	
	ofPoint o;
	
	for (int j = 0, k; j < objs; ++j){		
		k = mv_selObj[j];		
		if (Global::msp_global->mp_problem->getOptType(k) == MIN_OPT){			
			if (curObjRange.size() == 0){
				o[j] = -size / 2 + size*(v[k] - m_rangeObj[k].min) / m_rangeObj[k].range;
			}
			else{
				double offset = m_offsetObj*size*(fabs(curObjRange[k].first - m_rangeObj[k].min) / m_rangeObj[k].range);
				if (curObjRange[k].second - m_rangeObj[k].min>0)
					o[j] = -size / 2 + (size - offset)*(v[k] - m_rangeObj[k].min) / (curObjRange[k].second - m_rangeObj[k].min);
				else
					o[j] = -size / 2;
			}
			
		}
		else{
			if (curObjRange.size() == 0){
				o[j] = -size / 2 + size*(m_rangeObj[k].max - v[k]) / m_rangeObj[k].range;
			}
			else{
				double offset = m_offsetObj*size*(fabs(curObjRange[k].second - m_rangeObj[k].max) / m_rangeObj[k].range);
				if (m_rangeObj[k].max - curObjRange[k].first > 0 || curObjRange[k].second == 0 && curObjRange[k].first == 0)
					o[j] = size / 2 - (size - offset)*(m_rangeObj[k].max - v[k]) / (m_rangeObj[k].max - curObjRange[k].first);
				else
					o[j] = size / 2;
			}		
		}

		if (o[j]>size / 2) o[j] = size / 2;
		if (o[j]<-size / 2) o[j] = -size / 2;
	}
	return move(o);
}

void RenderProCont::setInfeasibleValue(vector<vector<double>>& obj) {

	vector<double> extrem(Global::msp_global->mp_problem->getNumObj());
	for (int o = 0; o<Global::msp_global->mp_problem->getNumObj(); ++o) {
		if (Global::msp_global->mp_problem->getOptType() == MIN_OPT) {
			extrem[o] = LONG_MIN;
			for (auto i = 0; i < obj.size();++i) {
				if (m_valid[i]&&obj[i][o]>extrem[o]) {
					extrem[o] = obj[i][o];
				}
			}
		}
		else {
			extrem[o] = LONG_MAX;
			for (auto i = 0; i < obj.size(); ++i) {
				if (m_valid[i] && obj[i][o]<extrem[o]) {
					extrem[o] = obj[i][o];
				}
			}
		}
	}
	for (int o = 0; o<Global::msp_global->mp_problem->getNumObj(); ++o) {
		for (auto i = 0; i < obj.size(); ++i) {
			if (!m_valid[i]) 			obj[i][o] = extrem[o];
		}
	}

}