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

void WriteGraph(string outputFolder, int dut, int firstRun, int lastRun, string toSkip, double pointingRes, string noiseFileName, string thresholdFileName, string settingsFileFolder, double BBOverWrite)
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
//  TTree *tree = new TTree("tree","Output");
//  tree->Branch("",&point,"x:y:z");
//  tree->Branch("hpx","TH1F",&hpx,128000,0);
  std::map<int,int> runNumberConvert;
  vector<Run> runs;
  Run run;
  int tmp = 0;
  ifstream settingsFile;
  string settingsFileName;
  if (settingsFileFolder.compare("") == 0) settingsFileName = outputFolder + Form("/settings_DUT%d.txt",dut);
  else settingsFileName = settingsFileFolder + Form("/settings_DUT%d.txt",dut);
  settingsFile.open(settingsFileName.c_str());
  string line;
  getline(settingsFile,line);
  int runNumber=0, irr=0, nEvent=0, isData=0;
  double energy=0, ithr=0, idb=0, vcasn=0, vaux=0, vcasp=0, vreset=0, BB=0, readoutDelay=0, triggerDelay=0, strobeLength=0, strobeBLength=0;
  string chipID, rate;
  vector<double> thr(4), thrE(4), noise(4), noiseE(4), eff(4), nTrack(4), nTrackFound(4);
//  tree->Branch("nEvent",&nEvent);
//  tree->Branch("eff",&eff);
//  tree->Branch("run",&run,"runNumber/I:nEvent/I");
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
//      ithr = ithr/51*500; //Converting to pA
//    ithrM = ithrM/4096*1000; //Converting to pA
    for (int iSector=0; iSector<4; iSector++) 
    {
      thr[iSector] *= 7.;
      thrE[iSector] *= 7.;
      noise[iSector] *= 7.;
      noiseE[iSector] *= 7.;
    }
    bool isNoise = true;
    if (isData == 1) {isNoise = false;}
    run.Set(runNumber,vcasn,vaux,vcasp,vreset,ithr,idb,thr,thrE,noise,noiseE,BB,irr,chipID,readoutDelay,triggerDelay,strobeLength,strobeBLength,isNoise,nEvent);
    run.setEff(eff);
    run.setnTr(nTrack);
    run.setnTrpA(nTrackFound);
    runs.push_back(run);
    cout << runs[tmp].getRunNumber() << " (" << tmp << ") : " << runs[tmp].getVcasn() << "\t" << runs[tmp].getIthr() << "\t" << runs[tmp].getBB() << "\t" << runs[tmp].isNoise() << endl;
    tmp++;
//    tree->Fill();
  }
  int nRun = tmp;
  for (int i=0;i<nRun;i++)
  {
    if (!runs[i].isNoise()) 
      for (int j=0;j<nRun;j++)
      {
        if (i == j) continue;
        if (runs[i].equalSettings(runs[j]))
        {
//          cerr << runs[i].getRunNumber() << "\t" << runs[j].getRunNumber() << endl;
          runs[i].setThr(runs[j].getThr());
          runs[i].setThrE(runs[j].getThrE());
          runs[i].setNoise(runs[j].getNoise());
          runs[i].setNoiseE(runs[j].getNoiseE());
          break;
        }
      }
  }
//  cerr << nRun << endl;
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
  vector<TGraphAsymmErrors*> efficiencyThr(4);
//  vector<TGraphAsymmErrors*> efficiencyIthr(4);
  vector<TGraph2D*> efficiencyIthrVcasn(4);
  vector<TGraph2D*> nTrIthrVcasn(4);
  vector<TGraph2D*> nTrpAIthrVcasn(4);
  for (int i=0; i<4; i++)
  {
    efficiencyThr[i] = new TGraphAsymmErrors;
    efficiencyIthrVcasn[i] = new TGraph2D;
    nTrIthrVcasn[i] = new TGraph2D;
    nTrpAIthrVcasn[i] = new TGraph2D;
//    efficiencyIthr[i] = new TGraphAsymmErrors;
  }
  vector<double> nTr0(4,0), nTrpA0(4,0);
  vector<TH1*> clusterSizeHisto(4);
  vector<TGraphErrors*> clusterSizeThr(4);
  vector<TGraph2DErrors*> clusterSizeIthrVcasn(4);
//  vector<TGraphErrors*> clusterSizeIthr(4);
  vector<TH1*> residualXHisto(4);
  vector<TGraphErrors*> residualXThr(4);
  vector<TGraphErrors*> resolutionXThr(4);
  vector<TH1*> residualYHisto(4);
  vector<TGraphErrors*> residualYThr(4);
  vector<TGraphErrors*> resolutionYThr(4);
  vector<TGraphErrors*> residualThr(4);
  vector<TGraph2DErrors*> residualIthrVcasn(4);
  vector<TGraphErrors*> resolutionThr(4);
  vector<TGraph2DErrors*> resolutionIthrVcasn(4);
  TH2* fakeHitHistoFromNoise;
  TH1* noiseOccupancyBeforeRemovalFromNoiseHisto;
  TH1* noiseOccupancyAfterRemovalFromNoiseHisto;
  vector<TGraphErrors*> noiseOccupancyBeforeRemovalThrFromNoise(4);
  vector<TGraphErrors*> noiseOccupancyAfterRemovalThrFromNoise(4);
  vector<TGraph2DErrors*> noiseOccupancyBeforeRemovalIthrVcasnFromNoise(4);
  vector<TGraph2DErrors*> noiseOccupancyAfterRemovalIthrVcasnFromNoise(4);
