// -*- C++ -*-
//
// Package:    TriHadronAnalyzer
// Class:      TriHadronAnalyzer
// 
// class TriHadronAnalyzer TriHadronAnalyzer.cc TriHadronCorr/TriHadronAnalyzer/src/TriHadronAnalyzer.cc

// Description: Three-particle correlations
//
// Original Author:  Monika Sharma
//         Created:  Mon Apr 27 08:32:15 CDT 2015

// system include files
#include <memory>
#include <algorithm>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
//HI stuff
#include "RecoHI/HiCentralityAlgos/interface/CentralityProvider.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include <DataFormats/TrackReco/interface/Track.h>
#include <DataFormats/TrackReco/interface/TrackFwd.h>
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "RecoJets/JetAlgorithms/interface/JetAlgoHelper.h"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include "TVector3.h"
#include "assert.h"
#include <stdlib.h>
#include <TRandom.h>

using namespace std;
using namespace edm;
using namespace reco;

class TriHadronAnalyzer : public edm::EDAnalyzer {
public:
    explicit TriHadronAnalyzer(const edm::ParameterSet&);
    ~TriHadronAnalyzer();
    static bool vtxSort(const reco::Vertex &  a, const reco::Vertex & b);
    bool TrackQualityCuts(const reco::Track & track, const reco::Vertex & vertexCollectionSelected);
    int getEtaRegion(double eta);
    
    
private:
    virtual void beginJob() ;
    virtual void analyze(const edm::Event&, const edm::EventSetup&);
    virtual void endJob() ;
    void initHistos(const edm::Service<TFileService> & fs);
    
    // ----------member data ---------------------------
    std::map<std::string,TH1F*> evtPerf_;
    std::map<std::string,TH1F*> trkPerf_;
    std::map<std::string,TH1F*> vtxPerf_;
    std::map<std::string,TH2F*> vtxPerf2D_;
    std::map<std::string,TH1F*> hdNdEtaVzBin_;
    std::map<std::string,TH1F*> hEventVzBin_;
    std::map<std::string,TH2D*> hSignal_;
    std::map<std::string,TH2D*> hBackground_;
    
    
    TH1F* nMultTrig_;
    TH1F* vertices_;
    TH2F* corrFactors_;
    
    int nevt_;
    int ntrack_;
    int nvertex_;
    double pi_;
    int bkgFactor_;
    int tHighPurityTracks_;
    int nVzBins;
    int nHITracks;
    
    edm::InputTag vertexSrc_;
    edm::InputTag trackSrc_;
    double etaMin_;
    double etaMax_;
    double ptMin_;
    double vertexZMax_;
    
    std::string qualityString_;
    
    std::vector<double> etaBins_;
    std::vector<double> vzBins_;
    std::vector<double> NptBins_;
    std::vector<std::vector<TVector3>> pVectVect_trg;
    std::vector<std::vector<TVector3>> pVectVect_ass1;
    std::vector<std::vector<TVector3>> pVectVect_ass2;
    std::vector<double> zvtxVect;
    
    double cutDzErrMax_;
    double cutDxyErrMax_;
    double cutPtErrMax_;
    double cutMultMin_;
    double cutMultMax_;
    
    double etaMinTrg_;
    double etaMaxTrg_;
    double etaMinAsso1_;
    double etaMaxAsso1_;
    double etaMinAsso2_;
    double etaMaxAsso2_;
    double ptMinTrg_;
    double ptMaxTrg_;
    double ptMinAsso1_;
    double ptMaxAsso1_;
    double ptMinAsso2_;
    double ptMaxAsso2_;
    
};

TriHadronAnalyzer::TriHadronAnalyzer(const edm::ParameterSet& iConfig):
nevt_(0),
ntrack_(0),
nvertex_(0),
pi_(3.1415927),
vertexSrc_(iConfig.getParameter<edm::InputTag>("vertexSrc")),
trackSrc_(iConfig.getParameter<edm::InputTag>("trackSrc")),
etaMin_(iConfig.getParameter<double>("etaMin")),
etaMax_(iConfig.getParameter<double>("etaMax")),
ptMin_(iConfig.getParameter<double>("ptMin")),
vertexZMax_(iConfig.getParameter<double>("vertexZMax")),
qualityString_(iConfig.getParameter<std::string>("qualityString")),
etaBins_(iConfig.getParameter<std::vector<double> >("etaBins")),
vzBins_(iConfig.getParameter<std::vector<double> >("vzBins"))
{
    edm::Service<TFileService> fs;
    initHistos(fs);
    
    nVzBins = vzBins_.size()-1;
    
    NptBins_ = iConfig.getParameter<std::vector<double> >("NptBins");
    cutMultMin_ = iConfig.getParameter<double>("cutMultMin");
    cutMultMax_ = iConfig.getParameter<double>("cutMultMax");
    cutDzErrMax_ = iConfig.getUntrackedParameter<double>("cutDzErrMax", 3.0);
    cutDxyErrMax_ = iConfig.getUntrackedParameter<double>("cutDxyErrMax", 3.0);
    cutPtErrMax_ = iConfig.getUntrackedParameter<double>("cutPtErrMax", 0.1);
    
    etaMinTrg_ = iConfig.getParameter<double>("etaMinTrg");
    etaMaxTrg_ = iConfig.getParameter<double>("etaMaxTrg");
    etaMinAsso1_ = iConfig.getParameter<double>("etaMinAsso1");
    etaMaxAsso1_ = iConfig.getParameter<double>("etaMaxAsso1");
    etaMinAsso2_ = iConfig.getParameter<double>("etaMinAsso2");
    etaMaxAsso2_ = iConfig.getParameter<double>("etaMaxAsso2");
    ptMinTrg_ = iConfig.getParameter<double>("ptMinTrg");
    ptMaxTrg_ = iConfig.getParameter<double>("ptMaxTrg");
    ptMinAsso1_ = iConfig.getParameter<double>("ptMinAsso1");
    ptMaxAsso1_ = iConfig.getParameter<double>("ptMaxAsso1");
    ptMinAsso2_ = iConfig.getParameter<double>("ptMinAsso2");
    ptMaxAsso2_ = iConfig.getParameter<double>("ptMaxAsso2");
    bkgFactor_ = iConfig.getUntrackedParameter<int>("bkgFactor", 20);
    
}


