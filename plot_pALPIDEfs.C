#include "plot_pALPIDEfs.h"

vector<int> toSkipV;
int lowestRunNumber;
int highestRunNumber;
bool Skip(int runNumber)
{
  for (unsigned int i=0;i<toSkipV.size();i++)
    if (runNumber == toSkipV[i]) return true;
  if (runNumber < lowestRunNumber || runNumber > highestRunNumber) return true;
  else return false;
}

void WriteGraph(string outputFolder, int dut, int firstRun, int lastRun, string toSkip, string pointingRes, string noiseFileName, string thresholdFileName, string settingsFileFolder, double BBOverWrite)
{
  double globalBB;
  int globalIrr;
  string globalFileInfo;
  lowestRunNumber = firstRun;
  highestRunNumber = lastRun;
  toSkipV.clear();
  std::istringstream toSkipIs(toSkip);
  string toSkip1;
  while( toSkipIs >> toSkip1)
  {
    size_t intervalPos = toSkip1.find("-");
    if (intervalPos == string::npos)
    {
      int toSkipInt = 0;
      std::istringstream toSkipIs1(toSkip1);
      toSkipIs1 >>toSkipInt;
      toSkipV.push_back(toSkipInt);
    }
    else 
    {
      int lower = 0;
      std::istringstream lowerIs(toSkip1.substr(0,intervalPos));
      lowerIs >> lower;
      int higher = 0;
      std::istringstream higherIs(toSkip1.substr(intervalPos+1)); 
      higherIs >> higher;
      if (lower > higher) cerr << "Wrong order for runs to be skipped" << endl;
      for (int i=lower; i<=higher; i++)
        toSkipV.push_back(i);
    }    
  }
  vector<double> pointingResV;
  std::istringstream pointingResIs(pointingRes);
  string pointingResStr;
  while( pointingResIs >> pointingResStr)
  {
    double pointingResTmp;
    pointingResTmp = atof(pointingResStr.c_str());
    pointingResV.push_back(pointingResTmp);
  }
  if (pointingResV.size() == 0)
    for (int iSector=0; iSector<4; iSector++)
      pointingResV.push_back(0);
  std::map<int,int> runNumberConvert;
  vector<Run> runs;
  Run run;
  int tmp = 0;
  ifstream settingsFile;
  string settingsFileName;
  if (settingsFileFolder.compare("") == 0) settingsFileName = outputFolder + Form("/settings_DUT%d.txt",dut);
  else settingsFileName = settingsFileFolder + Form("/settings_DUT%d.txt",dut);
  settingsFile.open(settingsFileName.c_str());
  if (!settingsFile.is_open())
  {
    cerr << "File at " << settingsFileName << " with settings not found" << endl;
    return;
  }
  string line;
  getline(settingsFile,line);
  int runNumber=0, irr=0, nEvent=0, isData=0;
  double energy=0, ithr=0, idb=0, vcasn=0, vaux=0, vcasp=0, vreset=0, BB=0, readoutDelay=0, triggerDelay=0, strobeLength=0, strobeBLength=0;
  string chipID, rate;
  vector<double> thr(4), thrE(4), noise(4), noiseE(4), eff(4), nTrack(4), nTrackFound(4);
  while (getline(settingsFile, line))
  {
    stringstream strstr(line);
    string word;
    int nWords = 0;
    while (getline(strstr,word, ';'))
    {
      switch(nWords) {
      case 0: 
        runNumber = std::atoi(word.c_str());
        break;
      case 1:
        energy = std::atof(word.c_str());
        break;
      case 2:
        chipID = word;
        break;
      case 3:
        irr = std::atoi(word.c_str());
        break;
      case 4:
        rate = word;
        break;
      case 5:
        BB = std::atof(word.c_str());
        break;
      case 6:
        ithr = std::atof(word.c_str());
        break;
      case 7:
        idb = std::atof(word.c_str());
        break;
      case 8:
        vcasn = std::atof(word.c_str());
        break;
      case 9:
        vaux = std::atof(word.c_str());
        break;
      case 10:
        vcasp = std::atof(word.c_str());
        break;
      case 11:
        vreset = std::atof(word.c_str());
        break;
      case 12:
        for (int iSector=0; iSector<4; iSector++)
        {
          thr[iSector] = std::atof(word.c_str());
          getline(strstr,word, ';');
          thrE[iSector] = std::atof(word.c_str());
          getline(strstr,word, ';');
        }
        for (int iSector=0; iSector<4; iSector++)
        {
          noise[iSector] = std::atof(word.c_str());
          getline(strstr,word, ';');
          noiseE[iSector] = std::atof(word.c_str());
          if (iSector != 3) getline(strstr,word, ';');
        }
        break;
      case 13:
        readoutDelay = std::atof(word.c_str());
        break;
      case 14:
        triggerDelay = std::atof(word.c_str());
        break;
      case 15:
        strobeLength = std::atof(word.c_str());
        break;
      case 16:
        strobeBLength = std::atof(word.c_str());
        break;
      case 17:
        isData = std::atoi(word.c_str());
        break;
      case 18:
        nEvent = std::atoi(word.c_str());
        break;
      case 19:
        for (int iSector=0; iSector<4; iSector++)
        {
          eff[iSector] = std::atof(word.c_str());
          getline(strstr,word, ';');
          nTrack[iSector] = std::atof(word.c_str());
          getline(strstr,word, ';');
          nTrackFound[iSector] = std::atof(word.c_str());
          if (iSector != 3) getline(strstr,word, ';');
        }
        break;
      default:
        break;
      }
      if (Skip(runNumber)) break;
      nWords++;
    }
    if (Skip(runNumber)) continue; 
    runNumberConvert[runNumber] = tmp;
    for (int iSector=0; iSector<4; iSector++) 
    {
      thr[iSector] *= 7.;
      thrE[iSector] *= 7.;
      noise[iSector] *= 7.;
      noiseE[iSector] *= 7.;
    }
    bool isNoise = true;
    if (isData == 1) {isNoise = false;}
    double ithrpA = ithr/51*500; //Converting to pA
    run.Set(runNumber,vcasn,vaux,vcasp,vreset,ithr,ithrpA,idb,thr,thrE,noise,noiseE,BB,irr,chipID,readoutDelay,triggerDelay,strobeLength,strobeBLength,isNoise,nEvent,energy);
    for (int iSector=0; iSector<4; iSector++) eff[iSector] = eff[iSector]*100;
    run.setEff(eff);
    run.setnTr(nTrack);
    run.setnTrpA(nTrackFound);
    run.setPlotFlag(true);
    runs.push_back(run);
    cout << runs[tmp].getRunNumber() << " (" << tmp << ") : " << runs[tmp].getVcasn() << "\t" << runs[tmp].getIthr() << "\t" << runs[tmp].getBB() << "\t" << runs[tmp].isNoise() << endl;
    tmp++;
  }
  int nRun = tmp;
  if (nRun == 0)
  {
    cerr << "No runs from the list of runs you provided was in the setting file" << endl;
    return;
  }
  if ((unsigned int)nRun < lastRun-firstRun+1-toSkipV.size())
    cerr << "Some runs from the list of runs you provided were not in the setting file. Continuing, but you should check what happened to those runs." << endl;
  for (int i=0;i<nRun;i++)
  {
    if (!runs[i].isNoise()) 
      for (int j=0;j<nRun;j++)
      {
        if (i == j) continue;
	if (!runs[j].isNoise()) continue;
        if (runs[i].equalSettings(runs[j]))
        {
          runs[i].setThr(runs[j].getThr());
          runs[i].setThrE(runs[j].getThrE());
          runs[i].setNoise(runs[j].getNoise());
          runs[i].setNoiseE(runs[j].getNoiseE());
          break;
        }
      }
  }
  globalBB = runs[0].getBB();
  if (globalBB == -4) globalBB = BBOverWrite;
  globalIrr = runs[0].getIrradiation();
  globalFileInfo = runs[0].getChipID();
  for (int i=1;i<nRun;i++)
  {
    if (runs[i].getBB() != runs[0].getBB())
    {
      cerr << "Back bias is not the same for all runs, strange but continuing" << endl;
      globalBB = -100;
    }
    if (runs[i].getIrradiation() != runs[0].getIrradiation()) 
    {
      cerr << "Irradiation level is not the same for all runs, strange but continuing" << endl;
      globalIrr = -100;
    }
    if (runs[i].getChipID() != runs[0].getChipID())
    {
      cerr << "Chip ID is not the same for all runs, strange but continuing" << endl;
      globalFileInfo = "";
    }
  }

  settingsFile.close();
  vector<TGraph2D*> efficiencyIthrVcasn(4);
  vector<TGraph2D*> nTrIthrVcasn(4);
  vector<TGraph2D*> nTrpAIthrVcasn(4);
  vector<TGraph2D*> thresholdIthrVcasn(4);
  for (int i=0; i<4; i++)
  {
    efficiencyIthrVcasn[i] = new TGraph2D;
    nTrIthrVcasn[i] = new TGraph2D;
    nTrpAIthrVcasn[i] = new TGraph2D;
    thresholdIthrVcasn[i] = new TGraph2D;
  }
  vector<double> nTr0(4,0), nTrpA0(4,0);
  vector<TH1*> clusterSizeHisto(4);
  vector<TGraph2DErrors*> clusterSizeIthrVcasn(4);
  vector<TH1*> residualXHisto(4);
  vector<TH1*> residualYHisto(4);
  vector<TGraph2DErrors*> residualIthrVcasn(4);
  vector<TGraph2DErrors*> resolutionIthrVcasn(4);
  TH2* fakeHitHistoFromNoise;
  TH1* noiseOccupancyBeforeRemovalFromNoiseHisto;
  TH1* noiseOccupancyAfterRemovalFromNoiseHisto;
  vector<TGraph2DErrors*> noiseOccupancyBeforeRemovalIthrVcasnFromNoise(4);
  vector<TGraph2DErrors*> noiseOccupancyAfterRemovalIthrVcasnFromNoise(4);

  for (int i=0; i<4; i++)
  {
    noiseOccupancyBeforeRemovalIthrVcasnFromNoise[i] = new TGraph2DErrors;
    noiseOccupancyAfterRemovalIthrVcasnFromNoise[i] = new TGraph2DErrors;
    clusterSizeIthrVcasn[i] = new TGraph2DErrors;
    residualIthrVcasn[i] = new TGraph2DErrors;
    resolutionIthrVcasn[i] = new TGraph2DErrors;
  }
  for (int i=0;i<nRun;i++)
  {
    if (runs[i].isNoise()) continue;
    string fileName = outputFolder +  Form("/run%06d/histogram/run%06d-analysis_DUT%d.root",runs[i].getRunNumber(),runs[i].getRunNumber(),dut);
    TFile* histFile = new TFile(fileName.c_str(),"READONLY");
    if (!histFile || histFile->IsZombie())
      continue;
    for (int iSector=0; iSector<4; iSector++)
    {
      clusterSizeHisto[iSector] = (TH1I*)histFile->Get(Form("Analysis/Sector_%d/clusterSizeHisto_%d",iSector,iSector));
      residualXHisto[iSector] = (TH1I*)histFile->Get(Form("Analysis/Sector_%d/residualXPAlpide_30.0_%d",iSector,iSector));
      residualYHisto[iSector] = (TH1I*)histFile->Get(Form("Analysis/Sector_%d/residualYPAlpide_30.0_%d",iSector,iSector));
    }
    runs[i].setClusterSize(clusterSizeHisto);
    runs[i].setResidualX(residualXHisto);
    runs[i].setResidualY(residualYHisto);
  }
  vector<TH1*> clusterSizeHisto2(4);
  vector<TH1*> residualXHisto2(4);
  vector<TH1*> residualYHisto2(4);
  for (int i=0;i<nRun;i++)
  {
    if (runs[i].isNoise()) continue;
    clusterSizeHisto = runs[i].getClusterSize();
    residualXHisto = runs[i].getResidualX();
    residualYHisto = runs[i].getResidualY();
    bool isEmpty = true;
    for (int iSector=0; iSector<4; iSector++)
      if (residualXHisto[iSector]->GetMean() != 0 || residualXHisto[iSector]->GetRMS() != 0 || residualYHisto[iSector]->GetMean() != 0 || residualYHisto[iSector]->GetRMS() != 0) isEmpty = false;
    if (isEmpty) continue;
    for (int iSector=0; iSector<4; iSector++)
    {
      TH1 *residualXHistoTmp = (TH1*)residualXHisto[iSector]->Clone("residualXHistoTmp");
      TH1 *residualYHistoTmp = (TH1*)residualYHisto[iSector]->Clone("residualYHistoTmp");
      residualXHistoTmp->Reset();
      residualYHistoTmp->Reset();
      for (int iBin=1; iBin<residualXHisto[iSector]->GetNbinsX()+1; iBin++)
      {
        if (residualXHisto[iSector]->GetBinCenter(iBin)-residualXHisto[iSector]->GetMean() > -0.3 && residualXHisto[iSector]->GetBinCenter(iBin)-residualXHisto[iSector]->GetMean() < 0.3)
          residualXHistoTmp->SetBinContent(residualXHisto[iSector]->FindBin(residualXHisto[iSector]->GetBinCenter(iBin)-residualXHisto[iSector]->GetMean()),residualXHisto[iSector]->GetBinContent(iBin));
      }
      for (int iBin=1; iBin<residualYHisto[iSector]->GetNbinsX()+1; iBin++)
      {
        if (residualYHisto[iSector]->GetBinCenter(iBin)-residualYHisto[iSector]->GetMean() > -0.3 && residualYHisto[iSector]->GetBinCenter(iBin)-residualYHisto[iSector]->GetMean() < 0.3)
          residualYHistoTmp->SetBinContent(residualYHisto[iSector]->FindBin(residualYHisto[iSector]->GetBinCenter(iBin)-residualYHisto[iSector]->GetMean()),residualYHisto[iSector]->GetBinContent(iBin));
      }
      residualXHisto[iSector] = (TH1*)residualXHistoTmp->Clone();
      residualYHisto[iSector] = (TH1*)residualYHistoTmp->Clone();
    }
    for (int j=i+1;j<nRun;j++)
    {
      if (runs[j].isNoise()) continue;
      if (runs[i].equalSettings(runs[j]))
      {
        runs[j].setPlotFlag(false);
        clusterSizeHisto2 = runs[j].getClusterSize();
        residualXHisto2 = runs[j].getResidualX();
        residualYHisto2 = runs[j].getResidualY();
        for (int iSector=0; iSector<4; iSector++)
        {
          clusterSizeHisto[iSector]->Add(clusterSizeHisto2[iSector]);
          clusterSizeHisto2[iSector]->Reset();

          TH1 *residualXHistoTmp = (TH1*)residualXHisto2[iSector]->Clone("residualXHistoTmp");
          TH1 *residualYHistoTmp = (TH1*)residualYHisto2[iSector]->Clone("residualYHistoTmp");
          residualXHistoTmp->Reset();
          residualYHistoTmp->Reset();
          for (int iBin=1; iBin<residualXHisto2[iSector]->GetNbinsX()+1; iBin++)
          {
            if (residualXHisto2[iSector]->GetBinCenter(iBin)-residualXHisto2[iSector]->GetMean() > -0.3 && residualXHisto2[iSector]->GetBinCenter(iBin)-residualXHisto2[iSector]->GetMean() < 0.3)
              residualXHistoTmp->SetBinContent(residualXHisto2[iSector]->FindBin(residualXHisto2[iSector]->GetBinCenter(iBin)-residualXHisto2[iSector]->GetMean()),residualXHisto2[iSector]->GetBinContent(iBin));
          }
          for (int iBin=1; iBin<residualYHisto2[iSector]->GetNbinsX()+1; iBin++)
          {
            if (residualYHisto2[iSector]->GetBinCenter(iBin)-residualYHisto2[iSector]->GetMean() > -0.3 && residualYHisto2[iSector]->GetBinCenter(iBin)-residualYHisto2[iSector]->GetMean() < 0.3)
              residualYHistoTmp->SetBinContent(residualYHisto2[iSector]->FindBin(residualYHisto2[iSector]->GetBinCenter(iBin)-residualYHisto2[iSector]->GetMean()),residualYHisto2[iSector]->GetBinContent(iBin));
          }
          residualXHisto[iSector]->Add(residualXHistoTmp);
          residualXHisto2[iSector]->Reset();
          residualYHisto[iSector]->Add(residualYHistoTmp);
          residualYHisto2[iSector]->Reset();
        }
        runs[j].setClusterSize(clusterSizeHisto2);
        runs[j].setResidualX(residualXHisto2);
        runs[j].setResidualY(residualYHisto2);
      }
    }
    runs[i].setClusterSize(clusterSizeHisto);
    runs[i].setResidualX(residualXHisto);
    runs[i].setResidualY(residualYHisto);
  }
  for (int i=0;i<nRun;i++)
  {
    if (runs[i].isNoise()) continue;
    vector<double> clusterSizeV, clusterSizeVRMS, residualV, residualVE, resolutionV, resolutionVE;
    for (int iSector=0; iSector<4; iSector++)
    {
      clusterSizeHisto = runs[i].getClusterSize();
      clusterSizeV.push_back(clusterSizeHisto[iSector]->GetMean());
      clusterSizeVRMS.push_back(clusterSizeHisto[iSector]->GetRMS());
      residualXHisto = runs[i].getResidualX();
      Int_t fitStatusX = -1, fitStatusY = -1;
      TFitResultPtr resultX, resultY;
      if (residualXHisto[iSector]->GetEntries() != 0)
      {
        resultX = residualXHisto[iSector]->Fit("gaus","QNOS");
        fitStatusX = resultX;
      }
      residualYHisto = runs[i].getResidualY();
      if (residualYHisto[iSector]->GetEntries() != 0)
      {
        resultY = residualYHisto[iSector]->Fit("gaus","QNOS");
        fitStatusY = resultY;
      }
      double resX = 0, resY = 0, resXE = 0, resYE = 0;
      if (fitStatusX == 0 && fitStatusY == 0)
      {
        resX = sqrt(resultX->Parameter(2)*1000*resultX->Parameter(2)*1000);
        resY = sqrt(resultY->Parameter(2)*1000*resultY->Parameter(2)*1000);
        resXE = resultX->ParError(2);
        resYE = resultY->ParError(2);
      }
      residualV.push_back((resX+resY)/2.);
      residualVE.push_back(sqrt(resXE*resXE+resYE*resYE)*1000);
      if (fitStatusX == 0 && fitStatusY == 0) 
      {
        resX = sqrt(resultX->Parameter(2)*1000*resultX->Parameter(2)*1000-pointingResV[iSector]*pointingResV[iSector]);
        resY = sqrt(resultY->Parameter(2)*1000*resultY->Parameter(2)*1000-pointingResV[iSector]*pointingResV[iSector]);
        resXE = resultX->ParError(2);
        resYE = resultY->ParError(2);
      }
      resolutionV.push_back((resX+resY)/2.);
      resolutionVE.push_back(sqrt(resXE*resXE+resYE*resYE)*1000);
      if (clusterSizeHisto[iSector]->Integral() < 100) continue;
      clusterSizeIthrVcasn[iSector]->SetPoint(clusterSizeIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),clusterSizeHisto[iSector]->GetMean());
      clusterSizeIthrVcasn[iSector]->SetPointError(clusterSizeIthrVcasn[iSector]->GetN()-1,0,0,clusterSizeHisto[iSector]->GetMeanError());
      if (fitStatusX == 0 && fitStatusY == 0)
      {
        resX = sqrt(resultX->Parameter(2)*1000*resultX->Parameter(2)*1000);
        resY = sqrt(resultY->Parameter(2)*1000*resultY->Parameter(2)*1000);
        residualIthrVcasn[iSector]->SetPoint(residualIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),(resX+resY)/2.);
        residualIthrVcasn[iSector]->SetPointError(residualIthrVcasn[iSector]->GetN()-1,0,0,sqrt(resultX->ParError(2)*resultX->ParError(2)+resultY->ParError(2)*resultY->ParError(2))*1000);
        resX = sqrt(resultX->Parameter(2)*1000*resultX->Parameter(2)*1000-pointingResV[iSector]*pointingResV[iSector]);
        resY = sqrt(resultY->Parameter(2)*1000*resultY->Parameter(2)*1000-pointingResV[iSector]*pointingResV[iSector]);
        resolutionIthrVcasn[iSector]->SetPoint(resolutionIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),(resX+resY)/2.);
        resolutionIthrVcasn[iSector]->SetPointError(resolutionIthrVcasn[iSector]->GetN()-1,0,0,sqrt(resultX->ParError(2)*resultX->ParError(2)+resultY->ParError(2)*resultY->ParError(2))*1000);
      }
    }
    if (runs[i].PlotFlag())
    {
      runs[i].setClusterSizeVector(clusterSizeV);
      runs[i].setClusterSizeVectorRMS(clusterSizeVRMS);
      runs[i].setResidualVector(residualV);
      runs[i].setResidualVectorE(residualVE);
      runs[i].setResolutionVector(resolutionV);
      runs[i].setResolutionVectorE(resolutionVE);
    }
  }
  for (int i=0;i<nRun;i++)
  {
    if (runs[i].isNoise()) continue;
    for (int j=i+1;j<nRun;j++)
    {
      if (runs[j].isNoise()) continue;
      if (runs[i].equalSettings(runs[j])) 
      {
        runs[j].setPlotFlag(false);
        vector<double> nTr(4), nTrpA(4);
        for (int iSector=0; iSector<4; iSector++)
        {
          nTr[iSector] = runs[i].getnTr()[iSector]+runs[j].getnTr()[iSector];
          nTrpA[iSector] = runs[i].getnTrpA()[iSector]+runs[j].getnTrpA()[iSector];
        }
        runs[i].setnTr(nTr);
        runs[i].setnTrpA(nTrpA);
        runs[j].setnTr(nTr0);
        runs[j].setnTrpA(nTrpA0);
      }
    }
  }

  for (int i=0;i<nRun;i++)
  {
    vector<double> effV;
    if (runs[i].isNoise()) continue;
    for (int iSector=0; iSector<4; iSector++)
    {
      double nTrd = runs[i].getnTr()[iSector];
      double nTrpAd = runs[i].getnTrpA()[iSector];
      double effd = nTrpAd/nTrd;
      effV.push_back(effd*100);
      if (runs[i].getnTr()[iSector] > 100)
      {
        efficiencyIthrVcasn[iSector]->SetPoint(efficiencyIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),effd*100);
        nTrIthrVcasn[iSector]->SetPoint(nTrIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),nTrd);
        nTrpAIthrVcasn[iSector]->SetPoint(nTrpAIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),nTrpAd);
        if (runs[i].getThr()[iSector] != 0) thresholdIthrVcasn[iSector]->SetPoint(thresholdIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),runs[i].getThr()[iSector]);
      }
    }
    runs[i].setEff(effV);
    effV.clear();
  }

  for (int i=0;i<nRun;i++)
  {
    if (!runs[i].isNoise()) continue;
    string fileName = outputFolder +  Form("/run%06d/histogram/run%06d-noise.root",runs[i].getRunNumber(),runs[i].getRunNumber());
    TFile* histFile = new TFile(fileName.c_str(),"READONLY");
    if (!histFile || histFile->IsZombie())
      continue;
    fakeHitHistoFromNoise = (TH2I*)histFile->Get(Form("noiseMap_%d",dut));

    runs[i].setFakeHitHistoFromNoise(fakeHitHistoFromNoise);
  }    
  TH2* fakeHitHistoFromNoise2;
  vector<TH1F*>noiseOccupancy;
  cerr << "Looking for hotest pixels to remove" << endl; 
  for (int i=0;i<nRun;i++)
  {
    if (!runs[i].isNoise()) continue;
    fakeHitHistoFromNoise = runs[i].getFakeHitHistoFromNoise();
      
    for (int j=i+1;j<nRun;j++)
    {
      if (!runs[j].isNoise()) continue;
      if (runs[i].equalSettings(runs[j]))
      {
        fakeHitHistoFromNoise2 = runs[j].getFakeHitHistoFromNoise();
        fakeHitHistoFromNoise->Add(fakeHitHistoFromNoise2);
        fakeHitHistoFromNoise2->Reset();
        runs[j].setFakeHitHistoFromNoise(fakeHitHistoFromNoise2);
	      
        runs[i].setnEvent(runs[i].getnEvent()+runs[j].getnEvent());
        runs[j].setPlotFlag(false);
      }
    }
    runs[i].setFakeHitHistoFromNoise(fakeHitHistoFromNoise);
    noiseOccupancy = CalculateNoiseFromNoise(fakeHitHistoFromNoise,i,runs);

    noiseOccupancyBeforeRemovalFromNoiseHisto = noiseOccupancy[0];
    noiseOccupancyAfterRemovalFromNoiseHisto = noiseOccupancy[1];
    noiseOccupancy.clear();
    vector<double> noiseOccupancyBeforeRemovalV, noiseOccupancyBeforeRemovalVE, noiseOccupancyAfterRemovalV, noiseOccupancyAfterRemovalVE;
    for (int iSector=0; iSector<4; iSector++)
    {
	  
      if (runs[i].getnEvent() == 0) continue;
      if(!runs[i].PlotFlag()) continue;
      noiseOccupancyBeforeRemovalIthrVcasnFromNoise[iSector]->SetPoint(noiseOccupancyBeforeRemovalIthrVcasnFromNoise[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinContent(iSector+1));
      noiseOccupancyBeforeRemovalIthrVcasnFromNoise[iSector]->SetPointError(noiseOccupancyBeforeRemovalIthrVcasnFromNoise[iSector]->GetN()-1,0,0,noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinError(iSector+1));
      noiseOccupancyAfterRemovalIthrVcasnFromNoise[iSector]->SetPoint(noiseOccupancyAfterRemovalIthrVcasnFromNoise[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),noiseOccupancyAfterRemovalFromNoiseHisto->GetBinContent(iSector+1));
      noiseOccupancyAfterRemovalIthrVcasnFromNoise[iSector]->SetPointError(noiseOccupancyAfterRemovalIthrVcasnFromNoise[iSector]->GetN()-1,0,0,noiseOccupancyAfterRemovalFromNoiseHisto->GetBinError(iSector+1));
      noiseOccupancyBeforeRemovalV.push_back(noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinContent(iSector+1));
      noiseOccupancyBeforeRemovalVE.push_back(noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinError(iSector+1));
      noiseOccupancyAfterRemovalV.push_back(noiseOccupancyAfterRemovalFromNoiseHisto->GetBinContent(iSector+1));
      noiseOccupancyAfterRemovalVE.push_back(noiseOccupancyAfterRemovalFromNoiseHisto->GetBinError(iSector+1));
    }
    runs[i].setNoiseOccupancyBeforeRemovalV(noiseOccupancyBeforeRemovalV);
    runs[i].setNoiseOccupancyBeforeRemovalVE(noiseOccupancyBeforeRemovalVE);
    runs[i].setNoiseOccupancyAfterRemovalV(noiseOccupancyAfterRemovalV);
    runs[i].setNoiseOccupancyAfterRemovalVE(noiseOccupancyAfterRemovalVE);
    noiseOccupancyBeforeRemovalFromNoiseHisto->Delete();
    noiseOccupancyAfterRemovalFromNoiseHisto->Delete();
  }
  cerr << "Finished looking for hotest pixels to remove" << endl; 
  vector<TGraph2DErrors*> noiseOccupancyAfterRemovalIthrVcasnFromLab(4);
  for (int iSector=0; iSector<4; iSector++)
    noiseOccupancyAfterRemovalIthrVcasnFromLab[iSector] = new TGraph2DErrors;
  if (noiseFileName.compare("") !=0)
  {
    TFile* noiseFile = new TFile(noiseFileName.c_str(),"READONLY");
    if (noiseFile && !noiseFile->IsZombie())
    {
      vector<TGraphAsymmErrors*> noiseFromLab(4);
      for (int iSector=0; iSector<4; iSector++)
      {
        string noiseGraphName = "g_noiseVsITHR_" + globalFileInfo + "_TEMP28.0_VBB" + Form("%0.1f_VCASN135_RATE10000_BUSY50_sec%d", globalBB, iSector);
        noiseFromLab[iSector] = (TGraphAsymmErrors*)noiseFile->Get(noiseGraphName.c_str());
        double x=0,y=0;
        for (int i=0; i<noiseFromLab[iSector]->GetN(); i++)
        {
          noiseFromLab[iSector]->GetPoint(i,x,y);
          noiseOccupancyAfterRemovalIthrVcasnFromLab[iSector]->SetPoint(i,x,135.,y);
          noiseOccupancyAfterRemovalIthrVcasnFromLab[iSector]->SetPointError(i,0,0,0);
        }
      }
    }
    
  }
  vector<TGraph2DErrors*> temporalNoiseIthrVcasnFromLab(4);
  vector<TGraph2DErrors*> thresholdIthrVcasnFromLab(4);
  for (int iSector=0; iSector<4; iSector++)
  {
    temporalNoiseIthrVcasnFromLab[iSector] = new TGraph2DErrors;
    thresholdIthrVcasnFromLab[iSector] = new TGraph2DErrors;
  }
  if (thresholdFileName.compare("") !=0)
  {
    TFile* thresholdFile = new TFile(thresholdFileName.c_str(),"READONLY");
    if (thresholdFile && !thresholdFile->IsZombie())
    {
      vector<TGraphAsymmErrors*> temporalNoise(4);
      vector<TGraphAsymmErrors*> threshold(4);
      for (int iSector=0; iSector<4; iSector++)
      {
        string temporalNosieGraphName = "g_thresnoiseVsITHR_" + globalFileInfo + "_TEMP28.0_VBB" + Form("%0.1f_VCASN135_sec%d", globalBB, iSector);
        string thresholdGraphName = "g_thresVsITHR_" + globalFileInfo + "_TEMP28.0_VBB" + Form("%0.1f_VCASN135_sec%d", globalBB, iSector);
        temporalNoise[iSector] = (TGraphAsymmErrors*)thresholdFile->Get(temporalNosieGraphName.c_str());
        threshold[iSector] = (TGraphAsymmErrors*)thresholdFile->Get(thresholdGraphName.c_str());
        double x=0,y=0;
        for (int i=0; i<temporalNoise[iSector]->GetN(); i++)
        {
          temporalNoise[iSector]->GetPoint(i,x,y);
          temporalNoiseIthrVcasnFromLab[iSector]->SetPoint(i,x,135.,y);
          temporalNoiseIthrVcasnFromLab[iSector]->SetPointError(i,0,0,0);
        }
        for (int i=0; i<threshold[iSector]->GetN(); i++)
        {
          threshold[iSector]->GetPoint(i,x,y);
          thresholdIthrVcasnFromLab[iSector]->SetPoint(i,x,135.,y);
          thresholdIthrVcasnFromLab[iSector]->SetPointError(i,0,0,0);
        }
      }
    }
  }
  string outputFileName;
  string BBStr, irrStr, firstRunStr, lastRunStr;
  BBStr = Form("%0.f", globalBB);
  irrStr = Form("%d", globalIrr);
  firstRunStr = Form("%d", firstRun);
  lastRunStr = Form("%d", lastRun);
  outputFileName = outputFolder + "/graphs";
  if (globalFileInfo != "") outputFileName += "_" + globalFileInfo;
  if (globalBB != -100) outputFileName += "_BB" + BBStr + "V";
  if (globalIrr != -100) outputFileName += "_Irr" + irrStr;
  outputFileName += "_" + firstRunStr + "-" + lastRunStr + ".root";
  cerr << outputFileName << endl;
  TFile* outputFile = new TFile(outputFileName.c_str(),"RECREATE");
  TTree *tree = new TTree("tree","Output");
  treeFill(tree, runs);
  tree->Write();
  Write(noiseOccupancyBeforeRemovalIthrVcasnFromNoise, "noiseOccupancyBeforeRemovalFromNoiseIthrVcasn2D");
  Write(noiseOccupancyAfterRemovalIthrVcasnFromNoise, "noiseOccupancyAfterRemovalFromNoiseIthrVcasn2D");
  Write(noiseOccupancyAfterRemovalIthrVcasnFromLab,"noiseOccupancyAfterRemovalFromLabIthrVcasn2D");
  Write(thresholdIthrVcasnFromLab,"thresholdFromLabIthrVcasn2D");
  Write(temporalNoiseIthrVcasnFromLab,"temporalNoiseFromLabIthrVcasn2D");
  Write(efficiencyIthrVcasn,"efficiencyIthrVcasn2D");
  Write(nTrIthrVcasn,"nTrIthrVcasn2D");
  Write(nTrpAIthrVcasn,"nTrpAIthrVcasn2D");
  Write(thresholdIthrVcasn,"thresholdIthrVcasn2D");
  Write(clusterSizeIthrVcasn,"clusterSizeIthrVcasn2D");
  Write(residualIthrVcasn,"residualIthrVcasn2D");
  Write(resolutionIthrVcasn,"resolutionIthrVcasn2D");
  outputFile->Close();
}

