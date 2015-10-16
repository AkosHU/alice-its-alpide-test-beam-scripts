#define DEBUG 0
#define M_PI           3.14159265358979323846
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <utility>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <numeric>
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TGraph2D.h"
#include "TGraph2DErrors.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TFitResult.h"
#include "TFrame.h"
#include "TGaxis.h"
#include "TROOT.h"
#include "TMath.h"
#include "TTree.h"

using namespace std;

class Run {
    int runNumber, irradiation, nEvent;
    double ithr, ithrpA, idb, vcasn, vaux, vcasp, vreset, BB, readoutDelay, triggerDelay, strobeLength, strobeBLength, energy;
    vector<double> eff, nTr, nTrpA, thr, thrE, noise, noiseE, clusterSizeV, clusterSizeVRMS, residualV, residualVE, resolutionV, resolutionVE, noiseOccupancyBeforeRemovalV, noiseOccupancyBeforeRemovalVE, noiseOccupancyAfterRemovalV, noiseOccupancyAfterRemovalVE;
    vector<TH1*> clusterSize, residualX, residualY;
    TH2* fakeHitHistoFromNoise;
    bool noiseRun, plotFlag;
    string chipID;
  public:
    Run(){
      runNumber = -1;
    }
    Run(int r, double casn, double aux, double casp, double reset, double i, double ipA, double db, vector<double> t, vector<double> tE, vector<double> n, vector<double> nE, double bb, int irr, string id, double rD, double tD, double sL, double sBL, bool nR, int nEv, double ene){
      runNumber = r;
      vcasn = casn;
      vaux = aux;
      vcasp = casp;
      vreset = reset;
      readoutDelay = rD;
      triggerDelay = tD;
      strobeLength = sL;
      strobeBLength = sBL;
      ithr = i;
      ithrpA = ipA;
      idb = db;
      thr = t;
      thrE = tE;
      noise = n;
      noiseE = nE; 
      noiseRun = nR;
      BB = bb;
      irradiation = irr;
      chipID = id;
      nEvent = nEv;
      energy = ene;
    }
    void Set(int r, double casn, double aux, double casp, double reset, double i,double ipA, double db, vector<double> t, vector<double> tE, vector<double> n, vector<double> nE, double bb, int irr, string id, double rD, double tD, double sL, double sBL, bool nR, int nEv, double ene){
      runNumber = r;
      vcasn = casn;
      vaux = aux;
      vcasp = casp;
      vreset = reset;
      readoutDelay = rD;
      triggerDelay = tD;
      strobeLength = sL;
      strobeBLength = sBL;
      ithr = i;
      ithrpA = ipA;
      idb = db;
      thr = t;
      thrE = tE;
      noise = n;
      noiseE = nE; 
      noiseRun = nR;
      BB = bb;
      irradiation = irr;
      chipID = id;
      nEvent = nEv;
      energy = ene;
    }
    void getAllParameters(int &r, double &casn, double &aux, double &casp, double &reset, double &i, double &ipA, double &db, vector<double> &t, vector<double> &tE, vector<double> &n, vector<double> &nE, double &bb, int &irr, string &id, double &rD, double &tD, double &sL, double &sBL, bool &nR, int &nEv, double &ene, vector<double> &efficiency, vector<double> &nTrack, vector<double> &nTrackpA, vector<double> &cs, vector<double> &csRMS, vector<double> &residual, vector<double> &residualE, vector<double> &res, vector<double> &resE, vector<double> &noiB, vector<double> &noiBE, vector<double> &noiA, vector<double> &noiAE){
      r = runNumber;
      casn = vcasn;
      aux = vaux;
      casp = vcasp;
      reset = vreset;
      rD = readoutDelay;
      tD = triggerDelay;
      sL = strobeLength;
      sBL = strobeBLength;
      i = ithr;
      ipA = ithrpA;
      db = idb;
      t = thr;
      tE = thrE;
      n = noise;
      nE = noiseE; 
      nR = noiseRun;
      bb = BB;
      irr = irradiation;
      id = chipID;
      nEv = nEvent;
      ene = energy;
      efficiency = eff;
      nTrack = nTr;
      nTrackpA = nTrpA;
      cs = clusterSizeV;
      csRMS = clusterSizeVRMS;
      residual = residualV;
      residualE = residualVE;
      res = resolutionV;
      resE = resolutionVE;
      noiB = noiseOccupancyBeforeRemovalV;
      noiBE = noiseOccupancyBeforeRemovalVE;
      noiA = noiseOccupancyAfterRemovalV;
      noiAE = noiseOccupancyAfterRemovalVE;
    }
    int getRunNumber() {return runNumber;}
    double getVcasn() {return vcasn;}
    double getVaux() {return vaux;}
    double getVcasp() {return vcasp;}
    double getVreset() {return vreset;}
    double getIthr() {return ithr;}
    double getIdb() {return idb;}
    double getBB() {return BB;}
    string getChipID() {return chipID;}
    double getReadoutDelay() {return readoutDelay;}
    double getTriggerDelay() {return triggerDelay;}
    double getStrobeLength() {return strobeLength;}
    double getStrobeBLength() {return strobeBLength;}
    double getnEvent() {return nEvent;}
    bool PlotFlag(){return plotFlag;}
    int getIrradiation() {return irradiation;}
    void setRunNumber(int r) {runNumber = r; return;}
    void setnEvent(double nEv) {nEvent = nEv; return;}
    void setEff(vector<double> e) {eff = e; return;}
    void setnTr(vector<double> tr) {nTr=tr; return;}
    void setnTrpA(vector<double> trp) {nTrpA = trp; return;}
    void setThr(vector<double> t) {thr = t; return;}
    void setThrE(vector<double> tE) {thrE = tE; return;}
    void setNoise(vector<double> n) {noise = n; return;}
    void setNoiseE(vector<double> nE) {noiseE = nE; return;}
    void setClusterSizeVector(vector<double> cs) {clusterSizeV = cs; return;}
    void setClusterSizeVectorRMS(vector<double> csRMS) {clusterSizeVRMS = csRMS; return;}
    void setResidualVector(vector<double> res) {residualV = res; return;}
    void setResidualVectorE(vector<double> resE) {residualVE= resE; return;}
    void setResolutionVector(vector<double> res) {resolutionV = res; return;}
    void setResolutionVectorE(vector<double> resE) {resolutionVE = resE; return;}
    void setNoiseOccupancyBeforeRemovalV(vector<double> noi) {noiseOccupancyBeforeRemovalV = noi; return;}
    void setNoiseOccupancyBeforeRemovalVE(vector<double> noi) {noiseOccupancyBeforeRemovalVE = noi; return;}
    void setNoiseOccupancyAfterRemovalV(vector<double> noi) {noiseOccupancyAfterRemovalV = noi; return;}
    void setNoiseOccupancyAfterRemovalVE(vector<double> noi) {noiseOccupancyAfterRemovalVE = noi; return;}
    void setClusterSize(vector<TH1*> cS) {clusterSize = cS; return;}
    void setResidualX(vector<TH1*> rX) {residualX = rX; return;}
    void setResidualY(vector<TH1*> rY) {residualY = rY; return;}
    void setFakeHitHistoFromNoise(TH2* fakeHitHisto){fakeHitHistoFromNoise = fakeHitHisto; return;}
    void setPlotFlag(bool b) {plotFlag = b; return;}
    void setIsNoise(bool n) {noiseRun = n; return;}
    vector<double> getEff() {return eff;}
    vector<double> getnTr() {return nTr;}
    vector<double> getnTrpA() {return nTrpA;}
    vector<double> getThr() {return thr;}
    vector<double> getThrE() {return thrE;}
    vector<double> getNoise() {return noise;}
    vector<double> getNoiseE() {return noiseE;}
    vector<double> getClusterSizeV() {return clusterSizeV;}
    vector<double> getClusterSizeRMSV() {return clusterSizeVRMS;}
    vector<double> getResidualV() {return residualV;}
    vector<double> getResolutionV() {return resolutionV;}
    vector<double> getNoiseOccupancyBeforeRemoval() {return noiseOccupancyBeforeRemovalV;}
    vector<double> getNoiseOccupancyBeforeRemovalE() {return noiseOccupancyBeforeRemovalVE;}
    vector<double> getNoiseOccupancyAfterRemoval() {return noiseOccupancyAfterRemovalV;}
    vector<double> getNoiseOccupancyAfterRemovalE() {return noiseOccupancyAfterRemovalVE;}
    vector<TH1*> getClusterSize() {return clusterSize;}
    vector<TH1*> getResidualX() {return residualX;}
    vector<TH1*> getResidualY() {return residualY;}
    TH2* getFakeHitHistoFromNoise(){return fakeHitHistoFromNoise;}
    bool isNoise() {return noiseRun;}
    bool equalSettings(Run run2)
    {
      if (vcasn == run2.getVcasn() && vaux == run2.getVaux() && vcasp == run2.getVcasp() && vreset == run2.getVreset() && ithr == run2.getIthr() && idb == run2.getIdb() && readoutDelay == run2.getReadoutDelay() && triggerDelay == run2.getTriggerDelay() && strobeLength == run2.getStrobeLength() && strobeBLength == run2.getStrobeBLength() && BB == run2.getBB() && irradiation == run2.getIrradiation() && chipID == run2.getChipID()) return true;
      else return false;
    }
    bool canBeAveraged(Run run2)
    {
      if (vcasn == run2.getVcasn() && vaux == run2.getVaux() && vcasp == run2.getVcasp() && vreset == run2.getVreset() && ithr == run2.getIthr() && idb == run2.getIdb() && readoutDelay == run2.getReadoutDelay() && triggerDelay == run2.getTriggerDelay() && strobeLength == run2.getStrobeLength() && strobeBLength == run2.getStrobeBLength() && BB == run2.getBB() && irradiation == run2.getIrradiation() && noiseRun == run2.isNoise()) return true;
      else return false;
    }
    void print()
    {
      cout << "Vcasn: " << vcasn << endl;
      cout << "Vaux: " << vaux << endl;
      cout << "Vcasp: " << vcasp << endl;
      cout << "Vreset: " << vreset << endl;
      cout << "Ithr: " << ithr << endl;
      cout << "Idb: " << idb << endl;
      cout << "Readout delay: " << readoutDelay << endl;
      cout << "Trigger delay: " << triggerDelay << endl;
      cout << "Strobe length: " << strobeLength << endl;
      cout << "StrobeB length: " << strobeBLength << endl;
      cout << "Vbb: " << BB << endl;
      cout << "Irradiation level: " << irradiation << endl;
      cout << "Chip ID: " << chipID << endl;
    }
};

