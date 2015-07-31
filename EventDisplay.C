//macro to display clusters in all the planes in one event, it nneds the file run-RUNNUMBER-converter.slcio that is created only running the processing in DEBUG mode


#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TObject.h>
#include <TROOT.h>
#include <stdio.h>
#include <stdlib.h>
#include <TCanvas.h>
#include "TString.h"
#include  "TAxis.h"
#include <TSystem.h>
#include <TSystemDirectory.h>
#include "TMath.h"
#include "TPad.h"
#include <TF1.h>
#include <TF2.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH2C.h>
#include <TH2D.h>
#include <TFile.h>
#include <TRandom.h>
#include <TRootEmbeddedCanvas.h>
#include <TGraph.h>
#include "TTimer.h"
#include "TLine.h"
#include "TDatime.h"
#include <TApplication.h>
#include "TColor.h"
#include <iostream>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TGClient.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGButtonGroup.h>
#include <TG3DLine.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TMath.h>
#include <TStyle.h>
#include <vector>
//#include <sys/stat.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <sstream>
#include <fstream>
#include "TText.h"
#include "TLegend.h"
//#include "linkdef.h"
//#include "dict.h"

using namespace std;

//_________________________________________________________________________________________________
//// Marco  to view location of clusters in planes of telescope
//
////1) LOAD ENVIRONMENT (needed by event dump command)
////2) root EventDisplay.C
////3) to row "Input file name"  give path to the   lcio/run00????-converter.slcio from the run
////   that is to be scanned
////
//// XL,XU, YL, YU  + Zoom     make zoom in to the region of interest  rectange  
////  lower left corner[XL,YL]    upper right corner [XU,YU]         
//

//_________________________________________________________________________________________________
//                         GLOBAL DEFINITIONS
//flags when opening/reading input file :
enum {kEmpty=-1,kOldFile, kNewFile};
//flags of histograms

const Int_t kRow   = 4; 
const Int_t kCol   = 2; 
const Int_t kPlanes = 7; //Number of telescope planes 
const Int_t kPixX  = 1024; //Number of pixels of the chip along X
const Int_t kPixY  = 512; //Number of pixles of the chip along Y 
const Int_t kSectorPixX  = kPixX/4; 

//__________________________________________________________________________________________________
//                          AUXILIARY CLASSES

////////////////////////////////////////////////////////////////
class TextMargin : public TGHorizontalFrame {
    //class used to draw text + number entry input field
   public:
      TextMargin(const TGWindow *p, const char *name, Float_t defaultVal, Int_t arithmetics) : TGHorizontalFrame(p){
          if(arithmetics==0){ //box with integer inputs
             fEntry = new TGNumberEntry(this, TMath::Nint(defaultVal), 6, -1, TGNumberFormat::kNESInteger);
          }else{  //box with real number inputs
             fEntry = new TGNumberEntry(this, defaultVal, 6, -1, TGNumberFormat::kNESReal);
          }
          AddFrame(fEntry, new TGLayoutHints(kLHintsLeft,0,0,0,0));
          TGLabel *label = new TGLabel(this, name);
          AddFrame(label, new TGLayoutHints(kLHintsLeft, 10));
      }

      TGTextEntry *GetEntry() const { return fEntry->GetNumberEntry(); }

   protected:
      TGNumberEntry *fEntry;  //number entry

   ClassDef(TextMargin, 0)
};
//
//_________________________________________________________________________
//                     MAIN GUI FRAME CLASS

class MyMainFrame {
   RQ_OBJECT("MyMainFrame")  // provides a standalone class signal/slot capability
		//not needed if MyMainFrame inherits from TGMainFrame
   public:
      MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
      virtual ~MyMainFrame();

      //--------------- Member functions -----------------
      void  ReadEvent(Bool_t bButton=kTRUE); 
      void  NextEvent();
      void  PreviousEvent(); 

      void  SetEventNumber(char* d);  //Set event number
      void  SetRangeLowX(char* d);//Zoom ranges
      void  SetRangeUpX(char* d);
      void  SetRangeLowY(char* d);
      void  SetRangeUpY(char* d);


      void    UpdateCanvas();  //Drawing functions
      void    Zoom();        //unzoom histograms
      void    Unzoom();        //unzoom histograms
      void    Print();         //print figures 
      void    Exit();

      void AdjustSectorBorders(Int_t b1, Int_t b2);

