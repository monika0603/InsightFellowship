//
//  TriggerRatesBarChart.c
//  Created by Monika Sharma on 11/17/15.
//
TCanvas *canvasDressing(TString name = "c1")
{
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetPalette(1);
    gStyle->SetFrameFillColor(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetPadBorderSize(0);
    gStyle->SetCanvasBorderSize(0);
    gStyle->SetFrameLineColor(1);
    gStyle->SetFrameLineWidth(2.0);
    gStyle->SetCanvasColor(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadColor(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    gStyle->SetLegendBorderSize(0);
    gStyle->SetErrorX(0.5);
    gStyle->SetTextFont(42);
    gStyle->SetLabelFont(42,"x");
    gStyle->SetLabelFont(42,"y");
    gStyle->SetLabelFont(42,"z");
    gStyle->SetTitleFont(42,"x");
    gStyle->SetTitleFont(42,"y");
    gStyle->SetTitleFont(42,"z");
    gStyle->SetEndErrorSize(1);
    
    TCanvas *c1 = new TCanvas(name,"Bar chart for trigger rates",200,10,700,900);
    c1->Range(4.819967,-13.30288,6.070376,78.37912);
    c1->SetFillColor(0);
    c1->SetBorderMode(0);
    c1->SetBorderSize(2);
    c1->SetLeftMargin(0.1439791);
    c1->SetRightMargin(0.03534031);
    c1->SetTopMargin(0.07777778);
    c1->SetBottomMargin(0.145098);
    c1->SetFrameBorderMode(0);
    c1->SetFrameBorderMode(0);
    
    return c1;
}

TCanvas *TriggerRatesBarChart() {
    
    TCanvas *c1 = canvasDressing("c1");
    c1->cd();
    
    const int n = 9;
    double x[n] = {0.4, 0.42, 0.5, 0.52, 0.6, 0.62, 0.7, 0.8, 0.82};
    double y[n] = {67.7, 12.5, 53.93, 11.3, 40.23, 11.4, 27.96, 17.1, 5.2};
    
    TGraph *gr1 = new TGraph(n, x, y);
    gr1->SetFillColor(kBlue);
    gr1->SetTitle("");
    
    gr1->GetXaxis()->SetLabelFont(42);
    gr1->GetXaxis()->SetLabelOffset(0.01);
    gr1->GetXaxis()->SetTitleSize(0.05);
    gr1->GetXaxis()->SetTitleOffset(0.99);
    gr1->GetXaxis()->SetLabelFont(42);
    gr1->GetXaxis()->SetLabelSize(0.04);
    gr1->GetXaxis()->SetTitleFont(42);
    gr1->GetYaxis()->SetTitle("'HISinglePhoton15' trigger rates (Hz)");
    gr1->GetYaxis()->SetLabelFont(42);
    gr1->GetYaxis()->SetLabelOffset(0.01);
    gr1->GetYaxis()->SetTitleOffset(1.28);
    gr1->GetYaxis()->SetTitleSize(0.05);
    gr1->GetYaxis()->SetTitleFont(42);
    gr1->GetYaxis()->SetLabelSize(0.05);
    gr1->GetYaxis()->SetNdivisions(509);
    gr1->Draw("AB");
    
    const int nn = 8;
    double xn[nn] = {0.44, 0.45, 0.54, 0.55, 0.64, 0.65, 0.84, 0.85};
    double yn[nn] = {12.5, 0.0, 11.3, 0.0, 11.4, 0.0, 5.2, 0.0};
    
    TGraph *gr2 = new TGraph(nn, xn, yn);
    gr2->SetFillColor(kRed);
    gr2->Draw("B");
    
    TText *tv1 = new TText(0.3505237,-12.19446, "R9 cut");
    tv1->SetTextAlign(11); tv1->SetTextSize(0.04);
    tv1->SetTextAngle(45);
    tv1->SetTextColor(4);
    tv1->Draw();
    
    TText *tv2 = new TText(0.3804381,-13.69211, "R9 & HE cut");
    tv2->SetTextAlign(11); tv2->SetTextSize(0.04);
    tv2->SetTextAngle(45);
    tv2->SetTextColor(2);
    tv2->Draw();
    
    TLine *line = new TLine(0.36,10.1,0.86,9.9);
    line->SetLineStyle(2);
    line->SetLineWidth(3);
    line->Draw();
    
    TLatex *   tex = new TLatex(0.51,65.5,"allotted bandwidth = 10 Hz");
    tex->SetTextFont(42);
    tex->SetLineWidth(2);
    tex->Draw();
    
    return c1;
    
}