void mergeGraphs(string files, string outputFolder)
{
  vector<string> filesV;
  std::istringstream filesIs(files);
  string filesStr;
  while( filesIs >> filesStr)
    filesV.push_back(filesStr);

  vector<TFile*> graphFile(filesV.size());
  for (unsigned int i=0; i<filesV.size(); i++)
    graphFile[i] =  new TFile(filesV[i].c_str(),"READONLY");

  vector<Run> runs;
  vector< vector<Run> > runsToAdd;
  size_t PRFirstPos = filesV[0].find("PR");
  string PRFirst = filesV[0].substr(PRFirstPos+2,4);
  bool averageRes = true;
  for (unsigned int iFile=0; iFile<filesV.size(); iFile++)
  {
    if (!graphFile[iFile] || graphFile[iFile]->IsZombie())
      return;
    size_t PRPos = filesV[iFile].find("PR");
    string PR = filesV[iFile].substr(PRPos+2,4);
    if (PR.compare(PRFirst) != 0)
    {
      cerr << "Pointing resolution is not the same for all the files, cannot average the resolution values like this!" << endl;
      averageRes = false;
    }
    TTree *tree = (TTree*)graphFile[iFile]->Get("tree");
    if (!tree || tree->IsZombie())
      return;
    treeRead(tree, runs);
    runsToAdd.push_back(runs);
    runs.clear();
  }
  vector<Run> mergedRuns;
  double BB = -1;
  int irradiation = -1;
  for (unsigned int iRun=0; iRun<runsToAdd[0].size(); iRun++)
  {
    vector<vector<double> > eff, thr, thrE, noise, noiseE, clusterSize, clusterSizeRMS, residual, resolution, noiseOccupancyBeforeRemoval, noiseOccupancyAfterRemoval;
    bool isNoise = runsToAdd[0][iRun].isNoise();
    eff.push_back(runsToAdd[0][iRun].getEff());
    thr.push_back(runsToAdd[0][iRun].getThr());
    thrE.push_back(runsToAdd[0][iRun].getThrE());
    noise.push_back(runsToAdd[0][iRun].getNoise());
    noiseE.push_back(runsToAdd[0][iRun].getNoiseE());
    clusterSize.push_back(runsToAdd[0][iRun].getClusterSizeV());
    clusterSizeRMS.push_back(runsToAdd[0][iRun].getClusterSizeRMSV());
    residual.push_back(runsToAdd[0][iRun].getResidualV());
    resolution.push_back(runsToAdd[0][iRun].getResolutionV());
    noiseOccupancyBeforeRemoval.push_back(runsToAdd[0][iRun].getNoiseOccupancyBeforeRemoval());
    noiseOccupancyAfterRemoval.push_back(runsToAdd[0][iRun].getNoiseOccupancyAfterRemoval());
    for (unsigned int i=1; i<runsToAdd.size(); i++)
    {
      for (unsigned int jRun=0; jRun<runsToAdd[i].size(); jRun++)
      {
        if (runsToAdd[0][iRun].canBeAveraged(runsToAdd[i][jRun]))
        {
          eff.push_back(runsToAdd[i][jRun].getEff());
          thr.push_back(runsToAdd[i][jRun].getThr());
          thrE.push_back(runsToAdd[i][jRun].getThrE());
          noise.push_back(runsToAdd[i][jRun].getNoise()); 
          noiseE.push_back(runsToAdd[i][jRun].getNoiseE()); 
          clusterSize.push_back(runsToAdd[i][jRun].getClusterSizeV()); 
          clusterSizeRMS.push_back(runsToAdd[i][jRun].getClusterSizeRMSV()); 
          residual.push_back(runsToAdd[i][jRun].getResidualV()); 
          resolution.push_back(runsToAdd[i][jRun].getResolutionV()); 
          noiseOccupancyBeforeRemoval.push_back(runsToAdd[i][jRun].getNoiseOccupancyBeforeRemoval()); 
          noiseOccupancyAfterRemoval.push_back(runsToAdd[i][jRun].getNoiseOccupancyAfterRemoval()); 
          break;
        }
      }
    }
    vector<double> effFinal(12,0), thrFinal(12,0), thrEFinal(12,0), noiseFinal(12,0), noiseEFinal(12,0), clusterSizeFinal(12,0), clusterSizeRMSFinal(12,0), residualFinal(12,0), resolutionFinal(12,0), noiseOccupancyBeforeRemovalFinal(12,0), noiseOccupancyAfterRemovalFinal(12,0);
    vector<double> tmp;
    for (int iSector=0; iSector<4; iSector++)
    {
      if (!isNoise)
      {
        for (unsigned int i=0; i<eff.size(); i++) 
        {
          if (eff[i].size() == 0) continue;
          tmp.push_back(eff[i][iSector]);
        }
        effFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
        effFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
        effFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
        tmp.clear();

        for (unsigned int i=0; i<clusterSize.size(); i++)
        {
          if (clusterSize[i].size() == 0) continue;
          tmp.push_back(clusterSize[i][iSector]);
        }
        clusterSizeFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
        clusterSizeFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
        clusterSizeFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
        tmp.clear();

        for (unsigned int i=0; i<clusterSizeRMS.size(); i++)
        {
          if (clusterSizeRMS[i].size() == 0) continue;
          tmp.push_back(clusterSizeRMS[i][iSector]);
        }
        clusterSizeRMSFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
        clusterSizeRMSFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
        clusterSizeRMSFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
        tmp.clear();

        for (unsigned int i=0; i<residual.size(); i++)
        {
          if (residual[i].size() == 0) continue;
          tmp.push_back(residual[i][iSector]);
        }
        residualFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
        residualFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
        residualFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
        tmp.clear();
      }
      else
      {
        for (unsigned int i=0; i<noiseOccupancyBeforeRemoval.size(); i++)
        {
          if (noiseOccupancyBeforeRemoval[i].size() == 0) continue;
          tmp.push_back(noiseOccupancyBeforeRemoval[i][iSector]);
        }
        noiseOccupancyBeforeRemovalFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
        noiseOccupancyBeforeRemovalFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
        noiseOccupancyBeforeRemovalFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
        tmp.clear();

        for (unsigned int i=0; i<noiseOccupancyAfterRemoval.size(); i++)
        {
          if (noiseOccupancyAfterRemoval[i].size() == 0) continue;
          tmp.push_back(noiseOccupancyAfterRemoval[i][iSector]);
        }
        noiseOccupancyAfterRemovalFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
        noiseOccupancyAfterRemovalFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
        noiseOccupancyAfterRemovalFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
        tmp.clear();
      }
      for (unsigned int i=0; i<resolution.size(); i++)
      {
        if (resolution[i].size() == 0) continue;
        tmp.push_back(resolution[i][iSector]);
      }
      resolutionFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
      resolutionFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
      resolutionFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
      tmp.clear();

      for (unsigned int i=0; i<thr.size(); i++)
      {
        if (thr[i].size() == 0) continue;
        if (thr[i][iSector] != 0) tmp.push_back(thr[i][iSector]);
      }
      if (tmp.size() == 0) tmp.push_back(0);
      thrFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
      thrFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
      thrFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
      tmp.clear();

      for (unsigned int i=0; i<thrE.size(); i++)
      {
        if (thrE[i].size() == 0) continue;
        if (thrE[i][iSector] != 0) tmp.push_back(thrE[i][iSector]);
      }
      if (tmp.size() == 0) tmp.push_back(0);
      thrEFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
      thrEFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
      thrEFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
      tmp.clear();

      for (unsigned int i=0; i<noise.size(); i++)
      {
        if (noise[i].size() == 0) continue;
        if (noise[i][iSector] != 0) tmp.push_back(noise[i][iSector]);
      }
      if (tmp.size() == 0) tmp.push_back(0);
      noiseFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
      noiseFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
      noiseFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
      tmp.clear();

      for (unsigned int i=0; i<noiseE.size(); i++)
      {
        if (noiseE[i].size() == 0) continue;
        if (noiseE[i][iSector] != 0) tmp.push_back(noiseE[i][iSector]);
      }
      if (tmp.size() == 0) tmp.push_back(0);
      noiseEFinal[iSector] = std::accumulate(tmp.begin(), tmp.end(),0.0)/tmp.size();
      noiseEFinal[4+iSector] = *std::min_element(tmp.begin(), tmp.end());
      noiseEFinal[8+iSector] = *std::max_element(tmp.begin(), tmp.end());
      tmp.clear();
    }
    Run run;
    int nEvent, runNumber;
    double ithr, ithrpA, idb, vcasn, vaux, vcasp, vreset, readoutDelay, triggerDelay, strobeLength, strobeBLength, energy;
    vector<double> threshold, thresholdRMS, efficiency, nTrack, nTrackpALPIDE, temporalNoise, temporalNoiseRMS, clusterSizeV, clusterSizeRMSV, residualV, residualE, resolutionV, resolutionE, noiseOccupancyBeforeRemovalV, noiseOccupancyBeforeRemovalE, noiseOccupancyAfterRemovalV, noiseOccupancyAfterRemovalE;
    string chipID;
    runsToAdd[0][iRun].getAllParameters(runNumber, vcasn, vaux, vcasp, vreset, ithr, ithrpA, idb, threshold, thresholdRMS, temporalNoise, temporalNoiseRMS, BB, irradiation, chipID, readoutDelay, triggerDelay, strobeLength, strobeBLength, isNoise, nEvent, energy, efficiency, nTrack, nTrackpALPIDE, clusterSizeV, clusterSizeRMSV, residualV, residualE, resolutionV, resolutionE,  noiseOccupancyBeforeRemovalV, noiseOccupancyBeforeRemovalE, noiseOccupancyAfterRemovalV, noiseOccupancyAfterRemovalE);
    run.Set(0,vcasn, vaux, vcasp, vreset, ithr, ithrpA, idb, thrFinal, thrEFinal, noiseFinal, noiseEFinal, BB, irradiation, "",readoutDelay, triggerDelay, strobeLength, strobeBLength, isNoise, nEvent, energy);
    run.setEff(effFinal);
    run.setClusterSizeVector(clusterSizeFinal);
    run.setClusterSizeVectorRMS(clusterSizeRMSFinal);
    run.setResidualVector(residualFinal);
    run.setResolutionVector(resolutionFinal);
    run.setNoiseOccupancyBeforeRemovalV(noiseOccupancyBeforeRemovalFinal);
    run.setNoiseOccupancyAfterRemovalV(noiseOccupancyAfterRemovalFinal);
    run.setPlotFlag(true);
    mergedRuns.push_back(run);
  }
  string outputFileName = "";
  outputFileName = outputFolder + "/graphs_merged";
  outputFileName += Form("_BB%0.0fV_Irr%d_PR",BB,irradiation);
  if (!averageRes) outputFileName += ".root";
  else outputFileName += PRFirst + ".root";
  cerr << outputFileName << endl;
  TFile* outputFile = new TFile(outputFileName.c_str(),"RECREATE");
  TTree *tree = new TTree("tree","Output");
  treeFill(tree, mergedRuns);
  tree->Write();
  outputFile->Close();
}

