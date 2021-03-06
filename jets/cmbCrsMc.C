int cmbCrsMc(char *file = "ptbin.list", char* runlist = "text.full", 
	  //char* dir_base = "output/mc_crs_weight_ueoff", char* mcfilepre = "run12.k.sub", 
	  char* dir_base = "output", char* mcfilepre = "run12.l", 
	  int ver = 1, int first = 0)
{
  gSystem->Load("StMyDef.so");
  gSystem->Load("StMyHistUtil.so");
  gSystem->Load("StMyJetSpin.so");
  ifstream in(file);
  char ptbin[16];
  const int Ntrg = 3;
  //  const int Nver = 2;
  char *trg[Ntrg] = {"JP0", "JP1", "JP2"};

  //  char dir_base[] = "output/mc";
  //  char dir[128];

  TFile *fout = new TFile(Form("%s.%s.v%d.w.cmb.crs.root", file, mcfilepre, ver), "recreate");

  //MyJetMC *jetmc[Ntrg];
  MyJetYield *jetyld[Ntrg];
  for(int itrg = 0; itrg < Ntrg; itrg++){
    //jetmc[itrg] = new MyJetMC(trg[itrg]);
    jetyld[itrg] = new MyJetYield(trg[itrg]);
  }
  MyJetYield *jetyldcmb = new MyJetYield("Cmb");
  MyJetQA *jetqa[Ntrg];
  for(int itrg = 0; itrg < Ntrg; itrg++){
    jetqa[itrg] = new MyJetQA(trg[itrg]);
  }

  MyJetRes2D *jetresparticle = new MyJetRes2D("CmbParticle");
  MyJetRes2D *jetresprtc[Ntrg];
  for(int itrg = 0; itrg < Ntrg; itrg++){
    jetresprtc[itrg] = new MyJetRes2D(Form("%sParticle", trg[itrg]));
  }
  
  MyJetRes2D *jetresparton = new MyJetRes2D("CmbParton");
  MyJetRes2D *jetresprtn[Ntrg];
  for(int itrg = 0; itrg < Ntrg; itrg++){
    jetresprtn[itrg] = new MyJetRes2D(Form("%sParton", trg[itrg]));
  }
  
//  MyJetPar *jetparton = new MyJetPar("parton");
//  MyJetPar *jetparticle = new MyJetPar("particle");
  MyJetYieldPar *jetyldparton = new MyJetYieldPar("Parton");
  MyJetYieldPar *jetyldparticle = new MyJetYieldPar("Particle");
  TFile *finfo = TFile::Open(Form("analyze_logs/ptbin.info.%s.root", runlist));
  TH1F *hweight = (TH1F *)finfo->Get("weight");
  double weight[13];
  for(int ipt = 0; ipt < 13; ipt++){
    weight[ipt] = hweight->GetBinContent(ipt+1);
    //weight[ipt] = 1.;
  }
  finfo->Close();
  float fudge[13];
  ifstream in_fudge(Form("fudge/fudge.%s.%s.rebin.5.txt", file, runlist));
  if(!in_fudge) cout<<"can't not open fudge factor file"<<endl;
  float ptmin, ptmax, ff, ffee;
  int counter = 0;
  while(in_fudge >> ptmin >> ptmax >> ff >> ffee){
    cout<<ptmin<<" "<<ff<<endl;
    //if(counter <= 4)
      fudge[counter] = ff;
    //else fudge[counter] = 1.;
    counter++;
  }
  in_fudge.close();
  counter = 0;
  while(in >> ptbin){
    //if(counter == 0) { counter++; continue; }
    float ww = fudge[counter]*weight[counter];
    for(int ii = 0; ii < 10; ii++){
      //if(ii%2 == 1) continue;
      TFile *fin = TFile::Open(Form("%s/%s/%s.%d.crs.root", dir_base, ptbin, ptbin, ii));
      cout<<Form("%s/%s/%s.%d.crs.root", dir_base, ptbin, ptbin, ii)<<endl;
//      TFile *fin = TFile::Open(Form("%s/%s/%s.%s.mc.root", dir_base, mcfilepre, mcfilepre, ptbin));
//      cout<<Form("%s/%s/%s.%s.mc.root", dir_base, mcfilepre, mcfilepre, ptbin)<<endl;
      fin->cd();
      for(int itrg = 0; itrg < Ntrg; itrg++){
        if(counter <= 1 && itrg >= 1) continue;
        //MyJetMC *pmc = (MyJetMC *)fin->Get(Form("%sJetMc", trg[itrg]));
        //pmc->GetJetMC()->Print();
        //jetmc[itrg]->Add(pmc, ww);
        //yield
        MyJetYield *pyld = (MyJetYield *)fin->Get(Form("%sJetYield", trg[itrg]));
        jetyld[itrg]->Add(pyld, ww);
        delete pyld;
        //
        MyJetQA *pqa = (MyJetQA *)fin->Get(Form("%sJetQA", trg[itrg]));
        jetqa[itrg]->Add(pqa, ww);
        delete pqa;

	MyJetRes2D *presprtc = (MyJetRes2D*) fin->Get(Form("%sJetParticleRes", trg[itrg]));
	jetresprtc[itrg]->Add(presprtc, ww);
	delete presprtc;
	
	MyJetRes2D *presprtn = (MyJetRes2D*) fin->Get(Form("%sJetPartonRes", trg[itrg]));
	jetresprtn[itrg]->Add(presprtn, ww);
	delete presprtn;
      }
      //MyJetPar *pa = (MyJetPar *)fin->Get("JetMcParton");
      //jetparton->Add(pa, ww);

      //MyJetPar *pb = (MyJetPar *)fin->Get("JetMcParticle");
      //jetparticle->Add(pb, ww);

      MyJetYield *pyldcmb = (MyJetYield *)fin->Get(Form("%sJetYield", "Cmb"));
      jetyldcmb->Add(pyldcmb, ww);
      delete pyldcmb;
      //
      MyJetYieldPar *pya = (MyJetYieldPar *)fin->Get("YieldPartonMc");
      jetyldparton->Add(pya, ww);
      delete pya;

      MyJetYieldPar *pyb = (MyJetYieldPar *)fin->Get("YieldParticleMc");
      jetyldparticle->Add(pyb, ww);
      delete pyb;

      MyJetRes2D *presparticle = (MyJetRes2D*) fin->Get(Form("%sJetParticleRes", "Cmb"));
      jetresparticle->Add(presparticle, ww);
      delete presparticle;

      MyJetRes2D *presparton = (MyJetRes2D*) fin->Get(Form("%sJetPartonRes", "Cmb"));
      jetresparton->Add(presparton, ww);
      delete presparton;

      fin->Close();
    }
    cout<<counter<<" "<<ptbin<<" w: "<<weight[counter]<<" f: "<<fudge[counter]<<" wf: "<<ww<<endl;
    counter++;
  }

  fout->Write();
  fout->Close();
  return 1;
}
