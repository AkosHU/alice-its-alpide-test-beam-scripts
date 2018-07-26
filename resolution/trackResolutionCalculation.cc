#include <iostream>
#include <TRandom.h>
#include <TMath.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include "TFitResult.h"
//#include "Minuit2/FCNBase.h"
//#include "TFitterMinuit.h"
#include "TSystem.h"
#include "TFile.h"

using namespace std;

int DoAnalFit(int nPlanes, bool * passiveLayer, double * Z, double * scattering,  double * pos, double *err);
int GaussjSolve(double *alfa,double *beta,int n);
double Resolution(int nPlanes, bool vacuum, bool airAtFitting, double * inputDist, double * thickness,  double * resolution, int dutID, double energy, int nEvent);

int main()
{

  cout.setf(std::ios::boolalpha);
  int N;
  bool vacuum, airAtFitting;
  cout << "Number of planes: ";
  cin >> N;
  cout << N << endl;
  cout << "Is it in vacuum? ";
  cin >> vacuum;
  cout << vacuum << endl;
  cout << "Should we take the air into consideration at fitting? ";
  cin >> airAtFitting;
  cout << airAtFitting << endl;
  int nScatteringLayer = (!vacuum) ? 2*N-1 : N;
  double thickness[nScatteringLayer];
  cout << "Thickness of the planes in order of z in mm:" << endl;
  for (int i=0; i<N; i++)
  {
    cin >> thickness[(!vacuum) ? 2*i : i];
    cout << thickness[(!vacuum) ? 2*i : i] << "\t";
  }
  double resolution[nScatteringLayer];
  cout << endl << "Resolution of the planes in order of z in mm:" << endl;
  for (int i=0; i<N; i++)
  {
    cin >> resolution[(!vacuum) ? 2*i : i];
    cout << resolution[(!vacuum) ? 2*i : i]  << "\t";
  }
  double distance[N-1];
  cout << endl << "Distance of the planes in order of z in mm:" << endl;
  for (int i=0; i<N-1; i++)
  {
    cin >> distance[i];
    cout << distance[i] << "\t";
  }
  int dutID;
  cout << endl << "Which layer is the DUT? ";
  cin >> dutID;
  cout << dutID << endl;
  double energy;
  cout << "Beam energy in MeV: ";
  cin >> energy;
  cout << energy << endl;;
  int nEvent;
  cout << "Number of events: ";
  cin >> nEvent;
  cout << nEvent << endl << endl << endl;
  cout << endl << "The resolution is " << Resolution(N,vacuum,airAtFitting,distance,thickness,resolution,dutID,energy,nEvent)*1000 << " micron." << endl;

  return 0;
}