void compareDifferentGraphsFromTree(string files, string xName, string hist, int iSector, string canVary, string oneValueName, string oneValue, bool addBB, bool addIrr, bool addChipNumber, bool addRate)
{
  vector<string> filesV;
  std::istringstream filesIs(files);
  string filesStr;
  while( filesIs >> filesStr)
    filesV.push_back(filesStr);

  vector<TFile*> graphFile(filesV.size());
  for (unsigned int i=0; i<filesV.size(); i++)
    graphFile[i] =  new TFile(filesV[i].c_str(),"READONLY");

  vector<string> histV;
  std::istringstream histIs(hist);
  string histStr;
  while( histIs >> histStr)
    histV.push_back(histStr);
  if (histV.size() > 2)
  {
    cerr << "Too many histograms, only 1 or 2 is possible!" << endl;
    return;
  }

  double xlow, xhigh, ylow1, yhigh1, line1, ylow2, yhigh2, line2;
  bool log1, log2;
  string xTitle, yTitle1, yTitle2, legendTitle, legendTitle1, legendTitle2="";
  bool defaultsFineX = getDefaultsOneAxis(xName, xlow, xhigh, line1, log1, xTitle, legendTitle);
  bool defaultsFineY1 = getDefaultsOneAxis(histV[0], ylow1, yhigh1, line1, log1, yTitle1, legendTitle1);
  bool defaultsFineY2 = true;
  if (histV.size() == 2) defaultsFineY2 = getDefaultsOneAxis(histV[1], ylow2, yhigh2, line2, log2, yTitle2, legendTitle2);
  if (!defaultsFineX || !defaultsFineY1 || !defaultsFineY2)
  {
    cerr << "Default labels and axis settings not found" << endl;
    return;
  }
  legendTitle = legendTitle1 + "#color[2]{" + legendTitle2 + "}";
  compareDifferentGraphsFromTree(files, xName, hist, iSector, xlow, xhigh, xTitle, ylow1, yhigh1, line1, log1, yTitle1, ylow2, yhigh2, line2, log2, yTitle2, legendTitle, canVary, oneValueName, oneValue, addBB, addIrr, addChipNumber, addRate);
}

