#define DEBUG 1
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include "TCanvas.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
using namespace std;

void qualityCheckNoise(int runNumber, string histFolder, string outputFolder, int nDetector) 
{
  vector<TH2*> rawHitMap(nDetector);
  vector<TCanvas*> rawHitMapC(nDetector);
  for (unsigned int i=0; i<rawHitMapC.size(); i++) rawHitMapC[i] = new TCanvas(Form("rawHitMapC_%d",i),Form("rawHitMapC_%d",i),800,600);
  string fileName;
  fileName = histFolder + Form("/run%06d-noise.root",runNumber);
  TFile* noiseFile = new TFile(fileName.c_str(),"READONLY");
  if (!noiseFile || noiseFile->IsZombie())
  { 
    cerr << "No noise file " << fileName << endl;
    return;
  }
  for (int iDetector=0; iDetector<nDetector; iDetector++)
  {
    rawHitMap[iDetector] = (TH2I*)noiseFile->Get(Form("noiseMap_%d",iDetector));
    rawHitMapC[iDetector]->cd();
    rawHitMapC[iDetector]->SetLogz();
    rawHitMap[iDetector]->SetMinimum(0.9);
    rawHitMap[iDetector]->Draw("COLZ");
    string outputFile = outputFolder + Form("/rawHitMap_%d.pdf",iDetector);
    rawHitMapC[iDetector]->SaveAs(outputFile.c_str());
  }
}

