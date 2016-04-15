/****************************************************************************
*
* This is a part of TotemDQM and TOTEM offline software.
* Authors:
*   Jan Kašpar (jan.kaspar@gmail.com)
*   Rafał Leszko (rafal.leszko@gmail.com)
*
****************************************************************************/

#include "DQM/TotemRP/interface/TotemRPDQMSource.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/TotemRPDetId/interface/TotemRPDetId.h"

#include "Geometry/Records/interface/VeryForwardRealGeometryRecord.h"
#include "Geometry/VeryForwardGeometryBuilder/interface/TotemRPGeometry.h"

#include <string>

using namespace std;
using namespace edm;

//----------------------------------------------------------------------------------------------------

TotemRPDQMSource::DiagonalPlots::DiagonalPlots(DQMStore::IBooker &ibooker, int _id) : id(_id)
{
  bool top45 = id & 2;
  bool top56 = id & 1;
  bool diag = (top45 != top56);

  char name[50];
  sprintf(name, "%s 45%s - 56%s",
    (diag) ? "diagonal" : "antidiagonal",
    (top45) ? "top" : "bot",
    (top56) ? "top" : "bot"
  );

  ibooker.setCurrentFolder(string("Totem/RP/") + name);

  h_lrc_x_d = ibooker.book2D("dx left vs right", string(name) + " : dx left vs. right, histogram;#Delta x_{45};#Delta x_{56}", 50, 0., 0., 50, 0., 0.);
  h_lrc_x_n = ibooker.book2D("xn left vs right", string(name) + " : xn left vs. right, histogram;x^{N}_{45};x^{N}_{56}", 50, 0., 0., 50, 0., 0.);
  h_lrc_x_f = ibooker.book2D("xf left vs right", string(name) + " : xf left vs. right, histogram;x^{F}_{45};x^{F}_{56}", 50, 0., 0., 50, 0., 0.);

  h_lrc_y_d = ibooker.book2D("dy left vs right", string(name) + " : dy left vs. right, histogram;#Delta y_{45};#Delta y_{56}", 50, 0., 0., 50, 0., 0.);
  h_lrc_y_n = ibooker.book2D("yn left vs right", string(name) + " : yn left vs. right, histogram;y^{N}_{45};y^{N}_{56}", 50, 0., 0., 50, 0., 0.);
  h_lrc_y_f = ibooker.book2D("yf left vs right", string(name) + " : yf left vs. right, histogram;y^{F}_{45};y^{F}_{56}", 50, 0., 0., 50, 0., 0.);
}

//----------------------------------------------------------------------------------------------------

TotemRPDQMSource::ArmPlots::ArmPlots(DQMStore::IBooker &ibooker, int _id) : id(_id)
{
  ibooker.setCurrentFolder(string("Totem/") + TotemRPDetId::armName(id, TotemRPDetId::nPath));

  h_numRPWithTrack_top = ibooker.book1D("number of top RPs with tracks", "number of top RPs with tracks;number of top RPs with tracks", 5, -0.5, 4.5);
  h_numRPWithTrack_hor = ibooker.book1D("number of hor RPs with tracks", "number of hor RPs with tracks;number of hor RPs with tracks", 5, -0.5, 4.5);
  h_numRPWithTrack_bot = ibooker.book1D("number of bot RPs with tracks", "number of bot RPs with tracks;number of bot RPs with tracks", 5, -0.5, 4.5);

  h_trackCorr = ibooker.book2D("track RP correlation", "track RP correlation", 13, -0.5, 12.5, 13, -0.5, 12.5);
  TH2F *h_trackCorr_h = h_trackCorr->getTH2F();
  TAxis *xa = h_trackCorr_h->GetXaxis(), *ya = h_trackCorr_h->GetYaxis();
  xa->SetBinLabel(1, "210, near, top"); ya->SetBinLabel(1, "210, near, top");
  xa->SetBinLabel(2, "bot"); ya->SetBinLabel(2, "bot");
  xa->SetBinLabel(3, "hor"); ya->SetBinLabel(3, "hor");
  xa->SetBinLabel(4, "far, hor"); ya->SetBinLabel(4, "far, hor");
  xa->SetBinLabel(5, "top"); ya->SetBinLabel(5, "top");
  xa->SetBinLabel(6, "bot"); ya->SetBinLabel(6, "bot");
  xa->SetBinLabel(8, "220, near, top"); ya->SetBinLabel(8, "220, near, top");
  xa->SetBinLabel(9, "bot"); ya->SetBinLabel(9, "bot");
  xa->SetBinLabel(10, "hor"); ya->SetBinLabel(10, "hor");
  xa->SetBinLabel(11, "far, hor"); ya->SetBinLabel(11, "far, hor");
  xa->SetBinLabel(12, "top"); ya->SetBinLabel(12, "top");
  xa->SetBinLabel(13, "bot"); ya->SetBinLabel(13, "bot");

  h_trackCorr_overlap = ibooker.book2D("track RP correlation hor-vert overlaps", "track RP correlation hor-vert overlaps", 13, -0.5, 12.5, 13, -0.5, 12.5);
}