double Resolution(int nPlanes, bool vacuum, bool airAtFitting, double * inputDist, double * thickness, double * resolution, int dutID, double energy, int nEvent)
{
// Parameters: 
// 1. number of planes (nPlanes)
// 2. Is is in vacuum?
// 3. Do we take the air into account at fitting?
// 4. Distance in Z
// 5. thickness of the planes
// 6. Resolution of the planes
// 7. Which is the Dut
// 8. Beam energy
// 9. Number of events

  static TH1F * residualX = new TH1F("residualX","residualX",100,-0.1,0.1);
  static TH1F * residualY = new TH1F("residualY","residualY",100,-0.1,0.1);
  int nScatteringLayer = (!vacuum) ? 2*nPlanes-1 : nPlanes;
  double Z[nScatteringLayer];
  double radiationLength[nScatteringLayer];

  for (int i=0; i<nPlanes; i++)
    if (i == 0) Z[0] = 0;
    else Z[(!vacuum) ? 2*i : i] = Z[(!vacuum) ? 2*i-2 : i-1]+inputDist[i-1];
  for (int i=1; i<nScatteringLayer  && !vacuum; i+=2)
  {
    Z[i] = Z[i-1] + inputDist[(i-1)/2]/2.0;
    thickness[i] = inputDist[(i-1)/2];
    resolution[i] = -1;
    radiationLength[i] = 3.039e5;
  }
  
  bool passiveLayer[nScatteringLayer];
  for (int i=0; i<nScatteringLayer; i++)
  {
    if (resolution[i] <= 0 || i == ((!vacuum)?(dutID-1)*2:dutID-1)) passiveLayer[i] = true;
    else  passiveLayer[i] = false;
    if (resolution[i] >= 0) radiationLength[i] = 93.688;
  }
  for (int iEvent=0; iEvent<nEvent; iEvent++)
  {
    double xhitpos = gRandom->Uniform(-15.,15.);
    double yhitpos = gRandom->Uniform(-7.65,7.65);
    double measuredX[nScatteringLayer];
    double measuredY[nScatteringLayer];
    double scattering[nScatteringLayer];
    double fitX[nScatteringLayer];
    double fitEX[nScatteringLayer];
    double fitY[nScatteringLayer];
    double fitEY[nScatteringLayer];
    double dTheta[nScatteringLayer];
    bool passiveLayerFit[nScatteringLayer];
    double ZFit[nScatteringLayer];
    double resolutionFit[nScatteringLayer];

    double thetaX = 0;
    double thetaY = 0;
    bool inPlane = true;
    for (unsigned int iPlanes=0; iPlanes<nScatteringLayer; iPlanes++) 
    {
      resolutionFit[iPlanes]=resolution[iPlanes];
      resolutionFit[(!vacuum)?(dutID-1)*2:dutID-1]=0;
      double resolX = resolutionFit[iPlanes];
      double resolY = resolutionFit[iPlanes];
      dTheta[iPlanes] = 13.6/energy*TMath::Sqrt(thickness[iPlanes]/radiationLength[iPlanes])*(1.0+0.038*TMath::Log(thickness[iPlanes]/radiationLength[iPlanes]));
//      cerr << dTheta[iPlanes] << "\t" << dTheta[iPlanes]/3.14*180. << endl;
//      cerr << TMath::Tan(dTheta[iPlanes])*19.6*1000 << endl;
      scattering[iPlanes] = 1.0/(dTheta[iPlanes]*dTheta[iPlanes]);
      if (iPlanes == 0)
      {
        measuredX[0] = xhitpos + gRandom->Gaus(0.0,resolX);
        measuredY[0] = yhitpos + gRandom->Gaus(0.0,resolY);
      }
      else
      {
        thetaX += gRandom->Gaus(0.0,dTheta[iPlanes-1]);
        thetaY += gRandom->Gaus(0.0,dTheta[iPlanes-1]);
        xhitpos += (Z[iPlanes]-Z[iPlanes-1]) * TMath::Tan(thetaX);
        yhitpos += (Z[iPlanes]-Z[iPlanes-1]) * TMath::Tan(thetaY);
        if (xhitpos > 15.0 || xhitpos < -15.0 || yhitpos > 7.65 || yhitpos < -7.65)
        {
          inPlane = false;
          iEvent--;
          break;
        }
        measuredX[iPlanes] = xhitpos + gRandom->Gaus(0.0,resolX);
        measuredY[iPlanes] = yhitpos + gRandom->Gaus(0.0,resolY);
      }
    }
    if (!inPlane) continue;
    int tmp = 0;
    for (unsigned int iPlanes=0; iPlanes<nScatteringLayer; iPlanes++)
    {
      if (airAtFitting || resolutionFit[iPlanes] >= 0) 
      {
        fitX[tmp]=measuredX[iPlanes];
        fitEX[tmp]=resolutionFit[iPlanes];
        fitY[tmp]=measuredY[iPlanes];
        fitEY[tmp]=resolutionFit[iPlanes];
        passiveLayerFit[tmp]=passiveLayer[iPlanes];
        ZFit[tmp]=Z[iPlanes];
        if (!airAtFitting)
        {
          resolutionFit[tmp]=resolutionFit[iPlanes];
          scattering[tmp]=scattering[iPlanes];
          measuredX[tmp]=measuredX[iPlanes];
          measuredY[tmp]=measuredY[iPlanes];
        }
        tmp ++;
      }
    }
  
    int status = 0;
    if (airAtFitting) status = DoAnalFit(nScatteringLayer,passiveLayerFit,ZFit,scattering,fitX,fitEX);
    else              status = DoAnalFit(nPlanes,passiveLayerFit,ZFit,scattering,fitX,fitEX);
    if(status) 
    {
      cerr << "Fit failed" << endl;
      return 0;
    }
    if (airAtFitting) status = DoAnalFit(nScatteringLayer,passiveLayerFit,ZFit,scattering,fitY,fitEY);
    else              status = DoAnalFit(nPlanes,passiveLayerFit,ZFit,scattering,fitY,fitEY);
    if(status) 
    {
      cerr << "Fit failed" << endl;
      return 0;
    }
    double result = (fitEX[((!vacuum&&airAtFitting)?(dutID-1)*2:dutID-1)]+fitEY[((!vacuum&&airAtFitting)?(dutID-1)*2:dutID-1)])/2.;
    if (nEvent == 1) return result;
    residualX->Fill(fitX[((!vacuum&&airAtFitting)?(dutID-1)*2:dutID-1)]-measuredX[((!vacuum&&airAtFitting)?(dutID-1)*2:dutID-1)]);
    residualY->Fill(fitY[((!vacuum&&airAtFitting)?(dutID-1)*2:dutID-1)]-measuredY[((!vacuum&&airAtFitting)?(dutID-1)*2:dutID-1)]);
  }
  TFitResultPtr rx = residualX->Fit("gaus","SQ");
  TFitResultPtr ry = residualY->Fit("gaus","SQ");
  residualX->Reset();
  residualY->Reset();
  return (rx->Parameter(2)+ry->Parameter(2))/2.;
}

