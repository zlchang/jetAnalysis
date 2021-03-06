const int mNTRG = 3;
char* mTRGNAME[] = {"JP0", "JP1", "JP2"};

TFile *fdata;
TFile *fembed;
TFile *fout;
class MyDrawObj;

int compareHist(const char *filedata = "run12.final.v2.list.ue.v1.0.crs.root", 
               const char *fileembed = "ptbin.list.run12.l.v1.w.cmb.crs.root", 
		char *ver = "CrsV0")
{
  gSystem->Load("StMyHistUtil.so");
  gStyle->SetOptStat(0);
  //const int Ntrg = 3;
  //char *trg[] = {"JP0", "JP1", "JP2"};

  //const double ptbin[16] = {5.0, 6.0, 7.0, 8.2, 9.6, 11.2, 13.1, 15.3, 17.9, 20.9, 24.5, 28.7, 33.6, 39.3, 46.0, 53.8};

  fout = new TFile(Form("data.mc.crs.compare.%s.root", ver), "recreate");
  TH1::SetDefaultSumw2(kTRUE);
  fdata = TFile::Open(Form("%s", filedata));
  fembed = TFile::Open(Form("%s", fileembed));

  plot("JetPt", ver);
  MyDrawObj rt(0, 0.8);
  plotProfile("JetRtVsPt", ver, rt);
  MyDrawObj twrMult(4, 24);
  plotProfile("JetTowerMultVsPt", ver, twrMult);
  MyDrawObj trkMult(0, 12);
  plotProfile("JetTrackMultVsPt", ver, trkMult);
  //fembed->Close();
  //fdata->Close();
  //fout->Write();
  //fout->Close();
  return 1;
}
void plot(char *name, char* ver)
{
  int ncx = mNTRG/2+1; int ncy = (mNTRG+1)/ncx;
  TCanvas *c = new TCanvas(Form("c%s", name), Form("c%s", name), ncx*400, ncy*300);
  c->Divide(ncx, ncy);
  for(int i = 0; i < mNTRG; i++){
    //pt
    TH1F *hd = (TH1F *)fdata->Get(Form("%s%s", mTRGNAME[i], name));
    if(!hd) continue;
    hd->Print();
    TH1F *hdc = normHistogram(hd);
    TH1F *he = (TH1F *)fembed->Get(Form("%s%s", mTRGNAME[i], name));
    TH1F *hec = normHistogram(he);
    TH1F *hjetpt = compareHistogram(hdc, hec);
    //hjetpt->Print("all");
    c->cd(i+1);
    TPad *p1 = new TPad(Form("p1%s%s", mTRGNAME[i], name), Form("p1%s%s", mTRGNAME[i], name), 0, 0.4, 1, 1);
    p1->Draw();
    TPad *p2 = new TPad(Form("p2%s%s", mTRGNAME[i], name), Form("p2%s%s", mTRGNAME[i], name), 0, 0, 1, 0.4);
    //TPad p2("p1", "p1", 0, 0, 1, 0.4);
    p2->Draw();
    p1->SetLogy(1);
    drawHistRatio(p1, p2, hd, he, hjetpt, 0, -1, 1e-8, 1, 0.4);
    //c.Print(Form("%s.jet.ratio.%s.pdf(", mTRGNAME[i], ver));
  }
  c->Print(Form("%sRatio%s.png", name, ver));

}
void plotProfile(char *name, char* ver, MyDrawObj obj)
{
  int ncx = mNTRG/2+1; int ncy = (mNTRG+1)/ncx;
  TCanvas *c = new TCanvas(Form("c%s", name), Form("c%s", name), ncx*400, ncy*300);
  c->Divide(ncx, ncy);
  for(int i = 0; i < mNTRG; i++){
    //pt
    TProfile *hd = (TProfile *)fdata->Get(Form("%s%s", mTRGNAME[i], name));
    if(!hd) continue;
    //hd->Print();
    TProfile *he = (TProfile *)fembed->Get(Form("%s%s", mTRGNAME[i], name));
    TH1D *hr = compareProfile(hd, he, true);
    //hr->Print("all");
    c->cd(i+1);
    TPad *p1 = new TPad(Form("p1%s%s", mTRGNAME[i], name), Form("p1%s%s", mTRGNAME[i], name), 0, 0.4, 1, 1);
    p1->Draw();
    TPad *p2 = new TPad(Form("p2%s%s", mTRGNAME[i], name), Form("p2%s%s", mTRGNAME[i], name), 0, 0, 1, 0.4);
    //TPad p2("p1", "p1", 0, 0, 1, 0.4);
    p2->Draw();
    //p1->SetLogy(1);
    double ymin = obj.ymin;
    double ymax = obj.ymax;
    Printf("ymin=%lf ymax=%lf\n", ymin, ymax);
    drawHistRatio(p1, p2, hd, he, hr, 0, -1, ymin, ymax, 0.6, false);
    //hr->Draw("pE");
    //hr->SetMarkerStyle(20);
    //c.Print(Form("%s.jet.ratio.%s.pdf(", mTRGNAME[i], ver));
  }
  c->Print(Form("%sRatio%s.png", name, ver));
}
/*
void trimHistogram(TH1D *h, double xmin = 0.1)
{
  for(int i = 1; i <= h->GetNbinsX(); i++){
    double lowedge = h->GetBinLowEdge(i);
    double width = h->GetBinWidth(i);
    if(lowedge + width < xmin){
      h->SetBinContent(i, 0);
      h->SetBinError(i, 0);
    }
  }
}
*/