bool Skip(int runNumber);
vector<TH1F*> CalculateNoiseFromNoise(TH2* fakeHitHisto, int runNumberIndex, vector<Run> runs);

void WriteGraph(string outputFolder, int dut, int firstRun, int lastRun, string toSkip="", double pointingRes=0, string noiseFileName="", string thresholdFileName="", string settingsFileFolder="", double BBOverWrite = 0);

void mergeGraphs(string files, string outputFolder="./");

void compareDifferentGraphsFromTree(string files, string xName, string hist, int iSector, string canVary="", bool addBB=true, bool addIrr=true, bool addChipNumber=true, bool addRate=false);
void compareDifferentGraphsFromTree(string files, string xName, string hist, int iSector, double xlow, double xhigh, string xTitle, double ylow1, double yhigh1, double line1, bool log1, string yTitle1, double ylow2, double yhigh2, double line2, bool log2, string yTitle2, string legendTitle, string canVary, bool addBB, bool addIrr, bool addChipNumber, bool addRate);
void compareDifferentGraphs2D(string files, string hist, int sector, bool IthrVcasn, double IthrVcasnValue, bool BB=true, bool irr=true, bool chip=true, bool rate=false);
void compareDifferentGraphs2D(string file, string hist, int sector, bool IthrVcasn, double IthrVcasnValue, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle1, double y1low, double y1high, bool log1, double line1, const char* yTitle2, double y2low, double y2high, double line2, bool log2, bool BB, bool irr, bool chip, bool rate);
void compareOneHistogram(string files, string hist, string sectorStr, bool IthrVcasn, double IthrVcasnValue, int type, bool BB=false, bool irr=true, bool chip=false, bool rate=false, string comparison = "");
void compareOneHistogram(string file, string hist, string sectorStr, bool IthrVcasn, double IthrVcasnValue, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle, double ylow, double yhigh, bool log, double line, bool BB, bool irr, bool chip, bool rate, const char* title, int type);
void compareDifferentIthrVcasn2D(string file, string hist, int sector=-1);
void compareDifferentIthrVcasn2D(string file, string hist, int sector, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle1, double y1low, double y1high, bool log1, double line1, const char* yTitle2, double y2low, double y2high, double line2, bool log2);
void compareDifferentSectors2D(string file, string hist,  bool IthrVcasn=true, double IthrVcasnValue=-1);
void compareDifferentSectors2D(string file, string hist,  bool IthrVcasn, double IthrVcasnValue, const char* yTitle1, const char* yTitle2,const char* xTitle, const char* legend, double y1low, double y1high, double line1, double y2low, double y2high, double line2, double xlow, double xhigh, bool log1, bool log2);