int DoAnalFit(int nPlanes, bool * passiveLayer, double * Z, double * scattering,  double * pos, double *err)
{
  double inverseDist[nPlanes];
  for(int ipl=0; ipl<nPlanes;ipl++)
  {
    if(!passiveLayer[ipl] && err[ipl]>0)
      err[ipl]=1./err[ipl]/err[ipl] ;
    else
      err[ipl] = 0. ;

    pos[ipl]*=err[ipl];
    if (ipl > 0) inverseDist[ipl-1] = 1.0/(Z[ipl]-Z[ipl-1]);
  }

  double _fitArray[nPlanes*nPlanes];
  for(int ipl=0; ipl<nPlanes;ipl++)
  {
    for(int jpl=0; jpl<nPlanes;jpl++)
      {
        int imx=ipl+jpl*nPlanes;


        _fitArray[imx] = 0.;

        if(jpl==ipl-2)
          _fitArray[imx] += inverseDist[ipl-2]*inverseDist[ipl-1]*scattering[ipl-1] ;

        if(jpl==ipl+2)
          _fitArray[imx] += inverseDist[ipl]*inverseDist[ipl+1]*scattering[ipl+1] ;

        if(jpl==ipl-1)
          {
            if(ipl>0 &&  ipl < nPlanes-1)
              _fitArray[imx] -= inverseDist[ipl-1]*(inverseDist[ipl]+inverseDist[ipl-1])*scattering[ipl] ;
            if(ipl>1)
              _fitArray[imx] -= inverseDist[ipl-1]*(inverseDist[ipl-1]+inverseDist[ipl-2])*scattering[ipl-1] ;
          }

        if(jpl==ipl+1)
          {
            if(ipl>0 && ipl < nPlanes-1)
              _fitArray[imx] -= inverseDist[ipl]*(inverseDist[ipl]+inverseDist[ipl-1])*scattering[ipl] ;
            if(ipl < nPlanes-2)
              _fitArray[imx] -= inverseDist[ipl]*(inverseDist[ipl+1]+inverseDist[ipl])*scattering[ipl+1] ;
          }

        if(jpl==ipl)
          {
            _fitArray[imx] += err[ipl] ;

            if(ipl>0 && ipl<nPlanes-1)
              _fitArray[imx] += scattering[ipl]*(inverseDist[ipl]+inverseDist[ipl-1])*(inverseDist[ipl]+inverseDist[ipl-1]) ;

            if(ipl > 1 )
              _fitArray[imx] += scattering[ipl-1]*inverseDist[ipl-1]*inverseDist[ipl-1] ;

            if(ipl < nPlanes-2)
              _fitArray[imx] += scattering[ipl+1]*inverseDist[ipl]*inverseDist[ipl] ;
          }

      }
  }

  int status=GaussjSolve(_fitArray,pos,nPlanes) ;

  if(status)
    {
      cerr << "Singular matrix in track fitting algorithm ! " << endl;
      for(int ipl=0;ipl<nPlanes;ipl++)
        err[ipl]=0. ;
    }
  else
    for(int ipl=0;ipl<nPlanes;ipl++)
      err[ipl]=sqrt(_fitArray[ipl+ipl*nPlanes]);

  return status ;
}