      //--------------- Data members ---------------
      TString   fInputFileName; //input file name
      FILE     *fileInput;      //input file pointer

      //Frames
      TGMainFrame *fMainFrame;  //mother level frame
      UInt_t       fMainFrameHeightPx;    //mother frame height in pixels
      UInt_t       fMainFrameWidthPx;     //mother frame width in pixels

      TGVerticalFrame *fVFrame;//arrange mother frame as vertical frame

      //Daughter 2.nd gen frames  from top frame
      TGGroupFrame *fButtonsVFrame;   //Exit/Reset part

      TGHorizontalFrame *fX0HFrame; // 
      TGHorizontalFrame *fX1HFrame; //
      TGHorizontalFrame *fX2HFrame; //
      TGHorizontalFrame *fX3HFrame; //
      TGHorizontalFrame *fX4HFrame; //

      TGLabel *fLabelRunNumber;     
      TGLabel *fLabelEventNumber;     

      TextMargin   *fEventNumberEntry;
      TGTextButton *fReadEvent;  
      TGTextButton *fReadNextEvent; 
      TGTextButton *fReadPreviousEvent; 

      TGTextButton *fZoom;  
      TextMargin   *fSetterRgLowX;
      TextMargin   *fSetterRgUpX;
      TextMargin   *fSetterRgLowY;
      TextMargin   *fSetterRgUpY;

      TGTextButton *fExit;   //kill gui
      TGTextButton *fUnzoom; //unzoom histograms
      TGTextButton *fPrint; //print histograms

      TGGroupFrame *fInHFrame;
      TGTextEntry *fInputFileBox; //sets input file name

      //Parameters
      Int_t  fRunNumber;    //Run Number
      Int_t  fEventNumber;  //Event Number

      //histograms
      TH2I   *fDataRaw[kPlanes]; //raw data [1024x512]
      TCanvas *cPlane[kPlanes];
      TLegend *leg[kPlanes];

      //Zoom ranges for drawing 
      Int_t fRangeLowX;
      Int_t fRangeUpX;
      Int_t fRangeLowY;
      Int_t fRangeUpY;
 
