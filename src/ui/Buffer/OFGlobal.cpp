#include "OFGlobal.h"
#include "../../imp/Global/global.h"
#include<io.h>
#include<stdio.h>
#include<conio.h>

bool g_algTermination=false;
bool g_algExited=true;
bool g_algRunning=false;
int g_drawSpeed=60;
bool g_problemLaodingStarted=false;
vector<int> g_selObj;
int g_selZ;
bool g_showPop = true;
FreePeaks g_fp;
OnePeak g_onePeak[3], g_curPeak;
int g_curType = 0;
bool g_showGOP = true;
bool g_showDomination = false;
//Universal function which sets normals for the triangle mesh
void ofgSetNormals( ofMesh &mesh ){
	//The number of the vertices
	int nV = mesh.getNumVertices();
	//The number of the triangles
	int nT = mesh.getNumIndices() / 3;
	vector<ofPoint> norm( nV ); //Array for the normals
	//Scan all the triangles. For each triangle add its
	//normal to norm's vectors of triangle's vertices
	for (int t=0; t<nT; t++) {
		//Get indices of the triangle t
		int i1 = mesh.getIndex( 3 * t );
		int i2 = mesh.getIndex( 3 * t + 1 );
		int i3 = mesh.getIndex( 3 * t + 2 );
		//Get vertices of the triangle
		const ofPoint &v1 = mesh.getVertex( i1 );
		const ofPoint &v2 = mesh.getVertex( i2 );
		const ofPoint &v3 = mesh.getVertex( i3 );
		//Compute the triangle's normal
		ofPoint dir = ( (v2 - v1).crossed( v3 - v1 ) ).normalized();
		//Accumulate it to norm array for i1, i2, i3
		norm[ i1 ] += dir;
		norm[ i2 ] += dir;
		norm[ i3 ] += dir;
	}
	//Normalize the normal's length
	for (int i=0; i<nV; i++) {
	norm[i].normalize();
	}
	//Set the normals to mesh
	mesh.clearNormals();
	mesh.addNormals( norm );
}
void ofgsetup(){
	g_fp.spaceRatio.resize(2);
	g_fp.spaceRatio[0] = 1; g_fp.spaceRatio[1] = 0;

	g_fp.treeBox.resize(2);
	g_fp.treeBox[0] = 1; g_fp.treeBox[1] = 0;
	vector<int> trans(4,-1);
	trans[0] = 0;
	g_curPeak.transform = trans;
	g_curPeak.height.first = 100;
	g_curPeak.height.second = 100;
}
void ofgSetGlobalParameters(){
	Global::ms_curAlgId=Global::msm_alg[(Global::g_arg[param_algName])];
	Global::ms_curProId=Global::msm_pro[(Global::g_arg[param_proName])];
}

vector<string> ofgGet_filelist()
{
	ostringstream os;
	os<<Global::g_arg[param_workingDir]<<"Problem/Combination/TSP/data/*.tsp";
	vector<string> fileName;
    long Handle;
	struct _finddata_t FileInfo;
	if((Handle=_findfirst(os.str().c_str(),&FileInfo))==-1L)
		cout<<"没有找到匹配的项目"<<endl;
	else
	{
		fileName.push_back(FileInfo.name);
		while( _findnext(Handle,&FileInfo)==0)
			fileName.push_back(FileInfo.name);
		_findclose(Handle);
	}
	return fileName;
}