string getLegend(string file, bool addBB=true, bool addIrr=true, bool addChipNumber=false, bool addRate=false);
bool getDefaultsOneAxis(string graph, double& low, double& high, double& line, bool& log, string& title, string& legend);
bool getDefaults(string graph, double& xlow, double& xhigh, double& ylow, double& yhigh, double& line, bool& log, string& xTitle, string& yTitle, string& legend, double& x2low, double& x2high, string& xTitle2);

void Draw2D(TGraph2D* graph, const char* canvas, string zTitle, string title, bool logZ, double zlow, double zhigh);
void Draw(vector<TGraph*> graph, string canvas, const char* titleX, const char* titleY, vector<string> legendStr, double rangeLow, double rangeHigh, double line, double xLow=90, double xHigh=210, bool log=false, const char* canvasTitle="");
void DrawSame(vector<TGraph*> graph1, vector<TGraph*> graph2, const char* canvas, const char* titleX, const char* titleY, double xlow, double xhigh, double rangeLow, double rangeHigh, const char* legendTitle, vector<string> legendStr, bool log, double line, vector<string> legendStr2, const char* title);
void DrawOverDifferentGraphs(vector<TGraph*> graph1, double rangeLow1, double rangeHigh1, double line1, const char* titleY1, vector<TGraph*> graph2, double rangeLow2, double rangeHigh2, double line2, const char* titleY2, const char* canvas, const char* legendTitle, vector<string> legendStr, double xlow=80, double xhigh=420,bool log1=false, bool log2=false, const char* titleX="Threshold in electrons",const char* canvasTitle="");

TGraph* Get1DFrom2D(TGraph2D* graph, bool IthrVcasn, double value, bool isEfficiency=false, TFile* graphFile=0, int sector=0);
TGraph* reorder(TGraph* graphOrig);
TGraphErrors* reorder(TGraphErrors* graphOrig);
TGraphAsymmErrors* reorder(TGraphAsymmErrors* graphOrig);

void Write(vector<TGraphErrors*> graph1, string title);
void Write(vector<TGraphAsymmErrors*> graph1, string title);
void Write(vector<TGraph2D*> graph1, string title);
void Write(vector<TGraph2DErrors*> graph1, string title);

void WriteTextFile(vector<TGraphErrors*> graph1, string fileName);
void WriteTextFile(vector<TGraphAsymmErrors*> graph1, string fileName);

int histFromData(string histName);

void treeFill(TTree* tree, vector<Run> runs);
void treeRead(TTree* tree, vector<Run> &runs);
