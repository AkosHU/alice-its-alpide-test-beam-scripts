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

void qualityCheckfs(int runNumber, int firstDUT, int lastDUT, string histFolder, string outputFolder, int nDetector) 
{
  vector<TH2*> rawHitMap(nDetector);
  vector<TH2*> efficiency(lastDUT-firstDUT+1);
  vector<TH1*> residualX(lastDUT-firstDUT+1);
  vector<TH1*> residualY(lastDUT-firstDUT+1);
  vector<TH2*> correlationX(nDetector-1);
  vector<TH2*> correlationY(nDetector-1);
  vector<TH1*> prealignX(nDetector-1);
  vector<TH1*> prealignY(nDetector-1);
  vector<TCanvas*> rawHitMapC(nDetector);
  vector<TCanvas*> efficiencyC(lastDUT-firstDUT+1);
  vector<TCanvas*> residualC(lastDUT-firstDUT+1);
  vector<TCanvas*> correlationC(nDetector-1);
  vector<TCanvas*> prealignC(nDetector-1);
  for (unsigned int i=0; i<rawHitMapC.size(); i++) rawHitMapC[i] = new TCanvas(Form("rawHitMapC_%d",i),Form("rawHitMapC_%d",i),800,600);
  for (unsigned int i=0; i<efficiencyC.size(); i++) efficiencyC[i] = new TCanvas(Form("efficiencyC_%d",i),Form("efficiencyC_%d",i),800,600);
  for (unsigned int i=0; i<residualC.size(); i++) 
  {
    residualC[i] = new TCanvas(Form("residualC_%d",i),Form("residualC_%d",i),800,600);
    residualC[i]->Divide(1,2);
  }
  for (unsigned int i=0; i<correlationC.size(); i++) 
  {
    correlationC[i] = new TCanvas(Form("correlationC_%d",i),Form("correlationC_%d",i),800,600);
    correlationC[i]->Divide(2,1);
  }
  for (unsigned int i=0; i<prealignC.size(); i++) 
  {
    prealignC[i] = new TCanvas(Form("prealignC_%d",i),Form("prealignC_%d",i),800,600);
    prealignC[i]->Divide(2,1);
  }
  string fileName;
  bool deadColumnAvailable = true;
  fileName = histFolder + Form("/run%06d-deadColumn.root",runNumber);
  TFile* deadColumnFile = new TFile(fileName.c_str(),"READONLY");
  if (!deadColumnFile || deadColumnFile->IsZombie())
  { 
    cerr << "No dead column file " << fileName << endl;
    deadColumnAvailable = false;
  }
  for (int iDetector=0; iDetector<nDetector && deadColumnAvailable; iDetector++)
  {
    rawHitMap[iDetector] = (TH2I*)deadColumnFile->Get(Form("hitMap_%d",iDetector));
    rawHitMapC[iDetector]->cd();
    rawHitMapC[iDetector]->SetLogz();
    rawHitMap[iDetector]->SetMinimum(0.9);
    rawHitMap[iDetector]->Draw("COLZ");
    string outputFile = outputFolder + Form("/important/rawHitMap_%d.pdf",iDetector);
    rawHitMapC[iDetector]->SaveAs(outputFile.c_str());
  }
  for (int iDUT=0; iDUT<=lastDUT-firstDUT; iDUT++)
  {
    fileName = histFolder + Form("/run%06d-analysis_DUT%d.root",runNumber,iDUT+firstDUT);
    TFile* analysisFile = new TFile(fileName.c_str(),"READONLY");
    if (!analysisFile || analysisFile->IsZombie())
    {
	    cerr << "No analysis file " << fileName << endl;
	    continue;
    }
    efficiency[iDUT] = (TH2F*)analysisFile->Get("Analysis/efficiencyHisto");
    residualX[iDUT] = (TH1F*)analysisFile->Get("Analysis/Sector_1/residualXPAlpide_30.0_1");
    residualY[iDUT] = (TH1F*)analysisFile->Get("Analysis/Sector_1/residualYPAlpide_30.0_1");
    efficiencyC[iDUT]->cd();
    efficiency[iDUT]->Draw("COLZ");
    string outputFile = outputFolder + Form("/important/efficiency_DUT%d.pdf",iDUT+firstDUT);
    efficiencyC[iDUT]->SaveAs(outputFile.c_str());
    residualC[iDUT]->cd(1);
    residualX[iDUT]->Draw();
    residualC[iDUT]->cd(2);
    residualY[iDUT]->Draw();
    outputFile = outputFolder + Form("/important/residual_sector1_DUT%d.pdf",iDUT+firstDUT);
    residualC[iDUT]->SaveAs(outputFile.c_str());
  }
  bool clusteringAvailable = true;
  fileName = histFolder + Form("/run%06d-clustering.root",runNumber);
  TFile* clusteringFile = new TFile(fileName.c_str(),"READONLY");
  if (!clusteringFile || clusteringFile->IsZombie())
  {
	  cerr << "No clustering file " << fileName << endl;
	  clusteringAvailable = false;
  }
  for (int iDetector=1; iDetector<nDetector && clusteringAvailable; iDetector++)
  {
    correlationX[iDetector-1] = (TH2I*)clusteringFile->Get(Form("Correlator/ClusterX/ClusterXCorrelationHisto_d0_d%d",iDetector));
    correlationY[iDetector-1] = (TH2I*)clusteringFile->Get(Form("Correlator/ClusterY/ClusterYCorrelationHisto_d0_d%d",iDetector));
    correlationC[iDetector-1]->cd(1)->SetLogz();
    correlationX[iDetector-1]->Draw("COLZ");
    correlationC[iDetector-1]->cd(2)->SetLogz();
    correlationY[iDetector-1]->Draw("COLZ");
    string outputFile = outputFolder + Form("/others/correlation_0_%d.pdf",iDetector);
    correlationC[iDetector-1]->SaveAs(outputFile.c_str());
  }

  bool prealignAvailable = true;
  fileName = histFolder + Form("/run%06d-prealign.root",runNumber);
  TFile* prealignFile = new TFile(fileName.c_str(),"READONLY");
  if (!prealignFile || prealignFile->IsZombie())
  {
    cerr << "No prealign file " << fileName << endl;
    prealignAvailable = false;
  }
  for (int iDetector=1; iDetector<nDetector && prealignAvailable; iDetector++)
  {
    prealignX[iDetector-1] = (TH2I*)prealignFile->Get(Form("PreAligner/plane_%d/hitXCorr_0_to_%d",iDetector,iDetector));
    prealignY[iDetector-1] = (TH2I*)prealignFile->Get(Form("PreAligner/plane_%d/hitYCorr_0_to_%d",iDetector,iDetector));
    prealignC[iDetector-1]->cd(1);
    prealignX[iDetector-1]->Draw();
    prealignC[iDetector-1]->cd(2);
    prealignY[iDetector-1]->Draw();
    string outputFile = outputFolder + Form("/others/prealign_0_%d.pdf",iDetector);
    prealignC[iDetector-1]->SaveAs(outputFile.c_str());
  }

}