TriHadronAnalyzer::~TriHadronAnalyzer()
{
}

void
TriHadronAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    
    tHighPurityTracks_ = 0;
    vector<TVector3> pVect_trg;
    vector<TVector3> pVect_ass1;
    vector<TVector3> pVect_ass2;
    
    Handle<reco::TrackCollection> tracks;
    iEvent.getByLabel(trackSrc_, tracks);
    
    Handle<std::vector<reco::Vertex> > vertex;
    iEvent.getByLabel(vertexSrc_, vertex);
    
    std::vector<reco::Vertex> vsorted = *vertex;
    // sort the vertcies by number of tracks in descending order
    // use chi2 as tiebreaker
    std::sort( vsorted.begin(), vsorted.end(), TriHadronAnalyzer::vtxSort );
    // skip events with no PV, this should not happen
    if( vsorted.size() == 0) return;
    // skip events failing vertex cut
    if( fabs(vsorted[0].z()) > vertexZMax_ ) return;
    
    nevt_++;
    evtPerf_["Nvtx"]->Fill(vsorted.size());
    evtPerf_["Ntrk"]->Fill(tracks->size());
    
    int lumi = iEvent.getLuminosityBlock().luminosityBlock();
    evtPerf_["Lumi"]->Fill(lumi);
    evtPerf_["NvtxLumi"]->Fill(lumi,vsorted.size());
    
    int vcount = 0;
    for( const auto & vi :  vsorted )
    {
        vtxPerf_["Ntrk"]->Fill(vi.tracksSize());
        vtxPerf_["x"]->Fill(vi.x());
        vtxPerf_["y"]->Fill(vi.y());
        vtxPerf_["z"]->Fill(vi.z());
        vtxPerf2D_["Ntrk2D"]->Fill(vcount,vi.tracksSize());
        vertices_->Fill(0.5);
        vcount++;
        nvertex_++;
    }
    
    for (unsigned int i =1; i<vsorted.size(); i++)
    {
        double dz = fabs( vsorted[i].z() - vsorted[0].z() );
        double dx = fabs( vsorted[i].x() - vsorted[0].x() );
        double dy = fabs( vsorted[i].y() - vsorted[0].y() );
        double dxy  = sqrt ( dx*dx + dy*dy );
        vtxPerf_["assocVtxDz"]->Fill(dz);
        vtxPerf2D_["assocVtxDzNtrk"]->Fill(dz,vsorted[i].tracksSize() );
        vtxPerf_["assocVtxDxy"]->Fill(dxy);
        vtxPerf2D_["assocVtxDxyNtrk"]->Fill(dxy,vsorted[i].tracksSize() );
    }
    
    math::XYZPoint vtxPoint(0.0,0.0,0.0);
    double vzErr =0.0, vxErr=0.0, vyErr=0.0;
    
    // use vertex w most tracks as primary vertex
    if( vsorted.size() != 0 )
    {
        vtxPoint=vsorted.begin()->position();
        vzErr=vsorted.begin()->zError();
        vxErr=vsorted.begin()->xError();
        vyErr=vsorted.begin()->yError();
    }
    
    for( const auto & track : *tracks ) //de-referencing the pointer "tracks" to track and auto will automatically know the type of tracks. This is a new way of looping over the tracks. It is guaranteed to run over all the tracks.
    {
        double dxy=0.0, dz=0.0, dxysigma=0.0, dzsigma=0.0;
        dxy = track.dxy(vtxPoint);
        dz = track.dz(vtxPoint);
        dxysigma = sqrt(track.d0Error()*track.d0Error()+vxErr*vyErr);
        dzsigma = sqrt(track.dzError()*track.dzError()+vzErr*vzErr);
        
        if( !TrackQualityCuts(track, vsorted[0])) continue;
        
        tHighPurityTracks_++;
        
        char histoName1[200];
        char histoName2[200];
        for(int kVz=0; kVz<nVzBins; kVz++) {
            if(vsorted[0].z() > vzBins_[kVz] && vsorted[0].z() <= vzBins_[kVz+1])
            {
                sprintf(histoName1, "hdNdEta_VzBin_%d", kVz);
                hdNdEtaVzBin_[histoName1]->Fill(track.eta());
                sprintf(histoName2, "nEventsVzBin_%d", kVz);
                hEventVzBin_[histoName2]->Fill(0.5);
            }
        }
        
        if( track.eta() <= etaMax_ && track.eta() >= etaMin_ && track.pt() > ptMin_)
        {
            trkPerf_["Nhit"]->Fill(track.numberOfValidHits());
            trkPerf_["pt"]->Fill(track.pt());
            trkPerf_["eta"]->Fill( track.eta() );
            trkPerf_["ptHigh"]->Fill(track.pt());
            trkPerf_["phi"]->Fill(track.phi());
            trkPerf_["dxyErr"]->Fill(dxy/dxysigma);
            trkPerf_["dzErr"]->Fill(dz/dzsigma);
            trkPerf_["chi2"]->Fill(track.normalizedChi2());
            trkPerf_["pterr"]->Fill(track.ptError() / track.pt() );
            ntrack_++;
        }
        
    }
    
    ////////// Centrality definition in PbPb //////////////
    CentralityProvider * centProvider = 0;
    if (!centProvider) centProvider = new CentralityProvider(iSetup);
    centProvider->newEvent(iEvent,iSetup);
    double hiBin = centProvider->getBin();

    if( !(hiBin >= cutMultMin_ && hiBin < cutMultMax_)) return;
    evtPerf_["NHPtrk"]->Fill(tHighPurityTracks_);
    
    for( const auto & track : *tracks)
    {
        if( !TrackQualityCuts(track, vsorted[0])) continue;
        
        TVector3 pvector;
        pvector.SetPtEtaPhi(track.pt(),track.eta(),track.phi());
        
        if(track.eta()<=etaMaxAsso1_ && track.eta()>=etaMinAsso1_
           && track.pt()<ptMaxAsso1_ && track.pt()>ptMinAsso1_)
        {
            pVect_ass1.push_back(pvector);
            trkPerf_["ptAsso1"]->Fill(track.pt());
            trkPerf_["etaAsso1"]->Fill(track.eta());
            trkPerf_["phiAsso1"]->Fill(track.phi());
        }
        
        if(track.eta()<=etaMaxAsso2_ && track.eta()>=etaMinAsso2_
           && track.pt()<ptMaxAsso2_ && track.pt()>ptMinAsso2_)
        {
            pVect_ass2.push_back(pvector);
            trkPerf_["ptAsso2"]->Fill(track.pt());
            trkPerf_["etaAsso2"]->Fill(track.eta());
            trkPerf_["phiAsso2"]->Fill(track.phi());
        }
        
        
        TVector3 pvector1;
        pvector1.SetPtEtaPhi(track.pt(),track.eta(),track.phi());
        
        if(track.eta()<=etaMaxTrg_ && track.eta()>=etaMinTrg_
           && track.pt()<=ptMaxTrg_ && track.pt()>=ptMinTrg_)
        {
            pVect_trg.push_back(pvector);
            trkPerf_["ptTrg"]->Fill(track.pt());
            trkPerf_["etaTrg"]->Fill(track.eta());
            trkPerf_["phiTrg"]->Fill(track.phi());
        }
    }
    
    /////// Calculating the combinatorial bkg term for tri-hadron correlations ////////////////
    int nMultTrg = (int)pVect_trg.size();
    int nMultAsso1 = (int)pVect_ass1.size();
    int nMultAsso2 = (int)pVect_ass2.size();
    
    nMultTrig_->Fill(0.5, nMultTrg);
    
    double phi_RndmTrg = gRandom->Uniform(-pi_, pi_);
    for(int nass_f=0; nass_f<nMultAsso1; nass_f++)
    {
        TVector3 pvector_ass1 = (pVect_ass1)[nass_f];
        double phi_ass_f = pvector_ass1.Phi();
        double eta_ass_f = pvector_ass1.Eta();
        int ass_fEta_ = getEtaRegion(eta_ass_f);
                
        int iBin_f = corrFactors_->FindBin(eta_ass_f, vsorted[0].z());
        double eff_f = corrFactors_->GetBinContent(iBin_f);
        eff_f = 1.0;
            
        for(int nass_s=0; nass_s<nMultAsso2; nass_s++)
        {
            if(nass_s == nass_f) continue;
            TVector3 pvector_ass2 = (pVect_ass2)[nass_s];
            double phi_ass_s = pvector_ass2.Phi();
            double eta_ass_s = pvector_ass2.Eta();
            int ass_sEta_ = getEtaRegion(eta_ass_s);
            
            int iBin_s = corrFactors_->FindBin(eta_ass_s, vsorted[0].z());
            double eff_s = corrFactors_->GetBinContent(iBin_s);
            eff_s = 1.0;
    
            double deltaPhi1 = phi_ass_f - phi_RndmTrg;
            double deltaPhi2 = phi_ass_s - phi_RndmTrg;
        
            /////////////// Only for sigma, delta ///////////////
            double deltaPhi1_1 = deltaPhi1;
            double deltaPhi2_2 = deltaPhi2;
            
            if( deltaPhi1_1<-pi_/2 )  deltaPhi1_1 += 2*pi_;
            if( deltaPhi1_1>3*pi_/2 ) deltaPhi1_1 -= 2*pi_;
            if( deltaPhi1_1 < 0 ) deltaPhi1_1 += 2*pi_;
            
            if( deltaPhi2_2<-pi_/2 )  deltaPhi2_2 += 2*pi_;
            if( deltaPhi2_2>3*pi_/2 ) deltaPhi2_2 -= 2*pi_;
            if( deltaPhi2_2 < 0 ) deltaPhi2_2 += 2*pi_;
        
            double sigma = (deltaPhi1_1 + deltaPhi2_2)/2.0 - pi_;
            double delta = (deltaPhi1_1 - deltaPhi2_2)/2.0;
            /////////////////////////////////////////////////////
        
            if(deltaPhi1 > pi_) deltaPhi1 = deltaPhi1 - 2*pi_;
            if(deltaPhi1 < -pi_) deltaPhi1 = deltaPhi1 + 2*pi_;
            if(deltaPhi1 > -pi_ && deltaPhi1 < -pi_/2.0) deltaPhi1 = deltaPhi1 + 2*pi_;
                    
            if(deltaPhi2 > pi_) deltaPhi2 = deltaPhi2 - 2*pi_;
            if(deltaPhi2 < -pi_) deltaPhi2 = deltaPhi2 + 2*pi_;
            if(deltaPhi2 > -pi_ && deltaPhi2 < -pi_/2.0) deltaPhi2 = deltaPhi2 + 2*pi_;
                    
            if(deltaPhi1 == 0 && deltaPhi2 == 0) exit(EXIT_FAILURE);
                
            if(ass_fEta_ == 0 && ass_sEta_== 0) {
                hSignal_["combBkg0"]->Fill(deltaPhi1,deltaPhi2,1.0/eff_f/eff_s);
                hSignal_["combBkg_SD0"]->Fill(sigma,delta,1.0/eff_f/eff_s);
            }
            
            /*  if(ass_fEta_ == 1 && ass_sEta_== 1) {
            hSignal_["combBkg1"]->Fill(deltaPhi1,deltaPhi2,1.0/nMultTrg/eff_f/eff_s);
            hSignal_["combBkg_SD1"]->Fill(sigma,delta,1.0/nMultTrg/eff_f/eff_s);
                }*/
        
            if(ass_fEta_ == 0 && ass_sEta_== 1) {
                hSignal_["combBkg_af0_as1"]->Fill(deltaPhi1,deltaPhi2,1.0/eff_f/eff_s);
                hSignal_["combBkg_SD0_af0_as1"]->Fill(sigma,delta,1.0/eff_f/eff_s);
            }
            
            /*  if(ass_fEta_ == 1 && ass_sEta_== 0) {
            hSignal_["combBkg_af1_as0"]->Fill(deltaPhi1,deltaPhi2,1.0/nMultTrg/eff_f/eff_s);
            hSignal_["combBkg_SD0_af1_as0"]->Fill(sigma,delta,1.0/nMultTrg/eff_f/eff_s);
        }*/
        } //Loop over associated particles
    } //Loop over associated particles
    
    /////// Calculating the signal for tri-hadron correlations ////////////////
    
    for(int ntrg=0; ntrg<nMultTrg; ++ntrg)
    {
        TVector3 pvector_trg = (pVect_trg)[ntrg];
        double phi_trg = pvector_trg.Phi();
      // double eta_trg = pvector_trg.Eta();
      // int trgEta_ = getEtaRegion(eta_trg); Because trigger particle can come from anywhere
        
        for(int nass_f=0; nass_f<nMultAsso1; nass_f++)
        {
            TVector3 pvector_ass1 = (pVect_ass1)[nass_f];
            double phi_ass_f = pvector_ass1.Phi();
            double eta_ass_f = pvector_ass1.Eta();
            int ass_fEta_ = getEtaRegion(eta_ass_f);
            
            int iBin_f = corrFactors_->FindBin(eta_ass_f, vsorted[0].z());
            double eff_f = corrFactors_->GetBinContent(iBin_f);
            eff_f = 1.0;
            
            for(int nass_s=0; nass_s<nMultAsso2; nass_s++)
            {
                if(nass_s == nass_f) continue;
                TVector3 pvector_ass2 = (pVect_ass2)[nass_s];
                double phi_ass_s = pvector_ass2.Phi();
                double eta_ass_s = pvector_ass2.Eta();
                int ass_sEta_ = getEtaRegion(eta_ass_s);
                
                int iBin_s = corrFactors_->FindBin(eta_ass_s, vsorted[0].z());
                double eff_s = corrFactors_->GetBinContent(iBin_s);
                eff_s = 1.0;
                
                double deltaPhi1 = phi_ass_f - phi_trg;
                double deltaPhi2 = phi_ass_s - phi_trg;
                
                /////////////// Only for sigma, delta ///////////////
                double deltaPhi1_1 = deltaPhi1;
                double deltaPhi2_2 = deltaPhi2;
                
                if( deltaPhi1_1<-pi_/2 )  deltaPhi1_1 += 2*pi_ ;
                if( deltaPhi1_1>3*pi_/2 ) deltaPhi1_1 -= 2*pi_ ;
                if( deltaPhi1_1 < 0 ) deltaPhi1_1 += 2*pi_;
                
                if( deltaPhi2_2<-pi_/2 )  deltaPhi2_2 += 2*pi_ ;
                if( deltaPhi2_2>3*pi_/2 ) deltaPhi2_2 -= 2*pi_ ;
                if( deltaPhi2_2 < 0 ) deltaPhi2_2 += 2*pi_;
                
                double sigma = (deltaPhi1_1 + deltaPhi2_2)/2.0 - pi_;
                double delta = (deltaPhi1_1 - deltaPhi2_2)/2.0;
                /////////////////////////////////////////////////////
                
                if(deltaPhi1 > pi_) deltaPhi1 = deltaPhi1 - 2*pi_;
                if(deltaPhi1 < -pi_) deltaPhi1 = deltaPhi1 + 2*pi_;
                if(deltaPhi1 > -pi_ && deltaPhi1 < -pi_/2.0) deltaPhi1 = deltaPhi1 + 2*pi_;
                
                if(deltaPhi2 > pi_) deltaPhi2 = deltaPhi2 - 2*pi_;
                if(deltaPhi2 < -pi_) deltaPhi2 = deltaPhi2 + 2*pi_;
                if(deltaPhi2 > -pi_ && deltaPhi2 < -pi_/2.0) deltaPhi2 = deltaPhi2 + 2*pi_;
            
                if(deltaPhi1 == 0 && deltaPhi2 == 0) exit(EXIT_FAILURE);
                
                if(ass_fEta_ == 0 && ass_sEta_==0) {
             //       hSignal_["0"]->Fill(deltaPhi1,deltaPhi2,1.0/nMultTrg/eff_f/eff_s);
             //       hSignal_["SD0"]->Fill(sigma,delta,1.0/nMultTrg/eff_f/eff_s);
                    hSignal_["0"]->Fill(deltaPhi1,deltaPhi2,1.0/eff_f/eff_s);
                    hSignal_["SD0"]->Fill(sigma,delta,1.0/eff_f/eff_s);
                }
                
              /*  if(ass_fEta_ == 1 && ass_sEta_==1) {
                    hSignal_["1"]->Fill(deltaPhi1,deltaPhi2,1.0/nMultTrg/eff_f/eff_s);
                    hSignal_["SD1"]->Fill(sigma,delta,1.0/nMultTrg/eff_f/eff_s);
                }*/
                
                if(ass_fEta_ == 0 && ass_sEta_==1) {
                //    hSignal_["af0_as1"]->Fill(deltaPhi1,deltaPhi2,1.0/nMultTrg/eff_f/eff_s);
                //    hSignal_["SD_af0_as1"]->Fill(sigma,delta,1.0/nMultTrg/eff_f/eff_s);
                    hSignal_["af0_as1"]->Fill(deltaPhi1,deltaPhi2,1.0/eff_f/eff_s);
                    hSignal_["SD_af0_as1"]->Fill(sigma,delta,1.0/eff_f/eff_s);
                }
                
             /*   if(ass_fEta_ == 1 && ass_sEta_==0) {
                    hSignal_["af1_as0"]->Fill(deltaPhi1,deltaPhi2,1.0/nMultTrg/eff_f/eff_s);
                    hSignal_["SD_af1_as0"]->Fill(sigma,delta,1.0/nMultTrg/eff_f/eff_s);
                }*/
            } //Loop over associated particles
        } //Loop over associated particles
    } //Loop over trigger particles
    
    pVectVect_trg.push_back(pVect_trg);
    pVectVect_ass1.push_back(pVect_ass1);
    pVectVect_ass2.push_back(pVect_ass2);
    zvtxVect.push_back(vsorted[0].z());
    
}

