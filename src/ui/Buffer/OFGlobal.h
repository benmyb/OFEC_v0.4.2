#ifndef OFGLOBAL_H
#define OFGLOBAL_H

#include "ofMain.h"


extern bool g_algTermination;
extern bool g_systemExit;
extern bool g_algExited;
extern bool g_algRunning;
extern int g_drawSpeed;
extern bool g_problemLaodingStarted;
extern vector<int> g_selObj;
extern int g_selZ;

enum MyViewport{viewMain=0,viewTop,viewBot};
enum MyCamera{camMain,camTop,camBot};
void ofgSetNormals( ofMesh &mesh );
void ofgSetGlobalParameters();
vector<string> ofgGet_filelist();

inline int toZ(int i,int j, int N){
	return i*N+j;
}
struct FreePeaks{
	int numTree=1,numBox=2, numGOP=1,numTrap=0,numLOP=1,numObj=1;
	vector<float> spaceRatio;
	vector<int> treeBox;
	bool center = true, equaldiv = false;
	float rhoh=0.1, rhow = 0.2;
};
struct OnePeak{
	set<string> shape;
	float minHeight = 0;
	pair<float, float> height;
	int basin = 0;
	vector<int> transform;
	bool noise = 0;
	vector<pair<bool, vector<double>>> constraint;
};
void ofgsetup();
extern FreePeaks g_fp;
extern int g_curType;
extern OnePeak g_onePeak[3],g_curPeak;
#endif