void compareDifferentGraphsFromTree(string files, string xName, string hist, int iSector, double xlow, double xhigh, string xTitle, double ylow1, double yhigh1, double line1, bool log1, string yTitle1, double ylow2, double yhigh2, double line2, bool log2, string yTitle2, string legendTitle, string canVary, string oneValueName, string oneValue, bool addBB, bool addIrr, bool addChipNumber, bool addRate)
{
  static const string namesA[] = {"ithr", "vcasn", "vaux", "idb", "vcasp", "vreset", "BB", "readoutDelay", "triggerDelay", "strobeLength", "strobeBLength", "energy"};
  vector<string> names (namesA, namesA + sizeof(namesA) / sizeof(namesA[0]) );


  bool merged = false;
  if (files.find("merged") != string::npos)
  {
    cout << "Assuming merged runs, errors will indicate the chip to chip fluctuations" << endl; 
    merged = true;
  }
  vector<string> filesV;
  std::istringstream filesIs(files);
  string filesStr;
  while( filesIs >> filesStr)
    filesV.push_back(filesStr);

  vector<TFile*> graphFile(filesV.size());
  for (unsigned int i=0; i<filesV.size(); i++)
    graphFile[i] =  new TFile(filesV[i].c_str(),"READONLY");

  vector<string> histV;
  std::istringstream histIs(hist);
  string histStr;
  while( histIs >> histStr)
    histV.push_back(histStr);
  if (histV.size() > 2)
  {
    cerr << "Too many histograms, only 1 or 2 is possible!" << endl;
    return;
  }
  if (xName.compare(histV[0]) == 0 || (histV.size() == 2 && (xName.compare(histV[1]) == 0 || histV[0].compare(histV[1]) == 0)))
  {
    cerr << "Two of the thee axises are the same, exiting!" << endl;
    return;
  }
  vector<string> oneValueNameV;
  std::istringstream oneValueNameIs(oneValueName);
  string oneValueNameStr;
  while( oneValueNameIs >> oneValueNameStr)
    oneValueNameV.push_back(oneValueNameStr);

  vector<int> oneValueV;
  std::istringstream oneValueIs(oneValue);
  int oneValueI;
  while( oneValueIs >> oneValueI)
    oneValueV.push_back(oneValueI);

  if (oneValueNameV.size() != oneValueV.size())
  {
    cerr << "If you don't allow all parameters to be varied give as many parameter names as values." << endl;
    return;
  }

  bool allCanVary = false;
  vector<string> canVaryV;
  if (canVary.empty()) allCanVary = true;
  else
  {
    std::istringstream canVaryIs(canVary);
    string canVaryStr;
    while( canVaryIs >> canVaryStr)
      canVaryV.push_back(canVaryStr);
  }

  vector<TGraph*> graph1V;
  vector<TGraph*> graph2V;
  vector<string> legendV;
  for (unsigned int iFile=0; iFile<graphFile.size(); iFile++)
  {
    if (!graphFile[iFile] || graphFile[iFile]->IsZombie())
      return;
    TTree* tree = (TTree*)graphFile[iFile]->Get("tree");
    if (!tree->GetBranchStatus(xName.c_str()))
    {
      cerr << "Branch name " << xName << " used as x doesn't exist." << endl;
      return;
    }
    if (!tree->GetBranchStatus(histV[0].c_str()) || (histV.size() == 2 && !tree->GetBranchStatus(histV[1].c_str())))
    {
      if (histV.size() == 2) cerr << "Branch name " << histV[0] << " or " << histV[1]  << " used az y doesn't exist." << endl;
      else cerr << "Branch name " << histV[0] << " used az y doesn't exist." << endl;
      return;
    }
    
    int nEntries = tree->GetBranch(xName.c_str())->GetEntries();
    int nEntriesy1 = tree->GetBranch(histV[0].c_str())->GetEntries();
    int nEntriesy2 = nEntriesy1;
    if (histV.size() == 2) nEntriesy2 = tree->GetBranch(histV[1].c_str())->GetEntries();
    if (nEntries != nEntriesy1 || nEntriesy1 != nEntriesy2)
    {
      cerr << "Problem different number of entries in the two specified branches" << endl;
      return;
    }
    vector<vector<double> > settings;
    vector<string> legends;
    vector<string> varyingNames;
    if (!allCanVary)
    {
      vector<bool> settingsChangeV(names.size(),false);
      vector<double> setting(names.size());
      for (unsigned int iNames=0; iNames<names.size(); iNames++)
        tree->SetBranchAddress(names[iNames].c_str(),&setting[iNames]);
      vector<double> settingPrev(names.size(),-100);
      for (int iEntries=0; iEntries<nEntries; iEntries++)
      {
        tree->GetEntry(iEntries);
        for (unsigned int iNames=0; iNames<names.size(); iNames++)
        {
          if (settingsChangeV[iNames]) continue;
          bool canVaryB = false;
          for (unsigned int iVary=0; iVary<canVaryV.size(); iVary++)
            if (names[iNames].compare(canVaryV[iVary]) == 0)
            {
              canVaryB = true;
              break;
            }
          if (canVaryB) continue;
          if (iEntries!=0 && setting[iNames] != settingPrev[iNames])
          {
            settingsChangeV[iNames] = true;
//            continue;
          }
          settingPrev[iNames] = setting[iNames];
        }
      }
      tree->Clear();
      tree = (TTree*)graphFile[iFile]->Get("tree");
      int nChangedSettings = 0;
      for (unsigned int iNames=0; iNames<names.size(); iNames++)
        if (settingsChangeV[iNames]) nChangedSettings++;
      vector<double> settingsTmp(nChangedSettings);
      int tmp = 0;
      for (unsigned int iNames=0; iNames<names.size(); iNames++)
      {
        if (settingsChangeV[iNames])
        {
          tree->SetBranchAddress(names[iNames].c_str(),&settingsTmp[tmp]);
          tmp++;
          //TODO
          varyingNames.push_back(names[iNames]);
        }
      }
      if (settingsTmp.size() == 0) allCanVary = true;
      if (!allCanVary)
      {
        for (int iEntries=0; iEntries<nEntries; iEntries++)
        {
          tree->GetEntry(iEntries);
          bool hasAppeared = false;
          for (unsigned int iSetting=0; iSetting<settingsTmp.size(); iSetting++)
          {
            for (unsigned int iOneValue=0; iOneValue<oneValueNameV.size(); iOneValue++)
            {
              if (oneValueNameV[iOneValue].compare(varyingNames[iSetting]) == 0 && oneValueV[iOneValue] != settingsTmp[iSetting]) 
              {
                hasAppeared = true;
                break;
              }
            }
            if (hasAppeared) break;
          }
          for (unsigned int iChangedSettings=0; iChangedSettings<settings.size() && !hasAppeared; iChangedSettings++)
          {
            for (unsigned int iSetting=0; iSetting<settingsTmp.size(); iSetting++)
            {
              if (settingsTmp[iSetting] != settings[iChangedSettings][iSetting])
                break;
              else if (iSetting != settingsTmp.size()-1) continue;
              hasAppeared = true;
            }
          }
          if (!hasAppeared) 
            settings.push_back(settingsTmp);
        }
        for (unsigned int iSetting=0; iSetting<settings.size(); iSetting++)
        {
          string legend = "";
          for (unsigned int iChangedSetting=0; iChangedSetting<settings[iSetting].size(); iChangedSetting++)
          {
            bool onlyOneValue = false;
            for (unsigned int iOneValue=0; iOneValue<oneValueNameV.size(); iOneValue++)
              if (varyingNames[iChangedSetting].compare(oneValueNameV[iOneValue]) == 0)
              {
                onlyOneValue = true;
                break;
              }
            if (onlyOneValue) continue;
            if (!legend.empty()) legend += ", ";
            legend += varyingNames[iChangedSetting];
            legend += Form(": %0.f",settings[iSetting][iChangedSetting]);
          }
          legends.push_back(legend);
        }
      }
    }
    int size = 1;
    if (!allCanVary) size = settings.size();
    vector<TGraphAsymmErrors*> graph1(size);
    vector<TGraphAsymmErrors*> graph2(size);
    for (int iSetting=0; iSetting<size; iSetting++)
    {
      graph1[iSetting] = new TGraphAsymmErrors();
      graph2[iSetting] = new TGraphAsymmErrors();
    }
    vector<double>* xP = 0;
    vector<double>* xEP = 0;
    vector<double>* yP1 = 0;
    vector<double>* y1EP = 0;
    vector<double>* yP2 = 0;
    vector<double>* y2EP = 0;
    vector<double>* nTrack = 0;
    vector<double>* nTrackpALPIDE = 0;
    vector<double> x, xE, y1, y1E, y2, y2E;
    double xD=0, xED=0, y1D=0, y1ED=0, y2D=0, y2ED=0;
    bool isNoise;
    tree->Clear();
    tree = (TTree*)graphFile[iFile]->Get("tree");
    tree->SetBranchAddress("isNoise",&isNoise);
    string classNameX = tree->GetBranch(xName.c_str())->GetClassName();
    bool xErrorOk = true, y0ErrorOk = true, y1ErrorOk = true;;
    if (classNameX.find("vector<double>") != string::npos)
    {
      tree->SetBranchAddress(xName.c_str(),&xP);
      string xEName = xName+"E";
      if (tree->GetBranchStatus(xEName.c_str()) && histV[0].compare(xEName) != 0 && (histV.size() < 2 || histV[1].compare(xEName) != 0)) tree->SetBranchAddress(xEName.c_str(),&xEP);
      else
      {
        xEName = xName+"RMS";
        if (tree->GetBranchStatus(xEName.c_str()) && histV[0].compare(xEName) != 0 && (histV.size() < 2 || histV[1].compare(xEName) != 0)) tree->SetBranchAddress(xEName.c_str(),&xEP);
        else xErrorOk = false;
      }
    }
    else if (classNameX.find("string") != string::npos)
      {cerr << "Strings as labels on the axis is not yet implemented" << endl; return;}
    else
      tree->SetBranchAddress(xName.c_str(),&xD);
    string classNameY1 = tree->GetBranch(histV[0].c_str())->GetClassName();
//      tree->SetBranchAddress(histV[0].c_str(),&yP1);
    if (classNameY1.find("vector<double>") != string::npos)
    {
      tree->SetBranchAddress(histV[0].c_str(),&yP1);
      string yEName = histV[0]+"E";
      if (tree->GetBranchStatus(yEName.c_str()) && xName.compare(yEName) != 0 && (histV.size() < 2 || histV[1].compare(yEName) != 0)) tree->SetBranchAddress(yEName.c_str(),&y1EP);
      else
      {
        yEName = histV[0]+"RMS";
        if (tree->GetBranchStatus(yEName.c_str()) && xName.compare(yEName) != 0 && (histV.size() < 2 || histV[1].compare(yEName) != 0)) tree->SetBranchAddress(yEName.c_str(),&y1EP);
        else y0ErrorOk = false;
      }
    }
    else if (classNameY1.find("string") != string::npos)
      {cerr << "Strings as labels on the axis is not yet implemented" << endl; return;}
    else
      tree->SetBranchAddress(histV[0].c_str(),&y1D);
    string classNameY2 = "";
    if (histV.size() == 2)
    {
      classNameY2 = tree->GetBranch(histV[1].c_str())->GetClassName();
      if (classNameY2.find("vector<double>") != string::npos)
      {
        tree->SetBranchAddress(histV[1].c_str(),&yP2);
        string yEName = histV[1]+"E";
        if (tree->GetBranchStatus(yEName.c_str()) && xName.compare(yEName) != 0 &&  histV[0].compare(yEName) != 0) tree->SetBranchAddress(yEName.c_str(),&y2EP);
        else
        {
          yEName = histV[1]+"RMS";
          if (tree->GetBranchStatus(yEName.c_str()) && xName.compare(yEName) != 0 &&  histV[0].compare(yEName) != 0) tree->SetBranchAddress(yEName.c_str(),&y2EP);
          y1ErrorOk = false;
        }
      }
      else if (classNameY2.find("string") != string::npos)
        {cerr << "Strings as labels on the axis is not yet implemented" << endl; return;}
      else
        tree->SetBranchAddress(histV[1].c_str(),&y2D);
    }
    if (histV[0].find("efficiency") != string::npos || (histV.size() == 2 && histV[1].find("efficiency")) != string::npos || xName.find("efficiency") != string::npos)
    {
      if (!merged) tree->SetBranchAddress("nTrack",&nTrack);
      if (!merged) tree->SetBranchAddress("nTrackpALPIDE",&nTrackpALPIDE);
    }
    vector<int> nPoint1(size,0), nPoint2(size,0);
    vector<double> settingsTmp(varyingNames.size());
    for (unsigned int iNames=0; iNames<varyingNames.size(); iNames++)
      tree->SetBranchAddress(varyingNames[iNames].c_str(),&settingsTmp[iNames]);
    bool thresholdFromNoise = true;
    for (int i=0; i<nEntries; i++)
    {
      int index = -1;
      tree->GetEntry(i);
      if (i == 0)  thresholdFromNoise = isNoise;
      if ((histFromData(xName) == 2 || histFromData(histV[0]) == 2 || histFromData(histV[1]) == 2) && ((histFromData(xName) != histFromData(histV[0])) || (histV.size() == 2 && histFromData(xName) != histFromData(histV[1]))))
        thresholdFromNoise = isNoise;
      if (allCanVary) index = 0;
      else
        for (unsigned int iChangingSetting=0; iChangingSetting<settings.size(); iChangingSetting++)
        {
          bool indexFound = true;
          for (unsigned int iSetting=0; iSetting<settingsTmp.size(); iSetting++)
          {
            if (settingsTmp[iSetting] != settings[iChangingSetting][iSetting])
            {
              indexFound = false;
              break;
            }
          }
          if (!indexFound) continue;
          index = iChangingSetting;
          break;
        }
      if (index == -1) continue;
      bool noX = true;
      if ((histFromData(xName) == 2 && thresholdFromNoise == isNoise) || (histFromData(xName) == 1 && !isNoise) || (histFromData(xName) == 0 && isNoise))
      {
        if (classNameX.find("vector<double>") != string::npos)
        {
          x = *xP;
          xD = x[iSector];
          if (!merged && xName.find("efficiency") == string::npos && xName.find("threshold") == string::npos && xName.find("RMS") == string::npos && xErrorOk)
          {
            xE = *xEP;
            xED = xE[iSector];
          }
          if ((xName.find("threshold") != string::npos || xName.find("temporalNoise") != string::npos) && x[iSector] == 0) continue;
        }
        noX = false;
      }
      if (noX) continue;
      if ((histFromData(histV[0]) == 2 && thresholdFromNoise == isNoise) || (histFromData(histV[0]) == 1 && !isNoise) || (histFromData(histV[0]) == 0 && isNoise))
      {
        if (classNameY1.find("vector<double>") != string::npos)
        {
          y1 = *yP1;
          y1D = y1[iSector];
          if (!merged && histV[0].find("efficiency") == string::npos && histV[0].find("RMS") == string::npos && y0ErrorOk)
          {
            y1E = *y1EP;
            y1ED = y1E[iSector];
          }
          if ((histV[0].find("threshold") != string::npos || histV[0].find("temporalNoise") != string::npos) && y1[iSector] == 0) continue;
        }
        graph1[index]->SetPoint(nPoint1[index],xD,y1D);
        if (!merged && histV[0].find("efficiency") != string::npos)
        {
          vector<double> nTr = *nTrack;
          vector<double> nTrpA = *nTrackpALPIDE;
          double mean = (nTrpA[iSector]+1)/(nTr[iSector]+2);
          double sigma = sqrt(((nTrpA[iSector]+1)*(nTrpA[iSector]+2))/((nTr[iSector]+2)*(nTr[iSector]+3))-((nTrpA[iSector]+1)*(nTrpA[iSector]+1))/((nTr[iSector]+2)*(nTr[iSector]+2)));
          graph1[index]->SetPointError(nPoint1[index],xED,xED,y1D-(mean-sigma)*100.,(mean+sigma)*100.-y1D);
        }
        else if (!merged && xName.find("efficiency") != string::npos)
        {
          vector<double> nTr = *nTrack;
          vector<double> nTrpA = *nTrackpALPIDE;
          double mean = (nTrpA[iSector]+1)/(nTr[iSector]+2);
          double sigma = sqrt(((nTrpA[iSector]+1)*(nTrpA[iSector]+2))/((nTr[iSector]+2)*(nTr[iSector]+3))-((nTrpA[iSector]+1)*(nTrpA[iSector]+1))/((nTr[iSector]+2)*(nTr[iSector]+2)));
          graph1[index]->SetPointError(nPoint1[index],xD-(mean-sigma)*100.,(mean+sigma)*100.-xD,y1ED,y1ED);
        }
        else if (merged) 
        {
          if (classNameX.find("vector<double>") != string::npos && classNameY1.find("vector<double>") != string::npos)
          graph1[index]->SetPointError(nPoint1[index],x[iSector]-x[iSector+4],x[iSector+8]-x[iSector],y1[iSector]-y1[iSector+4],y1[iSector+8]-y1[iSector]);
          else if (classNameX.find("vector<double>") != string::npos)
            graph1[index]->SetPointError(nPoint1[index],x[iSector]-x[iSector+4],x[iSector+8]-x[iSector],y1ED,y1ED);
          else if (classNameY1.find("vector<double>")!= string::npos) 
            graph1[index]->SetPointError(nPoint1[index],xED,xED,y1[iSector]-y1[iSector+4],y1[iSector+8]-y1[iSector]);
        }
        else graph1[index]->SetPointError(nPoint1[index],xED,xED,y1ED,y1ED);
        nPoint1[index]++;
      }
      if (histV.size() == 2 && ((histFromData(histV[1]) == 2 && thresholdFromNoise == isNoise) || (histFromData(histV[1]) && !isNoise) || (!histFromData(histV[1]) && isNoise)))
//      if (histV.size() == 2)
      {
        if (classNameY2.find("vector<double>") != string::npos)
        {
          y2 = *yP2;
          y2D = y2[iSector];
          if (!merged && histV[1].find("efficiency") == string::npos && histV[1].find("RMS") == string::npos && y1ErrorOk)
          {
            y2E = *y2EP;
            y2ED = y2E[iSector];
          }
        }
        graph2[index]->SetPoint(nPoint2[index],xD,y2D);
        if (!merged && histV[1].find("efficiency") != string::npos)
        {
          vector<double> nTr = *nTrack;
          vector<double> nTrpA = *nTrackpALPIDE;
          double mean = (nTrpA[iSector]+1)/(nTr[iSector]+2);
          double sigma = sqrt(((nTrpA[iSector]+1)*(nTrpA[iSector]+2))/((nTr[iSector]+2)*(nTr[iSector]+3))-((nTrpA[iSector]+1)*(nTrpA[iSector]+1))/((nTr[iSector]+2)*(nTr[iSector]+2)));
          graph2[index]->SetPointError(nPoint2[index],xED,xED,y2D-(mean-sigma)*100.,(mean+sigma)*100.-y2D);
        }
        else if (!merged && xName.find("efficiency") != string::npos)
        {
          vector<double> nTr = *nTrack;
          vector<double> nTrpA = *nTrackpALPIDE;
          double mean = (nTrpA[iSector]+1)/(nTr[iSector]+2);
          double sigma = sqrt(((nTrpA[iSector]+1)*(nTrpA[iSector]+2))/((nTr[iSector]+2)*(nTr[iSector]+3))-((nTrpA[iSector]+1)*(nTrpA[iSector]+1))/((nTr[iSector]+2)*(nTr[iSector]+2)));
          graph2[index]->SetPointError(nPoint2[index],xD-(mean-sigma)*100.,(mean+sigma)*100.-xD,y2ED,y2ED);
        }
        else if (merged){ //graph2[index]->SetPointError(nPoint2[index],x[iSector]-x[iSector+4],x[iSector+8]-x[iSector],y2[iSector]-y2[iSector+4],y2[iSector+8]-y2[iSector]);
          if (classNameX.find("vector<double>") != string::npos && classNameY2.find("vector<double>") != string::npos)
            graph2[index]->SetPointError(nPoint2[index],x[iSector]-x[iSector+4],x[iSector+8]-x[iSector],y2[iSector]-y2[iSector+4],y2[iSector+8]-y2[iSector]);
          else if (classNameX.find("vector<double>") != string::npos)
            graph2[index]->SetPointError(nPoint2[index],x[iSector]-x[iSector+4],x[iSector+8]-x[iSector],y2ED,y2ED);
          else if (classNameY2.find("vector<double>")!= string::npos) 
            graph2[index]->SetPointError(nPoint2[index],xED,xED,y2[iSector]-y2[iSector+4],y2[iSector+8]-y2[iSector]);
        }
        else graph2[index]->SetPointError(nPoint2[index],xED,xED,y2ED,y2ED);
        nPoint2[index]++;
      }
    }
    for (int iSetting=0; iSetting<size; iSetting++)
    {
      string legend = getLegend(filesV[iFile], addBB, addIrr, addChipNumber, addRate);
      if (legend.compare("      ") != 0 && !allCanVary && legends.size() > 0 && !legends[iSetting].empty()) legend += ", ";
      if (legends.size() > 0) legend += legends[iSetting];
      legendV.push_back(legend);
      graph1[iSetting] = reorder(graph1[iSetting]);
      graph1V.push_back(graph1[iSetting]);
      graph2[iSetting] = reorder(graph2[iSetting]);
      graph2V.push_back(graph2[iSetting]);
    }
  }
  string canvasName = xName + histV[0];
  if (histV.size() == 1) Draw(graph1V, canvasName.c_str(), xTitle.c_str(), yTitle1.c_str(), legendV, ylow1, yhigh1, line1, xlow, xhigh, log1, Form("Sector %d", iSector));
  else if (histV.size() == 2) DrawOverDifferentGraphs(graph1V, ylow1, yhigh1, line1, yTitle1.c_str(), graph2V, ylow2, yhigh2, line2, yTitle2.c_str(), canvasName.c_str(), legendTitle.c_str(), legendV, xlow, xhigh, log1, log2, xTitle.c_str(), Form("Sector %d", iSector));
}

vector<TH1F*> CalculateNoiseFromNoise(TH2* fakeHitHisto, int runNumberIndex, vector<Run> runs) 
{
  int nEvent = runs[runNumberIndex].getnEvent();
  int maxNPixels = 20;
  int noiseWithRemove[4] = {0};
  int removedPixels[4] = {0};

  vector<TH1F*> noiseOccupancy(2);
  for (int x=1; x<=fakeHitHisto->GetNbinsX(); x++)
  {
    int index = -1;
    for (int iSector=0; iSector<4; iSector++)
      if (x>=256*iSector+1 && x<256*(iSector+1)+1)
      {
        index = iSector;
        break;
      }
    if (index == -1) {cerr << "Index is not specifiedfor x " << x << endl; continue;}
    for (int y=1; y<=fakeHitHisto->GetNbinsY(); y++)
      noiseWithRemove[index] += fakeHitHisto->GetBinContent(x,y);
  }
  noiseOccupancy[0] = new TH1F("noiseOccupancyBeforeRemovalHisto","noiseOccupancyBeforeRemovalHisto",4,0,4);

  for (int iSector=0; iSector<4; iSector++)
  {
    noiseOccupancy[0]->SetBinContent(iSector+1,(double)noiseWithRemove[iSector]/nEvent/131072);  
    noiseOccupancy[0]->SetBinError(iSector+1,sqrt((double)noiseWithRemove[iSector])/nEvent/131072);
  }
  int index = -1;
  bool lowEnough[4] = {false};
  for (int iSector=0; iSector<4; iSector++)
    if (!lowEnough[iSector] && noiseWithRemove[iSector] == 0) lowEnough[iSector] = true;
  while (1)
  {
    int bin=-1, x=0, y=0, z=0;
    if (bin == fakeHitHisto->GetMaximumBin()) break;
    bin = fakeHitHisto->GetMaximumBin();
    fakeHitHisto->GetBinXYZ(bin,x,y,z);
    for (int iSector=0; iSector<4; iSector++)
      if (x>=256*iSector+1 && x<256*(iSector+1)+1)
      {
        index = iSector;
        break;
      }
    if (index == -1) {cerr << "Index is not specifiedfor x " << x << endl; continue;}
    if (!lowEnough[index] && removedPixels[index] < maxNPixels && noiseWithRemove[index] != 0)
    {
      noiseWithRemove[index] -= fakeHitHisto->GetBinContent(x,y);
      removedPixels[index]++;
    }
    else if (removedPixels[index] >= maxNPixels || noiseWithRemove[index] == 0)
      lowEnough[index] = true;
    fakeHitHisto->SetBinContent(x,y,0);
    if (lowEnough[0] && lowEnough[1] && lowEnough[2] && lowEnough[3]) break;
    if (noiseWithRemove[index] == 0) lowEnough[index] = true;
  }

  noiseOccupancy[1] = new TH1F("noiseOccupancyAfterRemovalHisto","noiseOccupancyAfterRemovalHisto",4,0,4);     
  
  for (int iSector=0; iSector<4; iSector++)
  {
    noiseOccupancy[1]->SetBinContent(iSector+1,(double)noiseWithRemove[iSector]/nEvent/131072);
    noiseOccupancy[1]->SetBinError(iSector+1,sqrt((double)noiseWithRemove[iSector])/nEvent/131072); 
  }

  return noiseOccupancy;
}

void compareDifferentGraphs2D(string files, string hist, int sector, bool IthrVcasn, double IthrVcasnValue, bool BB, bool irr, bool chip, bool rate)
{
  vector<string> histV;
  std::istringstream histIs(hist);
  string histStr;
  while( histIs >> histStr)
    histV.push_back(histStr);

  bool log1, log2;
  double x1low, x1high, x2low, x2high, y1low, y1high, line1, y2low, y2high, line2;
  string xTitle1, xTitle2, yTitle1, yTitle2, legend1, legend2="";
  bool defaultsFine = false;
  defaultsFine = getDefaults(histV[0], x1low, x1high, y1low, y1high, line1, log1, xTitle1, yTitle1, legend1, x2low, x2high, xTitle2);
  if (histV.size() == 2 && defaultsFine) defaultsFine = getDefaults(histV[1], x1low, x1high, y2low, y2high, line2, log2, xTitle1, yTitle2, legend2, x2low, x2high, xTitle2);
  string legend = legend1 + "#color[2]{" + legend2 +"}";
  if (defaultsFine) compareDifferentGraphs2D(files, hist, sector, IthrVcasn, IthrVcasnValue, xTitle1.c_str(), xTitle2.c_str(), x1low, x1high, x2low, x2high, legend.c_str(), yTitle1.c_str(), y1low, y1high, log1, line1, yTitle2.c_str(), y2low, y2high, line2, log2, BB, irr, chip, rate);
}

