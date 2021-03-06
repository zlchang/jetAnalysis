#ifndef STHISTUTILITIES
#define STHISTUTILITIES
class TH1D;
class TH1;
class TH2D;
class TGraphErrors;

TH1D *ratioHist(const char *name, const TH1 *ha, const TH1 *hb);
TGraphErrors *histToGraph(const TH1D *h, const char *name);
TH2D *histSquare(const TH1 *he);
#endif 
