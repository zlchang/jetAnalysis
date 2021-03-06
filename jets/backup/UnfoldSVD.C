TFile *mFile;
TFile *mFileTest;
TFile *mFilePy;
TFile *mFileOut;
const int Ntrg = 4;

const char *trg[] = {"JP0", "JP1", "JP2", "Cmb"};

const char *par = "Particle";
TH2D *hdetVspar;
TH2D *hdetVsparTest;
//TH1D *hdet;
//TH1D *hproj;
TH1D *hpar;
//TH1D *hparTest;
//
TH2D *hparPy;

TH1D *hjet;
//TH2D *htwojet;

int i = 3;
int Ntrun[] = {1, 3, 5, 1};
int Mtrun[] = {12, 15, 20, 20};
int Nb = Mtrun[i] - Ntrun[i] + 1;
TArrayD *xbins;
TArrayD *ybins;

int dnshift = 0;
int upshift = 0;

int step = 1;

int Nrep = 100;
bool mFlag = true;

//particle 21; parton 18
int mReg = 15;
//cmb
//double fReg = 0.01;
double fReg = 1;

TCanvas *cRes;
TCanvas *cBias;

int UnfoldSVD(//const char *file = "ptbin.list.run12.l.v2.w.cmb.crs.root",
	      //const char* testfile = "run12.final.v2.list.v0.1.crs.root",
	      //const char* pyfile = "ptbin.list.run12.l.v1.w.cmb.pmc.root",
              const char *file = "ptbin.list.run12.l.v1.w.cmb.crs.root",
              const char *testfile = "ptbin.list.run12.l.v1.w.cmb.crs.root",
	      //const char* testfile = "run12.final.v2.list.ue.v1.0.crs.root",
	      const char* pyfile = "ptbin.list.run12.l.ue.v1.w.cmb.pmc.root",
	      const char* ver = "UeV1"
	      )
{
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  TH1::SetDefaultSumw2(kTRUE);
  gStyle->SetOptStat(0);
  gStyle->SetHistLineWidth(2);

  gSystem->Load("StMyJetUnfold");
  gSystem->Load("StMyJetUtilities");
  //gSystem->Load("/star/u/zchang/software/local/lib/libRooUnfold.so");
  
  mFile = TFile::Open(file);
  mFileTest = TFile::Open(testfile);
  mFilePy = TFile::Open(pyfile);
  
  mFileOut = new TFile(Form("%s%s%sUnfoldSVD.root", trg[i], par, ver), "recreate");

  hdetVspar = (TH2D*) mFile->Get(Form("%sJetPtVs%sJetPtRes", trg[i], par));
  if(!hdetVspar) { Info("Read", Form("histogram %sJetPtVs%setPt not found", trg[i], par)); return 0;}
  //hdetVspar->Print("all");
  //TMatrixD res = histToMatrixTrunc2d(hdetVspar, Ntrun[i], Mtrun[i], 2);  
  //TMatrixD resOrig = histToMatrix(hdetVspar);
  //resOrig.Print();  
  TMatrixD res = histToMatrixTrunc2dn(hdetVspar, Ntrun[i], Mtrun[i], Ntrun[i]+dnshift, Mtrun[i]-upshift, 2, mFlag);  
  res.Print();
  //error on the response matrix
  TMatrixD resErr = histToMatrixTruncError2dn(hdetVspar, Ntrun[i], Mtrun[i], Ntrun[i]+dnshift, Mtrun[i]-upshift, 2, mFlag);  
  resErr.Sqrt();
  resErr.Print();
  //
  //hpar = hdetVspar->ProjectionY(Form("%sParticle", hdetVspar->GetName()));
  //hpar->Print("all");
  //TVectorD x = histToVector2d(hpar, Ntrun[i]+dnshift, Mtrun[i]-upshift, 2, mFlag);  
  //TMatrixD x = histToMatrix2d(hpar, Ntrun[i], Mtrun[i], 2); 
  TVectorD x = projCols(res); 
  //efficiency
  hparPy = (TH2D*) mFilePy->Get(Form("%sJetEta", par));
  //hparPy->Print("all");
  if(!hparPy) { Info("Read", Form("histogram %sJetEta not found", par)); return 0;}
  TVectorD veff = effPar(hdetVspar, hparPy);
  TH1D *heff = vectorToHist(veff, "Efficiency");
  ElementDiv(x, veff);
  //cout<<"x: "<<endl;
  x.Print();
  //response probability raw train
  TMatrixD resp(res);
  resp.NormByRow(x);
  TMatrixD resErrp(resErr);
  resErrp.NormByRow(x);

  TCanvas *ceff = new TCanvas("ceff", "ceff");
  ceff->cd();heff->Draw();
  heff->GetXaxis()->SetTitle("particle jet p_{T} bin index");
  ceff->Print(Form("%s%s%sEff.png", trg[i],par,ver));
 

  //matrixToHist(resp)->Draw("colz");
  //matching ratios
  //TH1D *hdet = hdetVspar->ProjectionX(Form("%sDetector", hdetVspar->GetName()));
  //TVectorD vdet = histToVector2d(hdet, Ntrun[i], Mtrun[i], 2, mFlag);
  TH2D *hjetdet = (TH2D*) mFile->Get(Form("%sDetectorJet", trg[i]));
  if(!hjetdet) { Info("Read", Form("histogram %sDetectorJet not found", trg[i])); return 0;}
  //TVectorD vdet = projRows(res);
  //vdet.Print("all");
  //hjetdet->Print("all");
  //TVectorD vjetdet = hist2dToVector(hjetdet, Ntrun[i], Mtrun[i], mFlag);
  //vjetdet.Print();
  //TVectorD vr(vdet); ElementDiv(vr, vjetdet);
  TVectorD vr = matchRatio(hdetVspar, hjetdet);
  //vr.Print();
  TCanvas *cr = new TCanvas("cr", "cr");
  TH1D *hratio = vectorToHist(vr, "matchRatio");
  cr->cd();hratio->Draw();
  hratio->GetXaxis()->SetTitle("detector jet p_{T} bin index");
  cr->Print(Form("%s%s%sMatchRatio.png", trg[i],par,ver));
  //test sample
  TH2D *hjetmc = (TH2D*) mFileTest->Get(Form("%sDetectorJet", trg[i]));
  //if(!hjetmc) { Info("Read", Form("histogram %sDetectorJet not found", trg[i])); return 0;}
  //get particle spectrum axis
  //TH2D* hparticle = (TH2D*) mFileTest->Get("ParticleJetEta");
  TH2D* hparticle = (TH2D*) mFilePy->Get(Form("%sJetEta", par));
  //test
  TArrayD *parbins = getBins(hparticle->GetXaxis()->GetXbins(), Ntrun[i]+dnshift, Mtrun[i]-upshift, mFlag);
  //cout<<"size: "<<parbins->GetSize()<<endl;
  //for(int ib = 0; ib < parbins->GetSize(); ib++){
  //cout<<ib<<" "<<parbins->At(ib)<<endl;
  //}
  TArrayD *detbins = getBins(hjetmc->GetXaxis()->GetXbins(), Ntrun[i], Mtrun[i], true);
  ///*
  //pt shift
  /*TProfile2D *hprofpt = (TProfile2D*) mFile->Get(Form("%sJetPtVs%sProfile", trg[i], par));
  TCanvas *cpt = new TCanvas("cpt", "cpt");
  TProfile *h1 = hprofpt->ProfileX("eta1", 1, 1, "s");
  //updateRmsAsError(h1);
  h1->Draw();
  h1->GetXaxis()->SetTitle("detector jet p_{T} [GeV]");
  h1->GetYaxis()->SetTitle("particle jet <p_{T}> [GeV]");
  h1->SetLineColor(kRed);
  //h1->Print("all");
  TProfile *h2 = hprofpt->ProfileX("eta2", 2, 2, "s");
  //updateRmsAsError(h2);
  h2->Draw("same");
  h2->SetLineColor(kBlue);
  //h2->Print("all");
  for(int ib = 0; ib < parbins->GetSize(); ib++){
    TLine *ln = new TLine(detbins->At(0), parbins->At(ib), detbins->At(detbins->GetSize()-1), parbins->At(ib));
    ln->Draw("same");
  }
  TLegend *lg = new TLegend(0.2, 0.7, 0.4, 0.8);
  lg->AddEntry(h1, "|#eta| < 0.5", "l");
  lg->AddEntry(h2, "0.5 < |#eta| < 0.9", "l");
  lg->Draw("same");

  cpt->Print(Form("%s%sPt.png", trg[i],ver));
  */
  //*/
  //plotRes(resp, resErrp, detbins, parbins, Form("%sShift%dResponse", trg[i], dnshift));
  //return -2;
  //}
  //return -2;

  TVectorD b = hist2dToVector(hjetmc, Ntrun[i], Mtrun[i], mFlag);
  b.Print();
  //matching ratio to b
  ElementMult(b, vr);

  htwojet = (TH2D*) mFileTest->Get(Form("%sTwoJet", trg[i]));
  if(!htwojet) { Info("Read", Form("histogram %sTwoJet not found", trg[i])); return 0;}
  
  TH2D *hvar = getVariance(hjetmc, htwojet);
  //hvar->Print("all");
  TMatrixD cov = histToMatrixTrunc2d(hvar, Ntrun[i], Mtrun[i], 2, mFlag);  
  cov.Print();
  //matching ratio correction to covariance matrix
  TMatrixD mr = DiagMatrix(vr);
  cov = mr*cov*mr;

  //TCanvas *ca = new TCanvas("ca", "ca");
  //ca->cd();
  //hdetVspar->Draw("colz");

  //TCanvas *cb = new TCanvas("cb", "cb");
  //cb->cd();
  //hdetVsparTest->Draw("colz");
  //
  //hdetVsparTest = (TH2D*) mFileTest->Get(Form("%sJetPtVsParticleJetPtRes", trg[i]));
  //if(!hdetVsparTest) { Info("Read", Form("histogram %sJetPtVsParticleJetPt not found", trg[i])); return 0;}

  //TMatrixD resTest = histToMatrixTrunc2d(hdetVsparTest, Ntrun[i], Mtrun[i], Ntrun[i]+dnshift, Mtrun[i]-upshift, 2);  
  //hparTest = hdetVsparTest->ProjectionY(Form("%sParticleTest", hdetVspar->GetName()));
  //hparTest->Print("all");
  //TVectorD xTest = histToVector2d(hparTest, Ntrun[i]+dnshift, Mtrun[i]-upshift, 2);  
  //cout<<"xTest: "<<endl;
  //xTest.Print("all");
  //response probability raw train
  //TMatrixD respTest(resTest);
  //respTest.NormByRow(xTest);
  //TMatrixD respDiff = resp-respTest;
  //respDiff.Print();
  //return -4;
  
  //TCanvas *ca = new TCanvas("ca", "ca");
  //ca->cd();
  //hdetVspar->Draw("colz");

  //TCanvas *cb = new TCanvas("cb", "cb");
  //cb->cd();
  //hdetVsparTest->Draw("colz");
  //TCanvas *ca = new TCanvas("ca", "ca");
  //ca->cd();
  //hpar->Draw();
  //hparTest->Draw("same");
  //hparTest->SetLineColor(2);
  //gPad->SetLogy(1);
  //ca->Print(Form("%sParticle.png", trg[i]));
  //
  //
  TMatrixD covf = covFactor(cov);
  //don't consider covariance matrix
  //covf.UnitMatrix();
  TMatrixD rescov = covf*res;
  TVectorD bcov = covf*b;

  //TGraph *grchi2 = new TGraph(Nrep+1);
  //response probability
  TMatrixD rescovp(rescov);
  //rescovp.NormByRow(x);
  //unfold

  //unfold without regularization
  TMatrixD ex0;
  TVectorD x0 = SVD(rescovp, bcov, ex0);
  ElementMult(x0, x);
  TMatrixD xm = DiagMatrix(x);
  ex0 = xm*ex0*xm;
  TH1D *hx0 = vectorToHist(x0, ex0, "recoNo");  
  TH2D *hex0 = matrixToHist(ex0, "recoNoError");
  //regularization
  int nxx = rescovp.GetNcols();
  TMatrixD cc = regMatrix(nxx/2, 2);
  TDecompSVD rsvd(rescovp);
  TVectorD sr = rsvd.GetSig();

  double reg = sr(mReg)*fReg;

  TCanvas *csig = new TCanvas("csig", "csig");
  TH1D *hsig = vectorToHist(sr, "hsig");
  csig->cd();
  hsig->Draw();
  gPad->SetLogy(1);
  TAxis *xaxis = hsig->GetXaxis();
  xaxis->SetTitle("bin index");
  double xmin = xaxis->GetXmin(), xmax = xaxis->GetXmax();
  TLine *lreg = new TLine(xmin, reg, xmax, reg);
  lreg->Draw("same");
  hsig->GetYaxis()->SetRangeUser(0.01*sr.Min(), 2*sr.Max());
  hsig->GetYaxis()->SetTitle("Singular Values");
  csig->Print(Form("%s%s%sSigAndReg.png", trg[i],par,ver));
  //
  TMatrixD ex;
  TVectorD xs = svdReg(rescovp, x, bcov, cc, reg, ex);
  //TMatrixD cn = reg*cc;
  //TMatrixD resn = addRows(rescovp, cn);

  //TVectorD bzero(cc.GetNrows());
  //bzero.Zero();
  //TVectorD bn = addRows(bcov, bzero);
  //bn.Print();
  //TVectorD xs = SVD(resn, bn, ex);
  //
  //double mod = TMath::Sqrt(x.Norm2Sqr());
  //TMatrixD ex0;
  //TVectorD x0 = SVD(rescovp, bcov, ex0);
  //x0.Print();
  //grchi2->SetPoint(0, 0, chi2Diff(x0, xTest));
  //TCanvas *ct = new TCanvas("ct", "ct");
  //TH1D *hx0 = vectorToHist(x0, ex0, "test0");
  //ct->cd();hx0->Draw();
  //hx0->SetLineColor(kRed);
  //TLine *ln = new TLine(x0.GetNrows()/2, hx0->GetYaxis()->GetXmin(), x0.GetNrows()/2, hx0->GetYaxis()->GetXmax());
  //ln->Draw("same");
  //canvas
  cResp = new TCanvas("cResp", "cResp");
  TH1D *hxs = vectorToHist(xs, ex, "reco");
  TH2D *hex = matrixToHist(ex, "recoError");
  cResp->cd(); hxs->Draw();
  gPad->SetLogy(1);
  hxs->SetLineColor(kRed);
  hxs->GetXaxis()->SetTitle("particle jet p_{T} bin index");
  
  //response
  TProfile *hprof = getRespSyst(res, resErr, x, covf, bcov, cc, reg);
  cResp->cd(); hxs->Draw("same");
  TCanvas *ct = new TCanvas("ct", "ct");
  ct->cd(); hxs->Draw();
  ct->cd(); hprof->Draw("pE2same");
  hprof->Print("all");
  hprof->SetLineColor(kBlue);
  hprof->SetFillColor(kBlue);
  TVectorD eresp(hprof->GetNbinsX());
  for(int ib = 1; ib <= hprof->GetNbinsX(); ib++){
     eresp(ib-1) = hprof->GetBinError(ib);
  }
  ct->cd(); gPad->SetLogy(1);
  double factor = xs.Sum()/x.Sum();
  TVector xscale = factor*x;
  TH1D *hxp = vectorToHist(xscale, "scale");
  ct->cd(); hxp->Draw("same");
  cout<<"reg: "<<reg<<endl;
  //bias
  TProfile *hprofbias = getBiasSyst(xscale, rescovp, cc, reg, factor);
  hprofbias->Print("all");
  TVectorD ebias(hprofbias->GetNbinsX());
  for(int ib = 1; ib <= hprofbias->GetNbinsX(); ib++){
     double cont = hprofbias->GetBinContent(ib);
     double error = hprofbias->GetBinError(ib);
     double bs = TMath::Sqrt(cont*cont+error*error); 
     //double bs = error;
     //double bs = TMath::Sqrt(cont);
     ebias(ib-1) = bs;
  }
  TH1D *hxbias = vectorToHist(xs, ebias, "recoBias");
  ct->cd(); hxbias->Draw("same");
  hxbias->SetLineColor(kGreen);
  hxbias->Print("all");
  TLegend *lct = new TLegend(0.7, 0.7, 0.85, 0.85);
  lct->AddEntry(hxs, "reco", "l");
  lct->AddEntry(hxp, "scaled emb", "l");
  lct->AddEntry(hprof, "resp", "l");
  lct->AddEntry(hxbias, "bias", "l");
  lct->Draw("same");
  hxs->Draw("same");
  ct->Print(Form("%s%s%sResTest.png", trg[i],par,ver));
  //
  cResp->Print(Form("%s%s%sRespSyst.png", trg[i],par,ver));
  cBias->cd(2); hxs->Draw("same");
  cBias->Print(Form("%s%s%sBiasSyst.png", trg[i],par,ver));

  TCanvas *creg = new TCanvas("creg", "creg");
  TLegend *lgreg = new TLegend(0.7, 0.7, 0.85, 0.85);
  creg->cd();hxs->Draw();
  hxs->GetYaxis()->SetRangeUser(0.01*xs.Min(), 2*xs.Max());
  creg->cd(); hx0->Draw("same");
  hx0->SetLineColor(kBlue);
  gPad->SetLogy(1);
  lgreg->AddEntry(hxs, "w reg.", "l");
  lgreg->AddEntry(hx0, "w/o reg.", "l");
  lgreg->Draw("same");
  creg->Print(Form("%s%s%sWithNoReg.png", trg[i],par,ver));

  //errors
  TCanvas *crr = new TCanvas("crr", "crr");
  TVectorD xe = getDiag(ex); xe.Sqrt();
  TVectorD rstat = ElemDiv(xe, xs);
  rstat.Print();
  TH1D *hrstat = vectorToHist(rstat, "hStat");
  hrstat->Draw(); hrstat->GetYaxis()->SetRangeUser(-0.1, 0.8);
  TVectorD rresp = ElemDiv(eresp, xs);
  TH1D *hrresp = vectorToHist(rresp, "hResp");
  hrresp->Draw("same"); hrresp->SetLineColor(kRed);
  TVectorD rbias = ElemDiv(ebias, xs);
  TH1D *hrbias = vectorToHist(rbias, "hBias");
  hrbias->Draw("same"); hrbias->SetLineColor(kBlue);
  crr->BuildLegend(0.6,0.7,0.8,0.85);
  crr->Print(Form("%s%s%sErrorRatio.png", trg[i],par,ver));
  
  mFileOut->Write();
  //mFileOut->Close();
  //mFilePy->Close();
  //mFileTest->Close();
  //mFile->Close();
  return 1;
}
TProfile* getBiasSyst(const TVectorD &x, const TMatrixD &rescovp, const TMatrixD &cc, double reg, double factor = 1.0)
{
  cBias = new TCanvas("cBias", "cBias");
  cBias->Divide(2,2);
  TH1D *hxx = vectorToHist(x, "xx");
  cBias->cd(1); hxx->Draw();
  gPad->SetLogy(1);
  hxx->SetLineColor(kRed);
  hxx->GetXaxis()->SetTitle("particle jet bin bin index");
  hxx->GetYaxis()->SetRangeUser(0.1*x.Min(), 3*x.Max());
  
  int nx = x.GetNrows();
  //use error on the mean not rms;
  //use rms not error on the mean, "s";
  TProfile *hprof = new TProfile("profBiasSyst", "; particle jet p_{T} bin index;diff #pm rms", nx, 0, nx, "s");
  //test replica
  //TH2D *hparw0 = (TH2D*) mFilePy->Get(Form("ParticleJetEta_iset_0"));
  //TVectorD xparw0 = hist2dToVector(hparw0, Ntrun[i], Mtrun[i], mFlag);
  //xparw0 *= factor;
  for(int irep = 0; irep < Nrep; irep++){
    /* 
    TVectorD vsmr(nx);
    gRandom->RndmArray(nx, vsmr.GetMatrixArray());
    vsmr *= 0.2; // random number from 0.5 to 1.5
    vsmr += 0.7; // random number from 0.5 to 1.5
    //vsmr = 2*vsmr; // random number from 0 to 2
    vsmr.Print();
    
    TVectorD xTest(x);
    ElementMult(xTest, vsmr);
    TH1D *hxtest = vectorToHist(xTest, Form("xTest%d", irep));
    cBias->cd(1); hxtest->Draw("same");
    gPad->SetLogy(1);
    hxtest->SetLineColor(kBlue-10);
    ElementDiv(xTest, x);
    xTest.Print();
    */
    ///*
    //use replica in the pythia file
    TH2D *hparw = (TH2D*) mFilePy->Get(Form("ParticleJetEta_iset_%d", irep+1));
    TVectorD xparw = hist2dToVector(hparw, Ntrun[i], Mtrun[i], mFlag);
    xparw *= factor;
    TH1D *hxparw = vectorToHist(xparw, Form("xTest%d", irep));
    cBias->cd(1); hxparw->Draw("same");
    gPad->SetLogy(1);
    hxparw->SetLineColor(kBlue-10);
    TVectorD xTest(xparw);
    ElementDiv(xTest, x);
    //*/
    TVectorD bcovTest = rescovp*xTest;
    TMatrixD ex;
    TVectorD xs = svdReg(rescovp, x, bcovTest, cc, reg, ex);
    xs.Print();
    //cout<<"SVD: "<<reg<<endl;
    //xs.Print();
    //xTest.Print();
    //return -2;
    //ex.Print();
    TVectorD xrec(xs);
    //ElementMult(xrec, x);
    TH1D *hxrec = vectorToHist(xrec, Form("testbiasrep%d", irep));
    cBias->cd(2);
    if(irep == 0){
      hxrec->Draw();
      gPad->SetLogy(1);
      hxrec->GetXaxis()->SetTitle("particle jet p_{T} bin index");
      hxrec->GetYaxis()->SetRangeUser(0.1*x.Min(), 3*x.Max());
    }else
      hxrec->Draw("same");
    hxrec->SetLineColor(kGreen-10);

    for(int ir = 0; ir < xs.GetNrows(); ir++){
      double bias = xs(ir)-xTest(ir)*x(ir);
      //double bias = xparw(ir)-xparw0(ir);
      Printf("%d %e", ir, bias);
      //double bias = (xs(ir)-xTest(ir)*x(ir))/x(ir);
      //double bias = TMath::Power(xs(ir)-xTest(ir), 2);
      hprof->Fill(ir, bias);
    }
  }
  cBias->cd(1); hxx->Draw("same");
  cBias->cd(3); hprof->Draw();
  return hprof;
}
TProfile* getRespSyst(const TMatrixD &res, const TMatrixD &resErr, const TVectorD &xx, const TMatrixD &covf, const TVectorD &bcov, const TMatrixD &cc, double reg)
{
  TProfile *hprof = new TProfile("profRespSyst", ";particle jet p_{T} bin; diff #pm rms", res.GetNcols(), 0, res.GetNcols(), "s"); 
  for(int irep = 0; irep < Nrep; irep++){
    TMatrixD mrep = replica(res, resErr, irep);
    if(irep%10 == 0) mrep.Print();
    TMatrixD mcovrep = covf*mrep;
    TMatrixD mprep(mcovrep);
    //normalize
    //TVectorD x0rep = projCols(mrep);
    //mprep.NormByRow(x0rep);
    //mprep.NormByRow(xx);
    TMatrixD exrep;
    //TVectorD xrep = SVD(mprep, bcov, exrep);
    TVectorD xrep = svdReg(mprep, xx, bcov, cc, reg, exrep);
    xrep.Print();
    TH1D *hxrep = vectorToHist(xrep, Form("testresrep%d", irep));
    cResp->cd();hxrep->Draw("same");
    hxrep->SetLineColor(kBlue-10);
    for(int ir = 0; ir < xrep.GetNrows(); ir++){
       hprof->Fill(ir, xrep(ir));
    }
  }
  cResp->cd(); hprof->Draw("same");
  return hprof;
}
TVectorD svdReg(const TMatrixD &resp, const TVectorD &xi, const TVectorD &b, const TMatrixD &cc, double reg, TMatrixD &ex)
{
  TMatrixD cn = reg*cc;
  //cc.Print();
  TMatrixD respn = addRows(resp, cn);

  TVectorD bzero(cc.GetNrows());
  bzero.Zero();
  TVectorD bn = addRows(b, bzero);
  //bn.Print();

  //TMatrixD ex;
  TVectorD xs = SVD(respn, bn, ex);
  //cout<<"SVD: "<<endl;
  //xs.Print();
  //ex.Print();
  //TVectorD xrec(xs);
  ElementMult(xs, xi);
  TMatrixD xm = DiagMatrix(xi);
  ex = xm*ex*xm;

  //return xrec;
  //return xs.GetSub(0, resp.GetNcols()-1);
  return xs;
} 
Double_t chi2Diff(const TVectorD &v1, const TVectorD &v2)
{
  TVectorD diff = v1 - v2;
  return diff.Norm2Sqr();
}
TMatrixD replica(const TMatrixD &res, const TMatrixD &resErr, int seed)
{
  int nr = res.GetNrows();
  int nc = res.GetNcols();
  const int Nrndm = nr*nc;
  double rndm[Nrndm];
  gRandom->SetSeed(seed);
  gRandom->RndmArray(Nrndm, rndm);
  TMatrixD mrndm(nr, nc, rndm);
  TMatrixD resRep(nr, nc);
  resRep.Zero();
  for(int ir = 0; ir < nr; ir++){
    for(int ic = 0; ic < nc; ic++){
      if(mrndm(ir, ic) < 0.5) resRep(ir, ic) = res(ir,ic) + resErr(ir, ic);
      else resRep(ir, ic) = res(ir, ic) - resErr(ir, ic);
    }
  }
  //resRep.Print("all");
  return resRep;
}
void plotRes(const TMatrixD &resp, const TMatrixD &resErrp, const TArrayD *detbins, const TArrayD *parbins, char *name = "resp")
{
  int nx = 4;
  int ny = (resp.GetNcols()+nx-1)/nx;
  TCanvas *cres = new TCanvas("cres", "cres", 240*nx, 180*ny);
  cres->Divide(nx, ny); 
  for(int ib = 0; ib < resp.GetNcols(); ib++){
    cres->cd(ib+1);
    TVectorD p1(TMatrixDColumn(resp, ib));
    TVectorD p1err(TMatrixDColumn(resErrp, ib));
    TH1D *hist1 = vectorToHist(p1.GetSub(0, p1.GetNrows()/2-1), 
			       p1err.GetSub(0, p1err.GetNrows()/2-1), 
			       detbins, Form("hist1%d", ib));
    TH1D *hist2 = vectorToHist(p1.GetSub(p1.GetNrows()/2, p1.GetNrows()-1), 
			       p1err.GetSub(p1err.GetNrows()/2, p1err.GetNrows()-1),
			       detbins, Form("hist2%d", ib));
    hist1->Draw("histE");
    //hist1->GetYaxis()->SetRangeUser(0.2, 0.6);
    hist1->GetYaxis()->SetRangeUser(1e-03, 0.6);
    hist1->GetYaxis()->SetTitle("Prob.");
    hist1->GetXaxis()->SetTitle("detector jet p_{T} [GeV]");
    hist2->Draw("histEsame");
    hist2->SetLineColor(2);
    gPad->SetLogy(1);
    TPaveText *text = new TPaveText(0.7, 0.7, 0.9, 0.9, "NDC");
    text->SetBorderSize(0);
    text->SetFillColor(0);
    int jb = ib%(parbins->GetSize()-1);
    text->AddText(Form("(%.1f, %.1f)", parbins->At(jb), parbins->At(jb+1)));
    if(ib >= parbins->GetSize()-1) text->GetLine(0)->SetTextColor(2);
    text->Draw();
  }
  cres->Print(Form("%s.png", name));
}
TArrayD *getBins(TArrayD *arr, int nn, int mm, bool cut)
{
  TArrayD *bins = new TArrayD(mm-nn+2);
  cout<<"bins size: "<<bins->GetSize()<<endl;
  cout<<mm-nn+2<<endl;
  if(cut) (*bins)[0] = arr->At(nn-1);
  else 
    (*bins)[0] = arr->At(0);
  for(int ib = 1; ib < mm-nn+2; ib++){
    (*bins)[ib] = arr->At(nn+ib-1);
    //cout<<ib<<" "<<bins->GetSize()<<endl;
  }
  //cout<<"bins size: "<<bins->GetSize()<<endl;
  return bins;
}
TMatrixD unfoldBayes(const TMatrixD &prob, const TMatrixD &x, const TMatrixD &b)
{
  TVectorD xp(TMatrixDColumn(x, 0));
  double norm = xp.Sum();
  xp *= 1./norm;
  //x.Print();
  //xp.Print();
  //cout<<"prob: "<<endl;
  //prob.Print();
  TMatrixD pand(prob);
  pand.NormByRow(xp, "M");
  //cout<<"pand: "<<endl;
  //pand.Print();
  int nrow = pand.GetNrows();
  TVectorD pe(nrow);
  for(int ir = 0; ir < nrow; ir++){
    pe(ir) = TVectorD(TMatrixDRow(pand, ir)).Sum();
  }
  //cout<<"pe: "<<endl;
  //pe.Print();

  pand.NormByColumn(pe);
  //pand.Print();
  TMatrixD pandT = Transpose(pand);
  TMatrixD rec = pandT*b;
  //rec.Print();
  return rec;
} 
TMatrixD covFactor(const TMatrixD &cov)
{
  const int Nb = cov.GetNrows();
  TDecompSVD svdcov(cov);
  TVectorD scov = svdcov.GetSig();
  //scov.Print("all");
  TMatrixD ucov = svdcov.GetU();
  ucov.Transpose(ucov);
  //ucov.Print("all");
  //TMatrixD vcov = svdcov.GetV();
  //vcov.Print("all");
  TMatrixD r(Nb, Nb);
  for(int ir = 0; ir < Nb; ir++){
    if(scov[ir] > 0.) r(ir,ir) = 1./TMath::Sqrt(scov(ir));
  }
  //Info("", "new response matrix");
  TMatrixD rucov = r*ucov;
  return rucov;
}
TMatrixD regMatrix(int nx, int ny = 2)
{
  TMatrixD cc(nx*ny+nx, nx*ny);

  for(int ix = 0; ix < nx*ny+nx; ix++){
    if(ix < nx*ny){
      if(ix%nx == 0){
        cc(ix, ix) = -1;
        cc(ix, ix+1) = 1;
      }else if(ix%nx == nx-1){
        cc(ix, ix) = -1;
        cc(ix, ix-1) = 1;
      }
      else{
        cc(ix, ix-1) = 1;
        cc(ix, ix) = -2;
        cc(ix, ix+1) = 1;
      }
    }else{
      cc(ix, ix-nx*ny) = -1;
      cc(ix, ix-nx*ny+nx) = 1;
    }
  }
  return cc;
}
/*
TMatrixD svd(const TMatrixD &A, const TMatrixD &b)
{
  TDecompSVD svd(A);
  TVectorD s = svd.GetSig();
  TMatrixD u = svd.GetU();
  //u.Print("all");
  TMatrixD v = svd.GetV();

  TMatrixD uT(u);
  uT.Transpose(u);

  TMatrixD d = uT*b;

  TMatrixD z(A.GetNcols(), 1);

  for(int iz = 0; iz < A.GetNcols(); iz++){
    z(iz, 0) = d(iz, 0)/s(iz);
  }

  return v*z;
}
TMatrixD NormalEquation(const TMatrixD &res, const TMatrixD &b, const TMatrixD &c, double tau)
{
  TMatrixD resT = Transpose(res);
  TMatrixD res2 = resT*res;
  //Info("NormalEquation", "C matrix"); 
  //Cc.Print("all");

  TMatrixD cT= Transpose(c);
  TMatrixD c2 = cT*c;

  TMatrixD reg = tau*c2+res2;

  //Info("NormalEquation", "Reg matrix");
  //reg.Print("all");
  //TMatrixD regI(reg);
  //regI.Invert();
  //SVD inverse
  TMatrixD regI = Inverse(reg);
  //regI.Print("all");
  TMatrixD reco = regI*resT*b;

  //Info("NormalEquation", "reconstructed vector");
  //reco.Print("all");

  return reco;
}
*/
TVectorD getDiag(const TMatrixD &mm)
{
  int N = mm.GetNrows();
  int M = mm.GetNcols();
  int L = N > M ? M : N;
  TVectorD vv(L);
  for(int ix = 0; ix < L; ix++){
    vv(ix) = mm(ix, ix); 
  }
  return vv;
}
void updateRmsAsError(TProfile *h)
{
  int N = h->GetNbinsX();
  //cout<<N<<endl;
  for(int ib = 1; ib <= N; ib++){
    double cont = h->GetBinContent(ib);
    double err = h->GetBinError(ib);
    double entry = h->GetBinEffectiveEntries(ib);
    double nerr = err*TMath::Sqrt(entry);
    Printf("%lf, %lf, %lf, %lf\n", cont, err, entry, nerr);
    //h->SetBinError(ib, nerr);
  }
}
TVectorD matchRatio(TH2D* hres, TH2D *hjet)
{
  TMatrixD res = histToMatrixTrunc2dn(hres, Ntrun[i], Mtrun[i], Ntrun[i], Mtrun[i], 2, mFlag);
  //res.Print();
  TVectorD vdet = projRows(res);
  //vdet.Print();
  TVectorD vjetdet = hist2dToVector(hjet, Ntrun[i], Mtrun[i], mFlag);
  //vjetdet.Print();
  TVectorD vr(vdet); ElementDiv(vr, vjetdet);
  //vr.Print();
  return vr;
}
TVectorD effPar(TH2D *hres, TH2D *hpar)
{
  TMatrixD res = histToMatrixTrunc2dn(hres, Ntrun[i], Mtrun[i], Ntrun[i], Mtrun[i], 2, mFlag);
  //res.Print();
  TVectorD xres = projCols(res);
  //xres.Print();
  //TVectorD xpar = hist2dToVectorN(hpar, Ntrun[i], Mtrun[i], true, mFlag);
  TVectorD xpar = hist2dToVector(hpar, Ntrun[i], Mtrun[i], mFlag);
  //xpar.Print();
  TVectorD xeff(xres);
  ElementDiv(xeff, xpar);
  //xeff.Print();
  return xeff;
}
TVectorD hist2dToVectorN(const TH2D *h2, int nn, int mm, bool overflow, bool flag)
{
  TAxis *xaxis = h2->GetXaxis();
  TAxis *yaxis = h2->GetYaxis();

  int nx = xaxis->GetNbins();
  int ny = yaxis->GetNbins();

  //int tx = nx/tt;

  //cout<<nn<<" "<<mm<<endl;
  TVectorD h1v ((mm-nn+1)*ny);
  int lx = nx;
  int ly = ny;
  if(overflow) lx += 1;

  for(int iy = 1; iy <= ly; iy++){
    for(int ix = 1; ix <= lx; ix++){
      double content = h2->GetBinContent(ix, iy);
      int jx = convert2dIndex(ix, nn, mm, iy, flag);
      //cout<<ix<<" "<<iy<<" new: "<<jx<<" "<<jy<<endl;
      if(jx != -1)
        h1v(jx) += content;
    }
  }
  return h1v;
}
