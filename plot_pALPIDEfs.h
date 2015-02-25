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

using namespace std;

class Run {
    int runNumber, irradiation;
    double ithr, idb, vcasn, vaux, vcasp, vreset, BB, readoutDelay, triggerDelay, strobeLength, strobeBLength;
    vector<double> eff, nTr, nTrpA, thr, thrE, noise, noiseE;
    vector<TH1*> clusterSize, residualX, residualY;
    bool noiseRun;
    string chipID;
  public:
    Run(){
      runNumber = -1;
    }
    Run(int r, double casn, double aux, double casp, double reset, double i, double db, vector<double> t, vector<double> tE, vector<double> n, vector<double> nE, double bb, int irr, string id, double rD, double tD, double sL, double sBL, bool nR){
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
      idb = db;
      thr = t;
      thrE = tE;
      noise = n;
      noiseE = nE; 
      noiseRun = nR;
      BB = bb;
      irradiation = irr;
      chipID = id;
    }
    void Set(int r, double casn, double aux, double casp, double reset, double i, double db, vector<double> t, vector<double> tE, vector<double> n, vector<double> nE, double bb, int irr, string id, double rD, double tD, double sL, double sBL, bool nR){
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
      idb = db;
      thr = t;
      thrE = tE;
      noise = n;
      noiseE = nE; 
      noiseRun = nR;
      BB = bb;
      irradiation = irr;
      chipID = id;
    }
    int getRunNumber() {return runNumber;}
    double getVcasn() {return vcasn;}
    double getVaux() {return vaux;}
    double getVcasp() {return vcasp;}
    double getVreset() {return vreset;}
    double getIthr() {return ithr;}
    double getBB() {return BB;}
    string getChipID() {return chipID;}
    double getReadoutDelay() {return readoutDelay;}
    double getTriggerDelay() {return triggerDelay;}
    double getStrobeLength() {return strobeLength;}
    double getStrobeBLength() {return strobeBLength;}
    int getIrradiation() {return irradiation;}
    void setEff(vector<double> e) {eff = e; return;}
    void setnTr(vector<double> tr) {nTr=tr; return;}
    void setnTrpA(vector<double> trp) {nTrpA = trp; return;}
    void setThr(vector<double> t) {thr = t; return;}
    void setThrE(vector<double> tE) {thrE = tE; return;}
    void setNoise(vector<double> n) {noise = n; return;}
    void setNoiseE(vector<double> nE) {noiseE = nE; return;}
    void setClusterSize(vector<TH1*> cS) {clusterSize = cS; return;}
    void setResidualX(vector<TH1*> rX) {residualX = rX; return;}
    void setResidualY(vector<TH1*> rY) {residualY = rY; return;}
    vector<double> getEff() {return eff;}
    vector<double> getnTr() {return nTr;}
    vector<double> getnTrpA() {return nTrpA;}
    vector<double> getThr() {return thr;}
    vector<double> getThrE() {return thrE;}
    vector<double> getNoise() {return noise;}
    vector<double> getNoiseE() {return noiseE;}
    vector<TH1*> getClusterSize() {return clusterSize;}
    vector<TH1*> getResidualX() {return residualX;}
    vector<TH1*> getResidualY() {return residualY;}
    bool isNoise() {return noiseRun;}
    bool equalSettings(Run run2)
    {
      if (vcasn == run2.getVcasn() && vaux == run2.getVaux() && vcasp == run2.getVcasp() && vreset == run2.getVreset() && ithr == run2.getIthr() && readoutDelay == run2.getReadoutDelay() && triggerDelay == run2.getTriggerDelay() && strobeLength == run2.getStrobeLength() && strobeBLength == run2.getStrobeBLength() && BB == run2.getBB() && irradiation == run2.getIrradiation() && chipID == run2.getChipID()) return true;
     else return false;
    }
};

bool Skip(int runNumber);
TH1F* CalculateNoise(int dut, int run);
TH1F* CalculateNoiseFromNoise(int dut, int run);

void WriteGraph(string outputFolder, int dut, int firstRun, int lastRun, string toSkip="", double pointingRes=0, string settingsFileFolder="");
void WriteGraph_old(string outputFolder, int dut, int firstRun, int lastRun, string toSkip="", double pointingRes=0, string settingsFileFolder="");

