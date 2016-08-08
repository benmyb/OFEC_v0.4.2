#ifndef BUFFER_H
#define BUFFER_H
#include "Scene.h"
#include "OFGlobal.h"

extern bool g_algTermination;
extern int g_drawSpeed;
extern bool g_showPop;
// the buffer class holds a set of solutions to be drawn in solution space, objective space or both.
template<typename ED>
class Buffer:public Scene{
protected:
	list<vector<Solution<ED>>> m_buffer;	
	int m_solutBufSize;
	bool m_flagReady2Draw;
	int m_proBufSize;
	int m_proBufCurSize;	
	list<int> m_numBestIndis;
	bool m_updated;	//flag to indicate buffer update is done
	list<vector<int>> m_indiOfPop;
	list<int> m_numPops;
	vector<pair<double, double>>m_curObjRange;
	bool m_updateComplete = false;
public:	
	virtual void updateFitnessLandsacpe(){};
	virtual void popupData();
protected:
	virtual void updateBuffer(const vector<Algorithm*> *pop=0); //Items of pop must be inherited from Population
	virtual void updateBuffer(const vector<vector<Solution<ED>>*> *pop = 0);  
	virtual void drawSolution(){};
	virtual void drawObjective(){};
	virtual void drawSolutndObj(){};
	virtual void drawProblem(){};
	virtual void loadProblem(){};

public:
	Buffer(vector<ofRectangle> &view,vector<ofEasyCam> &cam, ofLight &light,ofTrueTypeFont  &m_font);
	~Buffer(){};
	
	void updateBuffer_(const vector<Algorithm*> *pop=0);
	void updateBuffer_(const vector< vector<Solution<ED>> * > *pop = 0);
	void drawBuffer();
	bool isReady2Draw();
	void setReady2Draw(bool);
	void loadProblem_();
	void updateFitnessLandsacpe_();
	const vector<pair<double, double>>& getCurObjRange(){ return m_curObjRange; }
};

template<typename ED>
Buffer<ED>::Buffer(vector<ofRectangle> &view, vector<ofEasyCam> &cam, ofLight &light, ofTrueTypeFont  &font) :Scene(view, cam, light, font), m_solutBufSize(3), \
m_flagReady2Draw(false), m_numBestIndis(0), m_updated(true), m_proBufSize(3), m_proBufCurSize(0){

}

template<typename ED>
void Buffer<ED>::updateBuffer(const vector<Algorithm*> *pops){
	// put best individuals after all individuals in solut
	vector<Solution<ED>> solut;
	vector<int> indiOfPop;
	int numBestIndis = 0;
	if (pops){

		int size(0);
		for (auto i = pops->begin(); i != pops->end(); ++i){
			size += reinterpret_cast<Population<ED,Individual<ED>>*>(*i)->getPopSize();
			size += reinterpret_cast<Population<ED,Individual<ED>>*>(*i)->getBest().size();
		}

		solut.resize(size);
		indiOfPop.resize(size);
		int j = 0;
		for (auto i = 0; i<pops->size(); ++i){
			for (auto k = 0; k<reinterpret_cast<Population<ED,Individual<ED>>*>((*pops)[i])->getPopSize(); ++k, ++j){
				if (g_algTermination) return;
				solut[j].data() = (*reinterpret_cast<Population<ED, Individual<ED>>*>((*pops)[i]))[k]->representative().data();
				indiOfPop[j] = i;
			}
		}
		for (auto i = 0; i<pops->size(); ++i){
			for (auto k = 0; k<reinterpret_cast<Population<ED,Individual<ED>>*>((*pops)[i])->getBest().size(); ++k, ++j){
				if (g_algTermination) return;
				solut[j].data() = reinterpret_cast<Population<ED,Individual<ED>>*>((*pops)[i])->getBest()[k]->data();
				indiOfPop[j] = i;
				++numBestIndis;
			}
		}

	}
	else{
		throw myException("no data come @ Buffer<ED>::updateBuffer(Population<ED,Individual<ED>> *pop)");
	}
	if (!solut.empty()){
		m_numBestIndis.push_back(numBestIndis);
		m_buffer.push_back(move(solut));
		m_indiOfPop.push_back(move(indiOfPop));
		m_numPops.push_back(pops->size());

		int objs = Global::msp_global->mp_problem->getNumObj();
		if (m_curObjRange.size() == 0) m_curObjRange.resize(objs);
	
		for (auto i = 0; i < objs; ++i){
			m_curObjRange[i].first = numeric_limits<double>::max();
			m_curObjRange[i].second = numeric_limits<double>::min();
		}
				
		for (auto &i : m_buffer.back()){
			for (auto j = 0; j < objs; ++j){
				if (m_curObjRange[j].first > i.obj(j)) m_curObjRange[j].first = i.obj(j);
				if (m_curObjRange[j].second < i.obj(j)) m_curObjRange[j].second = i.obj(j);
			}
		}		
	}
}


