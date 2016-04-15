
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidate.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPMulTrackCandidateCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPTrackCandidateDistinctCollectionsSet.h"
#include "DataFormats/Common/interface/Wrapper.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPMulFittedTrackCollection.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/CTPPSReco/interface/TotemRPCluster.h"
#include <map>
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProton.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonCollection.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHit.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RP2DHitDebug.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPair.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPReconstructedProtonPairCollection.h"

#include <vector>
#include "RecoTotemRP/RPRecoDataFormats/interface/CentralMassInfo.h"

#include "RecoTotemRP/RPRecoDataFormats/interface/RPStationTrackFit.h"
#include "RecoTotemRP/RPRecoDataFormats/interface/RPStationTrackFitCollection.h"



namespace {
  namespace {
    
    CentralMassInfo ctrinfo;
    edm::Wrapper<CentralMassInfo> wctrinfo;
    
    RPTrackCandidate tc;
    RPTrackCandidateCollection coll;
    RPMulTrackCandidateCollection mcoll;
    std::pair<unsigned int, RPTrackCandidate> p_rpid_rptrcand;
    std::pair<unsigned int, std::vector<RPTrackCandidate> > p_rpid_rptrcandvec;
    std::map<unsigned int, RPTrackCandidate> m_rpid_rptrcand;
    std::map<unsigned int, std::vector<RPTrackCandidate> > m_rpid_rptrcandvec;
    RPTrackCandidateDistinctCollectionsSet coll_set;
    std::vector<RPTrackCandidate> v_tr_cand;
    edm::Wrapper<RPTrackCandidateCollection> RPTrackCandidateCollectionWrapper;
    edm::Wrapper<RPMulTrackCandidateCollection> RPMulTrackCandidateCollectionWrapper;
    edm::Wrapper<RPTrackCandidateDistinctCollectionsSet> RPTrackCandidateDistinctCollectionsSetWrapper;
    
    edm::Wrapper<RPMulFittedTrackSetsCollection> wraprpmulttracksetcol; 
    
    std::vector<std::vector<RPTrackCandidate> > vvrptrcand;
    std::map<unsigned int, std::vector<std::vector<RPTrackCandidate> > > mapvvrptrcand;
    RPMulTrackCandidateSetsCollection rpmultracsetcandcol;
    edm::Wrapper<std::vector<std::vector<RPTrackCandidate> > > wrapvvrptrcand;
    edm::Wrapper<std::map<unsigned int, std::vector<std::vector<RPTrackCandidate> > > > wrapmapvvrptrcand;
    edm::Wrapper<RPMulTrackCandidateSetsCollection> wraprpmultracsetcandcol;
    
    std::less<unsigned int> lui;
    std::allocator<std::pair<const unsigned int,RPTrackCandidate> > apcuirptc;
    std::binary_function<unsigned int,unsigned int,bool> bf;

    RPMulFittedTrackCollection the_mtrack_cand_col;
    edm::Wrapper<RPMulFittedTrackCollection> the_w_mrpftc;
    
    RPDetHitPoint rpdhp;
    std::vector<RPDetHitPoint> vrdhp;
    edm::Wrapper<RPDetHitPoint> wrpdhp;
    edm::Wrapper<std::vector<RPDetHitPoint> > wvrdhp;
    
    RPReconstructedProton rprecprot;
    edm::Wrapper<RPReconstructedProton> wraprprecprot;
    std::vector<RPReconstructedProton> prprecprotvec;
    
    RPReconstructedProtonCollection rprpcol;
    edm::Wrapper<RPReconstructedProtonCollection> wrrprpcol;
    
    RP2DHit rp2dhit;
    edm::Wrapper<RP2DHit> wrp2dhit;
    RP2DHitDebug rp2debugdhit;
    edm::Wrapper<RP2DHitDebug> rwp2debugdhit;
    
    std::map<unsigned int, RP2DHitDebug> vrp2debugdhit;
    edm::Wrapper<std::map<unsigned int, RP2DHitDebug> > wvrp2debugdhit;
    std::pair<unsigned int, RP2DHitDebug> vrp2debughitpair;
    
    RPReconstructedProtonPair p;
    edm::Wrapper<RPReconstructedProtonPair> wp;
    std::vector<RPReconstructedProtonPair> vp;
    
    RPReconstructedProtonPairCollection pc;
    edm::Wrapper<RPReconstructedProtonPairCollection> wpc;

	RPStationTrackFit rpstf;
	std::vector<RPStationTrackFit> vrpstf;
	std::map<unsigned int, std::vector<RPStationTrackFit> > m_ui_vrpstf;
	RPStationTrackFitCollection rpstfc;
	edm::Wrapper<RPStationTrackFitCollection> w_rpstfc;

  }
}
