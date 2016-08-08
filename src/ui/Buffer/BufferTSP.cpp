#include "BufferTSP.h"
#include "../../imp/Problem/Combination/TSP/TravellingSalesman.h"
#include "../../imp/Problem/problem.h"
#include "OFGlobal.h"

extern bool g_algTermination;

BufferTSP::BufferTSP(vector<ofRectangle> &view,vector<ofEasyCam> &cam, ofLight &light,ofTrueTypeFont  &font,int numDim) :Buffer(view,cam,light,font)
{
	ofBackgroundHex(0x000000);

	m_cam[camMain].setDistance(1000);
	m_cam[camTop].setDistance(1000);
	
	initialize(numDim);
}

void BufferTSP::initialize(int numDim)
{
	ostringstream os;
	os<<Global::g_arg[param_workingDir]<<"Problem/Combination/TSP/data/"<<dynamic_cast<TravellingSalesman*>(Global::msp_global->mp_problem.get())->getFileName()<<".tsp";
	ifstream infile;
	infile.open(os.str().c_str());
	if(!infile)
		throw myException("read travelling salesman data error @BufferTSP::initialize");
	string line;
	vector<vector<double> > coord;
	while(getline(infile,line))
	{
		for(size_t i=0;i<line.size();i++)
			line[i]=toupper(line[i]);
		if(line.find("NODE_COORD_SECTION")!=string::npos)
		{
			coord.resize(numDim+2); //the last 2 dims keep the coordiate arrage, min,max
			for(int i=0;i<numDim+2;i++)
				coord[i].resize(2);
			int i=0;
			infile>>line;
			infile>>coord[i][0];
			infile>>coord[i][1];
			++i;
			coord[numDim][0]=coord[numDim+1][0]=coord[0][0];
			coord[numDim][1]=coord[numDim+1][1]=coord[0][1];
			while(infile>>line)
			{
				infile>>coord[i][0];
				infile>>coord[i][1];
				if(coord[numDim][0]>coord[i][0])
					coord[numDim][0]=coord[i][0];
				if(coord[numDim+1][0]<coord[i][0])
					coord[numDim+1][0]=coord[i][0];
				if(coord[numDim][1]>coord[i][1])
					coord[numDim][1]=coord[i][1];
				if(coord[numDim+1][1]<coord[i][1])
					coord[numDim+1][1]=coord[i][1];
				++i;
				if(i==numDim) break;
			}
		}
		else 
			continue;
	}

	if(coord.size()>0)
	{
		float width=m_view[viewMain].width;
		float height=m_view[viewMain].height;
		int loc;
		vector<ofVec3f> points(numDim);
		ofMesh mesh;
		for(int i=0;i<numDim;i++)
		{
			if(g_algTermination) return;
			points[i].x=-width/2+width*((coord[numDim+1][0]-coord[i][0])/(coord[numDim+1][0]-coord[numDim][0]));
			points[i].y=-height/2+height*((coord[numDim+1][1]-coord[i][1])/(coord[numDim+1][1]-coord[numDim][1]));
			points[i].z=0.0;
			mesh.addVertex(points[i]);
		}
		mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
		if(Global::msp_global->mp_problem->isGlobalOptKnown())
		{
			Optima<CodeVInt> & sol=CAST_TSP->getGOpt();
			m_meshOpt=mesh;
			for(int i=0;i<numDim;i++)
			{
				if(g_algTermination) return;
				loc=sol[0].data().m_x[i];
				m_meshOpt.addColor(ofFloatColor(56.,0.,0.));
				m_meshOpt.addIndex(loc);		
			}
		}
		m_mesh=m_meshOpt;
		for(int i=0;i<numDim;i++)
			m_mesh.setColor(i,ofFloatColor(56.,56.,0.));
	}
}

void BufferTSP::updateBuffer(const vector<Algorithm*> *pops)
{
	// put best individuals in solut
	vector<Solution<CodeVInt>> solut;
	vector<int> indiOfPop;
	int numBestIndis=0;
	m_proBufCurSize=1;
	if(pops){
		int size(0);
		for(auto i=pops->begin();i!=pops->end();++i)
			size+=reinterpret_cast<Population<CodeVInt,Individual<CodeVInt>>*>(*i)->getBest().size();

		solut.resize(size);
		indiOfPop.resize(size);
		int j=0;
		for(auto i=0;i<pops->size();++i){
			for(auto k=0;k<reinterpret_cast<Population<CodeVInt,Individual<CodeVInt>>*>((*pops)[i])->getBest().size();++k,++j){
				if(g_algTermination) return;
				solut[j].data() = reinterpret_cast<Population<CodeVInt, Individual<CodeVInt>>*>((*pops)[i])->getBest()[k]->data();
				indiOfPop[j]=i;
				++numBestIndis;
			}
		}
		
	}else{
		throw myException("no data come @ BufferTSP::updateBuffer(Population<CodeVInt,Individual<CodeVInt>> *pop)");
	}
	if(!solut.empty()){
		m_numBestIndis.push_back(numBestIndis);
		m_buffer.push_back(move(solut));
		m_indiOfPop.push_back(move(indiOfPop));
		m_numPops.push_back(pops->size());
	}
}

void BufferTSP::drawObjective()
{
	ofPushView();
	ofViewport(m_view[viewTop]);
	ofSetupScreen();

	stringstream ss;
	ss<<"evals: "<<Global::msp_global->mp_problem->getEvaluations();
	ofSetColor(255,0,0);
	m_font.drawString(ss.str(),5,15);
	ss.str("");
	ss<<"err: "<<m_buffer.front()[0].obj(0)-CAST_TSP->getGOpt()[0].obj(0);
	ofSetColor(255,0,0);
	m_font.drawString(ss.str(),5,30);

	ofPopView();
}

void BufferTSP::drawProblem()
{	
	int numDim=Global::msp_global->mp_problem->getNumDim();	
	int loc;
	ofPushView();
	ofViewport(m_view[viewMain]);
	ofSetupScreen();

	m_cam[camMain].begin(m_view[viewMain]);
	
	{
		unique_lock<mutex> lock(Scene::ms_mutexBuf);
		vector<Solution<CodeVInt>> & sol=m_buffer.front();
		for(int i=0;i<numDim;i++)
		{
			loc=sol[0].data().m_x[i];
			m_mesh.setIndex(i,loc);
			ofSetColor(0,255,255);
			stringstream os;
			os<<loc;
			m_font.drawString(os.str(),m_mesh.getVertex(loc).x,m_mesh.getVertex(loc).y+5);
		}

	}
	m_meshOpt.draw();
	m_mesh.draw();	
	m_cam[camMain].end();
	ofPopView();
}