//  vector<TGraphErrors*> noiseOccupancyAfterRemovalFromNoiseIthr(4);

  for (int i=0; i<4; i++)
  {
    noiseOccupancyBeforeRemovalThrFromNoise[i] = new TGraphErrors;
    noiseOccupancyBeforeRemovalIthrVcasnFromNoise[i] = new TGraph2DErrors;
    noiseOccupancyAfterRemovalThrFromNoise[i] = new TGraphErrors;
    noiseOccupancyAfterRemovalIthrVcasnFromNoise[i] = new TGraph2DErrors;
//    noiseOccupancyAfterRemovalFromNoiseIthr[i] = new TGraphErrors;
    clusterSizeThr[i] = new TGraphErrors;
    clusterSizeIthrVcasn[i] = new TGraph2DErrors;
//    clusterSizeIthr[i] = new TGraphErrors;
    residualXThr[i] = new TGraphErrors;
    resolutionXThr[i] = new TGraphErrors;
    residualYThr[i] = new TGraphErrors;
    resolutionYThr[i] = new TGraphErrors;
    residualThr[i] = new TGraphErrors;
    resolutionThr[i] = new TGraphErrors;
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
    for (int j=i+1;j<nRun;j++)
    {
      if (runs[j].isNoise()) continue;
      if (runs[i].equalSettings(runs[j]))
      {
        clusterSizeHisto2 = runs[j].getClusterSize();
        residualXHisto2 = runs[j].getResidualX();
        residualYHisto2 = runs[j].getResidualY();
        for (int iSector=0; iSector<4; iSector++)
        {
          clusterSizeHisto[iSector]->Add(clusterSizeHisto2[iSector]);
          clusterSizeHisto2[iSector]->Reset();
          residualXHisto[iSector]->Add(residualXHisto2[iSector]);
          residualXHisto2[iSector]->Reset();
          residualYHisto[iSector]->Add(residualYHisto2[iSector]);
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
//    if (runs[i].getPlace() == 0 && runs[i].getVcasn() ==145) residualXHisto[0]->Draw("SAME");
  }
  for (int i=0;i<nRun;i++)
  {
    if (runs[i].isNoise()) continue;
    for (int iSector=0; iSector<4; iSector++)
    {
      clusterSizeHisto = runs[i].getClusterSize();
      if (clusterSizeHisto[iSector]->Integral() < 100) continue;
      residualXHisto = runs[i].getResidualX();
      TFitResultPtr resultX = residualXHisto[iSector]->Fit("gaus","QNOS");
      Int_t fitStatusX = resultX;
      residualYHisto = runs[i].getResidualY();
      TFitResultPtr resultY = residualYHisto[iSector]->Fit("gaus","QNOS");
      Int_t fitStatusY = resultY;
      clusterSizeThr[iSector]->SetPoint(clusterSizeThr[iSector]->GetN(),runs[i].getThr()[iSector],clusterSizeHisto[iSector]->GetMean());
      clusterSizeThr[iSector]->SetPointError(clusterSizeThr[iSector]->GetN()-1,runs[i].getThrE()[iSector],clusterSizeHisto[iSector]->GetMeanError());
      clusterSizeIthrVcasn[iSector]->SetPoint(clusterSizeIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),clusterSizeHisto[iSector]->GetMean());
      clusterSizeIthrVcasn[iSector]->SetPointError(clusterSizeIthrVcasn[iSector]->GetN()-1,0,0,clusterSizeHisto[iSector]->GetMeanError());
//      clusterSizeIthr[iSector]->SetPoint(clusterSizeIthr[iSector]->GetN(),runs[i].getIthr(),clusterSizeHisto[iSector]->GetMean());
//      clusterSizeIthr[iSector]->SetPointError(clusterSizeIthr[iSector]->GetN()-1,0,clusterSizeHisto[iSector]->GetMeanError());
      if (fitStatusX == 0)
      {
        double resX = sqrt(resultX->Parameter(2)*1000*resultX->Parameter(2)*1000);
        residualXThr[iSector]->SetPoint(residualXThr[iSector]->GetN(),runs[i].getThr()[iSector],resX);
        residualXThr[iSector]->SetPointError(residualXThr[iSector]->GetN()-1,runs[i].getThrE()[iSector],resultX->ParError(2)*1000);
        resX = sqrt(resultX->Parameter(2)*1000*resultX->Parameter(2)*1000-pointingRes*pointingRes);
        resolutionXThr[iSector]->SetPoint(resolutionXThr[iSector]->GetN(),runs[i].getThr()[iSector],resX);
        resolutionXThr[iSector]->SetPointError(resolutionXThr[iSector]->GetN()-1,runs[i].getThrE()[iSector],resultX->ParError(2)*1000);
      }
      if (fitStatusY == 0)
      {
        double resY = sqrt(resultY->Parameter(2)*1000*resultY->Parameter(2)*1000);
        residualYThr[iSector]->SetPoint(residualYThr[iSector]->GetN(),runs[i].getThr()[iSector],resY);
        residualYThr[iSector]->SetPointError(residualYThr[iSector]->GetN()-1,runs[i].getThrE()[iSector],resultY->ParError(2)*1000);
        resY = sqrt(resultY->Parameter(2)*1000*resultY->Parameter(2)*1000-pointingRes*pointingRes);
        resolutionYThr[iSector]->SetPoint(resolutionYThr[iSector]->GetN(),runs[i].getThr()[iSector],resY);
        resolutionYThr[iSector]->SetPointError(resolutionYThr[iSector]->GetN()-1,runs[i].getThrE()[iSector],resultY->ParError(2)*1000);
      }
      if (fitStatusX == 0 && fitStatusY == 0)
      {
        double resX = sqrt(resultX->Parameter(2)*1000*resultX->Parameter(2)*1000);
        double resY = sqrt(resultY->Parameter(2)*1000*resultY->Parameter(2)*1000);
        residualThr[iSector]->SetPoint(residualThr[iSector]->GetN(),runs[i].getThr()[iSector],(resX+resY)/2.);
        residualThr[iSector]->SetPointError(residualThr[iSector]->GetN()-1,runs[i].getThrE()[iSector],sqrt(resultX->ParError(2)*resultX->ParError(2)+resultY->ParError(2)*resultY->ParError(2))*1000);
        residualIthrVcasn[iSector]->SetPoint(residualIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),(resX+resY)/2.);
        residualIthrVcasn[iSector]->SetPointError(residualIthrVcasn[iSector]->GetN()-1,0,0,sqrt(resultX->ParError(2)*resultX->ParError(2)+resultY->ParError(2)*resultY->ParError(2))*1000);
        resX = sqrt(resultX->Parameter(2)*1000*resultX->Parameter(2)*1000-pointingRes*pointingRes);
        resY = sqrt(resultY->Parameter(2)*1000*resultY->Parameter(2)*1000-pointingRes*pointingRes);
        resolutionThr[iSector]->SetPoint(resolutionThr[iSector]->GetN(),runs[i].getThr()[iSector],(resX+resY)/2.);
        resolutionThr[iSector]->SetPointError(resolutionThr[iSector]->GetN()-1,runs[i].getThrE()[iSector],sqrt(resultX->ParError(2)*resultX->ParError(2)+resultY->ParError(2)*resultY->ParError(2))*1000);
        resolutionIthrVcasn[iSector]->SetPoint(resolutionIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),(resX+resY)/2.);
        resolutionIthrVcasn[iSector]->SetPointError(resolutionIthrVcasn[iSector]->GetN()-1,0,0,sqrt(resultX->ParError(2)*resultX->ParError(2)+resultY->ParError(2)*resultY->ParError(2))*1000);
//        resolutionIthr[iSector]->SetPoint(resolutionIthr[iSector]->GetN(),runs[i].getIthr(),(resX+resY)/2.);
//        resolutionIthr[iSector]->SetPointError(resolutionIthr[iSector]->GetN()-1,0,sqrt(resultX->ParError(2)*resultX->ParError(2)+resultY->ParError(2)*resultY->ParError(2))*1000);
      }
    }
  }
  for (int i=0;i<nRun;i++)
  {
    if (runs[i].isNoise()) continue;
//    cerr << runs[i].getRunNumber() << endl;
    for (int j=i+1;j<nRun;j++)
    {
      if (runs[j].isNoise()) continue;
      if (runs[i].equalSettings(runs[j])) 
      {
//        cerr << runs[i].getRunNumber() << "\t" << runs[j].getRunNumber() << endl;
        vector<double> nTr(4), nTrpA(4);
        for (int iSector=0; iSector<4; iSector++)
        {
//          cerr << runs[i].getRunNumber()  << "\t" << runs[j].getRunNumber() << "\t" << iSector << "\t" << runs[i].getnTr()[iSector] << "\t" << runs[j].getnTr()[iSector] << endl;
//          cerr << runs[i].getRunNumber() << "\t" << runs[j].getRunNumber() << "\t" << iSector << "\t" << runs[i].getnTrpA()[iSector] <<"\t" << runs[j].getnTrpA()[iSector] << endl;
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
    if (runs[i].isNoise()) continue;
//    cerr << runs[i].getRunNumber() << endl;
    for (int iSector=0; iSector<4; iSector++)
    {
//      cerr << runs[i].getRunNumber() << endl;
      if (runs[i].getnTr()[iSector] > 100)
      {
        double nTrd = runs[i].getnTr()[iSector];
        double nTrpAd = runs[i].getnTrpA()[iSector];
        double effd = nTrpAd/nTrd;
        double mean = (nTrpAd+1)/(nTrd+2);
        double sigma = sqrt(((nTrpAd+1)*(nTrpAd+2))/((nTrd+2)*(nTrd+3))-((nTrpAd+1)*(nTrpAd+1))/((nTrd+2)*(nTrd+2)));
        efficiencyThr[iSector]->SetPoint(efficiencyThr[iSector]->GetN(),runs[i].getThr()[iSector],effd*100.); 
        efficiencyThr[iSector]->SetPointError(efficiencyThr[iSector]->GetN()-1,runs[i].getThrE()[iSector],runs[i].getThrE()[iSector],(effd-(mean-sigma))*100.,((mean+sigma)-effd)*100.);
        efficiencyIthrVcasn[iSector]->SetPoint(efficiencyIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),effd*100);
        nTrIthrVcasn[iSector]->SetPoint(nTrIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),nTrd);
        nTrpAIthrVcasn[iSector]->SetPoint(nTrpAIthrVcasn[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),nTrpAd);
//        efficiencyIthr[iSector]->SetPoint(efficiencyIthr[iSector]->GetN(),runs[i].getIthr(),effd*100.); 
//        efficiencyIthr[iSector]->SetPointError(efficiencyIthr[iSector]->GetN()-1,0,0,(effd-(mean-sigma))*100.,((mean+sigma)-effd)*100.);
      }
//      else cerr << runs[i].getRunNumber() << "\t" << iSector << endl;
    }
  }


  for (int i=0;i<nRun;i++)
  {
    if (!runs[i].isNoise()) continue;
    if (Skip(runs[i].getRunNumber())) continue;
    string fileName = outputFolder +  Form("/run%06d/histogram/run%06d-noise.root",runs[i].getRunNumber(),runs[i].getRunNumber());
    TFile* histFile = new TFile(fileName.c_str(),"READONLY");
    if (!histFile || histFile->IsZombie())
      continue;
    noiseOccupancyBeforeRemovalFromNoiseHisto = (TH1F*)histFile->Get(Form("noiseOccupancy_%d",dut));
    fakeHitHistoFromNoise = (TH2I*)histFile->Get(Form("noiseMap_%d",dut));
    noiseOccupancyAfterRemovalFromNoiseHisto = CalculateNoiseFromNoise(fakeHitHistoFromNoise,i,runs);
    for (int iSector=0; iSector<4; iSector++)
    {
      if (noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinContent(iSector+1) == 0) continue;
      noiseOccupancyBeforeRemovalThrFromNoise[iSector]->SetPoint(noiseOccupancyBeforeRemovalThrFromNoise[iSector]->GetN(),runs[i].getThr()[iSector],noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinContent(iSector+1));
      noiseOccupancyBeforeRemovalThrFromNoise[iSector]->SetPointError(noiseOccupancyBeforeRemovalThrFromNoise[iSector]->GetN()-1,runs[i].getThrE()[iSector],noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinError(iSector+1));
      noiseOccupancyBeforeRemovalIthrVcasnFromNoise[iSector]->SetPoint(noiseOccupancyBeforeRemovalIthrVcasnFromNoise[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinContent(iSector+1));
      noiseOccupancyBeforeRemovalIthrVcasnFromNoise[iSector]->SetPointError(noiseOccupancyBeforeRemovalIthrVcasnFromNoise[iSector]->GetN()-1,0,0,noiseOccupancyBeforeRemovalFromNoiseHisto->GetBinError(iSector+1));
      noiseOccupancyAfterRemovalThrFromNoise[iSector]->SetPoint(noiseOccupancyAfterRemovalThrFromNoise[iSector]->GetN(),runs[i].getThr()[iSector],noiseOccupancyAfterRemovalFromNoiseHisto->GetBinContent(iSector+1));
      noiseOccupancyAfterRemovalThrFromNoise[iSector]->SetPointError(noiseOccupancyAfterRemovalThrFromNoise[iSector]->GetN()-1,runs[i].getThrE()[iSector],noiseOccupancyAfterRemovalFromNoiseHisto->GetBinError(iSector+1));
      noiseOccupancyAfterRemovalIthrVcasnFromNoise[iSector]->SetPoint(noiseOccupancyAfterRemovalIthrVcasnFromNoise[iSector]->GetN(),runs[i].getIthr(),runs[i].getVcasn(),noiseOccupancyAfterRemovalFromNoiseHisto->GetBinContent(iSector+1));
      noiseOccupancyAfterRemovalIthrVcasnFromNoise[iSector]->SetPointError(noiseOccupancyAfterRemovalIthrVcasnFromNoise[iSector]->GetN()-1,0,0,noiseOccupancyAfterRemovalFromNoiseHisto->GetBinError(iSector+1));
//      noiseOccupancyAfterRemovalFromNoiseIthr[iSector]->SetPoint(noiseOccupancyAfterRemovalFromNoiseIthr[iSector]->GetN(),runs[i].getIthr(),noiseOccupancyAfterRemovalFromNoiseHisto->GetBinContent(iSector+1));
//      noiseOccupancyAfterRemovalFromNoiseIthr[iSector]->SetPointError(noiseOccupancyAfterRemovalFromNoiseIthr[iSector]->GetN()-1,0,noiseOccupancyAfterRemovalFromNoiseHisto->GetBinError(iSector+1));
    }
  }
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
        cerr << noiseGraphName << endl;
        noiseFromLab[iSector] = (TGraphAsymmErrors*)noiseFile->Get(noiseGraphName.c_str());
        double x=0,y=0;
        for (int i=0; i<noiseFromLab[iSector]->GetN(); i++)
        {
          noiseFromLab[iSector]->GetPoint(i,x,y);
          cerr << x << "\t" << y << endl;
          noiseOccupancyAfterRemovalIthrVcasnFromLab[iSector]->SetPoint(i,x,135.,y);
          cerr << noiseOccupancyAfterRemovalIthrVcasnFromLab[iSector]->GetX()[i] << endl;
          noiseOccupancyAfterRemovalIthrVcasnFromLab[iSector]->SetPointError(i,0,0,0);
        }
      }
    }
    
  }
  vector<TGraph2DErrors*> temperalNoiseIthrVcasnFromLab(4);
  vector<TGraph2DErrors*> thresholdIthrVcasnFromLab(4);
  for (int iSector=0; iSector<4; iSector++)
  {
    temperalNoiseIthrVcasnFromLab[iSector] = new TGraph2DErrors;
    thresholdIthrVcasnFromLab[iSector] = new TGraph2DErrors;
  }
  if (thresholdFileName.compare("") !=0)
  {
    TFile* thresholdFile = new TFile(thresholdFileName.c_str(),"READONLY");
    if (thresholdFile && !thresholdFile->IsZombie())
    {
      vector<TGraphAsymmErrors*> temperalNoise(4);
      vector<TGraphAsymmErrors*> threshold(4);
      for (int iSector=0; iSector<4; iSector++)
      {
        string temperalNosieGraphName = "g_thresnoiseVsITHR_" + globalFileInfo + "_TEMP28.0_VBB" + Form("%0.1f_VCASN135_sec%d", globalBB, iSector);
        string thresholdGraphName = "g_thresVsITHR_" + globalFileInfo + "_TEMP28.0_VBB" + Form("%0.1f_VCASN135_sec%d", globalBB, iSector);
        cerr << temperalNosieGraphName << endl << thresholdGraphName << endl;
        temperalNoise[iSector] = (TGraphAsymmErrors*)thresholdFile->Get(temperalNosieGraphName.c_str());
        threshold[iSector] = (TGraphAsymmErrors*)thresholdFile->Get(thresholdGraphName.c_str());
        double x=0,y=0;
        for (int i=0; i<temperalNoise[iSector]->GetN(); i++)
        {
          temperalNoise[iSector]->GetPoint(i,x,y);
          temperalNoiseIthrVcasnFromLab[iSector]->SetPoint(i,x,135.,y);
          temperalNoiseIthrVcasnFromLab[iSector]->SetPointError(i,0,0,0);
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
  string BBStr, irrStr, firstRunStr, lastRunStr, pointingResStr;
  BBStr = Form("%0.f", globalBB);
  irrStr = Form("%d", globalIrr);
  firstRunStr = Form("%d", firstRun);
  lastRunStr = Form("%d", lastRun);
  pointingResStr = Form("%.2f", pointingRes);
//  char BBStr [33];
//  snprintf(BBStr, sizeof(BBStr), "%d", globalBB);
//  char irrStr [33];
//  snprintf(irrStr, sizeof(irrStr), "%d", globalIrr);
//  ostringstream firstRunStr;
//  firstRunStr << firstRun;
//  ostringstream lastRunStr;
//  lastRunStr << lastRun;
//  ostringstream pointingResStr;
//  pointingResStr << pointingRes;
  outputFileName = outputFolder + "/graphs";
  if (globalFileInfo != "") outputFileName += "_" + globalFileInfo;
  if (globalBB != -100) outputFileName += "_BB" + BBStr + "V";
  if (globalIrr != -100) outputFileName += "_Irr" + irrStr;
  outputFileName += "_" + firstRunStr + "-" + lastRunStr + "_PR" + pointingResStr + "um.root";
  cerr << outputFileName << endl;
//  cerr << fileName << endl;
  TFile* outputFile = new TFile(outputFileName.c_str(),"RECREATE");
  Write(noiseOccupancyBeforeRemovalThrFromNoise, "noiseOccupancyBeforeRemovalFromNoiseThr");
  Write(noiseOccupancyBeforeRemovalIthrVcasnFromNoise, "noiseOccupancyBeforeRemovalFromNoiseIthrVcasn2D");
  Write(noiseOccupancyAfterRemovalThrFromNoise, "noiseOccupancyAfterRemovalFromNoiseThr");
  Write(noiseOccupancyAfterRemovalIthrVcasnFromNoise, "noiseOccupancyAfterRemovalFromNoiseIthrVcasn2D");
  Write(noiseOccupancyAfterRemovalIthrVcasnFromLab,"noiseOccupancyAfterRemovalFromLabIthrVcasn2D");
  Write(thresholdIthrVcasnFromLab,"thresholdFromLabIthrVcasn2D");
  Write(temperalNoiseIthrVcasnFromLab,"temperalNoiseFromLabIthrVcasn2D");
//  Write(noiseOccupancyAfterRemovalFromNoiseIthr, "noiseOccupancyAfterRemovalFromNoiseIthr");
  Write(efficiencyThr,"efficiencyThr");
//  Write(efficiencyIthr,"efficiencyIthr");
  Write(efficiencyIthrVcasn,"efficiencyIthrVcasn2D");
  Write(nTrIthrVcasn,"nTrIthrVcasn2D");
  Write(nTrpAIthrVcasn,"nTrpAIthrVcasn2D");
  Write(clusterSizeThr,"clusterSizeThr");
  Write(clusterSizeIthrVcasn,"clusterSizeIthrVcasn2D");
//  Write(clusterSizeIthr,"clusterSizeIthr");
//  Write(residualXThr,"residualXThr");
//  Write(residualYThr,"residualYThr");
  Write(residualThr,"residualThr");
  Write(residualIthrVcasn,"residualIthrVcasn2D");
//  Write(resolutionXThr,"resolutionXThr");
//  Write(resolutionYThr,"resolutionYThr");
  Write(resolutionThr,"resolutionThr");
  Write(resolutionIthrVcasn,"resolutionIthrVcasn2D");
//  Write(resolutionIthr,"resolutionIthr");
//  tree->Write();
  outputFile->Close();
//  WriteTextFile(efficiencyThr,"efficiency");
//  WriteTextFile(noiseOccupancyAfterRemovalThr, "noiseOccupancy");

}

TH1F* CalculateNoiseFromNoise(TH2* fakeHitHisto, int runNumberIndex, vector<Run> runs) 
{
  int nEvent = runs[runNumberIndex].getnEvent();
#if DEBUG
  TH1* noiseOccupancyBeforeRemovalHisto;
  TH1F* noiseOccupancyAfterRemovalHisto;
  TH1F* noiseOccupancyBeforeRemovalHisto2 = new TH1F("noiseOccupancyBeforeRemovalHisto2","noiseOccupancyBeforeRemovalHisto2",4,0,4);
  TH1F* removedPixelsHisto = new TH1F("removedPixelsHisto","removedPixelsHisto",4,0,4);
  TH1F* removedPixelsHisto2 = new TH1F("removedPixelsHisto2","removedPixelsHisto2",4,0,4);
#endif
  TH1F* noiseOccupancyAfterRemovalHisto2 = new TH1F("noiseOccupancyAfterRemovalHisto2","noiseOccupancyAfterRemovalHisto2",4,0,4);
  TH1F* noiseOccupancyAfterRemovalHisto3 = new TH1F("noiseOccupancyAfterRemovalHisto3","noiseOccupancyAfterRemovalHisto3",4,0,4);
  TH1I* firingFrequency[4];
  for (int i=0; i<4; i++)
  {
    firingFrequency[i] = new TH1I(Form("firingFrequency_%d",i),Form("firingFrequency_%d",i),1000,0,1);
  }
  int maxNPixels = 20;
  int noiseWithRemove2[4] = {0};
  int removedPixels2[4] = {0};
  int noise[4] = {0};
  int noiseWithRemove[4] = {0};
  for (int x=0; x<fakeHitHisto->GetNbinsX(); x++)
  {
    int index = -1;
    for (int iSector=0; iSector<4; iSector++)
      if (x>=1024/4.*iSector && x<1024/4.*(iSector+1))
      {
        index = iSector;
        break;
      }
    if (index == -1) {cerr << x << endl; continue;}
    for (int y=0; y<fakeHitHisto->GetNbinsY(); y++)
    {
      if (fakeHitHisto->GetBinContent(x,y)!=0)
        firingFrequency[index]->Fill((double)fakeHitHisto->GetBinContent(x,y)/nEvent);
      if ((double)fakeHitHisto->GetBinContent(x,y)/nEvent < 0.01)
        noiseWithRemove[index] += fakeHitHisto->GetBinContent(x,y);
#if DEBUG
      else
        removedPixelsHisto->Fill(index);
#endif
      noise[index] += fakeHitHisto->GetBinContent(x,y);
    }
  }
 
  for (int iSector=0; iSector<4; iSector++)
    noiseWithRemove2[iSector] = noise[iSector];
  int index = -1;
  bool lowEnough[4] = {false};
  for (int iSector=0; iSector<4; iSector++)
    if (!lowEnough[iSector] && noiseWithRemove2[iSector] == 0) {/*cerr << iSector << "done" << endl;*/ lowEnough[iSector] = true;}
  while (1)
  {
    int x=0, y=0, z=0;
    fakeHitHisto->GetBinXYZ(fakeHitHisto->GetMaximumBin(),x,y,z);
    for (int iSector=0; iSector<4; iSector++)
    {
      if (x>=256*iSector && x<256*(iSector+1))
      {
        index = iSector;
        break;
      }
    }
    if (index == -1) {cerr << x << endl; continue;}
    if (!lowEnough[index] && removedPixels2[index] < maxNPixels)// && (double)noiseWithRemove2[index]/nEvent/131072 > 1e-8)
    {
      noiseWithRemove2[index] -= fakeHitHisto->GetBinContent(x,y);
//        cerr << "sector " << index << " lowered by " << fakeHitHisto->GetBinContent(x,y) << endl;
        removedPixels2[index]++;
//        removedPixelsHisto2->Fill(index);
    }
    else if (!lowEnough[index] && removedPixels2[index] >= maxNPixels)
    {
      lowEnough[index] = true;
      cerr << "More than " << maxNPixels << " pixels needed to go below limit in sector " << index << endl;
//        cerr << index << "done" << endl;
    }
    else if (!lowEnough[index])
    {
      lowEnough[index] = true; 
//        cerr << index << "done" << endl;
    }
    fakeHitHisto->SetBinContent(x,y,0);
    if (lowEnough[0] && lowEnough[1] && lowEnough[2] && lowEnough[3]) break;
//      cerr << x << "\t" << y << "\t" << z << endl;
  }
#if DEBUG
  TCanvas* firingFrequencyC = new TCanvas("firingFrequencyC","firingFrequencyC",800,600);
  firingFrequencyC->Divide(2,2);
#endif
  for (int iSector=0; iSector<4; iSector++)
  {
#if DEBUG
    firingFrequencyC->cd(iSector+1)->SetLogy();
    firingFrequency[iSector]->Draw();
    noiseOccupancyBeforeRemovalHisto2->SetBinContent(iSector+1,(double)noise[iSector]/nEvent/131072);
#endif
    noiseOccupancyAfterRemovalHisto2->SetBinContent(iSector+1,(double)noiseWithRemove[iSector]/nEvent/131072);
    noiseOccupancyAfterRemovalHisto2->SetBinError(iSector+1,sqrt((double)noiseWithRemove[iSector])/nEvent/131072);
    noiseOccupancyAfterRemovalHisto3->SetBinContent(iSector+1,(double)noiseWithRemove2[iSector]/nEvent/131072);
    noiseOccupancyAfterRemovalHisto3->SetBinError(iSector+1,sqrt((double)noiseWithRemove2[iSector])/nEvent/131072);
#if DEBUG
    removedPixelsHisto2->SetBinContent(iSector+1,removedPixels2[iSector]);
#endif
  }
#if DEBUG
  TCanvas* noiseOccupancyC = new TCanvas("noiseOccupancyC","noiseOccupancyC",800,600);
  noiseOccupancyC->SetLogy();
  noiseOccupancyBeforeRemovalHisto->Draw(); 
  noiseOccupancyBeforeRemovalHisto2->SetLineColor(2);
  noiseOccupancyBeforeRemovalHisto2->Draw("SAME");
  noiseOccupancyAfterRemovalHisto->SetLineColor(1);
  noiseOccupancyAfterRemovalHisto->Draw("SAME");
  noiseOccupancyAfterRemovalHisto2->SetLineColor(3);
  noiseOccupancyAfterRemovalHisto2->Draw("SAME");
  noiseOccupancyAfterRemovalHisto3->SetLineColor(4);
  noiseOccupancyAfterRemovalHisto3->Draw("SAME");
  new TCanvas("removedPixelsC","removedPixelsC",800,600);
  removedPixelsHisto->Draw();
  removedPixelsHisto2->SetLineColor(2);
  removedPixelsHisto2->Draw("SAME");
#endif
  return noiseOccupancyAfterRemovalHisto3;
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
//      cerr << graphV[j]->GetN() << endl;
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
  if (sectorV.size() == 2) legend = (string)Form("Sector %d",sectorV[0]) + (string)Form("  #color[2]{Sector %d}",sectorV[1]);
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
    else legend = comparison;
  }
  
  if (defaultsFine) compareOneHistogram(files, hist, sectorStr, IthrVcasn, IthrVcasnValue, xTitle1.c_str(), xTitle2.c_str(), x1low, x1high, x2low, x2high, legend.c_str(), yTitle.c_str(), ylow, yhigh, log, line, BB, irr, chip, rate, "");
}

void compareOneHistogram(string files, string hist, string sectorStr, bool IthrVcasn, double IthrVcasnValue, const char* xTitle1, const char* xTitle2, double x1low, double x1high, double x2low, double x2high, const char* legend, const char* yTitle, double ylow, double yhigh, bool log, double line, bool BB, bool irr, bool chip, bool rate, const char* title)
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
  else if (sectorV.size() > 2)
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
//  cerr << "defaultsFine " << defaultsFine << endl;
//  cerr << x1low << "\t" << x1high << "\t" << xTitle1 << "\t" << y1low << "\t" << y1high << "\t" << yTitle1 << "\t" << log1 << endl;
//  cerr << x2low << "\t" << x2high << "\t" << xTitle2 << "\t" << y2low << "\t" << y2high << "\t" << yTitle2 << "\t" << log2 << endl;
//  cerr << legend << endl;
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
//    cerr << "graph2D1->GetN(): " << graph2D1->GetN() << endl;
    for (int i=0; i<graph2D1->GetN(); i++)
    {
 //     cerr << Z[i]  << "\t" << X1[i] << "\t" << Y1[i]<< endl;
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
//  graph->GetXaxis()->SetTitle("I_{thr} (DAC units)");
//  graph->GetYaxis()->SetTitle("V_{casn} (DAC units)");
//  graph->GetZaxis()->SetTitle(zTitle);
  graph->GetHistogram();
  string titleFinal = title + ";I_{thr} (DAC units);V_{casn} (DAC units);" + zTitle; 
  graph->SetTitle(titleFinal.c_str());
  graph->GetXaxis()->SetTitleOffset(1.3);
  graph->GetYaxis()->SetTitleOffset(1.3);
//  graph->GetZaxis()->SetTitleOffset(1.3);
  graph->GetXaxis()->SetTitleSize(0.05);
  graph->GetYaxis()->SetTitleSize(0.05);
  graph->GetZaxis()->SetTitleSize(0.05);
//  graph->GetXaxis()->SetLimits(9,60);
//  graph->GetYaxis()->SetLimits(70,100);
//  graph->GetZaxis()->SetLimits(0,110);
  graph->GetZaxis()->SetRangeUser(zlow,zhigh);
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
//  cerr << "defaultsFine " << defaultsFine << endl;
//  cerr << x1low << "\t" << x1high << "\t" << xTitle1 << "\t" << y1low << "\t" << y1high << "\t" << yTitle1 << "\t" << log1 << endl;
//  cerr << x2low << "\t" << x2high << "\t" << xTitle2 << "\t" << y2low << "\t" << y2high << "\t" << yTitle2 << "\t" << log2 << endl;
//  cerr << legend << endl;
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
    DrawOverSectors(graph1V,y1low,y1high,line1,yTitle1,graph2V,y2low,y2high,line2,yTitle2,canvas2.c_str(),legend,legendStr, xlow, xhigh, log1,log2,xTitle,title.c_str());
  }
  
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
  else if (graph.find("clusterSize") != string::npos) 
  {
    ylow = 0;
    yhigh = 5;
    line = -1;
    yTitle = "Average cluster size (pixels)";
    legend += "Cluster size   ";
  }
  else if (graph.find("temperalNoise")!= string::npos)
  {
    ylow = 0;
    yhigh = 20;
    yTitle = "Temperal noise (electrons)";
    legend += "Noise   ";
  }
  else if (graph.find("threshold")!= string::npos)
  {
    ylow = 50;
    yhigh = 250;
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
//  cerr << xlow << "\t" << xhigh << "\t" << xTitle << "\t" << ylow << "\t" << yhigh << "\t" << yTitle << "\t" << log<< endl;
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
  if (BBStr == "-") BBStr = file.substr(BBPos+2,3);
  else BBStr = file.substr(BBPos+2,2);
  if (BBPos == string::npos || !addBB) BB = -100;
  else BB = atoi(BBStr.c_str());
  size_t chipPos = file.find("W");
  string chipStr = "";
  if (chipPos!=string::npos) chipStr = file.substr(chipPos,5);
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
    cerr << "Number of legends doesn't correspond to the number of graphs!" << endl;
    drawLegend = false;
  }
  TCanvas * C = new TCanvas(canvas.c_str(),"",800,600);
  C->cd();
  if (log) C->SetLogy();
  int markerColorShift = 0;
  for (unsigned int i=0; i<graph.size(); i++)
  {
    if (i==4 || i==5) markerColorShift++;
    graph[i]->GetXaxis()->SetTitle(titleX);
    graph[i]->GetYaxis()->SetTitle(titleY);
    graph[i]->SetTitle(canvasTitle);
    graph[i]->SetFillColor(0);
    graph[i]->GetYaxis()->SetRangeUser(rangeLow,rangeHigh);
    graph[i]->SetLineColor(i+1+markerColorShift);
    graph[i]->SetMarkerStyle(i+20);
    graph[i]->SetMarkerSize(1.3);
    graph[i]->SetMarkerColor(i+1+markerColorShift);
    graph[i]->GetXaxis()->SetLimits(xLow,xHigh);

    graph[i]->Draw(i==0?"APL":"SAMEPL");
  }
  C->Update();
  TLine *l1=new TLine(C->GetUxmin(),line,C->GetUxmax(),line);
  l1->SetLineColor(1);
  l1->SetLineStyle(2);
  l1->Draw();
  TLegend * legend = new TLegend(0.1,0.1,0.77,0.3);
  if (drawLegend)
  {
    legend->SetFillColor(0);
    for (unsigned int i=0; i<legendStr.size(); i++)
      legend->AddEntry(graph[i]->Clone(), legendStr[i].c_str());
    legend->Draw();
  }
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

void DrawOverSectors(vector<TGraph*> graph1, double rangeLow1, double rangeHigh1, double line1, const char* titleY1, vector<TGraph*> graph2, double rangeLow2, double rangeHigh2, double line2, const char* titleY2, const char* canvas, const char* legendTitle, vector<string> legendStr, double xlow, double xhigh,bool log1, bool log2, const char* titleX, const char* title)
{
  TCanvas * C = new TCanvas(canvas,"",800,600);
  gROOT->Reset();
  C->cd();
  TPad *pad = new TPad("pad","",0,0,1,1);
  pad->SetFillColor(0);
  if (log1) pad->SetLogy();
  pad->Draw();
  pad->cd();
  TH1F *hr = pad->DrawFrame(xlow,rangeLow1,xhigh,rangeHigh1);
  hr->GetYaxis()->SetTitleOffset(1.15);
  hr->SetXTitle(titleX);
  hr->SetYTitle(titleY1);
  hr->GetXaxis()->SetTitleSize(0.045);
  hr->GetYaxis()->SetTitleSize(0.045);
  hr->SetTitle(title);
//  hr->SetTitle("Irradiated with 1e13 1 Mev n_{eq}/cm^{2}");
  pad->GetFrame()->SetFillColor(0);
  pad->GetFrame()->SetBorderSize(12);
  for (int i=graph1.size()-1; i>=0; i--)
  {
    graph1[i]->GetYaxis()->CenterLabels();
    graph1[i]->SetFillColor(0);
    graph1[i]->SetTitle("");
    graph1[i]->SetMarkerStyle(i==0?23:19+i);
//    graph1[i]->SetLineColor(i+1);
    graph1[i]->SetLineColor(1);
//  graph1[i]->SetLineColor(1);
    graph1[i]->SetMarkerSize(1.3);
    graph1[i]->SetMarkerColor(1);
//    graph1[i]->SetMarkerColor(i+1);
    graph1[i]->DrawClone((unsigned int) i==(graph1.size()-1)?"PL":"SAMEPL");

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
  for (int i=graph2.size()-1; i>=0; i--)
  {
    graph2[i]->SetFillColor(0);
    graph2[i]->SetMarkerStyle(i+20);
    graph2[i]->SetLineColor(2);
//    graph2[i]->SetLineColor(i+1);
    graph2[i]->SetMarkerStyle(i==0?32:23+i);
//    graph2[i]->SetMarkerStyle(i==3?32:24+i);
    graph2[i]->SetMarkerSize(1.3);
    graph2[i]->SetMarkerColor(2);
//    graph2[i]->SetMarkerColor(i+1);
    graph2[i]->SetTitle("");
    graph2[i]->DrawClone((unsigned int)i==(graph2.size()-1)?"PL":"SAMEPL");
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
  legend->SetFillColor(0);
  legend->SetNColumns(2);
  for (unsigned int i=0; i<legendStr.size(); i++)
  {
    legend->AddEntry(graph1[i]->Clone(), "      ");
    legend->AddEntry(graph2[i]->Clone(), legendStr[i].c_str());
  }
  legend->SetHeader(legendTitle);
  legend->Draw();
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
//  gROOT->Reset();
  C->cd();
  TPad *pad = new TPad("pad","",0,0,1,1);
  pad->SetFillColor(0);
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
//  hr->SetTitle("Irradiated with 1e13 1 Mev n_{eq}/cm^{2}");
  pad->GetFrame()->SetFillColor(0);
  pad->GetFrame()->SetBorderSize(12);
  for (unsigned int i=0; i<graph1.size(); i++)
  {
    graph1[i]->GetYaxis()->CenterLabels();
    graph1[i]->SetFillColor(0);
//    graph1[i]->SetTitle("");
    graph1[i]->SetMarkerStyle(marker1[i]);
//    graph1[i]->SetLineColor(i+1);
    graph1[i]->SetLineColor(1);
    graph1[i]->SetMarkerSize(1.3);
    graph1[i]->SetMarkerColor(1);
//    graph1[i]->SetMarkerColor(i+1);
    graph1[i]->DrawClone(i==0?"PL":"SAMEPL");

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
  for (unsigned int i=0; i<graph2.size(); i++)
  {
    graph2[i]->SetFillColor(0);
    graph2[i]->SetMarkerStyle(i+20);
    graph2[i]->SetLineColor(2);
//    graph2[i]->SetLineColor(i+1);
    graph2[i]->SetMarkerStyle(marker2[i]);
//    graph2[i]->SetMarkerStyle(i==3?32:24+i);
    graph2[i]->SetMarkerSize(1.3);
    graph2[i]->SetMarkerColor(2);
//    graph2[i]->SetMarkerColor(i+1);
//    graph2[i]->SetTitle("");
    graph2[i]->DrawClone(i==0?"PL":"SAMEPL");
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
//      cerr << fixed[i] << "\t" << varying[i] << "\t" << Z[i] << endl;
      if (!isEfficiency) 
      {
        graph1D->SetPoint(tmp,varying[i],Z[i]);
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
//      cerr << graph1[i]->GetErrorX(minIndex) << "\t" << graph1[i]->GetErrorYlow(minIndex)<< graph1[i]->GetErrorYhigh(minIndex) << endl;
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