void
TriHadronAnalyzer::initHistos(const edm::Service<TFileService> & fs)
{
    
    TH1D::SetDefaultSumw2();
    TH2D::SetDefaultSumw2();
    nMultTrig_ = fs->make<TH1F>("nMultTrig","Trigger multiplicity",1,0,1);
    vertices_ = fs->make<TH1F>("vertices","",1,0,1);
    
    evtPerf_["Ntrk"] = fs->make<TH1F>("evtNtrk","Tracks per event",100,0,400);
    evtPerf_["NHPtrk"] = fs->make<TH1F>("evtHPNtrk","High purity tracks per event",100,0,10000);
    evtPerf_["Nvtx"] = fs->make<TH1F>("evtNvtx","Primary Vertices per event",10,0,10);
    evtPerf_["NvtxLumi"] = fs->make<TH1F>("evtNvtxLumi","Primary Vertices by Lumi",200,0,2000);
    evtPerf_["Lumi"] = fs->make<TH1F>("evtLumi","Events by Lumi",200,0,2000);
    
    vtxPerf_["Ntrk"] = fs->make<TH1F>("vtxNtrk","Tracks per vertex",50,0,200);
    vtxPerf_["x"] = fs->make<TH1F>("vtxX","Vertex x position",1000,-1,1);
    vtxPerf_["y"] = fs->make<TH1F>("vtxY","Vertex y position",1000,-1,1);
    vtxPerf_["z"] = fs->make<TH1F>("vtxZ","Vertex z position",100,-30,30);
    vtxPerf_["assocVtxDz"] = fs->make<TH1F>("assocVtxDz","Z Distance from first PV; dz (cm)",200,0,50);
    vtxPerf_["assocVtxDxy"] = fs->make<TH1F>("assocVtxDxy","Rho Distance from first PV; dxy (cm)",200,0,4);
    
    vtxPerf2D_["Ntrk2D"] = fs->make<TH2F>("vtxNtrk2D","Tracks per vertex;vertex (sorted by Ntrk);Ntrk"
                                          ,10,0,10,200,0,200);
    vtxPerf2D_["assocVtxDzNtrk"] = fs->make<TH2F>("assocVtxDzNtrk",
                                                  "Z Distance from first PV vs Ntrk of assoc; dz (cm); Ntrk",
                                                  200,0,50,50,0,200);
    vtxPerf2D_["assocVtxDxyNtrk"] = fs->make<TH2F>("assocVtxDxyNtrk",
                                                   "Rho Distance from first PV vs Ntrk of assoc; dxy (cm); Ntrk",
                                                   200,0,4,50,0,200);
    
    trkPerf_["Nhit"] = fs->make<TH1F>("trkNhit", "Tracks by Number of Valid Hits;N hits",    35,  0,35);
    trkPerf_["pt"] = fs->make<TH1F>("trkPt", "Track p_{T} Distribution;p_{T} [GeV/c]",100,0,50);
    trkPerf_["ptHigh"] = fs->make<TH1F>("trkPtHigh", "Track p_{T} Distribution;p_{T} [GeV/c]",100,0,200);
    trkPerf_["eta"] = fs->make<TH1F>("trkEta", "Track iConfigeudorapidity Distribution;#eta",50,-2.5,2.5);
    trkPerf_["phi"] = fs->make<TH1F>("trkPhi", "Track Azimuthal Distribution;#phi",100,-3.15,3.15);
    trkPerf_["chi2"] = fs->make<TH1F>("trkChi2", "Track Normalized #chi^{2};#chi^{2}/n.d.o.f",60,0,6);
    trkPerf_["pterr"] = fs->make<TH1F>("trkPterr", "Track p_{T} error;#delta p_{T} / p_{T}",50,0,0.2);
    trkPerf_["dxyErr"] = fs->make<TH1F>("trkDxyErr", "Transverse DCA Significance;dxy / #sigma_{dxy}",100,-8,8);
    trkPerf_["dzErr"] = fs->make<TH1F>("trkDzErr", "Longitudinal DCA Significance;dz / #sigma_{dz}",100,-8,8);
    
    char histoName1[300];
    char histoTitle1[1000];
    char histoName2[200];
    char histoTitle2[200];
    
    for(int kVz=0; kVz<int(vzBins_.size()-1); kVz++) {
        
        sprintf(histoName1, "hdNdEta_VzBin_%d", kVz);
        sprintf(histoTitle1, "dNdEta distribution for %5.2f < V_{z} < %5.2f ", vzBins_[kVz], vzBins_[kVz+1]);
        hdNdEtaVzBin_[histoName1] = fs->make<TH1F>(histoName1, histoTitle1, 51, etaMin_, etaMax_);
        
        sprintf(histoName2, "nEventsVzBin_%d", kVz);
        sprintf(histoTitle2, "No of events for %5.2f < V_{z} < %5.2f ", vzBins_[kVz], vzBins_[kVz+1]);
        hEventVzBin_[histoName2] = fs->make<TH1F>(histoName2, histoTitle2, 1, 0, 1);
    }
    
    hSignal_["0"] = fs->make<TH2D>("hSignal0", "#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hSignal_["1"] = fs->make<TH2D>("hSignal1", "#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hSignal_["af0_as1"] = fs->make<TH2D>("hSignal_af0_as1", "#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hSignal_["af1_as0"] = fs->make<TH2D>("hSignal_af1_as0", "#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    
    hSignal_["SD0"] = fs->make<TH2D>("hSignal_SD0", "#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    hSignal_["SD1"] = fs->make<TH2D>("hSignal_SD1", "#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    hSignal_["SD_af0_as1"] = fs->make<TH2D>("hSignal_SD_af0_as1", "#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    hSignal_["SD_af1_as0"] = fs->make<TH2D>("hSignal_SD_af1_as0", "#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    
    hSignal_["combBkg0"] = fs->make<TH2D>("hSignal_combBkg0", "#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hSignal_["combBkg1"] = fs->make<TH2D>("hSignal_combBkg1", "#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hSignal_["combBkg_af0_as1"] = fs->make<TH2D>("hSignal_combBkg_af0_as1", "#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hSignal_["combBkg_af1_as0"] = fs->make<TH2D>("hSignal_combBkg_af1_as0", "#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    
    hSignal_["combBkg_SD0"] = fs->make<TH2D>("hSignal_combBkg_SD0", "#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    hSignal_["combBkg_SD1"] = fs->make<TH2D>("hSignal_combBkg_SD1", "#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    hSignal_["combBkg_SD0_af0_as1"] = fs->make<TH2D>("hSignal_combBkg_SD0_af0_as1", "#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    hSignal_["combBkg_SD0_af1_as0"] = fs->make<TH2D>("hSignal_combBkg_SD0_af1_as0", "#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    
    hBackground_["0"] = fs->make<TH2D>("hBackground0", ";#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hBackground_["1"] = fs->make<TH2D>("hBackground1", ";#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hBackground_["af0_as1"] = fs->make<TH2D>("hBackground_af0_as1", ";#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hBackground_["af1_as0"] = fs->make<TH2D>("hBackground_af1_as0", ";#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);

    hBackground_["SD0"] = fs->make<TH2D>("hBackground_SD0", ";#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    hBackground_["SD0_af0_as1"] = fs->make<TH2D>("hBackground_SD0_af0_as1", ";#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    
    hBackground_["combBkg0"] = fs->make<TH2D>("hBkg_combBkg0", ";#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hBackground_["combBkg_SD0"] = fs->make<TH2D>("hBkg_combBkgSD0", ";#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    
    hBackground_["combBkg_af0_as1"] = fs->make<TH2D>("hBkg_combBkg_af0_as1", ";#Delta#phi;#Delta#phi", 96,-pi_/2+pi_/32,3*pi_/2-pi_/32,96,-pi_/2+pi_/32,3*pi_/2-pi_/32);
    hBackground_["combBkg_SD0_af0_as1"] = fs->make<TH2D>("hBkg_combBkg_SD0_af0_as1", ";#Delta#phi;#Delta#phi", 96,-pi_,pi_,96,-pi_,pi_);
    
    trkPerf_["ptAsso1"] = fs->make<TH1F>("trkPtAsso1", "Associated (1) Track p_{T} Distribution;p_{T} [GeV/c]",100,0,10);
    trkPerf_["etaAsso1"] = fs->make<TH1F>("trkEtaAsso1", "Associated (1) Track pseudorapidity Distribution;#eta",51,-2.5,2.5);
    trkPerf_["phiAsso1"] = fs->make<TH1F>("trkPhiAsso1", "Associated (1) Track Azimuthal Distribution;#phi",100,-3.15,3.15);
    
    trkPerf_["ptAsso2"] = fs->make<TH1F>("trkPtAsso2", "Associated (2) Track p_{T} Distribution;p_{T} [GeV/c]",100,0,10);
    trkPerf_["etaAsso2"] = fs->make<TH1F>("trkEtaAsso2", "Associated (2) Track pseudorapidity Distribution;#eta",51,-2.5,2.5);
    trkPerf_["phiAsso2"] = fs->make<TH1F>("trkPhiAsso2", "Associated (2) Track Azimuthal Distribution;#phi",100,-3.15,3.15);
    
    trkPerf_["ptTrg"] = fs->make<TH1F>("trkPtTrg", "Trigger Track p_{T} Distribution;p_{T} [GeV/c]",100,0,10);
    trkPerf_["etaTrg"] = fs->make<TH1F>("trkEtaTrg", "Trigger Track pseudorapidity Distribution;#eta",51,-2.5,2.5);
    trkPerf_["phiTrg"] = fs->make<TH1F>("trkPhiTrg", "Trigger Track Azimuthal Distribution;#phi",100,-3.15,3.15);
}

bool
TriHadronAnalyzer::vtxSort( const reco::Vertex &  a, const reco::Vertex & b )
{
    if( a.tracksSize() != b.tracksSize() )
    return  a.tracksSize() > b.tracksSize() ? true : false ;
    else
    return  a.chi2() < b.chi2() ? true : false ;
}

bool
TriHadronAnalyzer::TrackQualityCuts(const reco::Track & track, const reco::Vertex & vertexCollectionSelected)
{
    
    math::XYZPoint vtxPoint(0.0,0.0,0.0);
    double vzErr =0.0, vxErr=0.0, vyErr=0.0;
    vtxPoint=vertexCollectionSelected.position();
    vzErr=vertexCollectionSelected.zError();
    vxErr=vertexCollectionSelected.xError();
    vyErr=vertexCollectionSelected.yError();
    
    double dxy=0.0, dz=0.0, dxysigma=0.0, dzsigma=0.0;
    dxy = track.dxy(vtxPoint);
    dz = track.dz(vtxPoint);
    dxysigma = sqrt(track.d0Error()*track.d0Error()+vxErr*vyErr);
    dzsigma = sqrt(track.dzError()*track.dzError()+vzErr*vzErr);
    
    if(track.quality(reco::TrackBase::qualityByName(qualityString_)) != 1)
    return false;
    if(fabs(dxy/dxysigma) > cutDxyErrMax_) return false;
    if(fabs(dz/dzsigma) > cutDzErrMax_) return false;
    if(track.ptError() / track.pt() > cutPtErrMax_) return false;
    
    return true;
    
}

int
TriHadronAnalyzer::getEtaRegion(const double eta)
{
    if(eta > -2.4 && eta < 0.0) return 0;
    if(eta >= 0.0 && eta < 2.4) return 1;
    
    else return -1;
}



// ------------ method called once each job just before starting event loop  ------------
void
TriHadronAnalyzer::beginJob()
{
    TFile * file = new TFile("/home/sharmam/CMSSW_5_3_22/src/ThreeParticleCorr/TriHadronAnalyzer/PbPbRun2011/CorrectionFactors_PbPb.root","r");
    corrFactors_ = (TH2F*)file->Get("hVzEtaEff_cent");
}

// ------------ method called once each job just after ending the event loop  ------------
void
TriHadronAnalyzer::endJob()
{
    ////////////////// Calculating background for pi0-hadron correlations //////////
    int nevttotal_trg = (int)pVectVect_trg.size();
    int nevttotal_ass1 = (int)pVectVect_ass1.size();
    int nevttotal_ass2 = (int)pVectVect_ass2.size();
    
    for(int nround=0;nround<bkgFactor_;nround++)
    {
        int ncount = 0;
        for(int nevt_trg=0; nevt_trg<nevttotal_trg; nevt_trg++)
        {
            int nevt_ass1 = gRandom->Integer(nevttotal_ass1);
            int nevt_ass2 = gRandom->Integer(nevttotal_ass2);
            if(nevt_trg == nevt_ass1) { nevt_trg--; continue; }
            if(fabs((zvtxVect)[nevt_trg]-(zvtxVect)[nevt_ass1])>0.5) {
                nevt_trg--;
                ncount++;
                if(ncount>5000) {nevt_trg++; ncount = 0;}
                continue;
            }
            
            if(nevt_trg == nevt_ass2) { nevt_trg--; continue; }
            if(fabs((zvtxVect)[nevt_trg]-(zvtxVect)[nevt_ass2])>0.5) {
                nevt_trg--;
                ncount++;
                if(ncount>5000) {nevt_trg++; ncount = 0;}
                continue;
            }
            
            vector<TVector3> pVectTmp_trg = (pVectVect_trg)[nevt_trg];
            vector<TVector3> pVectTmp_ass1 = (pVectVect_ass1)[nevt_ass1];
            vector<TVector3> pVectTmp_ass2 = (pVectVect_ass2)[nevt_ass2];
            
            int nMult_trg1 = pVectTmp_trg.size();
            int nMult_ass1 = pVectTmp_ass1.size();
            int nMult_ass2 = pVectTmp_ass2.size();
            
            for(int ntrg=0; ntrg<nMult_trg1; ++ntrg)
            {
                TVector3 pvectorTmp_trg = pVectTmp_trg[ntrg];
                double phi_trg = pvectorTmp_trg.Phi();
              //  double eta_trg = pvectorTmp_trg.Eta();
              //  int trgEta_ = getEtaRegion(eta_trg);
                
                for(int nass_f=0; nass_f<nMult_ass1; ++nass_f)
                {
                    TVector3 pvectorTmp_ass1 = pVectTmp_ass1[nass_f];
                    double phi_ass1 = pvectorTmp_ass1.Phi();
                    double eta_ass1 = pvectorTmp_ass1.Eta();
                    int ass_fEta_ = getEtaRegion(eta_ass1);
                    
                    double zvtx_f = (zvtxVect)[nass_f];
                    int iBin_f = corrFactors_->FindBin(eta_ass1, zvtx_f);
                    double eff_f = corrFactors_->GetBinContent(iBin_f);
                    eff_f = 1.0;
                    
                    for(int nass_s=0; nass_s<nMult_ass2; ++nass_s)
                    {
                        if( nass_f == nass_s ) continue;
                        TVector3 pvectorTmp_ass2 = pVectTmp_ass2[nass_s];
                        double phi_ass2 = pvectorTmp_ass2.Phi();
                        double eta_ass2 = pvectorTmp_ass2.Eta();
                        int ass_sEta_ = getEtaRegion(eta_ass2);
                        
                        double zvtx_s = (zvtxVect)[nass_s];
                        int iBin_s = corrFactors_->FindBin(eta_ass2, zvtx_s);
                        double eff_s = corrFactors_->GetBinContent(iBin_s);
                        eff_s = 1.0;
                        
                        double deltaPhi1 = phi_ass1 - phi_trg;
                        double deltaPhi2 = phi_ass2 - phi_trg;
                        
                        /////////////// Only for sigma, delta ///////////////
                        double deltaPhi1_1 = deltaPhi1;
                        double deltaPhi2_2 = deltaPhi2;
                        
                        if( deltaPhi1_1<-pi_/2 )  deltaPhi1_1 += 2*pi_ ;
                        if( deltaPhi1_1>3*pi_/2 ) deltaPhi1_1 -= 2*pi_ ;
                        if( deltaPhi1_1 < 0 ) deltaPhi1_1 += 2*pi_;
                        
                        if( deltaPhi2_2<-pi_/2 )  deltaPhi2_2 += 2*pi_ ;
                        if( deltaPhi2_2>3*pi_/2 ) deltaPhi2_2 -= 2*pi_ ;
                        if( deltaPhi2_2 < 0 ) deltaPhi2_2 += 2*pi_;
                        
                        double sigma = (deltaPhi1_1 + deltaPhi2_2)/2.0 - pi_;
                        double delta = (deltaPhi1_1 - deltaPhi2_2)/2.0;
                        /////////////////////////////////////////////////////
                        
                        if(deltaPhi1 > pi_) deltaPhi1 = deltaPhi1 - 2*pi_;
                        if(deltaPhi1 < -pi_) deltaPhi1 = deltaPhi1 + 2*pi_;
                        if(deltaPhi1 > -pi_ && deltaPhi1 < -pi_/2.0) deltaPhi1 = deltaPhi1 + 2*pi_;
                        
                        if(deltaPhi2 > pi_) deltaPhi2 = deltaPhi2 - 2*pi_;
                        if(deltaPhi2 < -pi_) deltaPhi2 = deltaPhi2 + 2*pi_;
                        if(deltaPhi2 > -pi_ && deltaPhi2 < -pi_/2.0) deltaPhi2 = deltaPhi2 + 2*pi_;
                        
                        if(deltaPhi1 == 0 && deltaPhi2 == 0) exit(EXIT_FAILURE);
                        
                        if(ass_fEta_ == 0 && ass_sEta_==0) {
                            if(eff_f==0 || eff_s==0 ) continue;
                        //    hBackground_["0"]->Fill(deltaPhi1,deltaPhi2,1.0/nMult_trg1/eff_f/eff_s);
                        //    hBackground_["SD0"]->Fill(sigma,delta,1.0/nMult_trg1/eff_f/eff_s);
                            hBackground_["0"]->Fill(deltaPhi1,deltaPhi2,1.0/eff_f/eff_s);
                            hBackground_["SD0"]->Fill(sigma,delta,1.0/eff_f/eff_s);
                        }
                        
                     /*   if(ass_fEta_ == 1 && ass_sEta_==1) {
                            if(eff_f==0 || eff_s==0 ) continue;
                            hBackground_["1"]->Fill(deltaPhi1,deltaPhi2,1.0/nMult_trg1/eff_f/eff_s); }*/
                        
                        if(ass_fEta_ == 0 && ass_sEta_==1) {
                            if(eff_f==0 || eff_s==0 ) continue;
                       //     hBackground_["af0_as1"]->Fill(deltaPhi1,deltaPhi2,1.0/nMult_trg1/eff_f/eff_s);
                       //     hBackground_["SD0_af0_as1"]->Fill(sigma,delta,1.0/nMult_trg1/eff_f/eff_s);
                            hBackground_["af0_as1"]->Fill(deltaPhi1,deltaPhi2,1.0/eff_f/eff_s);
                            hBackground_["SD0_af0_as1"]->Fill(sigma,delta,1.0/eff_f/eff_s);
                        }
                        
                     /*   if(ass_fEta_ == 1 && ass_sEta_==0) {
                            if(eff_f==0 || eff_s==0 ) continue;
                            hBackground_["af1_as0"]->Fill(deltaPhi1,deltaPhi2,1.0/nMult_trg1/eff_f/eff_s); }*/
                    
                    }
                }
            }
        }
    }
    
    ///////////// Second loop is for combinatorial-bkg ///////////////////
    for(int nround=0;nround<bkgFactor_;nround++)
    {
        int ncount = 0;
        for(int nevt_trg=0; nevt_trg<nevttotal_trg; nevt_trg++)
        {
            int nevt_ass1 = gRandom->Integer(nevttotal_ass1);
            int nevt_ass2 = gRandom->Integer(nevttotal_ass2);
            if(nevt_trg == nevt_ass1) { nevt_trg--; continue; }
            if(fabs((zvtxVect)[nevt_trg]-(zvtxVect)[nevt_ass1])>0.5) {
                nevt_trg--;
                ncount++;
                if(ncount>5000) {nevt_trg++; ncount = 0;}
                continue;
            }
            
            if(nevt_trg == nevt_ass2) { nevt_trg--; continue; }
            if(fabs((zvtxVect)[nevt_trg]-(zvtxVect)[nevt_ass2])>0.5) {
                nevt_trg--;
                ncount++;
                if(ncount>5000) {nevt_trg++; ncount = 0;}
                continue;
            }
            
            vector<TVector3> pVectTmp_ass1 = (pVectVect_ass1)[nevt_ass1];
            vector<TVector3> pVectTmp_ass2 = (pVectVect_ass2)[nevt_ass2];
            
            int nMult_ass1 = pVectTmp_ass1.size();
            int nMult_ass2 = pVectTmp_ass2.size();
            
            double phi_RndmTrg = gRandom->Uniform(-pi_, pi_);
                
            for(int nass_f=0; nass_f<nMult_ass1; ++nass_f)
            {
                TVector3 pvectorTmp_ass1 = pVectTmp_ass1[nass_f];
                double phi_ass1 = pvectorTmp_ass1.Phi();
                double eta_ass1 = pvectorTmp_ass1.Eta();
                int ass_fEta_ = getEtaRegion(eta_ass1);
                    
                double zvtx_f = (zvtxVect)[nass_f];
                int iBin_f = corrFactors_->FindBin(eta_ass1, zvtx_f);
                double eff_f = corrFactors_->GetBinContent(iBin_f);
                eff_f = 1.0;
                    
                for(int nass_s=0; nass_s<nMult_ass2; ++nass_s)
                {
                    if( nass_f == nass_s ) continue;
                    TVector3 pvectorTmp_ass2 = pVectTmp_ass2[nass_s];
                    double phi_ass2 = pvectorTmp_ass2.Phi();
                    double eta_ass2 = pvectorTmp_ass2.Eta();
                    int ass_sEta_ = getEtaRegion(eta_ass2);
                        
                    double zvtx_s = (zvtxVect)[nass_s];
                    int iBin_s = corrFactors_->FindBin(eta_ass2, zvtx_s);
                    double eff_s = corrFactors_->GetBinContent(iBin_s);
                    eff_s = 1.0;
                        
                    double deltaPhi1 = phi_ass1 - phi_RndmTrg;
                    double deltaPhi2 = phi_ass2 - phi_RndmTrg;
                        
                    /////////////// Only for sigma, delta ///////////////
                    double deltaPhi1_1 = deltaPhi1;
                    double deltaPhi2_2 = deltaPhi2;
                        
                    if( deltaPhi1_1<-pi_/2 )  deltaPhi1_1 += 2*pi_ ;
                    if( deltaPhi1_1>3*pi_/2 ) deltaPhi1_1 -= 2*pi_ ;
                    if( deltaPhi1_1 < 0 ) deltaPhi1_1 += 2*pi_;
                        
                    if( deltaPhi2_2<-pi_/2 )  deltaPhi2_2 += 2*pi_ ;
                    if( deltaPhi2_2>3*pi_/2 ) deltaPhi2_2 -= 2*pi_ ;
                    if( deltaPhi2_2 < 0 ) deltaPhi2_2 += 2*pi_;
                        
                    double sigma = (deltaPhi1_1 + deltaPhi2_2)/2.0 - pi_;
                    double delta = (deltaPhi1_1 - deltaPhi2_2)/2.0;
                    /////////////////////////////////////////////////////
                        
                    if(deltaPhi1 > pi_) deltaPhi1 = deltaPhi1 - 2*pi_;
                    if(deltaPhi1 < -pi_) deltaPhi1 = deltaPhi1 + 2*pi_;
                    if(deltaPhi1 > -pi_ && deltaPhi1 < -pi_/2.0) deltaPhi1 = deltaPhi1 + 2*pi_;
                        
                    if(deltaPhi2 > pi_) deltaPhi2 = deltaPhi2 - 2*pi_;
                    if(deltaPhi2 < -pi_) deltaPhi2 = deltaPhi2 + 2*pi_;
                    if(deltaPhi2 > -pi_ && deltaPhi2 < -pi_/2.0) deltaPhi2 = deltaPhi2 + 2*pi_;
                        
                    if(deltaPhi1 == 0 && deltaPhi2 == 0) exit(EXIT_FAILURE);
                        
                    if(ass_fEta_ == 0 && ass_sEta_==0) {
                        if(eff_f==0 || eff_s==0 ) continue;
                        hBackground_["combBkg0"]->Fill(deltaPhi1,deltaPhi2,1.0/eff_f/eff_s);
                        hBackground_["combBkg_SD0"]->Fill(sigma,delta,1.0/eff_f/eff_s);
                    }
                        
                    /*   if(ass_fEta_ == 1 && ass_sEta_==1) {
                        if(eff_f==0 || eff_s==0 ) continue;
                        hBackground_["1"]->Fill(deltaPhi1,deltaPhi2,1.0/nMult_trg1/eff_f/eff_s); }*/
                    
                    if(ass_fEta_ == 0 && ass_sEta_==1) {
                        if(eff_f==0 || eff_s==0 ) continue;
                        hBackground_["combBkg_af0_as1"]->Fill(deltaPhi1,deltaPhi2,1.0/eff_f/eff_s);
                        hBackground_["combBkg_SD0_af0_as1"]->Fill(sigma,delta,1.0/eff_f/eff_s);
                    }
                    
                    /*   if(ass_fEta_ == 1 && ass_sEta_==0) {
                        if(eff_f==0 || eff_s==0 ) continue;
                        hBackground_["af1_as0"]->Fill(deltaPhi1,deltaPhi2,1.0/nMult_trg1/eff_f/eff_s); }*/
                    
                }
            }
        }
    }
}

//define this as a plug-in
DEFINE_FWK_MODULE(TriHadronAnalyzer);