int GaussjSolve(double *alfa,double *beta,int n)
{
  int *ipiv;
  int *indxr;
  int *indxc;
  int i,j,k;
  int irow=0;
  int icol=0;
  double abs,big,help,pivinv;

  ipiv = new int[n];
  indxr = new int[n];
  indxc = new int[n];

  for(i=0;i<n;i++)ipiv[i]=0;

  for(i=0;i<n;i++)
    {
      big=0.;
      for(j=0;j<n;j++)
        {
          if(ipiv[j]==1)continue;
          for(k=0;k<n;k++)
            {
              if(ipiv[k]!=0)continue;
              abs=fabs(alfa[n*j+k]);
              if(abs>big)
                {
                  big=abs;
                  irow=j;
                  icol=k;
                }
            }
        }
      ipiv[icol]++;

      if(ipiv[icol]>1){
        delete[] ipiv;
        delete[] indxr;
        delete[] indxc;
        return 1;
      }

      if(irow!=icol)
        {
          help=beta[irow];
          beta[irow]=beta[icol];
          beta[icol]=help;
          for(j=0;j<n;j++)
            {
              help=alfa[n*irow+j];
              alfa[n*irow+j]=alfa[n*icol+j];
              alfa[n*icol+j]=help;
            }
        }
      indxr[i]=irow;
      indxc[i]=icol;

      if(alfa[n*icol+icol]==0.){
        delete[] ipiv;
        delete[] indxr;
        delete[] indxc;
        return 1;}

      help=alfa[n*icol+icol];
      pivinv=1./help;
      alfa[n*icol+icol]=1.;
      for(j=0;j<n;j++) alfa[n*icol+j]*=pivinv;

      beta[icol]*=pivinv;

      for(j=0;j<n;j++)
        {
          if(j==icol)continue;
          help=alfa[n*j+icol];
          alfa[n*j+icol]=0.;
          for(k=0;k<n;k++)
            alfa[n*j+k]-=alfa[n*icol+k]*help;
          beta[j]-=beta[icol]*help;
        }
    }

  for(i=n-1;i>=0;i--)
    {
      if(indxr[i]==indxc[i])continue;
      for(j=0;j<n;j++)
        {
          help=alfa[n*j+indxr[i]];
          alfa[n*j+indxr[i]]=alfa[n*j+indxc[i]];
          alfa[n*j+indxc[i]]=help;
        }
    }

  delete [] ipiv;
  delete [] indxr;
  delete [] indxc;

  return 0;
}
