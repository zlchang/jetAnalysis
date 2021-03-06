#ifndef TRG_UTILITIES
#define TGR_UTILITIES
#include<vector>
#include<map>
#include<iostream>
#include<string>

using namespace std;

class StJetCandidate;
class StJetSkimEvent;
class StJetEvent;
class MyJet;

float smearPtFrac(float rt);
bool check_badbx(int bx, vector<int> badbunch);
bool check_spinbit(int spinbit);
bool check_abort(int bx);

StJetCandidate *FindParJet(StJetCandidate* jet, StJetEvent *evntpar, int &index, float range, int &counter);
StJetCandidate *FindParJet(StJetCandidate* jet, StJetEvent *evntpar, int &index, float range = 1.4);
float DeltaR(float etaA, float phiA, float etaB, float phiB);
//merge sort
//string JetCategory(StJetCandidate *jetCnd, StJetSkimEvent *skimevnt, int th_plusi = 0);
struct JetPars
{
  JetPars(){
    jp0minpt = 6;
    jp0maxpt = 33.6;
    jp1minpt = 8.2;
    jp1maxpt = 39.3;
    jp2minpt = 15.3;
  }
  JetPars(double jp0min, double jp0max, double jp1min, double jp1max, double jp2min){
    jp0minpt = jp0min;
    jp0maxpt = jp0max;
    jp1minpt = jp1min;
    jp1maxpt = jp1max;
    jp2minpt = jp2min;
  }
  double jp0minpt;
  double jp0maxpt;
  double jp1minpt;
  double jp1maxpt;
  double jp2minpt;
};

float smearJetPt(MyJet *myjet, bool ue, bool smear);

string JetCategory(MyJet *myjet, StJetSkimEvent *skimevnt, map<string, int>mapTrigShouldFire, map<string, int>mapTrigDidFire, JetPars pars, bool flag = 0, bool smear = 0, int th_plus = 0);
string JetCategory(MyJet *myjet, StJetSkimEvent *skimevnt, map<string, int>mapTrigShouldFire, map<string, int>mapTrigDidFire, bool flag = 0, int th_plus = 0);
vector<MyJet*> mergesort(vector<MyJet*>, bool flag = false);
vector<MyJet*> merge(vector<MyJet*> a, vector<MyJet*> b, bool flag = false);
//JP triggers
bool MatchJetPatch(StJetCandidate *jetcnd, StJetSkimEvent *skim, int th, int shift = 0);
bool MatchBarrelJetPatch(StJetCandidate *jetcnd, StJetSkimEvent *skim, int th, int shift = 0);
bool MatchEndcapJetPatch(StJetCandidate *jetcnd, StJetSkimEvent *skim, int th, int shift = 0);
bool MatchOverlapJetPatch(StJetCandidate *jetcnd, StJetSkimEvent *skim, int th, int shift = 0);
//AJP trigger
bool MatchAdjacentJetPatch(StJetCandidate *jetcnd, StJetSkimEvent *skim, int th, int shift = 0);
bool MatchBarrelAdjacentJetPatch(StJetCandidate *jetcnd, StJetSkimEvent *skim, int th, int shift = 0);
bool MatchEndcapAdjacentJetPatch(StJetCandidate *jetcnd, StJetSkimEvent *skim, int th, int shift = 0);
void GetBarrelAdjacentJetPatchEtaPhi(StJetCandidate *jetcnd, int i, float &eta, float &phi);
void GetEndcapAdjacentJetPatchEtaPhi(StJetCandidate *jetcnd, int i, float &eta, float &phi);

bool HitJetPatch(float deteta, float phi, float jpeta, float jpphi);
//
#endif
