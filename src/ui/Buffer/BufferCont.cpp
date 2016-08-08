#include "BufferCont.h"
#include "OFGlobal.h"
#include "../../imp/Algorithm/PopulationCont.h"
extern unique_ptr<Scene> msp_buffer;

void BufferCont::updateBuffer(const vector<vector<Solution<CodeVReal>>*> *pops) {
	Buffer::updateBuffer(pops);
	
	if (g_algTermination) return;
	bool goptKnown = Global::msp_global->mp_problem->isGlobalOptKnown();
	if (goptKnown)
	{
		int numObj = GET_NUM_OBJ;
		unique_lock<mutex> lock(g_mutex);
		for (int i = 0; i < numObj; ++i) {
			if (m_curObjRange[i].first > m_proCont->m_gobjRange.back()[i].first) {
				m_curObjRange[i].first = m_proCont->m_gobjRange.back()[i].first;
			}

			if (m_curObjRange[i].second < m_proCont->m_gobjRange.back()[i].second) {
				m_curObjRange[i].second = m_proCont->m_gobjRange.back()[i].second;
			}

		}
		m_proCont->updateObj(m_view, m_curObjRange);

	}
	if (g_algTermination) return;
	static vector<ofFloatColor> color;
	vector<int> popsId;
	vector<ofFloatColor> indiColor;
	color.reserve(pops->size());
	popsId.reserve(m_indiOfPop.back().size());
	indiColor.reserve(m_indiOfPop.back().size());

	for (auto i = 0; i<pops->size(); ++i) {
		if (g_algTermination) return;
		popsId.push_back(i);
		if (!m_popsId.empty()) {
			int j = 0;
			for (; j<m_popsId.back().size(); ++j) {
				if (popsId.back() == m_popsId.back().at(j)) break;
			}
			if (j<m_popsId.back().size()) color.push_back(color[j]);
			else color.push_back(move(ofColor(ofRandom(200), ofRandom(200), ofRandom(200))));
		}
		else {
			color.push_back(move(ofColor(ofRandom(200), ofRandom(200), ofRandom(200))));
		}
	}

	m_popsId.push_back(move(popsId));
	updateInids();

	unsigned n = 0;
	for (n = 0; n<m_buffer.back().size(); ++n) {
		indiColor.push_back(color.at(m_indiOfPop.back().at(n)));
	}
	m_indiColor.push_back(move(indiColor));
}
void BufferCont::updateBuffer(const vector<Algorithm*> *pops){
	Buffer::updateBuffer(pops);

	if (g_algTermination) return;
	bool goptKnown = Global::msp_global->mp_problem->isGlobalOptKnown();
	if (goptKnown)
	{
		int numObj = GET_NUM_OBJ;
		unique_lock<mutex> lock(g_mutex);
		for (int i = 0; i < numObj; ++i) {
			if (m_curObjRange[i].first > m_proCont->m_gobjRange.back()[i].first) {
				m_curObjRange[i].first = m_proCont->m_gobjRange.back()[i].first;
			}

			if (m_curObjRange[i].second < m_proCont->m_gobjRange.back()[i].second) {
				m_curObjRange[i].second = m_proCont->m_gobjRange.back()[i].second;
			}

		}
		m_proCont->updateObj(m_view, m_curObjRange);

	}
	if (g_algTermination) return;
	static vector<ofFloatColor> color;
	vector<int> popsId;
	vector<ofFloatColor> indiColor;
	color.reserve(pops->size());
	popsId.reserve(m_indiOfPop.back().size());
	indiColor.reserve(m_indiOfPop.back().size());

	for (auto i = 0; i<pops->size(); ++i) {
		if (g_algTermination) return;
		popsId.push_back(reinterpret_cast<Population<CodeVReal, Individual<CodeVReal>>*>((*pops)[i])->getID());
		if (!m_popsId.empty()) {
			int j = 0;
			for (; j<m_popsId.back().size(); ++j) {
				if (popsId.back() == m_popsId.back().at(j)) break;
			}
			if (j<m_popsId.back().size()) color.push_back(color[j]);
			else color.push_back(move(ofColor(ofRandom(200), ofRandom(200), ofRandom(200))));
		}
		else {
			color.push_back(move(ofColor(ofRandom(200), ofRandom(200), ofRandom(200))));
		}
	}

	m_popsId.push_back(move(popsId));
	updateInids();

	unsigned n = 0;
	for (n = 0; n<m_buffer.back().size(); ++n) {
		indiColor.push_back(color.at(m_indiOfPop.back().at(n)));
	}
	m_indiColor.push_back(move(indiColor));
}

