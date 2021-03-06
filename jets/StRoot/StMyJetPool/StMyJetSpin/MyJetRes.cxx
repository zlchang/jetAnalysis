#include "MyJetRes.h"

ClassImp(MyJetRes2D);

void MyJetRes2D::Add(MyJetRes2D *jetres, float w)
{
  hjetmatchratioprof->Add(jetres->GetJetMatchRatio(), w);
  hjetres->Add(jetres->GetJetRes(), w);
  hjetptshiftprof->Add(jetres->GetJetPtShift(), w);
  hjetptratioprof->Add(jetres->GetJetPtScale(), w);
  //hjetpar->Add(jetres->GetParJet(), w);
  //hjetparT->Add(jetres->GetParTwoJet(), w);
}
void MyJetRes2D::FillResponse(float pt1, float eta1, float pt2, float eta2, float weight)
{
  int i1 = indexDet(pt1, eta1);
  int i2 = indexPar(pt2, eta2);

  hjetres->Fill(i1, i2, weight);
  hjetptshiftprof->Fill(i1, pt2-pt1, weight);
  if(pt1 > 0) hjetptratioprof->Fill(i1, pt2/pt1, weight);
}
//MyJetRes1D
void MyJetRes1D::Add(MyJetRes1D *jetres, float w)
{
  hjetmatchratioprof->Add(jetres->GetJetMatchRatio(), w);
  hjetres->Add(jetres->GetJetRes(), w);
  hjetptshiftprof->Add(jetres->GetJetPtShift(), w);
  hjetptratioprof->Add(jetres->GetJetPtScale(), w);
  //hjetpar->Add(jetres->GetParJet(), w);
  //hjetparT->Add(jetres->GetParTwoJet(), w);
}
void MyJetRes1D::FillResponse(float pt1, float pt2, float weight)
{
  hjetres->Fill(pt1, pt2, weight);
  hjetptshiftprof->Fill(pt1, pt2-pt1, weight);
  if(pt1 > 0) hjetptratioprof->Fill(pt1, pt2/pt1, weight);
}