//----------------------------------------------------------------------------------------------------

TotemRPDQMSource::StationPlots::StationPlots(DQMStore::IBooker &ibooker, int _id, std::set<unsigned int> planes, 
  bool allocateCorrelationPlots, CorrelationPlotsSelector *correlationPlotsSelector, int limit) : 
    id(_id)
{
  ibooker.setCurrentFolder(string("Totem/") + TotemRPDetId::stationName(id, TotemRPDetId::nPath));

  if (allocateCorrelationPlots)
    Add(ibooker, planes, correlationPlotsSelector, limit);
}

//----------------------------------------------------------------------------------------------------

void TotemRPDQMSource::StationPlots::Add(DQMStore::IBooker &ibooker, std::set<unsigned int> planes, CorrelationPlotsSelector *correlationPlotsSelector, int limit)
{
  int correlationPlotsCounter = 0;

  bool limited;
  if (limit == -1)
    limited = false;
  else
    limited = true;

  for (std::set<unsigned int>::iterator i = planes.begin(); i != planes.end(); i++)
  {
    for (std::set<unsigned int>::iterator j = i; j != planes.end(); j++)
    {
      if (hist[*i][*j] == 0 && hist[*j][*i] == 0 && *i != *j && i != j)
      {
        unsigned int plane1 = std::min(*i, *j);
        unsigned int plane2 = std::max(*i, *j);
        if (correlationPlotsSelector->IfTwoCorrelate(plane1, plane2) && (!limited || correlationPlotsCounter < limit))
        {
          char buf1[200];
          char buf2[200];
          unsigned int RPPlaneId1 = plane1 + 100 * id;
          unsigned int RPPlaneId2 = plane2 + 100 * id;
          std::string RPPlane1 = "";
          std::string RPPlane2 = "";
          RPPlane1 += TotemRPDetId::planeName(RPPlaneId1, TotemRPDetId::nPath);
          RPPlane2 += TotemRPDetId::planeName(RPPlaneId2, TotemRPDetId::nPath);
          size_t pos1 = RPPlane1.rfind('/');
          size_t pos2 = RPPlane2.rfind('/');
          
          if (pos1 == std::string::npos)
          {
            pos1 = 0;
          } else {
            RPPlane1 = RPPlane1.substr(0, pos1);
          }
          
          if (pos2 == std::string::npos) {
            pos2 = 0;
          } else {
            RPPlane2 = RPPlane2.substr(0, pos2);
          }

          pos1 = RPPlane1.rfind('/');
          pos2 = RPPlane2.rfind('/');

          if (pos1 == std::string::npos) {
            pos1 = 0;
          } else {
            RPPlane1 = RPPlane1.substr(pos1 + 1);
          }

          if (pos2 == std::string::npos)
          {
            pos2 = 0;
          } else {
            RPPlane2 = RPPlane2.substr(pos2 + 1);
          }
          RPPlane1 += '_';
          RPPlane2 += '_';

          sprintf(buf1, "%s%u", RPPlane1.c_str(), plane1 % 10);
          sprintf(buf2, "%s%u", RPPlane2.c_str(), plane2 % 10);

          hist[plane1][plane2] = ibooker.book2D(string("correlation profile ") +  buf1 + " vs " + buf2,
            string("correlation profile ") + buf1 + " vs " + buf2 + ";strip in " + buf1 + ";strip in " + buf2, 512, -0.5, 511.5, 512, -0.5, 511.5);

          correlationPlotsCounter++;
        }
      }
    }
  }

  if (limited && correlationPlotsCounter >= limit)
    printf("WARNING in TotemRPDQMSource > Number of correlation plots for station %i has been limited to %i.\n", id, limit);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

TotemRPDQMSource::PotPlots::PotPlots(DQMStore::IBooker &ibooker, unsigned int id)
{
  ibooker.setCurrentFolder(string("Totem/") + TotemRPDetId::rpName(id, TotemRPDetId::nPath));

  activity = ibooker.book1D("active planes", "active planes;number of active planes", 11, -0.5, 10.5);
  activity_u = ibooker.book1D("active planes U", "active planes U;number of active U planes", 11, -0.5, 10.5);
  activity_v = ibooker.book1D("active planes V", "active planes V;number of active V planes", 11, -0.5, 10.5);

  hit_plane_hist = ibooker.book2D("activity in planes (2D)", "activity in planes;plane number;strip number", 10, -0.5, 9.5, 32, -0.5, 511.5);

  patterns_u = ibooker.book1D("recognized patterns U", "recognized patterns U;number of recognized U patterns", 11, -0.5, 10.5); 
  patterns_v = ibooker.book1D("recognized patterns V", "recognized patterns V;number of recognized V patterns", 11, -0.5, 10.5); 

  h_planes_fit_u = ibooker.book1D("planes contributing to fit U", "planes contributing to fit U;number of planes contributing to U fit", 6, -0.5, 5.5);
  h_planes_fit_v = ibooker.book1D("planes contributing to fit V", "planes contributing to fit V;number of planes contributing to V fit", 6, -0.5, 5.5);

  event_category = ibooker.book1D("event category", "event category", 5, -0.5, 4.5);
  TH1F *event_category_h = event_category->getTH1F();
  event_category_h->GetXaxis()->SetBinLabel(1, "empty");
  event_category_h->GetXaxis()->SetBinLabel(2, "insufficient");
  event_category_h->GetXaxis()->SetBinLabel(3, "single-track");
  event_category_h->GetXaxis()->SetBinLabel(4, "multi-track");
  event_category_h->GetXaxis()->SetBinLabel(5, "shower");

  trackHitsCumulativeHist = ibooker.book2D("track XY profile", "track XY profile;x   (mm);y   (mm)", 100, -18., +18., 100, -18., +18.);

  track_u_profile = ibooker.book1D("track profile U", "track profile U; U   (mm)", 512, -256*66E-3, +256*66E-3);
  track_v_profile = ibooker.book1D("track profile V", "track profile V; V   (mm)", 512, -256*66E-3, +256*66E-3);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

TotemRPDQMSource::PlanePlots::PlanePlots(DQMStore::IBooker &ibooker, unsigned int id)
{
  ibooker.setCurrentFolder(string("Totem/") + TotemRPDetId::planeName(id, TotemRPDetId::nPath));

  digi_profile_cumulative = ibooker.book1D("digi profile", "digi profile;strip number", 512, -0.5, 511.5);
  cluster_profile_cumulative = ibooker.book1D("cluster profile", "cluster profile;cluster center", 1024, -0.25, 511.75);
  hit_multiplicity = ibooker.book1D("hit multiplicity", "hit multiplicity;hits/detector/event", 6, -0.5, 5.5);
  cluster_size = ibooker.book1D("cluster size", "cluster size;hits per cluster", 5, 0.5, 5.5);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

TotemRPDQMSource::TotemRPDQMSource(const edm::ParameterSet& ps) : 
  buildCorrelationPlots(ps.getUntrackedParameter<bool>("buildCorrelationPlots", false)),
  correlationPlotsLimit(ps.getUntrackedParameter<unsigned int>("correlationPlotsLimit", 50)),
  correlationPlotsSelector(ps.getUntrackedParameter<std::string>("correlationPlotsFilter", ""))
{
  tokenStripDigi = consumes< DetSetVector<TotemRPDigi> >(ps.getParameter<edm::InputTag>("tagStripDigi"));
  tokenDigiCluster = consumes< edm::DetSetVector<TotemRPCluster> >(ps.getParameter<edm::InputTag>("tagDigiCluster"));
  tokenRecoHit = consumes< edm::DetSetVector<TotemRPRecHit> >(ps.getParameter<edm::InputTag>("tagRecoHit"));
  tokenPatternColl = consumes< DetSetVector<TotemRPUVPattern> >(ps.getParameter<edm::InputTag>("tagPatternColl"));
  tokenTrackColl = consumes< DetSetVector<TotemRPLocalTrack> >(ps.getParameter<edm::InputTag>("tagTrackColl"));
  //tokenMultiTrackColl = consumes< RPMulFittedTrackCollection >(ps.getParameter<edm::InputTag>("tagMultiTrackColl"));
}

//----------------------------------------------------------------------------------------------------

TotemRPDQMSource::~TotemRPDQMSource()
{
}

//----------------------------------------------------------------------------------------------------

void TotemRPDQMSource::dqmBeginRun(edm::Run const &, edm::EventSetup const &)
{
}

//----------------------------------------------------------------------------------------------------

void TotemRPDQMSource::bookHistograms(DQMStore::IBooker &ibooker, edm::Run const &, edm::EventSetup const &)
{
  ibooker.cd();
  ibooker.setCurrentFolder("TotemRP");

  // initialize diagonals
  diagonalPlots[1] = DiagonalPlots(ibooker, 1);  // 45 bot - 56 top
  diagonalPlots[2] = DiagonalPlots(ibooker, 2);  // 45 top - 45 bot

  // initialize anti-diagonals
  diagonalPlots[0] = DiagonalPlots(ibooker, 0);  // 45 bot - 56 bot
  diagonalPlots[3] = DiagonalPlots(ibooker, 3);  // 45 top - 56 top

  // loop over arms
  for (unsigned int arm = 0; arm < 2; arm++)
  {
    armPlots[arm] = ArmPlots(ibooker, arm);

    // loop over stations
    for (unsigned int st = 0; st < 3; st += 2)
    {
      unsigned int stId = 10*arm + st;

      set<unsigned int> stationPlanes;

      // loop over RPs
      for (unsigned int rp = 0; rp < 6; ++rp)
      {
        unsigned int rpId = 10*stId + rp;

        potPlots[rpId] = PotPlots(ibooker, rpId);

        // loop over planes
        for (unsigned int pl = 0; pl < 10; ++pl)
        {
          unsigned int plId = 10*rpId + pl;
          planePlots[plId] = PlanePlots(ibooker, plId);

		  if (correlationPlotsSelector.IfCorrelate(plId))
		    stationPlanes.insert(plId % 100);
        }
      }

      stationPlots[stId] = StationPlots(ibooker, stId, stationPlanes,
        buildCorrelationPlots, &correlationPlotsSelector, correlationPlotsLimit);
    }
  }
}

//----------------------------------------------------------------------------------------------------

void TotemRPDQMSource::beginLuminosityBlock(edm::LuminosityBlock const& lumiSeg, 
                                            edm::EventSetup const& context) 
{
}

//----------------------------------------------------------------------------------------------------

void TotemRPDQMSource::analyze(edm::Event const& event, edm::EventSetup const& eventSetup)
{
  // get event setup data
  ESHandle<TotemRPGeometry> geometry;
  eventSetup.get<VeryForwardRealGeometryRecord>().get(geometry);

  // get event data
  Handle< DetSetVector<TotemRPDigi> > digi;
  event.getByToken(tokenStripDigi, digi);

  Handle< DetSetVector<TotemRPCluster> > digCluster;
  event.getByToken(tokenDigiCluster, digCluster);

  Handle< DetSetVector<TotemRPRecHit> > hits;
  event.getByToken(tokenRecoHit, hits);

  Handle<DetSetVector<TotemRPUVPattern>> patterns;
  event.getByToken(tokenPatternColl, patterns);

  Handle< DetSetVector<TotemRPLocalTrack> > tracks;
  event.getByToken(tokenTrackColl, tracks);

  //Handle< RPMulFittedTrackCollection > multiTracks;
  //event.getByToken(tokenMultiTrackColl, multiTracks);

  // check validity
  bool valid = true;
  valid &= digi.isValid();
  valid &= digCluster.isValid();
  valid &= hits.isValid();
  valid &= tracks.isValid();
  valid &= patterns.isValid();
  //valid &= multiTracks.isValid();

  if (!valid)
  {
    printf("ERROR in TotemDQMModuleRP::analyze > some of the required inputs are not valid. Skipping this event.\n");
    printf("\tdigi.isValid = %i\n", digi.isValid());
    printf("\tdigCluster.isValid = %i\n", digCluster.isValid());
    printf("\thits.isValid = %i\n", hits.isValid());
    printf("\ttracks.isValid = %i\n", tracks.isValid());
    printf("\tpatterns.isValid = %i\n", patterns.isValid());
    //printf("\tmultiTracks.isValid = %i\n", multiTracks.isValid());

    return;
  }
  
  //------------------------------
  // Plane Plots

  // digi profile cumulative
  for (DetSetVector<TotemRPDigi>::const_iterator it = digi->begin(); it != digi->end(); ++it)
  {
    unsigned int DetId = TotemRPDetId::rawToDecId(it->detId());
    for (DetSet<TotemRPDigi>::const_iterator dit = it->begin(); dit != it->end(); ++dit)
    {
      planePlots[DetId].digi_profile_cumulative->Fill(dit->getStripNumber());
    }
  }

  // cluster profile cumulative
  for (DetSetVector<TotemRPCluster>::const_iterator it = digCluster->begin(); it != digCluster->end(); it++)
  {
    unsigned int DetId = TotemRPDetId::rawToDecId(it->detId());
    for (DetSet<TotemRPCluster>::const_iterator dit = it->begin(); dit != it->end(); ++dit)
    {
      planePlots[DetId].cluster_profile_cumulative->Fill(dit->CentreStripPos());
    }
  }

  // hit multiplicity
  for (DetSetVector<TotemRPCluster>::const_iterator it = digCluster->begin(); it != digCluster->end(); it++)
  {
    unsigned int DetId = TotemRPDetId::rawToDecId(it->detId());
    planePlots[DetId].hit_multiplicity->Fill(it->size());
  }

  // cluster size
  for (DetSetVector<TotemRPCluster>::const_iterator it = digCluster->begin(); it != digCluster->end(); it++)
  {
    unsigned int DetId = TotemRPDetId::rawToDecId(it->detId());
    for (DetSet<TotemRPCluster>::const_iterator dit = it->begin(); dit != it->end(); ++dit)
      planePlots[DetId].cluster_size->Fill(dit->GetNumberOfStrips());
  }

  //------------------------------
  // Roman Pots Plots

  // plane activity histogram
  map<unsigned int, set<unsigned int> > planes;
  map<unsigned int, set<unsigned int> > planes_u;
  map<unsigned int, set<unsigned int> > planes_v;
  for (DetSetVector<TotemRPRecHit>::const_iterator it = hits->begin(); it != hits->end(); ++it)
  {
    unsigned int DetId = TotemRPDetId::rawToDecId(it->detId());
    unsigned int RPId = TotemRPDetId::rpOfDet(DetId);
    unsigned int planeNum = DetId % 10;
    planes[RPId].insert(planeNum);
    if (TotemRPDetId::isStripsCoordinateUDirection(DetId))
      planes_u[RPId].insert(planeNum);
    else
      planes_v[RPId].insert(planeNum);
  }
  
  for (std::map<unsigned int, PotPlots>::iterator it = potPlots.begin(); it != potPlots.end(); it++)
  {
    it->second.activity->Fill(planes[it->first].size());
    it->second.activity_u->Fill(planes_u[it->first].size());
    it->second.activity_v->Fill(planes_v[it->first].size());
  }
  
  for (DetSetVector<TotemRPCluster>::const_iterator it = digCluster->begin(); it != digCluster->end(); it++)
  {
    unsigned int DetId = TotemRPDetId::rawToDecId(it->detId());
    unsigned int RPId = TotemRPDetId::rpOfDet(DetId);
    unsigned int planeNum = DetId % 10;
    PotPlots &pp = potPlots[RPId];
    for (DetSet<TotemRPCluster>::const_iterator dit = it->begin(); dit != it->end(); ++dit)
      pp.hit_plane_hist->Fill(planeNum, dit->CentreStripPos());   
  }

  // recognized pattern histograms and event-category histogram
  for (auto &ds : *patterns)
  {
    unsigned int rpId = ds.detId();
    PotPlots &pp = potPlots[rpId];

    // count U and V patterns
    unsigned int u = 0, v = 0;
    for (auto &p : ds)
    {
      if (! p.getFittable())
        continue;

      if (p.getProjection() == TotemRPUVPattern::projU)
        u++;

      if (p.getProjection() == TotemRPUVPattern::projV)
        v++;
    }

    pp.patterns_u->Fill(u);
    pp.patterns_v->Fill(v);
  
    // determine category
    unsigned int category = 100;

    if (u == 0 && v == 0) category = 0;                   // empty
    if (u > 0 && v > 0 && u <= 3 && v <= 3) category = 3; // multi-track
    if (u+v == 1) category = 1;                           // insuff
    if (u == 1 && v == 1) category = 2;                   // 1-track
    if (u > 3 || v > 3) category = 4;                     // shower

    pp.event_category->Fill(category);
  }

  // cumulative RP fit plots
  for (auto &ds : *tracks)
  {
    unsigned int RPId = ds.detId();
    PotPlots &pp = potPlots[RPId];

    for (auto &ft : ds)
    {
      if (!ft.IsValid())
        continue;
     
      // number of planes contributing to (valid) fits
      unsigned int n_pl_in_fit_u = 0, n_pl_in_fit_v = 0;
      for (int hi = 0; hi < ft.GetHitEntries(); hi++)
      {
        unsigned int rawId = ft.GetHit(hi).DetId();  
        unsigned int decId = TotemRPDetId::rawToDecId(rawId);
        if (TotemRPDetId::isStripsCoordinateUDirection(decId))
          n_pl_in_fit_u++;
        else
          n_pl_in_fit_v++;
      }
      pp.h_planes_fit_u->Fill(n_pl_in_fit_u);
      pp.h_planes_fit_v->Fill(n_pl_in_fit_v);
  
      // mean position of U and V planes
      double rp_x = ( geometry->GetDetector(TotemRPDetId::decToRawId(RPId*10 + 0))->translation().x() +
                      geometry->GetDetector(TotemRPDetId::decToRawId(RPId*10 + 1))->translation().x() ) / 2.;
      double rp_y = ( geometry->GetDetector(TotemRPDetId::decToRawId(RPId*10 + 0))->translation().y() +
                      geometry->GetDetector(TotemRPDetId::decToRawId(RPId*10 + 1))->translation().y() ) / 2.;
  
      // mean read-out direction of U and V planes
      CLHEP::Hep3Vector rod_U = geometry->LocalToGlobalDirection(TotemRPDetId::decToRawId(RPId*10 + 1), CLHEP::Hep3Vector(0., 1., 0.));
      CLHEP::Hep3Vector rod_V = geometry->LocalToGlobalDirection(TotemRPDetId::decToRawId(RPId*10 + 0), CLHEP::Hep3Vector(0., 1., 0.));
  
      double x = ft.X0() - rp_x;
      double y = ft.Y0() - rp_y;
  
      pp.trackHitsCumulativeHist->Fill(x, y);
  
      double U = x * rod_U.x() + y * rod_U.y();
      double V = x * rod_V.x() + y * rod_V.y();
  
      pp.track_u_profile->Fill(U);
      pp.track_v_profile->Fill(V);
    }
  }

  //------------------------------
  // Station Plots

  // Correlation profile
  if (buildCorrelationPlots)
  {
    for (DetSetVector<TotemRPDigi>::const_iterator i = digi->begin(); i != digi->end(); i++)
    {
      for (DetSetVector<TotemRPDigi>::const_iterator j = i; j != digi->end(); j++)
      {
        if (i == j)
          continue;

        unsigned int DetId1 = TotemRPDetId::rawToDecId(i->detId());
        unsigned int DetId2 = TotemRPDetId::rawToDecId(j->detId());
        unsigned int StationId1 = DetId1 / 100;
        unsigned int StationId2 = DetId2 / 100;

        if (StationId1 != StationId2)
          continue;

        unsigned int RPPlaneId1 = DetId1 % 100;
        unsigned int RPPlaneId2 = DetId2 % 100;
        if (stationPlots[StationId1].hist[RPPlaneId1][RPPlaneId2])
        {
          for (DetSet<TotemRPDigi>::const_iterator di = i->begin(); di != i->end(); di++)
          {
            for (DetSet<TotemRPDigi>::const_iterator dj = j->begin(); dj != j->end(); dj++)
              stationPlots[StationId1].hist[RPPlaneId1][RPPlaneId2]->Fill(di->getStripNumber(), dj->getStripNumber());
          }
        }

      }
    }
  }
  
  //------------------------------
  // Arm Plots
  {
    map<unsigned int, unsigned int> mTop, mHor, mBot;

    for (auto p : armPlots)
    {
      mTop[p.first] = 0;
      mHor[p.first] = 0;
      mBot[p.first] = 0;
    }

    for (auto &ds : *tracks)
    {
      unsigned int rpId = ds.detId();
      unsigned int armNum = rpId / 100;
      unsigned int rpNum = rpId % 10;

      for (auto &tr : ds)
      {
        if (! tr.IsValid())
          continue;
  
        if (rpNum == 0 || rpNum == 4)
          mTop[armNum]++;
        if (rpNum == 2 || rpNum == 3)
          mHor[armNum]++;
        if (rpNum == 1 || rpNum == 5)
          mBot[armNum]++;
      }
    }

    for (auto &p : armPlots)
    {
      p.second.h_numRPWithTrack_top->Fill(mTop[p.first]);
      p.second.h_numRPWithTrack_hor->Fill(mHor[p.first]);
      p.second.h_numRPWithTrack_bot->Fill(mBot[p.first]);
    }

    // track RP correlation
    for (auto &ds1 : *tracks)
    {
      for (auto &tr1 : ds1)
      {
        if (! tr1.IsValid())
          continue;
  
        unsigned int rpId1 = ds1.detId();
        unsigned int arm1 = rpId1 / 100;
        unsigned int stNum1 = (rpId1 / 10) % 10;
        unsigned int rpNum1 = rpId1 % 10;
        unsigned int idx1 = stNum1/2 * 7 + rpNum1;
        bool hor1 = (rpNum1 == 2 || rpNum1 == 3);
  
        ArmPlots &ap = armPlots[arm1];
  
        for (auto &ds2 : *tracks)
        {
          for (auto &tr2 : ds2)
          {
            if (! tr2.IsValid())
              continue;
          
            unsigned int rpId2 = ds2.detId();
            unsigned int arm2 = rpId2 / 100;
            unsigned int stNum2 = (rpId2 / 10) % 10;
            unsigned int rpNum2 = rpId2 % 10;
            unsigned int idx2 = stNum2/2 * 7 + rpNum2;
            bool hor2 = (rpNum2 == 2 || rpNum2 == 3);
    
            if (arm1 != arm2)
              continue;
    
            ap.h_trackCorr->Fill(idx1, idx2); 
            
            if (hor1 != hor2)
              ap.h_trackCorr_overlap->Fill(idx1, idx2); 
          }
        }
      }
    }
  }
  
  //------------------------------
  // RP-system plots
  // TODO: this code needs
  //    * generalization for more than two RPs per arm
  //    * updating for tracks as DetSetVector
  /*
  for (auto &dp : diagonalPlots)
  {
    unsigned int id = dp.first;
    bool top45 = id & 2;
    bool top56 = id & 1;

    unsigned int id_45_n = (top45) ? 20 : 21;
    unsigned int id_45_f = (top45) ? 24 : 25;
    unsigned int id_56_n = (top56) ? 120 : 121;
    unsigned int id_56_f = (top56) ? 124 : 125;
  
    bool h_45_n = (tracks->find(id_45_n) != tracks->end() && tracks->find(id_45_n)->second.IsValid());
    bool h_45_f = (tracks->find(id_45_f) != tracks->end() && tracks->find(id_45_f)->second.IsValid());
    bool h_56_n = (tracks->find(id_56_n) != tracks->end() && tracks->find(id_56_n)->second.IsValid());
    bool h_56_f = (tracks->find(id_56_f) != tracks->end() && tracks->find(id_56_f)->second.IsValid());
  
    if (! (h_45_n && h_45_f && h_56_n && h_56_f) )
      continue;

    double x_45_n = tracks->find(id_45_n)->second.X0(), y_45_n = tracks->find(id_45_n)->second.Y0();
    double x_45_f = tracks->find(id_45_f)->second.X0(), y_45_f = tracks->find(id_45_f)->second.Y0();
    double x_56_n = tracks->find(id_56_n)->second.X0(), y_56_n = tracks->find(id_56_n)->second.Y0();
    double x_56_f = tracks->find(id_56_f)->second.X0(), y_56_f = tracks->find(id_56_f)->second.Y0();

    double dx_45 = x_45_f - x_45_n;
    double dy_45 = y_45_f - y_45_n;
    double dx_56 = x_56_f - x_56_n;
    double dy_56 = y_56_f - y_56_n;

    DiagonalPlots &pl = dp.second;

    pl.h_lrc_x_d->Fill(dx_45, dx_56);  
    pl.h_lrc_y_d->Fill(dy_45, dy_56);  
    
    pl.h_lrc_x_n->Fill(x_45_n, x_56_n);  
    pl.h_lrc_y_n->Fill(y_45_n, y_56_n);  
    
    pl.h_lrc_x_f->Fill(x_45_f, x_56_f);  
    pl.h_lrc_y_f->Fill(y_45_f, y_56_f);
  }
  */
}

//----------------------------------------------------------------------------------------------------

void TotemRPDQMSource::endLuminosityBlock(edm::LuminosityBlock const& lumiSeg, edm::EventSetup const& eSetup) 
{
}

//----------------------------------------------------------------------------------------------------

void TotemRPDQMSource::endRun(edm::Run const& run, edm::EventSetup const& eSetup)
{
}