void BufferCont::startMappingThread(){
	

	int numTask = std::thread::hardware_concurrency();

	if (numTask>m_buffer.back().size()) numTask = m_buffer.back().size();
	
	vector<thread> thrd;
	size_t num = (m_buffer.back().size() / numTask);
	size_t i = 0;

	vector<vector<int>> tsk(numTask);
	for (auto &t : tsk)	t.reserve(num+1);

	for (; i < m_buffer.back().size(); ++i){
		int k = i / num;
		if (k == numTask) break;
		tsk[k].push_back(i);
	}
	for (size_t j = 0; i < m_buffer.back().size(); ++i, ++j){
		tsk[j].push_back(i);
	}
	for (int i = 0; i<numTask; i++){
		thrd.push_back(thread(mappingThread, ref(tsk[i]), ref(*this)));
	}
	for (auto &i : thrd) i.join();
}
int BufferCont::mappingThread(const vector<int> &tsk, BufferCont& buf ){
	int objs = Global::msp_global->mp_problem->getNumObj()<3 ?  Global::msp_global->mp_problem->getNumObj():3;
	float sizeBot = buf.m_view[camBot].width < buf.m_view[camBot].height ? buf.m_view[camBot].width : buf.m_view[camBot].height;
	float sizeTop = buf.m_view[viewTop].width<buf.m_view[viewTop].height ? buf.m_view[viewTop].width : buf.m_view[viewTop].height;
	for (auto i : tsk){
		if (g_algTermination) return 0;
		buf.m_mainPoint.back()[i] = (move(buf.m_proCont->mapMainPoint(buf.m_buffer.back()[i].data())));
		buf.m_solPoint.back()[i] = move(buf.m_proCont->mapXtoScreen(buf.m_buffer.back()[i].data().m_x, sizeBot));
		buf.m_objPoint.back()[i] = move(buf.m_proCont->mapOtoScreen(buf.m_buffer.back()[i].data().m_obj, buf.m_curObjRange, sizeTop));
		
		if (buf.m_numBestIndis.size() > 0) {
			if (i <= buf.m_buffer.back().size() - buf.m_numBestIndis.back())	buf.m_sizes.back()[i] = (move(ofVec3f(10)));
			else		buf.m_sizes.back()[i] = (move(ofVec3f(20)));
		} else		buf.m_sizes.back()[i] = (move(ofVec3f(20)));
	}
	return 0;
}

void BufferCont::updateInids(){	
	m_mainPoint.push_back(move(vector <ofVec3f>(m_buffer.back().size())));
	m_sizes.push_back(move(vector <ofVec3f>(m_buffer.back().size())));
	m_objPoint.push_back(move(vector <ofVec3f>(m_buffer.back().size())));
	m_solPoint.push_back(move(vector <ofVec3f>(m_buffer.back().size())));
	startMappingThread();
}
void BufferCont::popupData(){
	Buffer::popupData();
	m_indiColor.pop_front();
	m_popsId.pop_front();
	m_mainPoint.pop_front();
	m_sizes.pop_front();
	m_objPoint.pop_front();
	m_solPoint.pop_front();
}