void compareDifferentGraphs2D(string files, string hist, int sector, bool IthrVcasn, double IthrVcasnValue, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle1, double y1low, double y1high, bool log1, double line1, const char* yTitle2, double y2low, double y2high, double line2, bool log2, bool BB, bool irr, bool chip, bool rate)
{
  vector<string> filesV;
  std::istringstream filesIs(files);
  string filesStr;
  while( filesIs >> filesStr)
    filesV.push_back(filesStr);

  vector<TFile*> graphFile(filesV.size());
  for (unsigned int i=0; i<filesV.size(); i++)
    graphFile[i] =  new TFile(filesV[i].c_str(),"READONLY");

  vector<string> histV;
  std::istringstream histIs(hist);
  string histStr;
  while( histIs >> histStr)
    histV.push_back(histStr);

  vector<TGraph2DErrors*> graph1V, graph2V;
  vector<TGraph*> graph1V1D, graph2V1D;
  vector<string> legendStr;
  string histname1 = histV[0] + Form("_%d",sector);
  string histname2 = "";
  if (histV.size() == 2) histname2 = histV[1] + Form("_%d",sector); 
  else if (histV.size() > 2)
  {
    cerr << "Give as argument 1 or 2 histograms for plotting." << endl;
    return;
  }
  for (unsigned int i=0; i<graphFile.size(); i++)
  {
    if (!graphFile[i] || graphFile[i]->IsZombie())
      continue;
    graph1V.push_back((TGraph2DErrors*)graphFile[i]->Get(histname1.c_str()));
    if (histname1.compare(Form("efficiencyIthrVcasn2D_%d",sector)) == 0 ) graph1V1D.push_back(Get1DFrom2D(graph1V[i],IthrVcasn,IthrVcasnValue,true,graphFile[i],sector));
    else graph1V1D.push_back(Get1DFrom2D(graph1V[i],IthrVcasn,IthrVcasnValue));
    if (histV.size() == 2) 
    {
      graph2V.push_back((TGraph2DErrors*)graphFile[i]->Get(histname2.c_str()));
      if (histname2.compare(Form("efficiencyIthrVcasn2D_%d",sector)) == 0 ) graph2V1D.push_back(Get1DFrom2D(graph2V[i],IthrVcasn,IthrVcasnValue,true,graphFile[i],sector));
      else graph2V1D.push_back(Get1DFrom2D(graph2V[i],IthrVcasn,IthrVcasnValue));
    }
    legendStr.push_back(getLegend(filesV[i],BB,irr,chip,rate));
  }
  string canvasTitle;
  if (IthrVcasn) canvasTitle= Form("Sector %d, I_{thr} = %.0f", sector,IthrVcasnValue);
  else canvasTitle= Form("Sector %d, V_{casn} = %.0f", sector,IthrVcasnValue);
  if (histV.size() == 1)
  {
    string canvas = histname1;
    if (IthrVcasn) Draw(graph1V1D,canvas,xTitle2,yTitle1,legendStr,y1low,y1high,line1,x2low,x2high,log1,canvasTitle.c_str());
    else Draw(graph1V1D,canvas,xTitle1,yTitle1,legendStr,y1low,y1high,line1,x1low,x1high,log1,canvasTitle.c_str());
  }
  else if (histV.size() == 2)
  {
    string canvas = histname1 + histname2;
    if (IthrVcasn) DrawOverDifferentGraphs(graph1V1D,y1low,y1high,line1,yTitle1,graph2V1D,y2low,y2high,line2,yTitle2,canvas.c_str(),legend,legendStr,x2low,x2high,log1,log2,xTitle2,canvasTitle.c_str());
    else DrawOverDifferentGraphs(graph1V1D,y1low,y1high,line1,yTitle1,graph2V1D,y2low,y2high,line2,yTitle2,canvas.c_str(),legend,legendStr,x1low,x1high,log1,log2,xTitle1,canvasTitle.c_str());
  }
}

void compareOneHistogram(string files, string hist, string sectorStr, bool IthrVcasn, double IthrVcasnValue,int type, bool BB, bool irr, bool chip, bool rate, string comparison)
{
  // Type: 0 - BB, 1 - irradiation, 2 - chip, 3 - rate, 4 - else (has to be specified)
  if (type == 4 && comparison.empty())
  {
    cerr << "Need text for type 4 to include as legend" << endl;
    return;
  } 
  bool log;
  double x1low, x1high, x2low, x2high, ylow, yhigh, line;
  string xTitle1, xTitle2, yTitle, legend1;
  bool defaultsFine = false;
  defaultsFine = getDefaults(hist, x1low, x1high, ylow, yhigh, line, log, xTitle1, yTitle, legend1, x2low, x2high, xTitle2);
  vector<int> sectorV;
  std::istringstream sectorIs(sectorStr);
  int sector;
  while( sectorIs >> sector)
    sectorV.push_back(sector);

  vector<string> filesV;
  std::istringstream filesIs(files);
  string filesStr;
  while( filesIs >> filesStr)
    filesV.push_back(filesStr);

  string legend;
  if (sectorV.size() == 2 && type != 4) legend = (string)Form("Sector %d",sectorV[0]) + (string)Form("  #color[2]{Sector %d}",sectorV[1]);
  else 
  {
    if (filesV.size() < 2)
    {
      cerr << "Too few files, exiting!" << endl;
      return;
    }
    string firstValue, secondValue = "";
    bool BBTmp=false, irrTmp=false, chipIDTmp=false, rateTmp=false;
    switch(type)
    {
      case 0:
        BBTmp = true;
        irrTmp = false;
        chipIDTmp = false;
        rateTmp = false;
        break;
      case 1:
        BBTmp = false;
        irrTmp = true;
        chipIDTmp = false;
        rateTmp = false;
        break;
      case 2:
        BBTmp = false;
        irrTmp = false;
        chipIDTmp = true;
        rateTmp = false;
        break;
      case 3:
        BBTmp = false;
        irrTmp = false;
        chipIDTmp = false;
        rateTmp = true;
        break;
      case 4:
        if (filesV.size() > 2)
        {
          cerr << "Too many different options to compare, exiting!" << endl;
          return;
        }
        else legend = comparison;
        break;
    }
    firstValue = getLegend(filesV[0],BBTmp,irrTmp,chipIDTmp,rateTmp);
    if (type != 4)
    {
      for (unsigned int i=1; i<filesV.size(); i++)
      {
        string tmp = getLegend(filesV[i],BBTmp,irrTmp,chipIDTmp,rateTmp);
        if (firstValue.compare(tmp) == 0 ) continue;
        else if (secondValue.empty()) 
          secondValue = tmp;
        else if (secondValue.compare(tmp) != 0)
        {
          cerr << "Too many different options to compare, exiting!" << endl;
          return;
        }
      }
      if (secondValue.empty())
      {
        cerr << "Too few different files, exiting!" << endl;
        return;
      }
      legend = firstValue.substr(6) + "  #color[2]{" + secondValue.substr(6) +"}";
    }
  }
  
  if (defaultsFine) compareOneHistogram(files, hist, sectorStr, IthrVcasn, IthrVcasnValue, xTitle1.c_str(), xTitle2.c_str(), x1low, x1high, x2low, x2high, legend.c_str(), yTitle.c_str(), ylow, yhigh, log, line, BB, irr, chip, rate, "", type);
}

void compareOneHistogram(string files, string hist, string sectorStr, bool IthrVcasn, double IthrVcasnValue, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle, double ylow, double yhigh, bool log, double line, bool BB, bool irr, bool chip, bool rate, const char* title, int type)
{
  string sectorLegendEntries[4] = {"PMOS reset, 1 #mum spacing (sector 0)", "PMOS reset, 2 #mum spacing (sector 1)", "Diode reset, 2 #mum spacing (sector 2)", "PMOS reset, 4 #mum spacing (sector 3)"};
  vector<string> filesV;
  std::istringstream filesIs(files);
  string filesStr;
  while( filesIs >> filesStr)
    filesV.push_back(filesStr);

  vector<int> sectorV;
  std::istringstream sectorIs(sectorStr);
  int sector;
  while( sectorIs >> sector)
    sectorV.push_back(sector);

  vector<TFile*> graphFile(filesV.size());
  for (unsigned int i=0; i<filesV.size(); i++)
    graphFile[i] =  new TFile(filesV[i].c_str(),"READONLY");

  vector<TGraph2DErrors*> graph1V, graph2V;
  vector<TGraph*> graph1V1D, graph2V1D;
  vector<string> legendStr, legendStr2;
  string histname;
  if (sectorV.size() == 1)
    histname = hist + Form("_%d",sectorV[0]);
//  else
  string secondValue = "";
  string firstValue = getLegend(filesV[0],BB,irr,chip,rate);
  if (sectorV.size() == 1)
  {
    for (unsigned int i=0; i<graphFile.size(); i++)
    {
      if (!graphFile[i] || graphFile[i]->IsZombie())
        continue;
      graph1V.push_back((TGraph2DErrors*)graphFile[i]->Get(histname.c_str()));
      string tmp = getLegend(filesV[i],BB,irr,chip,rate);
      if (firstValue.compare(tmp) == 0)
      {
        if (histname.compare(Form("efficiencyIthrVcasn2D_%d",sectorV[0])) == 0 ) graph1V1D.push_back(Get1DFrom2D(graph1V[i],IthrVcasn,IthrVcasnValue,true,graphFile[i],sectorV[0]));
        else graph1V1D.push_back(Get1DFrom2D(graph1V[i],IthrVcasn,IthrVcasnValue));
        legendStr2.push_back(getLegend(filesV[i],BB,irr,chip,rate));
//        legendStr2.push_back(getLegend(filesV[i],!BB,!irr,chip,rate));
      }
      else if (secondValue.empty() || secondValue.compare(tmp) == 0)
      {
        secondValue = tmp;
        if (histname.compare(Form("efficiencyIthrVcasn2D_%d",sectorV[0])) == 0 ) graph2V1D.push_back(Get1DFrom2D(graph1V[i],IthrVcasn,IthrVcasnValue,true,graphFile[i],sectorV[0]));
        else graph2V1D.push_back(Get1DFrom2D(graph1V[i],IthrVcasn,IthrVcasnValue));
        legendStr.push_back(getLegend(filesV[i],BB,irr,chip,rate));
//        legendStr.push_back(getLegend(filesV[i],!BB,!irr,chip,rate));
      }
      else
      {
        cerr << "Too many different options to compare, exiting!" << endl;
        return;
      }
    }
  }
  else if (sectorV.size() > 2 || (sectorV.size() == 2 && type == 4))
  {
    if (graphFile.size() != 2)
    {
      cerr << "Too many different options to compare, exiting!" << endl;
      return;
    }
    for (unsigned int i=0; i<graphFile.size(); i++)
    {
      if (!graphFile[i] || graphFile[i]->IsZombie())
      {
        cerr << "One of the two files corrupt, exiting" << endl;
        continue;
      }
    }
    for (unsigned int j=0; j<sectorV.size(); j++)
    {
      legendStr.push_back(sectorLegendEntries[sectorV[j]]);
      legendStr2.push_back("      ");
      histname = hist + Form("_%d",sectorV[j]);
      graph1V.push_back((TGraph2DErrors*)graphFile[0]->Get(histname.c_str()));
      graph2V.push_back((TGraph2DErrors*)graphFile[1]->Get(histname.c_str()));
      if (histname.compare(Form("efficiencyIthrVcasn2D_%d",sectorV[j])) == 0) graph1V1D.push_back(Get1DFrom2D(graph1V[j],IthrVcasn,IthrVcasnValue,true,graphFile[0],sectorV[j]));
      else graph1V1D.push_back(Get1DFrom2D(graph1V[j],IthrVcasn,IthrVcasnValue));
      if (histname.compare(Form("efficiencyIthrVcasn2D_%d",sectorV[j])) == 0) graph2V1D.push_back(Get1DFrom2D(graph2V[j],IthrVcasn,IthrVcasnValue,true,graphFile[1],sectorV[j]));
      else graph2V1D.push_back(Get1DFrom2D(graph2V[j],IthrVcasn,IthrVcasnValue));
    }
  }
  else
  {
    for (unsigned int i=0; i<graphFile.size(); i++)
    {
      legendStr.push_back(getLegend(filesV[i],BB,irr,chip,rate));
      legendStr2.push_back("      ");
      if (!graphFile[i] || graphFile[i]->IsZombie())
        continue;
      histname = hist + Form("_%d",sectorV[0]);
      graph1V.push_back((TGraph2DErrors*)graphFile[i]->Get(histname.c_str()));
      if (histname.compare(Form("efficiencyIthrVcasn2D_%d",sectorV[0])) == 0) graph1V1D.push_back(Get1DFrom2D(graph1V[i],IthrVcasn,IthrVcasnValue,true,graphFile[i],sectorV[0]));
      else graph1V1D.push_back(Get1DFrom2D(graph1V[i],IthrVcasn,IthrVcasnValue));
      histname = hist + Form("_%d",sectorV[1]);
      graph2V.push_back((TGraph2DErrors*)graphFile[i]->Get(histname.c_str()));
      if (histname.compare(Form("efficiencyIthrVcasn2D_%d",sectorV[1])) == 0) graph2V1D.push_back(Get1DFrom2D(graph2V[i],IthrVcasn,IthrVcasnValue,true,graphFile[i],sectorV[1]));
      else graph2V1D.push_back(Get1DFrom2D(graph2V[i],IthrVcasn,IthrVcasnValue));
    }
    
  }
  string canvasTitle;
  if (IthrVcasn) canvasTitle= Form("Sector %d, I_{thr} = %.0f", sector,IthrVcasnValue);
  else canvasTitle= Form("Sector %d, V_{casn} = %.0f", sector,IthrVcasnValue);
  string canvas = histname;
//if (IthrVcasn) DrawOverDifferentGraphs(graph1V1D,y1low,y1high,line1,yTitle1,graph2V1D,y2low,y2high,line2,yTitle2,canvas.c_str(),legend,legendStr,x2low,x2high,log1,log2,xTitle2,canvasTitle.c_str());
//else DrawOverDifferentGraphs(graph1V1D,y1low,y1high,line1,yTitle1,graph2V1D,y2low,y2high,line2,yTitle2,canvas.c_str(),legend,legendStr,x1low,x1high,log1,log2,xTitle1,canvasTitle.c_str());
  if (IthrVcasn) DrawSame(graph1V1D, graph2V1D, canvas.c_str(), xTitle2, yTitle, x2low, x2high, ylow, yhigh, legend, legendStr, log, line, legendStr2, title);
  else DrawSame(graph1V1D, graph2V1D, canvas.c_str(), xTitle1, yTitle, x1low, x1high, ylow, yhigh, legend, legendStr, log, line, legendStr2, title);
}

void compareDifferentIthrVcasn2D(string file, string hist, int sector)
{
  vector<string> histV;
  std::istringstream histIs(hist);
  string histStr;
  while( histIs >> histStr)
    histV.push_back(histStr);
  bool log1, log2;
  double x1low, x1high, x2low, x2high, y1low, y1high, line1, y2low, y2high, line2;
  string xTitle1, xTitle2, yTitle1, yTitle2, legend1, legend2="";
  bool defaultsFine = false;
  defaultsFine = getDefaults(histV[0], x1low, x1high, y1low, y1high, line1, log1, xTitle1, yTitle1, legend1, x2low, x2high, xTitle2);
  if (histV.size() == 2) defaultsFine = getDefaults(histV[1], x1low, x1high, y2low, y2high, line2, log2, xTitle1, yTitle2, legend2, x2low, x2high, xTitle2);
  string legend = legend1 + "#color[2]{" + legend2 +"}";
  if (defaultsFine) compareDifferentIthrVcasn2D(file, hist, sector, xTitle1.c_str(), xTitle2.c_str(), x1low, x1high, x2low, x2high, legend.c_str(), yTitle1.c_str(), y1low, y1high, log1, line1, yTitle2.c_str(), y2low, y2high, line2, log2);
}