template<typename ED>
void Buffer<ED>::updateBuffer(const vector< vector<Solution<ED>> *> *pops) {
	// put best individuals after all individuals in solut
	vector<Solution<ED>> solut;
	vector<int> indiOfPop;
	if (pops) {
		int size(0);
		for (auto i = pops->begin(); i != pops->end(); ++i) {
			size += (*i)->size();
		}

		solut.resize(size);
		indiOfPop.resize(size);
		int j = 0;
		for (auto i = 0; i<pops->size(); ++i) {
			for (auto k = 0; k<((*pops)[i])->size(); ++k, ++j) {
				if (g_algTermination) return;
				solut[j].data() = (*(*pops)[i])[k].data();
				indiOfPop[j] = i;
			}
		}
	}
	else {
		throw myException("no data come @ Buffer<ED>::updateBuffer(Population<ED,Individual<ED>> *pop)");
	}
	if (!solut.empty()) {
		m_buffer.push_back(move(solut));
		m_indiOfPop.push_back(move(indiOfPop));
		m_numPops.push_back(pops->size());

		int objs = Global::msp_global->mp_problem->getNumObj();
		if (m_curObjRange.size() == 0) m_curObjRange.resize(objs);

		for (auto i = 0; i < objs; ++i) {
			m_curObjRange[i].first = numeric_limits<double>::max();
			m_curObjRange[i].second = numeric_limits<double>::min();
		}

		for (auto &i : m_buffer.back()) {
			for (auto j = 0; j < objs; ++j) {
				if (m_curObjRange[j].first > i.obj(j)) m_curObjRange[j].first = i.obj(j);
				if (m_curObjRange[j].second < i.obj(j)) m_curObjRange[j].second = i.obj(j);
			}
		}
	}
}

template<typename ED> 
void Buffer<ED>::updateBuffer_(const vector<Algorithm*> *pop){
	{
		unique_lock<mutex> lock(ms_mutexBuf);
		if (!g_algTermination&&!pop)
		{
			m_condProBufEmpty.notify_one();
			m_condSolutBufFull.notify_one();
			return;
		}
		while (!g_algTermination && (m_buffer.size() == m_solutBufSize || ms_pause)){
			//if(ms_pause) cout<<"buffer is frozen"<<endl;
			//else 
			//cout<<"solution buffer is full"<<endl;
			m_condSolutBufFull.wait_for(lock, chrono::milliseconds(10));
		}


		if (!g_algTermination){
			//cout << "new data are coming" << endl;
			m_updateComplete = false;
			updateBuffer(pop);
			m_updateComplete = true;
			//cout << "data update done"<<endl;
			
		}
		m_flagReady2Draw = true;
		m_updated = true;
	}
	m_condSolutBufEmpty.notify_one();
}



template<typename ED>
void Buffer<ED>::updateBuffer_(const vector< vector<Solution<ED>> * > *pop) {
	{
		unique_lock<mutex> lock(ms_mutexBuf);
		if (!g_algTermination&&!pop)
		{
			m_condProBufEmpty.notify_one();
			m_condSolutBufFull.notify_one();
			return;
		}
		while (!g_algTermination && (m_buffer.size() == m_solutBufSize || ms_pause)) {
			//if(ms_pause) cout<<"buffer is frozen"<<endl;
			//else 
			//cout<<"solution buffer is full"<<endl;
			m_condSolutBufFull.wait_for(lock, chrono::milliseconds(10));
		}


		if (!g_algTermination) {
			//cout << "new data are coming" << endl;
			m_updateComplete = false;
			updateBuffer(pop);
			m_updateComplete = true;
			//cout << "data update done"<<endl;

		}
		m_flagReady2Draw = true;
		m_updated = true;
	}
	m_condSolutBufEmpty.notify_one();
}

template<typename ED>
void Buffer<ED>::drawBuffer(){
	{
		
		while (!g_algTermination && (m_buffer.size() == 0 || m_proBufCurSize == 0)){
			unique_lock<mutex> lock(ms_mutexBuf);
			if (!m_proBufCurSize){
				//cout<<"problem buffer is empty"<<endl;
				m_condProBufEmpty.wait_for(lock, chrono::milliseconds(10));
			}
			else{ 
				//cout<<"solution buffer is empty"<<endl;
				m_condSolutBufEmpty.wait_for(lock, chrono::milliseconds(10));
			}
		}
	}
	if (!g_algTermination){
		ofPushStyle();
		drawProblem();
		ofPopStyle();

		ofPushStyle();
		drawSolution();
		ofPopStyle();

		ofPushStyle();
		drawObjective();
		ofPopStyle();

		ofPushStyle();
		drawSolutndObj();
		ofPopStyle();

		if (m_updateComplete&&m_buffer.size()>1){
			popupData();
		}
		
	}

	m_condSolutBufFull.notify_one();
	m_condProBufFull.notify_one();
}

template<typename ED>
void Buffer<ED>::popupData(){
	m_buffer.pop_front();
	m_indiOfPop.pop_front();
	m_numPops.pop_front();
	if(m_numBestIndis.size()>0)	m_numBestIndis.pop_front();
}

template<typename ED>
bool Buffer<ED>::isReady2Draw(){
	return m_flagReady2Draw;
}

template<typename ED>
void Buffer<ED>::setReady2Draw(bool flag){
	{
		unique_lock<mutex> lock(ms_mutexBuf);
		m_flagReady2Draw = flag;
	}
}

template<typename ED>
void Buffer<ED>::loadProblem_(){
	{
		unique_lock<mutex> lock(ms_mutexBuf);
		loadProblem();
	}
}

template<typename ED>
void Buffer<ED>::updateFitnessLandsacpe_(){
	{
		unique_lock<mutex> lock(ms_mutexBuf);
		while (!g_algTermination&&m_proBufCurSize == m_proBufSize){
			//cout<<"problem buffer is full"<<endl;
			m_condProBufFull.wait_for(lock, chrono::milliseconds(100));
		}
	}
	updateFitnessLandsacpe();

	m_condProBufEmpty.notify_one();
}

#endif