void BufferCont::drawPoint(ofRectangle &view, ofEasyCam & cam, vector <ofVec3f> &data){

	m_vbo.setVertexData(&data[0], data.size(), GL_STATIC_DRAW);
	m_vbo.setNormalData(&m_sizes.front()[0], m_sizes.front().size(), GL_STATIC_DRAW);
	m_vbo.setColorData(&m_indiColor.front()[0], m_indiColor.front().size(), GL_STATIC_DRAW);
	glDepthMask(GL_FALSE);
	//ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();

	m_shader.begin();
	cam.begin(view);
	m_texture.bind();
	m_vbo.draw(GL_POINTS, 0, data.size());
	m_texture.unbind();
	cam.end();
	m_shader.end();

	ofDisablePointSprites();
	//ofDisableBlendMode();
	glDepthMask(GL_TRUE);
}
void BufferCont::drawSolution(){
	
	float size = m_view[camBot].width<m_view[camBot].height ? m_view[camBot].width : m_view[camBot].height;
	if (g_showPop)	drawPoint(m_view[viewBot], m_cam[camBot], m_solPoint.front());

	m_cam[camBot].begin(m_view[viewBot]);
	ofSetColor(255, 0, 0);
	ofVec3f start, end;
	start.x = -size / 2;	start.y = -size / 2;		start.z = 0;
	end.x = size / 2; 		end.y = -size / 2;		end.z = 0;
	ofDrawArrow(start, end, 5.0);
	m_font.drawString("x1", end.x, end.y);
	end.x = -size / 2; end.y = size / 2; end.z = 0;
	ofDrawArrow(start, end, 5.0);
	m_font.drawString("x2", end.x, end.y);
	m_cam[camBot].end();

	ofPushView();
	ofViewport(m_view[viewBot]);
	ofSetupScreen();
	stringstream ss;
	ss << "evals: " << Global::msp_global->mp_problem->getEvaluations();
	ofSetColor(0, 0, 0);
	m_font.drawString(ss.str(), 5, 25);
	ofPopView();

}
void BufferCont::drawObjective(){
	
	float size=m_view[viewTop].width<m_view[viewTop].height?m_view[viewTop].width:m_view[viewTop].height;
	int objs=Global::msp_global->mp_problem->getNumObj();

	ofVec3f start, end;
	m_cam[camTop].begin(m_view[viewTop]);
	int offset = 10;
	if(objs==1){
		ofSetColor(255,0,0);
		start.x = -size / 2; start.y = size / 2; start.z = 0;
		end.x = size / 2; end.y = size / 2; end.z = 0;	
		ofSetLineWidth(1.0);
		ofDrawArrow(start, end,5.0);
		m_font.drawString("f", end.x, end.y);
	}else if(objs==2){

		ofSetColor(255, 0, 0);
		start.x = -size / 2 - offset;	start.y = -size / 2 - offset;		start.z = 0;
		end.x = size / 2 + offset; 		end.y = -size / 2 - offset;		end.z = 0;
		ofDrawArrow(start, end, 5.0);
		m_font.drawString("f1", end.x, end.y);
		end.x = -size / 2 - offset; end.y = size / 2 + offset; end.z = 0;
		ofDrawArrow(start, end, 5.0);
		m_font.drawString("f2", end.x, end.y);	
	}else if(objs>=3){

		ofSetColor(255, 0, 0);
		start.x = -size / 2 - offset;	start.y = -size / 2 - offset;		start.z = -size / 2 - offset;
		end.x = size / 2 + offset; 		end.y = -size / 2 - offset;		end.z = -size / 2 - offset;
		ofDrawArrow(start, end, 5.0);

		ofPushMatrix();
		ofTranslate(0, 0, end.z);
		m_font.drawString("f1", end.x, end.y);
		ofPopMatrix();

		end.x = -size / 2 - offset; end.y = size / 2 + offset; end.z = -size / 2 - offset;
		ofDrawArrow(start, end, 5.0);

		ofPushMatrix();
		ofTranslate(0, 0, end.z);
		m_font.drawString("f2", end.x, end.y);
		ofPopMatrix();

		end.x = -size / 2 - offset; end.y = -size / 2 - offset; end.z = size / 2 + offset;
		ofDrawArrow(start, end, 5.0);

		ofPushMatrix();
		ofTranslate(0, 0, end.z);
		m_font.drawString("f3", end.x, end.y);
		ofPopMatrix();
	}
	m_cam[camTop].end();

	if (g_showPop)	drawPoint(m_view[viewTop], m_cam[camTop], m_objPoint.front());

}
void BufferCont::drawSolutndObj(){
	float size = m_view[viewMain].width<m_view[viewMain].height ? m_view[viewMain].width : m_view[viewMain].height;
	
	m_updated=false;	
	if (g_showPop)	drawPoint(m_view[viewMain], m_cam[camMain], m_mainPoint.front());
	
}
void BufferCont::drawProblem(){
	
	m_cam[camMain].begin(m_view[viewMain]);
	m_proCont->draw(m_proBufCurSize);
	m_cam[camMain].end();
	
	
	m_cam[camTop].begin(m_view[viewTop]);
	m_proCont->drawObjectiveSpace();
	m_cam[camTop].end();

	m_cam[camBot].begin(m_view[viewBot]);
	m_proCont->drawSolutionSpace(m_font);
	m_cam[camBot].end();

}


void BufferCont::updateFitnessLandsacpe(){
	m_proCont->updateLandscape(m_proBufCurSize);
}

void BufferCont::loadProblem(){
	
	m_proCont->setup(m_proBufCurSize,m_defaultZobj,m_view,m_curObjRange);

}

void BufferCont::updateSelObjAdZ(const vector<int> &obj, int defaultZobj)
{
	if (obj.size()>0)	m_proCont->setSelObj(obj);
	m_defaultZobj = defaultZobj;
}