void compareDifferentGraphs(string files, string hist, const char* yTitle1, const char* yTitle2,const char* xTitle, const char* legend, double y1low=1e-11, double y1high=1e-3, double line1=1e-5, double y2low=92, double y2high=100.1, double line2=99, double xlow=85, double xhigh=195, bool log1=false, bool log2=false);
void compareDifferentGraphs2D(string files, string hist, int sector, bool IthrVcasn, double IthrVcasnValue, bool BB=true, bool irr=true, bool chip=true, bool rate=false);
void compareDifferentGraphs2D(string file, string hist, int sector, bool IthrVcasn, double IthrVcasnValue, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle1, double y1low, double y1high, bool log1, double line1, const char* yTitle2, double y2low, double y2high, double line2, bool log2, bool BB, bool irr, bool chip, bool rate);
void compareDifferentIthrVcasn2D(string file, string hist, int sector=-1);
void compareDifferentIthrVcasn2D(string file, string hist, int sector, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle1, double y1low, double y1high, bool log1, double line1, const char* yTitle2, double y2low, double y2high, double line2, bool log2);
void compareDifferentSectors(string file, string hist, const char* yTitle1, const char* yTitle2,const char* xTitle, const char* legend, double y1low=1e-11, double y1high=1e-3, double line1=1e-5, double y2low=92, double y2high=100.1, double line2=99, double xlow=85, double xhigh=195, bool log1=false, bool log2=false);
void compareDifferentSectors2D(string file, string hist,  bool IthrVcasn=true, double IthrVcasnValue=-1);
void compareDifferentSectors2D(string file, string hist,  bool IthrVcasn, double IthrVcasnValue, const char* yTitle1, const char* yTitle2,const char* xTitle, const char* legend, double y1low, double y1high, double line1, double y2low, double y2high, double line2, double xlow, double xhigh, bool log1, bool log2);
void compareGraphs(string files, string hist, const char* yTitle, bool log=false, bool addChipNumber=false);
void plotOneGraph(string file);

void convertBrokenPixelsMap(string fileName);
void convertThresholdMap(string fileName);

string getLegend(string file, bool addBB=true, bool addIrr=true, bool addChipNumber=false, bool addRate=false);
bool getDefaults(string graph, double& xlow, double& xhigh, double& ylow, double& yhigh, double& line, bool& log, string& xTitle, string& yTitle, string& legend, double& x2low, double& x2high, string& xTitle2);

void AddPoint(TGraph* graph, double x, double y);
void AddPoint(TGraphAsymmErrors* graph, double x, double y, double yErrorLow, double yErrorHigh);
void AddPoint(TGraphErrors* graph, double x, double y, double yError);

void Draw2D(TGraph2D* graph, const char* canvas, string zTitle, string title, bool logZ, double zlow, double zhigh);
void Draw(vector<TGraph*> graph, string canvas, const char* titleX, const char* titleY, vector<string> legendStr, double rangeLow, double rangeHigh, double line, double xLow=90, double xHigh=210, bool log=false, const char* canvasTitle="");
void DrawSame(vector<TGraph*> graph1, vector<TGraph*> graph2, const char* canvas, const char* titleX, const char* titleY, double xlow, double xhigh, double rangeLow, double rangeHigh, const char* legendTitle, bool log=false);
void DrawOverSectors(vector<TGraph*> graph1, double rangeLow1, double rangeHigh1, double line1, const char* titleY1, vector<TGraph*> graph2, double rangeLow2, double rangeHigh2, double line2, const char* titleY2, const char* canvas, const char* legendTitle, vector<string> legendStr, double xlow=80, double xhigh=420,bool log1=false, bool log2=false, const char* titleX="Threshold in electrons", const char* title="");
void DrawOverDifferentGraphs(vector<TGraph*> graph1, double rangeLow1, double rangeHigh1, double line1, const char* titleY1, vector<TGraph*> graph2, double rangeLow2, double rangeHigh2, double line2, const char* titleY2, const char* canvas, const char* legendTitle, vector<string> legendStr, double xlow=80, double xhigh=420,bool log1=false, bool log2=false, const char* titleX="Threshold in electrons",const char* canvasTitle="");

TGraph* Get1DFrom2D(TGraph2D* graph, bool IthrVcasn, double value, bool isEfficiency=false, TFile* graphFile=0, int sector=0);

void Write(vector<TGraphErrors*> graph1, string title);
void Write(vector<TGraphAsymmErrors*> graph1, string title);
void Write(vector<TGraph2D*> graph1, string title);
void Write(vector<TGraph2DErrors*> graph1, string title);

void WriteTextFile(vector<TGraphErrors*> graph1, string fileName);
void WriteTextFile(vector<TGraphAsymmErrors*> graph1, string fileName);