void compareDifferentIthrVcasn2D(string file, string hist, int sector, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle1, double y1low, double y1high, bool log1, double line1, const char* yTitle2, double y2low, double y2high, double line2, bool log2)
{
  TFile* graphFile = new TFile(file.c_str(),"READONLY");;
  if (!graphFile || graphFile->IsZombie())
    return;

  vector<string> histV;
  std::istringstream histIs(hist);
  string histStr;
  while( histIs >> histStr)
    histV.push_back(histStr);

  int low = 0;
  int high = 3;
  if (sector != -1) 
  {
    low = sector;
    high = sector;
  }
  for (int iSector=low; iSector<=high; iSector++)
  {
    string histname1 = histV[0] + Form("_%d",iSector);
    TGraph2DErrors* graph2D1 = (TGraph2DErrors*)graphFile->Get(histname1.c_str());
    TGraph2DErrors* graph2D2 = 0;
    vector<TGraph*> graphIthr1;
    vector<TGraph*> graphVcasn1;
    vector<TGraph*> graphIthr2;
    vector<TGraph*> graphVcasn2;
    vector<double> X1, Y1, X2, Y2;
//    double* Z = graph2D1->GetZ();
    X1.assign(graph2D1->GetX(), graph2D1->GetX() + graph2D1->GetN());
    std::sort(X1.begin(), X1.end());
    Y1.assign(graph2D1->GetY(), graph2D1->GetY() + graph2D1->GetN());
    std::sort(Y1.begin(), Y1.end());
    vector<string> legendStrX;
    string histname2="";
    if (histV.size() == 2)
    {
      histname2 = histV[1] + Form("_%d",iSector);
      graph2D2 = (TGraph2DErrors*)graphFile->Get(histname2.c_str());
      X2.assign(graph2D2->GetX(), graph2D2->GetX() + graph2D2->GetN());
      std::sort(X2.begin(), X2.end());
      Y2.assign(graph2D2->GetY(), graph2D2->GetY() + graph2D2->GetN());
      std::sort(Y2.begin(), Y2.end());
    }
    vector<string> legendStrY;
    vector<double> usedX;
    vector<double> usedY;
    string canvas2D1 = Form("canvas2D1_%d",iSector) + histV[0];
    for (int i=0; i<graph2D1->GetN(); i++)
    {
      bool isUsedX = false;
      for (unsigned int j=0; j<usedX.size(); j++)
        if (usedX[j] == X1[i]) 
        {
          isUsedX = true;
          break;
        }
      bool isUsedY = false;
      for (unsigned int j=0; j<usedY.size(); j++)
        if (usedY[j] == Y1[i]) 
        {
          isUsedY = true;
          break;
        }
      if (isUsedX && isUsedY) continue;
      else
      {
        if (!isUsedY)
        {
          bool foundBothY = false;
          for (int j=0; histV.size() == 2 && j<graph2D2->GetN(); j++)
            if (Y1[i] == Y2[j]) 
            {
              foundBothY = true;
              break;
            }
          if (histV.size() == 1 || foundBothY)
          {
            std::ostringstream ostr;
            ostr << Y1[i];
            string legendStr = "V_{casn} = " + ostr.str();
            legendStrY.push_back(legendStr);
            usedY.push_back(Y1[i]);
            if (histname1.compare(Form("efficiencyIthrVcasn2D_%d",iSector)) == 0 ) graphIthr1.push_back(Get1DFrom2D(graph2D1,false,Y1[i],true,graphFile,iSector));
            else graphIthr1.push_back(Get1DFrom2D(graph2D1,false,Y1[i]));
          }
        }
        if (!isUsedX)
        {
          bool foundBothX = false;
          for (int j=0; histV.size() == 2 && j<graph2D2->GetN(); j++)
            if (X1[i] == X2[j]) 
            {
              foundBothX = true;
              break;
            }
          if (histV.size() == 1 || foundBothX)
          {
            std::ostringstream ostr;
            ostr << X1[i];
            string legendStr = "I_{thr} = " + ostr.str();
            legendStrX.push_back(legendStr);
            usedX.push_back(X1[i]);
            if (histname1.compare(Form("efficiencyIthrVcasn2D_%d", iSector)) == 0 ) graphVcasn1.push_back(Get1DFrom2D(graph2D1,true,X1[i],true,graphFile,iSector));
            else graphVcasn1.push_back(Get1DFrom2D(graph2D1,true,X1[i]));
          }
        }
      }
    }
    if (usedX.size() > 1 && usedY.size() > 1) Draw2D(graph2D1, canvas2D1.c_str(), yTitle1, Form("Sector %d",iSector), log1, y1low, y1high);

    if (histV.size() == 1) 
    {
      string canvasIthr = histV[0] + Form("_Ithr_Sector_%d",iSector);
      string canvasVcasn = histV[0] + Form("_Vcasn_Sector_%d",iSector);
      string canvasTitle = Form("Sector %d",iSector);
      Draw(graphVcasn1,canvasVcasn,xTitle2,yTitle1,legendStrX,y1low,y1high,line1,x2low,x2high,log1,canvasTitle.c_str());
      Draw(graphIthr1,canvasIthr,xTitle1,yTitle1,legendStrY,y1low,y1high,line1,x1low,x1high,log1,canvasTitle.c_str());
    }
    else if (histV.size() == 2)
    {
      usedX.clear();
      usedY.clear();
      for (int i=0; i<graph2D2->GetN(); i++)
      {
        bool isUsedX = false;
        for (unsigned int j=0; j<usedX.size(); j++)
          if (usedX[j] == X2[i]) 
          {
            isUsedX = true;
            break;
          }
        bool isUsedY = false;
        for (unsigned int j=0; j<usedY.size(); j++)
          if (usedY[j] == Y2[i]) 
          {
            isUsedY = true;
            break;
          }
        if (isUsedX && isUsedY) continue;
        else
        {
          if (!isUsedY)
          {
            bool foundBothY = false;
            for (int j=0; j<graph2D2->GetN(); j++)
              if (Y2[i] == Y1[j]) 
              {
                foundBothY = true;
                break;
              }
            if (foundBothY)
            {
              usedY.push_back(Y2[i]);
              if (histname2.compare(Form("efficiencyIthrVcasn2D_%d",iSector)) == 0 ) graphIthr2.push_back(Get1DFrom2D(graph2D2,false,Y2[i],true,graphFile,iSector));
              else graphIthr2.push_back(Get1DFrom2D(graph2D2,false,Y2[i]));
            }
          }
          if (!isUsedX)
          {
            bool foundBothX = false;
            for (int j=0; j<graph2D2->GetN(); j++)
              if (X2[i] == X1[j]) 
              {
                foundBothX = true;
                break;
              }
            if (foundBothX)
            {
              usedX.push_back(X2[i]);
              if (histname2.compare(Form("efficiencyIthrVcasn2D_%d",iSector)) == 0 ) graphVcasn2.push_back(Get1DFrom2D(graph2D2,true,X2[i],true,graphFile,iSector));
              else graphVcasn2.push_back(Get1DFrom2D(graph2D2,true,X2[i]));
            }
          }
        }
      }
      string canvas2D2 = Form("canvas2D2_%d",iSector) + histV[1];
      if (usedX.size() > 1 && usedY.size() > 1) Draw2D(graph2D2, canvas2D2.c_str(), yTitle2, Form("Sector %d",iSector),log2,y2low,y2high);
      string canvasIthr = histV[0] + histV[1] + Form("_Ithr_Sector %d",iSector);
      string canvasVcasn = histV[0] + histV[1] + Form("_Vcasn_Sector %d",iSector);
      string canvasTitle = Form("Sector %d",iSector);
      DrawOverDifferentGraphs(graphIthr1,y1low,y1high,line1,yTitle1,graphIthr2,y2low,y2high,line2,yTitle2,canvasIthr.c_str(),legend,legendStrY,x1low,x1high,log1,log2,xTitle1,canvasTitle.c_str());
      DrawOverDifferentGraphs(graphVcasn1,y1low,y1high,line1,yTitle1,graphVcasn2,y2low,y2high,line2,yTitle2,canvasVcasn.c_str(),legend,legendStrX,x2low,x2high,log1,log2,xTitle2,canvasTitle.c_str());
    }
    else {cerr << "Too many histrograms, one or two needed." << endl; return;}
  }
}

void Draw2D(TGraph2D* graph, const char* canvas, string zTitle, string title, bool logZ, double zlow, double zhigh)
{
  TCanvas* C = new TCanvas(canvas,"",800,600);
  if (logZ) C->SetLogz();
  graph->SetMarkerColor(1);
  graph->SetLineColor(1);
  graph->SetMarkerSize(1.3);
  graph->SetMarkerStyle(20);
  graph->GetHistogram();
  string titleFinal = title + ";I_{thr} (DAC units);V_{casn} (DAC units);" + zTitle; 
  graph->SetTitle(titleFinal.c_str());
  graph->GetXaxis()->SetTitleOffset(1.3);
  graph->GetYaxis()->SetTitleOffset(1.3);
  graph->GetXaxis()->SetTitleSize(0.05);
  graph->GetYaxis()->SetTitleSize(0.05);
  graph->GetZaxis()->SetTitleSize(0.05);
  graph->SetMinimum(zlow);
  graph->SetMaximum(zhigh); // Afterwards by zooming on the plot color is not adjusted, so if histogram is out of Z range it will stay purple... It can be changed by SetMinimum and SetMaximum again only.
  graph->Draw("TRI2P");
	
}

void compareDifferentSectors2D(string file, string hist, bool IthrVcasn, double IthrVcasnValue)
{
  TFile* graphFile = new TFile(file.c_str(),"READONLY");
  if (!graphFile || graphFile->IsZombie())
    return;
  vector<string> histV;
  std::istringstream histIs(hist);
  string histStr;
  while( histIs >> histStr)
    histV.push_back(histStr);
  bool log1, log2;
  double x1low, x1high, x2low, x2high, y1low, y1high, line1, y2low, y2high, line2;
  string xTitle1, xTitle2, yTitle1, yTitle2, legend1, legend2="";
  bool defaultsFine = false;
  defaultsFine = getDefaults(histV[0], x1low, x1high, y1low, y1high, line1, log1, xTitle1, yTitle1, legend1, x2low, x2high, xTitle2);
  if (histV.size() == 2) defaultsFine = getDefaults(histV[1], x1low, x1high, y2low, y2high, line2, log2, xTitle1, yTitle2, legend2, x2low, x2high, xTitle2);
  string legend = legend1 + "#color[2]{" + legend2 +"}";
  if (defaultsFine) 
  {
    if (IthrVcasnValue == -1)
    {
      vector<double> usedX;
      vector<double> usedY;
      for (int iSector=0; iSector<4; iSector++)
      {
        string histname1 = histV[0] + Form("_%d",iSector);
        TGraph2DErrors* graph2D1 = (TGraph2DErrors*)graphFile->Get(histname1.c_str());
        TGraph2DErrors* graph2D2 = 0;
        double* X1 = graph2D1->GetX();
        double* Y1 = graph2D1->GetY();
        double* X2 = 0;
        double* Y2 = 0;
        if (histV.size() == 1)
          for (int i=0; i<graph2D1->GetN(); i++)
          {
            bool isUsedX = false;
            for (unsigned int j=0; j<usedX.size(); j++)
              if (usedX[j] == X1[i]) 
              {
                isUsedX = true;
                break;
              }
            bool isUsedY = false;
            for (unsigned int j=0; j<usedY.size(); j++)
              if (usedY[j] == Y1[i]) 
              {
                isUsedY = true;
                break;
              }
            if (!isUsedX) 
            {
              compareDifferentSectors2D(file, hist, true, X1[i], yTitle1.c_str(), yTitle2.c_str(), xTitle2.c_str(), legend.c_str(), y1low, y1high, line1, y2low, y2high, line2, x2low, x2high, log1, log2);
              usedX.push_back(X1[i]);
            }
            if (!isUsedY)
            {
              compareDifferentSectors2D(file, hist, false, Y1[i], yTitle1.c_str(), yTitle2.c_str(), xTitle1.c_str(), legend.c_str(), y1low, y1high, line1, y2low, y2high, line2, x1low, x1high, log1, log2);
              usedY.push_back(Y1[i]);
            }
          }
        else if (histV.size() == 2)
        {
          string histname2 = histV[1] + Form("_%d",iSector);
          graph2D2 = (TGraph2DErrors*)graphFile->Get(histname2.c_str());
          X2 = graph2D2->GetX();
          Y2 = graph2D2->GetY();
          for (int i=0; i<graph2D1->GetN(); i++)
          {
            for (int j=0; j<graph2D2->GetN(); j++)
            {
              bool isUsedX = false;
              for (unsigned int k=0; k<usedX.size(); k++)
                if (usedX[k] == X2[j]) 
                {
                  isUsedX = true;
                  break;
                }
              bool isUsedY = false;
              for (unsigned int k=0; k<usedY.size(); k++)
                if (usedY[k] == Y2[j]) 
                {
                  isUsedY = true;
                  break;
                }
              if (X1[i] == X2[j] && !isUsedX)
              {
                compareDifferentSectors2D(file, hist, true, X1[i], yTitle1.c_str(), yTitle2.c_str(), xTitle2.c_str(), legend.c_str(), y1low, y1high, line1, y2low, y2high, line2, x2low, x2high, log1, log2);
                usedX.push_back(X1[i]);
              }         
              if (Y1[i] == Y2[j] && !isUsedY) 
              {
                compareDifferentSectors2D(file, hist, false, Y1[i], yTitle1.c_str(), yTitle2.c_str(), xTitle1.c_str(), legend.c_str(), y1low, y1high, line1, y2low, y2high, line2, x1low, x1high, log1, log2);
                usedY.push_back(Y1[i]);
              }
            }
          }
        }
      } 
    }
    else
    {
      if (IthrVcasn) compareDifferentSectors2D(file, hist, IthrVcasn, IthrVcasnValue, yTitle1.c_str(), yTitle2.c_str(), xTitle2.c_str(), legend.c_str(), y1low, y1high, line1, y2low, y2high, line2, x2low, x2high, log1, log2);
      else compareDifferentSectors2D(file, hist, IthrVcasn, IthrVcasnValue, yTitle1.c_str(), yTitle2.c_str(), xTitle1.c_str(), legend.c_str(), y1low, y1high, line1, y2low, y2high, line2, x1low, x1high, log1, log2);
    }
  }
}

void compareDifferentSectors2D(string file, string hist, bool IthrVcasn, double IthrVcasnValue, const char* yTitle1, const char* yTitle2,const char* xTitle, const char* legend, double y1low, double y1high, double line1, double y2low, double y2high, double line2, double xlow, double xhigh, bool log1, bool log2)
{
  string legendEntries[4] = {"PMOS reset, 1 #mum spacing (sector 0)", "PMOS reset, 2 #mum spacing (sector 1)", "Diode reset, 2 #mum spacing (sector 2)", "PMOS reset, 4 #mum spacing (sector 3)"};

  TFile* graphFile = new TFile(file.c_str(),"READONLY");
  if (!graphFile || graphFile->IsZombie())
    return;

  vector<string> histV;
  std::istringstream histIs(hist);
  string histStr;
  while( histIs >> histStr)
    histV.push_back(histStr);

  vector<TGraph*> graph1V;
  vector<TGraph*> graph2V;
  vector<string> legendStr;
  string title;
  if (IthrVcasn) title = "I_{thr} = ";
  else title = "V_{casn} = ";
  
  title += Form("%0.f", IthrVcasnValue);
  for (int i=0; i<4; i++)
  {
    string histname0 = histV[0] + Form("_%d",i);
    TGraph2D* graph2D1 = (TGraph2DErrors*)graphFile->Get(histname0.c_str());
    if (histname0.compare(Form("efficiencyIthrVcasn2D_%d",i)) == 0) graph1V.push_back(Get1DFrom2D(graph2D1,IthrVcasn,IthrVcasnValue,true,graphFile,i));
    else graph1V.push_back(Get1DFrom2D(graph2D1,IthrVcasn,IthrVcasnValue));
    legendStr.push_back(legendEntries[i]);
    if (histV.size() == 2)
    {
      string histname1 = histV[1] + Form("_%d",i);
      TGraph2D* graph2D2 = (TGraph2DErrors*)graphFile->Get(histname1.c_str());
      if (histname1.compare(Form("efficiencyIthrVcasn2D_%d",i)) == 0) graph2V.push_back(Get1DFrom2D(graph2D2,IthrVcasn,IthrVcasnValue,true,graphFile,i));
      else graph2V.push_back(Get1DFrom2D(graph2D2,IthrVcasn,IthrVcasnValue));
    }
    else if (histV.size() > 2)
    {
      cerr << "Use one or two histograms to be plotted" << endl;
      return;
    }
  }
  if (histV.size() == 1)
  {
    string canvas1 = histV[0] + Form("%0.f", IthrVcasnValue);
    if (IthrVcasn) canvas1 += "Ithr_C";
    else canvas1 += "Vcasn_C";
    Draw(graph1V,canvas1,xTitle,yTitle1,legendStr,y1low,y1high,line1,xlow,xhigh,log1,title.c_str());
  }
  if (histV.size() == 2)
  {
    string canvas2 = histV[0] + histV[1] + Form("%0.f", IthrVcasnValue) + "C";
    DrawOverDifferentGraphs(graph1V,y1low,y1high,line1,yTitle1,graph2V,y2low,y2high,line2,yTitle2,canvas2.c_str(),legend,legendStr, xlow, xhigh, log1,log2,xTitle,title.c_str());
  }
  
}

bool getDefaultsOneAxis(string graph, double& low, double& high, double& line, bool& log, string& title, string& legend)
{
  log = false;
  if (graph.find("noiseOccupancy") != string::npos)
  {
    low = 1e-8;
    high  = 1e-4;
    line = 1e-5;
    log = true;
    title = "Noise occupancy per event per pixel";
    legend += "Noise   ";
  }
  else if (graph.find("efficiency") != string::npos)
  {
    low = 90;
    high  = 100.1;
    line = 99;
    title = "Efficiency (%)";
    legend += "Efficiency   ";
  }
  else if (graph.find("resolution") != string::npos) 
  {
    low = 4;
    high = 8;
    line = 5;
    title = "Resolution (#mum)";
    legend += "Resolution   ";
  }
  else if (graph.find("residual") != string::npos) 
  {
    low = 4;
    high = 8;
    line = 5;
    title = "Residual (#mum)";
    legend += "Residual   ";
  }
  else if (graph.find("clusterSizeRMS") != string::npos) 
  {
    low = 0;
    high = 2;
    line = -100;
    title = "Cluster size RMS (pixels)";
    legend += "Cluster size RMS  ";
  }
  else if (graph.find("clusterSize") != string::npos) 
  {
    low = 0;
    high = 5;
    line = -100;
    title = "Average cluster size (pixels)";
    legend += "Cluster size   ";
  }
  else if (graph.find("temporalNoise")!= string::npos)
  {
    low = 0;
    high = 20;
    line = -100;
    title = "Temperal noise (electrons)";
    legend += "Noise   ";
  }
  else if (graph.find("thresholdRMS")!= string::npos)
  {
    low = 0;
    high = 30;
    line = -100;
    title = "Threshold RMS (electrons)";
    legend += "Threshold RMS  ";
  }
  else if (graph.find("threshold")!= string::npos || graph.find("Thr") != string::npos)
  {
    low = 50;
    high = 400;
    line = -100;
    title = "Threshold (electrons)";
    legend += "Threshold   ";
  }
  else if (graph.find("Ithr") != string::npos || graph.find("ithr") != string::npos)
  {
    if (graph.find("ithrpA") != string::npos)
    {
      low = 0;
      high  = 800;
      line = -100;
      title = "I_{thr} (pA)";
    }
    else
    {
      low = 0;
      high  = 80;
      line = -100;
      title = "I_{thr} (DAC units)";
    }
  }
  else if (graph.find("Vcasn") != string::npos || graph.find("vcasn") != string::npos)
  {
    low = 40;
    high  = 160;
    title = "V_{casn} (DAC units)";
  }
  else if (graph.find("noise") != string::npos)         
  {                                                
    low = 1e-8;                                   
    high  = 1e-4;                                 
    line = 1e-5;                                   
    log = true;                                    
    title = "Noise occupancy per event per pixel";
    legend += "Noise   ";                          
  }                                                
 
  else
  {
    cerr << "Unkown type!" << endl; 
    return false;
  }  
  return true;
}