      //sector borders
      TLine *fLine01;
      TLine *fLine12;
      TLine *fLine23;

};
//_____________________________________________________________________

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h):
   fInputFileName(""),
   fileInput(),
   fMainFrameHeightPx(h),
   fMainFrameWidthPx(w),
   fRunNumber(-1),
   fEventNumber(1),
   fRangeLowX(0),
   fRangeUpX(kPixX),
   fRangeLowY(0),
   fRangeUpY(kPixY)
{
   //constructor
   TString name,title;
   gStyle->SetPalette(1);

   //raw data histogram
   for(Int_t ip=0; ip<kPlanes; ip++){
      name = Form("hDataRaw%d",ip);
      fDataRaw[ip] = new TH2I(name,name,kPixX,0,kPixX,kPixY,0,kPixY);
      fDataRaw[ip]->GetXaxis()->SetTitle("X Pixel");
      fDataRaw[ip]->GetYaxis()->SetTitle("Y Pixel");
      fDataRaw[ip]->GetYaxis()->SetTitleOffset(1.3);
      fDataRaw[ip]->GetZaxis()->SetTitleOffset(100);
      fDataRaw[ip]->GetZaxis()->SetTitle(Form("PLANE %d",ip));


      leg[ip] = new TLegend(0.2,0.9,0.5,0.95," ","brNDC");
      leg[ip]->SetFillStyle(0);  
      leg[ip]->SetBorderSize(0);  
      leg[ip]->SetTextSize(0.041);
      leg[ip]->SetHeader(Form("Plane %d / Run %d / Event %d",ip,fRunNumber,fEventNumber));
   }

   //===========================================================
   //       Defalut drawing in canvases
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);


   for(Int_t ip=0; ip<kPlanes; ip++){
      name  = Form("cPlane%d",ip);
      title = Form("Chip plane %d",ip);
      cPlane[ip] = new TCanvas(name.Data(),title.Data(),250,150,kPixX,  kPixY );
      cPlane[ip]->cd()->SetGridy();
      cPlane[ip]->cd()->SetGridx();
      cPlane[ip]->cd()->SetTicky();
      cPlane[ip]->cd()->SetTickx();
      cPlane[ip]->SetMargin(0.1,0.03,0.1,0.03);
      cPlane[ip]->Draw(); 
 
   }


   fLine01 = new TLine(kSectorPixX,0,kSectorPixX, kPixY); 
   fLine12 = new TLine(2*kSectorPixX,0,2*kSectorPixX, kPixY);
   fLine23 = new TLine(3*kSectorPixX,0,3*kSectorPixX, kPixY);
   fLine01->SetLineColor(4); 
   fLine12->SetLineColor(4); 
   fLine23->SetLineColor(4);




   //Define placement of gadgets of GUI
  

   //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   //Mother Frame
   fMainFrame = new TGMainFrame(p,fMainFrameWidthPx,fMainFrameHeightPx, kFitHeight | kFitWidth);

   //Daughters of BOTTOM Level 2 horizontal frame
   fButtonsVFrame = new TGGroupFrame(fMainFrame,"Settings",kVerticalFrame);
   fButtonsVFrame->SetTitlePos(TGGroupFrame::kCenter);

   fX0HFrame = new TGHorizontalFrame(fButtonsVFrame,fMainFrameWidthPx,fMainFrameHeightPx/4, kFitHeight | kFitWidth);

   fX1HFrame = new TGHorizontalFrame(fButtonsVFrame,fMainFrameWidthPx,fMainFrameHeightPx/4, kFitHeight |kFitWidth);
   fX2HFrame = new TGHorizontalFrame(fButtonsVFrame,fMainFrameWidthPx,fMainFrameHeightPx/4, kFitHeight |kFitWidth);
   fX3HFrame = new TGHorizontalFrame(fButtonsVFrame,fMainFrameWidthPx,fMainFrameHeightPx/4, kFitHeight |kFitWidth);
   fX4HFrame = new TGHorizontalFrame(fButtonsVFrame,fMainFrameWidthPx,fMainFrameHeightPx/4, kFitHeight |kFitWidth);
 
   //X0
   fLabelRunNumber = new TGLabel(fX0HFrame,  Form("Run %d", fRunNumber));
   fLabelEventNumber = new TGLabel(fX0HFrame,  Form("Event %d", fEventNumber));

   //X1
   fEventNumberEntry = new TextMargin(fX1HFrame, "Event",fEventNumber,0); 
   fEventNumberEntry->GetEntry()->Connect("TextChanged(char*)", "MyMainFrame", this, "SetEventNumber(char*)");
   fReadEvent = new TGTextButton(fX1HFrame,"&Read Event");
   fReadEvent->Connect("Clicked()","MyMainFrame",this,"ReadEvent(Bool_t)");
   fReadNextEvent = new TGTextButton(fX1HFrame,"&Prev.Event");
   fReadNextEvent->Connect("Clicked()","MyMainFrame",this,"PreviousEvent()");
   fReadPreviousEvent = new TGTextButton(fX1HFrame,"&Next Event");
   fReadPreviousEvent->Connect("Clicked()","MyMainFrame",this,"NextEvent()");


   //X2
   fZoom = new TGTextButton(fX2HFrame,"Zoom");
   fZoom->Connect("Clicked()","MyMainFrame",this,"Zoom()");
   fSetterRgLowX = new TextMargin(fX2HFrame, "XL",fRangeLowX,0); 
   fSetterRgLowX->GetEntry()->Connect("TextChanged(char*)", "MyMainFrame", this, "SetRangeLowX(char*)");
   fSetterRgUpX = new TextMargin(fX2HFrame, "XU",fRangeUpX,0); 
   fSetterRgUpX->GetEntry()->Connect("TextChanged(char*)", "MyMainFrame", this, "SetRangeUpX(char*)");
   fSetterRgLowY = new TextMargin(fX2HFrame, "YL",fRangeLowY,0); 
   fSetterRgLowY->GetEntry()->Connect("TextChanged(char*)", "MyMainFrame", this, "SetRangeLowY(char*)");
   fSetterRgUpY = new TextMargin(fX2HFrame, "YU",fRangeUpY,0); 
   fSetterRgUpY->GetEntry()->Connect("TextChanged(char*)", "MyMainFrame", this, "SetRangeUpY(char*)");
 

   //X3
   fExit = new TGTextButton(fX3HFrame,"&Exit");
   fExit->Connect("Clicked()","MyMainFrame",this,"Exit()");
   fUnzoom = new TGTextButton(fX3HFrame,"&Unzoom Histograms");
   fUnzoom->Connect("Clicked()","MyMainFrame",this,"Unzoom()");
   fPrint = new TGTextButton(fX3HFrame,"&Print");
   fPrint->Connect("Clicked()","MyMainFrame",this,"Print()");


   //Daughers of fRow3HFrame
   fInHFrame = new TGGroupFrame(fX4HFrame,"Input File Name",kHorizontalFrame|kFitHeight | kFitWidth);
   fInHFrame->SetTitlePos(TGGroupFrame::kLeft);

   fInputFileBox = new TGTextEntry(fInHFrame);


   //-------------------------------------------------------------
   // Define hierarchy of frames in GUI

   /////////////////////////
   //box with input file name

   fX0HFrame->AddFrame(fLabelRunNumber, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX0HFrame->AddFrame(fLabelEventNumber, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));

   fX1HFrame->AddFrame(fReadEvent, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX1HFrame->AddFrame(fEventNumberEntry, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX1HFrame->AddFrame(fReadNextEvent, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX1HFrame->AddFrame(fReadPreviousEvent, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
 
   fX2HFrame->AddFrame(fZoom, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX2HFrame->AddFrame(fSetterRgLowX, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX2HFrame->AddFrame(fSetterRgUpX, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX2HFrame->AddFrame(fSetterRgLowY, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX2HFrame->AddFrame(fSetterRgUpY, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
 
   fX3HFrame->AddFrame(fUnzoom, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX3HFrame->AddFrame(fPrint, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX3HFrame->AddFrame(fExit, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));

   fInHFrame->AddFrame(fInputFileBox, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   fX4HFrame->AddFrame(fInHFrame, new TGLayoutHints(kLHintsExpandX, 1,1,1,1));
   //Bottom
   fButtonsVFrame->AddFrame(fX0HFrame, new TGLayoutHints(kLHintsExpandX , 2,2,2,2));
   fButtonsVFrame->AddFrame(fX1HFrame, new TGLayoutHints(kLHintsExpandX , 2,2,2,2));
   fButtonsVFrame->AddFrame(fX2HFrame, new TGLayoutHints(kLHintsExpandX , 2,2,2,2));
   fButtonsVFrame->AddFrame(fX3HFrame, new TGLayoutHints(kLHintsExpandX , 2,2,2,2));
   fButtonsVFrame->AddFrame(fX4HFrame, new TGLayoutHints(kLHintsExpandX, 2,2,2,2));
  /////////////////////////
   fMainFrame->AddFrame(fButtonsVFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 0,0,0,0));

   // Set a name to the main frame
   fMainFrame->SetWindowName("EVENT DISPLAY");
   // Map all subwindows of main frame
   fMainFrame->MapSubwindows();
   // Initialize the layout algorithm
   fMainFrame->Layout();
   // Map main frame
   fMainFrame->MapWindow();

  
   UpdateCanvas();

}
//_______________________________________________________________
void MyMainFrame::Print(){
 
   cout<<"TImage *img = TImage::Create();"<<endl;
   for(Int_t ip=0; ip<kPlanes; ip++){
      TString name  = Form("circ_run%d_event%d_chip%d.png", fRunNumber, fEventNumber,ip);
      cout<<"img->FromPad("<<cPlane[ip]->GetName()<<"); img->WriteImage(\""<<name.Data()<<"\");"<<endl;
   } 
}
//_______________________________________________________________
void MyMainFrame::SetEventNumber(char* d){
   fEventNumber = (Int_t) (atoi(d));
}
//_______________________________________________________________
void  MyMainFrame::SetRangeLowX(char* d){
   fRangeLowX = (Int_t) (atoi(d));
}
//_______________________________________________________________
void  MyMainFrame::SetRangeUpX(char* d){
   fRangeUpX = (Int_t) (atoi(d));
}
//_______________________________________________________________
void  MyMainFrame::SetRangeLowY(char* d){
   fRangeLowY = (Int_t) (atoi(d));
}
//_______________________________________________________________
void  MyMainFrame::SetRangeUpY(char* d){
   fRangeUpY = (Int_t) (atoi(d));
}
//_______________________________________________________________
void MyMainFrame::UpdateCanvas(){

   for(Int_t ip=0;ip<kPlanes;ip++){
      cPlane[ip]->cd();
      fDataRaw[ip]->Draw("col");
      leg[ip]->Draw();

      fLine01->Draw();
      fLine12->Draw();
      fLine23->Draw();

      cPlane[ip]->Modified();
      cPlane[ip]->Update();
   }

}
//_______________________________________________________________
void  MyMainFrame::NextEvent(){
   fEventNumber++;
   ReadEvent(kFALSE);
} 
//_______________________________________________________________
void  MyMainFrame::PreviousEvent(){
   fEventNumber--;
   ReadEvent(kFALSE);
}
//_______________________________________________________________
void MyMainFrame::ReadEvent(Bool_t bButton){

   if(bButton){
      fEventNumber = (Int_t) (atoi(fEventNumberEntry->GetEntry()->GetText()));
   }

   for(Int_t ip=0; ip<kPlanes; ip++){
      fDataRaw[ip]->Reset();
   }

   //Call event dump command
   TString filename    = fInputFileBox->GetText();
   TString command = Form("dumpevent %s %d|awk '/2chargeADC/'|cut -c57-100000000|sed 's/,sparsePixelType:2chargeADC : /,/'|sed 's/,/ /g' > /tmp/eventPA.txt",filename.Data(),fEventNumber+1);


   gSystem->Exec(command);


   string line;
   int x,y,a,b;
   int plane;

   ifstream myfile("/tmp/eventPA.txt",ios::in);
   if(!myfile.good()) return;

   if(myfile.is_open()){
      for(Int_t ip=0; ip < kPlanes; ip++){
         getline (myfile,line);
         istringstream iss(line);
         iss >> plane;

         while(iss>>x>>y>>a>>b){
            fDataRaw[plane]->Fill(x,y);   
	    //	    cout<<"x "<<x<<" y "<<y<<endl;
	 }
      }
      myfile.close();
   }


   fLabelEventNumber->SetText(Form("Event %d",fEventNumber));

   //Extract run number
   // /afs/cern.ch/work/f/fkrizek/EUTELESCOPE20150330/analysisBTF2394/converter/run002394-converter.slcio
   TObjArray* s = filename.Tokenize("//");
  
   TString sx  = ((TObjString*) (s->At(s->GetEntriesFast()-1)))->GetString();
   cout<<sx.Data()<<endl;
   TObjArray *t  = sx.Tokenize("-"); 
   TString tx  = ((TObjString*) (t->At(0)))->GetString();
   tx.ReplaceAll("run00","");
   fRunNumber = tx.Atoi();
  
   fLabelRunNumber->SetText(Form("Run %d",fRunNumber));


   for(Int_t ip=0;ip<kPlanes;ip++){
      leg[ip]->SetHeader(Form("Plane %d / Run %d / Event %d",ip,fRunNumber,fEventNumber));
   }
   
   UpdateCanvas();
}
//_______________________________________________________________
void MyMainFrame::Zoom(){ 
   //zoom histograms
   for(Int_t ip=0;ip<kPlanes;ip++){
      fDataRaw[ip]->GetXaxis()->SetRange(fRangeLowX, fRangeUpX);
      fDataRaw[ip]->GetYaxis()->SetRange(fRangeLowY, fRangeUpY);
   }

   AdjustSectorBorders(fRangeLowY, fRangeUpY);

   UpdateCanvas();

}
//_______________________________________________________________
void MyMainFrame::Unzoom(){
   //unzoom histograms
   for(Int_t ip=0;ip<kPlanes;ip++){
      fDataRaw[ip]->GetXaxis()->UnZoom();
      fDataRaw[ip]->GetYaxis()->UnZoom();
   }

   AdjustSectorBorders(0, kPixY);
   UpdateCanvas();

}
//_______________________________________________________________
MyMainFrame::~MyMainFrame(){

   Exit();
}
//_______________________________________________________________
void MyMainFrame::Exit(){
   //close everything and cleanup
//   if(fileInput) fclose(fileInput);  //close old file
//   delete [] fDataRaw; //raw data cleaned from bg clusters

   gApplication->Terminate(0);

   return;
}
//_______________________________________________________________
void MyMainFrame::AdjustSectorBorders(Int_t b1, Int_t b2){
   fLine01->SetY1(b1);
   fLine12->SetY1(b1);
   fLine23->SetY1(b1);
   fLine01->SetY2(b2);
   fLine12->SetY2(b2);
   fLine23->SetY2(b2);
}
//#####################################################################
//              main code
void EventDisplay(){

   new MyMainFrame(gClient->GetRoot(),600,200);


}


