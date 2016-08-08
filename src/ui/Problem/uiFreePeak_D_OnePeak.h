#pragma once
#include "ofxUI.h"
void setFreePeak_D_OnePeakVisible(ofxUISuperCanvas *ui, bool flag){
	ui->getWidget("FEATURE2")->setVisible(flag);
	ui->getWidget("HR1")->setVisible(flag);
	ui->getWidget("FEATURE3")->setVisible(flag);
	ui->getWidget("HR2")->setVisible(flag);
	ui->getWidget("NOISE")->setVisible(flag);
	ui->getWidget("CHANGE RATIO")->setVisible(flag);

}