bool getDefaults(string graph, double& xlow, double& xhigh, double& ylow, double& yhigh, double& line, bool& log, string& xTitle, string& yTitle, string& legend, double& x2low, double& x2high, string& xTitle2)
{
  log = false;
  x2low = 0;
  x2high = 0;
  xTitle2 = "";
  if (graph.find("noise") != string::npos)
  {
    ylow = 1e-8;
    yhigh  = 1e-4;
    line = 1e-5;
    log = true;
    yTitle = "Noise occupancy per event per pixel";
    legend += "Noise   ";
  }
  else if (graph.find("efficiency") != string::npos)
  {
    ylow = 90;
    yhigh  = 100.1;
    line = 99;
    yTitle = "Efficiency (%)";
    legend += "Efficiency   ";
  }
  else if (graph.find("resolution") != string::npos) 
  {
    ylow = 4;
    yhigh = 8;
    line = 5;
    yTitle = "Resolution (#mum)";
    legend += "Resolution   ";
  }
  else if (graph.find("residual") != string::npos) 
  {
    ylow = 4;
    yhigh = 8;
    line = 5;
    yTitle = "Residual (#mum)";
    legend += "Residual   ";
  }
  else if (graph.find("clusterSizeRMS") != string::npos) 
  {
    ylow = 0;
    yhigh = 2;
    line = -100;
    yTitle = "Cluster size RMS (pixels)";
    legend += "Cluster size RMS  ";
  }
  else if (graph.find("clusterSize") != string::npos) 
  {
    ylow = 0;
    yhigh = 5;
    line = -100;
    yTitle = "Average cluster size (pixels)";
    legend += "Cluster size   ";
  }
  else if (graph.find("temporalNoise")!= string::npos)
  {
    ylow = 0;
    yhigh = 20;
    line = -100;
    yTitle = "Temperal noise (electrons)";
    legend += "Noise   ";
  }
  else if (graph.find("threshold")!= string::npos)
  {
    ylow = 50;
    yhigh = 250;
    line = -100;
    yTitle = "Threshold (electrons)";
    legend += "Threshold   ";
  }
  else
  {
    cerr << "Unkown y type!" << endl;
    return false;
  }
  
  if (graph.find("2D") != string::npos)
  {
    xlow = 0;
    xhigh  = 80;
    xTitle = "I_{thr} (DAC units)";
    x2low = 40;
    x2high  = 175;
    xTitle2 = "V_{casn} (DAC units)";
  }
  else if (graph.find("Thr") != string::npos)
  {
    xlow = 70;
    xhigh  = 200;
    xTitle = "Threshold (e)";
  }
  else if (graph.find("Ithr") != string::npos)
  {
    xlow = 0;
    xhigh  = 80;
    xTitle = "I_{thr} (DAC units)";
  }
  else if (graph.find("Vcasn") != string::npos)
  {
    xlow = 40;
    xhigh  = 160;
    xTitle = "V_{casn} (DAC units)";
  }
  else
  {
    cerr << "Unkown x type!" << endl; 
    return false;
  }  
  return true;
}

string getLegend(string file, bool addBB, bool addIrr, bool addChipNumber, bool addRate)
{
  string irradiationLevels[5] = {"Non irradiated","0.25e13 1 MeV n_{eq}/cm^{2}","1e13 1 MeV n_{eq}/cm^{2}","700 krad","1e13 1 MeV n_{eq}/cm^{2} + 700 krad"};
  string rateLevels[4] = {"100-1000 events/spill","4000-4500 events/spill","10000-12000 events/spill","15000-18000 events/spill"};
  int BB;
  int irr;
  int rate;
  size_t irrPos = file.find("Irr");
  string irrStr = file.substr(irrPos+3,1);
  if (irrPos == string::npos || !addIrr) irr = -100;
  else irr = atoi(irrStr.c_str());
  size_t BBPos = file.find("BB");
  string BBStr = file.substr(BBPos+2,1);
  if (BBStr == "-") BBStr = file.substr(BBPos+2,2) + " V";
  else if (BBStr == "0" ) BBStr = file.substr(BBPos+2,1) + " V";
  else BBStr = "-" + file.substr(BBPos+2,1) + " V"; // BB is stored normally as a positive value in the config file, because it is supplied by inverting the cable.
  if (BBPos == string::npos || !addBB) BB = -100;
  else BB = atoi(BBStr.c_str());
  size_t chipPos = file.find("graphs_W");
  string chipStr = "";
  if (chipPos!=string::npos)
  {
    if (file.substr(chipPos+11,1) < 48 || file.substr(chipPos+11,1) > 57)  chipStr = file.substr(chipPos+7,4);
    else chipStr = file.substr(chipPos+7,5);
  }
  size_t ratePos = file.find("Rate");
  string rateStr = "";
  if (ratePos!=string::npos && addRate) 
  {
    rateStr = file.substr(ratePos+4,1);
    rate = atoi(rateStr.c_str());
  }
  else rate = -100;
  string legendEntry = "      ";
  if (BB == -100 && irr == -100 && (addBB || addIrr)) legendEntry = "Varying BB and iradiation";
  else
  {
    if (BB != -100)
    {
      legendEntry += "V_{BB} = " + BBStr;
      if (irr != -100) legendEntry += ", ";
    }
    if (irr != -100) legendEntry += irradiationLevels[irr];
  }
  if ((BB != -100 || irr != -100 ) && chipPos != string::npos && addChipNumber) legendEntry += ", " + chipStr;
  else if (chipPos != string::npos && addChipNumber) legendEntry += chipStr;
  if (ratePos != string::npos && addRate) legendEntry += " " + rateLevels[rate];
  else if (addRate) legendEntry += " Rate unknown";
  return legendEntry;
}

void Draw(vector<TGraph*> graph, string canvas, const char* titleX, const char* titleY, vector<string> legendStr, double rangeLow, double rangeHigh, double line, double xLow, double xHigh, bool log, const char* canvasTitle)
{
  bool drawLegend = true;
  if (graph.size() != legendStr.size()) 
  {
    cerr << "Number of legends doesn't correspond to the number of graphs! Number of legends: " << legendStr.size() << " Number of graphs: " << graph.size() << endl;
    drawLegend = false;
  }
  TCanvas * C = new TCanvas(canvas.c_str(),"",800,600);
  C->SetFillStyle(0);
  C->cd();
  if (log) C->SetLogy();
  int markerColorShift = 0;
  int tmp = 0;
  TLegend * legend = new TLegend(0.1,0.1,0.77,0.3);
  if (drawLegend)
    legend->SetFillColor(0);
  for (unsigned int i=0; i<graph.size(); i++)
  {
    if (graph[i]->GetN() == 0) continue;
    if (tmp+1+markerColorShift == 5 || tmp+1+markerColorShift == 7 || tmp+1+markerColorShift == 10) markerColorShift++;
    graph[i]->GetXaxis()->SetTitle(titleX);
    graph[i]->GetYaxis()->SetTitle(titleY);
    graph[i]->GetXaxis()->SetTitleOffset(0.9);
    graph[i]->GetYaxis()->SetTitleOffset(0.9);
    graph[i]->GetXaxis()->SetTitleSize(0.05);
    graph[i]->GetYaxis()->SetTitleSize(0.05);
    graph[i]->SetTitle(canvasTitle);
    graph[i]->SetFillColor(0);
    graph[i]->GetYaxis()->SetRangeUser(rangeLow,rangeHigh);
    graph[i]->SetLineColor(tmp+1+markerColorShift);
    graph[i]->SetMarkerStyle(tmp+20);
    graph[i]->SetMarkerSize(1.3);
    graph[i]->SetMarkerColor(tmp+1+markerColorShift);
    graph[i]->GetXaxis()->SetLimits(xLow,xHigh);

    graph[i]->Draw(tmp==0?"APL":"SAMEPL");
    if (drawLegend)
      legend->AddEntry(graph[i]->Clone(), legendStr[i].c_str());
    tmp++;
  }
  C->Update();
  TLine *l1=new TLine(C->GetUxmin(),line,C->GetUxmax(),line);
  l1->SetLineColor(1);
  l1->SetLineStyle(2);
  l1->Draw();
  if (drawLegend)
    legend->Draw();
//  string fileName = "./results/" + canvas + ".pdf";
//  C->SaveAs(fileName.c_str());
}

void DrawSame(vector<TGraph*> graph1, vector<TGraph*> graph2, const char* canvas, const char* titleX, const char* titleY, double xlow, double xhigh, double rangeLow, double rangeHigh, const char* legendTitle, vector<string> legendStr, bool log, double line, vector<string> legendStr2, const char* title)
{
  int marker1[7] = {20, 21, 22, 34, 29, 33, 23};
  int marker2[7] = {24, 25, 26, 28, 30, 27, 32};
  bool drawLegend = true;
  if (graph1.size() != legendStr.size() || graph2.size() != legendStr.size()) 
  {
    cerr << "Number of legends doesn't correspond to the number of graphs!" << endl;
    drawLegend = false;
  }
  TCanvas * C = new TCanvas(canvas,"",800,600);
  gROOT->Reset();
  C->cd();
  TPad *pad = new TPad("pad","",0,0,1,1);
  pad->SetFillColor(0);
  if (log) pad->SetLogy();
  pad->Draw();
  pad->cd();
  TH1F *hr = pad->DrawFrame(xlow,rangeLow,xhigh,rangeHigh);
  hr->SetXTitle(titleX);
  hr->SetYTitle(titleY);
  hr->GetYaxis()->SetTitleOffset(1.15);
  hr->GetXaxis()->SetTitleSize(0.045);
  hr->GetYaxis()->SetTitleSize(0.045);
  hr->SetTitle(title);
  pad->GetFrame()->SetFillColor(0);
  pad->GetFrame()->SetBorderSize(12);
  pad->Update();
  TLine *l1=new TLine(pad->GetUxmin(),line,pad->GetUxmax(),line);
  l1->SetLineColor(1);
  l1->SetLineStyle(2);
  l1->Draw();
  for (unsigned int i=0; i<graph1.size(); i++)
  {
    graph1[i]->GetXaxis()->SetTitle(titleX);
    graph1[i]->GetYaxis()->SetTitle(titleY);
    graph1[i]->SetFillColor(0);
    graph1[i]->GetYaxis()->SetRangeUser(rangeLow,rangeHigh);
    graph1[i]->SetLineColor(1);
    graph1[i]->SetMarkerStyle(marker1[i]);
    graph1[i]->SetMarkerSize(1.3);
    graph1[i]->SetMarkerColor(1);

    graph1[i]->DrawClone(i==3?"PL":"SAMEPL");
  }
  for (unsigned int i=0; i<graph2.size(); i++)
  {
    graph2[i]->GetXaxis()->SetTitle(titleX);
    graph2[i]->GetYaxis()->SetTitle(titleY);
    graph2[i]->SetFillColor(0);
    graph2[i]->GetYaxis()->SetRangeUser(rangeLow,rangeHigh);
    graph2[i]->SetLineColor(2);
    graph2[i]->SetMarkerStyle(marker2[i]);
    graph2[i]->SetMarkerSize(1.3);
    graph2[i]->SetMarkerColor(2);

    graph2[i]->DrawClone("SAMEPL");
  }
  TLegend * legend = new TLegend(0.1,0.1,0.77,0.3);
  if (drawLegend)
  {
    legend->SetFillColor(0);
    legend->SetNColumns(2);
    for (unsigned int i=0; i<legendStr.size(); i++)
    {
      legend->AddEntry(graph1[i]->Clone(), legendStr2[i].c_str());
      legend->AddEntry(graph2[i]->Clone(), legendStr[i].c_str());
    }
    legend->SetHeader(legendTitle);
    legend->Draw();
  }
}

void DrawOverDifferentGraphs(vector<TGraph*> graph1, double rangeLow1, double rangeHigh1, double line1, const char* titleY1, vector<TGraph*> graph2, double rangeLow2, double rangeHigh2, double line2, const char* titleY2, const char* canvas, const char* legendTitle, vector<string> legendStr, double xlow, double xhigh, bool log1, bool log2, const char* titleX, const char* canvasTitle)
{
  int marker1[7] = {20, 21, 22, 34, 29, 33, 23};
  int marker2[7] = {24, 25, 26, 28, 30, 27, 32};
  bool drawLegend = true;
  if (graph1.size() != legendStr.size() || graph2.size() != legendStr.size()) 
  {
    cerr << "Number of legends doesn't correspond to the number of graphs!" << endl;
    drawLegend = false;
  }
  TCanvas * C = new TCanvas(canvas,"",800,600);
  C->SetFillStyle(0);
  C->cd();
  TPad *pad = new TPad("pad","",0,0,1,1);
  pad->SetFillColor(0);
  pad->SetFillStyle(0);
  if (log1) pad->SetLogy();
  pad->Draw();
  pad->cd();
  TH1F *hr = pad->DrawFrame(xlow,rangeLow1,xhigh,rangeHigh1);
  hr->GetYaxis()->SetTitleOffset(1.15);
  hr->SetXTitle(titleX);
  hr->SetYTitle(titleY1);
  hr->GetXaxis()->SetTitleSize(0.045);
  hr->GetYaxis()->SetTitleSize(0.045);
  hr->SetTitle(canvasTitle);
  pad->GetFrame()->SetFillColor(0);
  pad->GetFrame()->SetBorderSize(12);
  int tmp = 0;

  vector<unsigned int> skipped;  
  for (unsigned int i=0; i<graph1.size(); i++)
    if (graph1[i]->GetN() == 0)
      skipped.push_back(i);
  for (unsigned int i=0; i<graph2.size(); i++)
    if (graph2[i]->GetN() == 0)
      skipped.push_back(i);

  for (unsigned int i=0; i<graph1.size(); i++)
  {
    bool toSkip = false;
    for (unsigned int iSkipped=0; iSkipped<skipped.size(); iSkipped++)
      if (skipped[iSkipped] == i)
      {
        toSkip = true;
        break;
      }
    if (toSkip) continue;
    graph1[i]->GetYaxis()->CenterLabels();
    graph1[i]->SetFillColor(0);
    graph1[i]->SetMarkerStyle(marker1[tmp]);
    graph1[i]->SetLineColor(1);
    graph1[i]->SetMarkerSize(1.3);
    graph1[i]->SetMarkerColor(1);
    graph1[i]->DrawClone(tmp==0?"PL":"SAMEPL");
    tmp++;
  }
  pad->Update();
  TLine *l1=new TLine(pad->GetUxmin(),line1,pad->GetUxmax(),line1);
  l1->SetLineColor(1);
  l1->SetLineStyle(2);
  l1->Draw();
  C->cd();
  TPad *overlay = new TPad("overlay","",0,0,1,1);
  overlay->SetFillStyle(4000);
  overlay->SetFillColor(0);
  overlay->SetFrameFillStyle(4000);
  if (log2) overlay->SetLogy();
  overlay->Draw();
  overlay->cd();
  Double_t xmin = pad->GetUxmin();
  Double_t ymin = rangeLow2;
  Double_t xmax = pad->GetUxmax();
  Double_t ymax = rangeHigh2;
  TH1F *hframe = overlay->DrawFrame(xmin,ymin,xmax,ymax);
  hframe->GetXaxis()->SetLabelOffset(99);
  hframe->GetYaxis()->SetLabelOffset(99);
  hframe->GetYaxis()->SetTickLength(0);
  tmp = 0;
  for (unsigned int i=0; i<graph2.size(); i++)
  {
    bool toSkip = false;
    for (unsigned int iSkipped=0; iSkipped<skipped.size(); iSkipped++)
      if (skipped[iSkipped] == i)
      {
        toSkip = true;
        break;
      }
    if (toSkip) continue;
    graph2[i]->SetFillColor(0);
    graph2[i]->SetMarkerStyle(i+20);
    graph2[i]->SetLineColor(2);
    graph2[i]->SetMarkerStyle(marker2[tmp]);
    graph2[i]->SetMarkerSize(1.3);
    graph2[i]->SetMarkerColor(2);
    graph2[i]->DrawClone(tmp==0?"PL":"SAMEPL");
    tmp++;
  }
  pad->Update();
  TLine *l2=new TLine(pad->GetUxmin(),line2,pad->GetUxmax(),line2);
  l2->SetLineColor(2);
  l2->SetLineStyle(2);
  l2->Draw();
  TGaxis *axis;
  if (!log2) axis = new TGaxis(xmax,ymin,xmax, ymax,ymin,ymax,510,"+L");
  else axis = new TGaxis(xmax,ymin,xmax, ymax,ymin,ymax,510,"+LG");
  axis->SetTitle(titleY2);
  axis->Draw();
  axis->SetLabelFont(42);
  axis->SetLabelSize(0.035);
  axis->SetTitleFont(42);
  axis->SetTitleOffset(1.1);
  axis->SetTitleColor(2);
  axis->SetLineColor(2);
  axis->SetLabelColor(2);
  axis->SetTitleSize(0.045);
  TLegend * legend = new TLegend(0.1,0.1,0.77,0.3);
  if (drawLegend)
  {
    legend->SetFillColor(0);
    legend->SetNColumns(2);
    for (unsigned int i=0; i<legendStr.size(); i++)
    {
      bool toSkip = false;
      for (unsigned int iSkipped=0; iSkipped<skipped.size(); iSkipped++)
        if (skipped[iSkipped] == i)
        {
          toSkip = true;
          break;
        }
      if (toSkip) continue;
      legend->AddEntry(graph1[i]->Clone(), "      ");
      legend->AddEntry(graph2[i]->Clone(), legendStr[i].c_str());
    }
    legend->SetHeader(legendTitle);
    legend->Draw();
  }
}

TGraph* Get1DFrom2D(TGraph2D* graph, bool IthrVcasn, double value, bool isEfficiency, TFile* graphFile, int sector)
{
  TGraphErrors* graph1D = new TGraphErrors;
  TGraphAsymmErrors* graph1DEff = new TGraphAsymmErrors;
  TGraph2DErrors* graphTr=0;
  TGraph2DErrors* graphTrFound=0;
  double* ZTr=0;
  double* ZTrFound=0;
  if (isEfficiency)
  {
    graphTr = (TGraph2DErrors*)graphFile->Get(Form("nTrIthrVcasn2D_%d",sector));
    graphTrFound = (TGraph2DErrors*)graphFile->Get(Form("nTrpAIthrVcasn2D_%d",sector));
    ZTr = graphTr->GetZ();
    ZTrFound = graphTrFound->GetZ();
  }
  double* X = graph->GetX();
  double* Y = graph->GetY();
  double* Z = graph->GetZ();
  double* errorZ = graph->GetEZ();
  double* fixed;  
  double* varying;  
  if (IthrVcasn) 
  {
    fixed = X; // Looking for Ithr=value
    varying = Y;
  }
  else
  {
    fixed = Y; // Looking for Vcasn=value
    varying = X;
  }
  int tmp = 0;
  for (int i=0; i<graph->GetN(); i++)
  {
    if (fixed[i] == value)
    {
      if (!isEfficiency) 
      {
        graph1D->SetPoint(tmp,varying[i],Z[i]);
        if (errorZ != NULL)
          graph1D->SetPointError(tmp,0,errorZ[i]);
      }
      else 
      {
        double mean = (ZTrFound[i]+1)/(ZTr[i]+2);
        double sigma = sqrt(((ZTrFound[i]+1)*(ZTrFound[i]+2))/((ZTr[i]+2)*(ZTr[i]+3))-((ZTrFound[i]+1)*(ZTrFound[i]+1))/((ZTr[i]+2)*(ZTr[i]+2)));
        graph1DEff->SetPoint(tmp,varying[i],Z[i]);
        graph1DEff->SetPointError(tmp,0,0,Z[i]-(mean-sigma)*100.,(mean+sigma)*100.-Z[i]);
      }
      tmp++;
    }
  }
  TGraphErrors* graphFinal = new TGraphErrors(graph1D->GetN());
  TGraphAsymmErrors* graphFinalEff = new TGraphAsymmErrors(graph1DEff->GetN());
  int nPoint = graph1D->GetN();
  for (int iPoint=0; iPoint<nPoint && !isEfficiency; iPoint++)
  {
    int minIndex = TMath::LocMin(graph1D->GetN(),graph1D->GetX());
    double x, y;
    graph1D->GetPoint(minIndex,x,y);
    graphFinal->SetPoint(iPoint,x,y);
    graphFinal->SetPointError(iPoint,graph1D->GetErrorX(minIndex),graph1D->GetErrorY(minIndex));
    graph1D->RemovePoint(minIndex);
  }
  nPoint = graph1DEff->GetN();
  for (int iPoint=0; iPoint<nPoint && isEfficiency; iPoint++)
  {
    int minIndex = TMath::LocMin(graph1DEff->GetN(),graph1DEff->GetX());
    double x, y;
    graph1DEff->GetPoint(minIndex,x,y);
    graphFinalEff->SetPoint(iPoint,x,y);
    graphFinalEff->SetPointError(iPoint,graph1DEff->GetErrorXlow(minIndex),graph1DEff->GetErrorXhigh(minIndex),graph1DEff->GetErrorYlow(minIndex),graph1DEff->GetErrorYhigh(minIndex));
    graph1DEff->RemovePoint(minIndex);
  }
  if (!isEfficiency) return graphFinal;
  else return graphFinalEff;
}

TGraph* reorder(TGraph* graphOrig)
{
  TGraph* graph = new TGraph(graphOrig->GetN());
  int nPoint = graphOrig->GetN();
  for (int iPoint=0; iPoint<nPoint; iPoint++)
  {
    int minIndex = TMath::LocMin(graphOrig->GetN(),graphOrig->GetX());
    double x, y;
    graphOrig->GetPoint(minIndex,x,y);
    graph->SetPoint(iPoint,x,y);
    graphOrig->RemovePoint(minIndex);
  }
  return graph;
}

TGraphErrors* reorder(TGraphErrors* graphOrig)
{
  TGraphErrors* graph = new TGraphErrors(graphOrig->GetN());
  int nPoint = graphOrig->GetN();
  for (int iPoint=0; iPoint<nPoint; iPoint++)
  {
    int minIndex = TMath::LocMin(graphOrig->GetN(),graphOrig->GetX());
    double x, y;
    graphOrig->GetPoint(minIndex,x,y);
    graph->SetPoint(iPoint,x,y);
    graph->SetPointError(iPoint,graphOrig->GetErrorX(minIndex),graphOrig->GetErrorY(minIndex));
    graphOrig->RemovePoint(minIndex);
  }
  return graph;
}

TGraphAsymmErrors* reorder(TGraphAsymmErrors* graphOrig)
{
  TGraphAsymmErrors* graph = new TGraphAsymmErrors(graphOrig->GetN());
  int nPoint = graphOrig->GetN();
  for (int iPoint=0; iPoint<nPoint; iPoint++)
  {
    int minIndex = TMath::LocMin(graphOrig->GetN(),graphOrig->GetX());
    double x, y;
    graphOrig->GetPoint(minIndex,x,y);
    graph->SetPoint(iPoint,x,y);
    graph->SetPointError(iPoint,graphOrig->GetErrorXlow(minIndex),graphOrig->GetErrorXhigh(minIndex),graphOrig->GetErrorYlow(minIndex),graphOrig->GetErrorYhigh(minIndex));
    graphOrig->RemovePoint(minIndex);
  }
  return graph;
}

void Write(vector<TGraphErrors*> graph1, string title)
{
  for (int i=0; i<4; i++)
  {
    TGraphErrors* graph = new TGraphErrors(graph1[i]->GetN());
    int nPoint = graph1[i]->GetN();
    for (int iPoint=0; iPoint<nPoint; iPoint++)
    {
      int minIndex = TMath::LocMin(graph1[i]->GetN(),graph1[i]->GetX());
      double x, y;  //TMath::MinElement(graph1[i]->GetN(),graph1[i]->GetX());
      graph1[i]->GetPoint(minIndex,x,y);
      graph->SetPoint(iPoint,x,y);
      graph->SetPointError(iPoint,graph1[i]->GetErrorX(minIndex),graph1[i]->GetErrorY(minIndex));
      graph1[i]->RemovePoint(minIndex);
    }
    string titleFinal = title + Form("_%d",i);
    graph->SetTitle(titleFinal.c_str());
    graph->SetFillColor(0);
    graph->SetMarkerStyle(i+20);
    graph->SetLineColor(i+1);
    graph->SetMarkerSize(1.3);
    graph->SetMarkerColor(i+1);
    graph->Write(titleFinal.c_str());
    graph1[i]=graph;
  }
}

void Write(vector<TGraphAsymmErrors*> graph1, string title)
{
  for (int i=0; i<4; i++)
  {
    TGraphAsymmErrors* graph = new TGraphAsymmErrors(graph1[i]->GetN());
    int nPoint = graph1[i]->GetN();
    for (int iPoint=0; iPoint<nPoint; iPoint++)
    {
      int minIndex = TMath::LocMin(graph1[i]->GetN(),graph1[i]->GetX());
      double x, y;  //TMath::MinElement(graph1[i]->GetN(),graph1[i]->GetX());
      graph1[i]->GetPoint(minIndex,x,y);
      graph->SetPoint(iPoint,x,y);
      graph->SetPointError(iPoint,graph1[i]->GetErrorX(minIndex),graph1[i]->GetErrorX(minIndex),graph1[i]->GetErrorYlow(minIndex),graph1[i]->GetErrorYhigh(minIndex));
      graph1[i]->RemovePoint(minIndex);
    }
    string titleFinal = title + Form("_%d",i);
    graph->SetTitle(titleFinal.c_str());
    graph->SetFillColor(0);
    graph->SetMarkerStyle(i+20);
    graph->SetLineColor(i+1);
    graph->SetMarkerSize(1.3);
    graph->SetMarkerColor(i+1);
    graph->Write(titleFinal.c_str());
    graph1[i]=graph;
  }
}

void Write(vector<TGraph2D*> graph, string title)
{
  for (int i=0; i<4; i++)
  {
    string titleFinal = title + Form("_%d",i);
    graph[i]->SetTitle(titleFinal.c_str());
    graph[i]->SetFillColor(0);
    graph[i]->SetMarkerStyle(i+20);
    graph[i]->SetLineColor(i+1);
    graph[i]->SetMarkerSize(1.3);
    graph[i]->SetMarkerColor(i+1);
    graph[i]->Write(titleFinal.c_str());
  }
}

void Write(vector<TGraph2DErrors*> graph, string title)
{
  for (int i=0; i<4; i++)
  {
    string titleFinal = title + Form("_%d",i);
    graph[i]->SetTitle(titleFinal.c_str());
    graph[i]->SetFillColor(0);
    graph[i]->SetMarkerStyle(i+20);
    graph[i]->SetLineColor(i+1);
    graph[i]->SetMarkerSize(1.3);
    graph[i]->SetMarkerColor(i+1);
    graph[i]->Write(titleFinal.c_str());
  }
}

void WriteTextFile(vector<TGraphErrors*> graph1, string fileName)
{
  string finalFileName = fileName + ".txt";
  ofstream file;
  file.open (finalFileName.c_str());
  file << "Threshold in electron " << "\t" << "RMS of the threshold" << "\t" <<  fileName << "\t" << "Lower Error" << "\t" << "Upper error" << endl;
  for (int i=1; i<4; i++)
  {
    file << "Sector " << i << endl;
    for (int j=0; j<graph1[i]->GetN(); j++)
    {
      double x=0, y=0;
      graph1[i]->GetPoint(j,x,y);
      file << x << "\t" << graph1[i]->GetErrorX(j) << "\t" << y << "\t" << graph1[i]->GetErrorY(j) << "\t" << graph1[i]->GetErrorY(j) << endl;
    }
  }
  file.close();
}

void WriteTextFile(vector<TGraphAsymmErrors*> graph1, string fileName)
{
  string finalFileName = fileName + ".txt";
  ofstream file;
  file.open (finalFileName.c_str());
  file << "Threshold in electron " << "\t" << "RMS of the threshold" << "\t" <<  fileName << "\t" << "Lower Error" << "\t" << "Upper error" << endl;
  for (int i=1; i<4; i++)
  {
    file << "Sector " << i << endl;
    for (int j=0; j<graph1[i]->GetN(); j++)
    {
      double x=0, y=0;
      graph1[i]->GetPoint(j,x,y);
      file << x << "\t" << graph1[i]->GetErrorX(j) << "\t" << y << "\t" << graph1[i]->GetErrorYlow(j) << "\t" << graph1[i]->GetErrorYhigh(j) << endl;
    }
  }
  file.close();
}

int histFromData(string histName)
{
  string data[4] = {"efficiency", "resolution", "clusterSize", "residual"};
  for (int iData=0; iData<4; iData++)
    if (histName.find(data[iData]) != string::npos) return 1;

  string noise[1] = {"noiseOccupancy"};
  for (int iNoise=0; iNoise<1; iNoise++)
    if (histName.find(noise[iNoise]) != string::npos) return 0;
  
  string both[4] = {"threshold", "temporalNoise", "ithr", "vcasn"};
  for (int iBoth=0; iBoth<4; iBoth++)
    if (histName.find(both[iBoth]) != string::npos) return 2;
  
  cerr << "Hist type unknown, cannot decide to take the data points from noise runs or from data runs" << endl;
  return -1;
}

void treeFill(TTree* tree, vector<Run> runs)
{
  int nRun = runs.size();
  int irradiation, nEvent, runNumber;
  double ithr, ithrpA, idb, vcasn, vaux, vcasp, vreset, BB, readoutDelay, triggerDelay, strobeLength, strobeBLength, energy;
  vector<double> threshold, thresholdRMS, efficiency, nTrack, nTrackpALPIDE, temporalNoise, temporalNoiseRMS, clusterSize, clusterSizeRMS, residual, residualE, resolution, resolutionE, noiseOccupancyBeforeRemoval, noiseOccupancyBeforeRemovalE, noiseOccupancyAfterRemoval, noiseOccupancyAfterRemovalE;
  bool isNoise;
  string chipID;
  tree->Branch("threshold",&threshold);
  tree->Branch("thresholdRMS",&thresholdRMS);
  tree->Branch("efficiency",&efficiency);
  tree->Branch("nTrack",&nTrack);
  tree->Branch("nTrackpALPIDE",&nTrackpALPIDE);
  tree->Branch("temporalNoise",&temporalNoise); 
  tree->Branch("temporalNoiseRMS",&temporalNoiseRMS);
  tree->Branch("clusterSize",&clusterSize);
  tree->Branch("clusterSizeRMS",&clusterSizeRMS);
  tree->Branch("residual",&residual);
  tree->Branch("residualE",&residualE);
  tree->Branch("resolution",&resolution);
  tree->Branch("resolutionE",&resolutionE);
  tree->Branch("noiseOccupancyBeforeRemoval",&noiseOccupancyBeforeRemoval);
  tree->Branch("noiseOccupancyBeforeRemovalE",&noiseOccupancyBeforeRemovalE);
  tree->Branch("noiseOccupancyAfterRemoval",&noiseOccupancyAfterRemoval);
  tree->Branch("noiseOccupancyAfterRemovalE",&noiseOccupancyAfterRemovalE);
  tree->Branch("isNoise",&isNoise);
  tree->Branch("ithr",&ithr);
  tree->Branch("ithrpA",&ithrpA);
  tree->Branch("idb",&idb);
  tree->Branch("vcasn",&vcasn);
  tree->Branch("vaux",&vaux);
  tree->Branch("vcasp",&vcasp);
  tree->Branch("vreset",&vreset);
  tree->Branch("BB",&BB);
  tree->Branch("readoutDelay",&readoutDelay);
  tree->Branch("triggerDelay",&triggerDelay);
  tree->Branch("strobeLength",&strobeLength);
  tree->Branch("strobeBLength",&strobeBLength);
  tree->Branch("irradiation",&irradiation);
  tree->Branch("nEvent",&nEvent);
  tree->Branch("energy",&energy);
  tree->Branch("chipID",&chipID);
  for (int i=0; i<nRun; i++)
  {
    if (Skip(runs[i].getRunNumber()) || !runs[i].PlotFlag()) continue;
    runs[i].getAllParameters(runNumber, vcasn, vaux, vcasp, vreset, ithr, ithrpA, idb, threshold, thresholdRMS, temporalNoise, temporalNoiseRMS, BB, irradiation, chipID, readoutDelay, triggerDelay, strobeLength, strobeBLength, isNoise, nEvent, energy, efficiency, nTrack, nTrackpALPIDE, clusterSize, clusterSizeRMS, residual, residualE, resolution, resolutionE,  noiseOccupancyBeforeRemoval, noiseOccupancyBeforeRemovalE, noiseOccupancyAfterRemoval, noiseOccupancyAfterRemovalE);
    tree->Fill();
  }
}

void treeRead(TTree* tree, vector<Run> &runs)
{
  int irradiation, nEvent, runNumber=0;
  double ithr, ithrpA, idb, vcasn, vaux, vcasp, vreset, BB, readoutDelay, triggerDelay, strobeLength, strobeBLength, energy;
  vector<double> *threshold=0, *thresholdRMS=0, *efficiency=0, *nTrack=0, *nTrackpALPIDE=0, *temporalNoise=0, *temporalNoiseRMS=0, *clusterSize=0, *clusterSizeRMS=0, *residual=0, *residualE=0, *resolution=0, *resolutionE=0, *noiseOccupancyBeforeRemoval=0, *noiseOccupancyBeforeRemovalE=0, *noiseOccupancyAfterRemoval=0, *noiseOccupancyAfterRemovalE=0;
  bool isNoise;
  string *chipID = 0;
  tree->SetBranchAddress("threshold",&threshold);
  tree->SetBranchAddress("thresholdRMS",&thresholdRMS);
  tree->SetBranchAddress("efficiency",&efficiency);
  tree->SetBranchAddress("nTrack",&nTrack);
  tree->SetBranchAddress("nTrackpALPIDE",&nTrackpALPIDE);
  tree->SetBranchAddress("temporalNoise",&temporalNoise); 
  tree->SetBranchAddress("temporalNoiseRMS",&temporalNoiseRMS);
  tree->SetBranchAddress("clusterSize",&clusterSize);
  tree->SetBranchAddress("clusterSizeRMS",&clusterSizeRMS);
  tree->SetBranchAddress("residual",&residual);
  tree->SetBranchAddress("residualE",&residualE);
  tree->SetBranchAddress("resolution",&resolution);
  tree->SetBranchAddress("resolutionE",&resolutionE);
  tree->SetBranchAddress("noiseOccupancyBeforeRemoval",&noiseOccupancyBeforeRemoval);
  tree->SetBranchAddress("noiseOccupancyBeforeRemovalE",&noiseOccupancyBeforeRemovalE);
  tree->SetBranchAddress("noiseOccupancyAfterRemoval",&noiseOccupancyAfterRemoval);
  tree->SetBranchAddress("noiseOccupancyAfterRemovalE",&noiseOccupancyAfterRemovalE);
  tree->SetBranchAddress("isNoise",&isNoise);
  tree->SetBranchAddress("ithr",&ithr);
  tree->SetBranchAddress("ithrpA",&ithrpA);
  tree->SetBranchAddress("idb",&idb);
  tree->SetBranchAddress("vcasn",&vcasn);
  tree->SetBranchAddress("vaux",&vaux);
  tree->SetBranchAddress("vcasp",&vcasp);
  tree->SetBranchAddress("vreset",&vreset);
  tree->SetBranchAddress("BB",&BB);
  tree->SetBranchAddress("readoutDelay",&readoutDelay);
  tree->SetBranchAddress("triggerDelay",&triggerDelay);
  tree->SetBranchAddress("strobeLength",&strobeLength);
  tree->SetBranchAddress("strobeBLength",&strobeBLength);
  tree->SetBranchAddress("irradiation",&irradiation);
  tree->SetBranchAddress("nEvent",&nEvent);
  tree->SetBranchAddress("energy",&energy);
  tree->SetBranchAddress("chipID",&chipID);

  Run run;
  int nEntries = tree->GetEntries();
  for (int i=0; i<nEntries; i++)
  {
    tree->GetEntry(i);
    run.Set(runNumber,vcasn,vaux,vcasp,vreset,ithr,ithrpA,idb,*threshold,*thresholdRMS,*temporalNoise,*temporalNoiseRMS,BB,irradiation,*chipID,readoutDelay,triggerDelay,strobeLength,strobeBLength,isNoise,nEvent,energy);
    run.setEff(*efficiency);
    run.setnTr(*nTrack);
    run.setnTrpA(*nTrackpALPIDE);
    run.setClusterSizeVector(*clusterSize);
    run.setClusterSizeVectorRMS(*clusterSizeRMS);
    run.setResidualVector(*residual);
    run.setResidualVectorE(*residualE);
    run.setResolutionVector(*resolution);
    run.setResolutionVectorE(*resolutionE);
    run.setNoiseOccupancyBeforeRemovalV(*noiseOccupancyBeforeRemoval);
    run.setNoiseOccupancyBeforeRemovalVE(*noiseOccupancyBeforeRemovalE);
    run.setNoiseOccupancyAfterRemovalV(*noiseOccupancyAfterRemoval);
    run.setNoiseOccupancyAfterRemovalVE(*noiseOccupancyAfterRemovalE);
    runs.push_back(run);
  }
}

