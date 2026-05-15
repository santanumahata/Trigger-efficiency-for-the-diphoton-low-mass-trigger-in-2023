// =============================================================================
//  DiphotonTnPAnalyzer_OldString.cc
//
//  For: Run2023C-v1, Run2023C-v2
//  HLT filter bits same as 2022 ("old strings")
//
//  Seeded  : hltEG30Iso60CaloId15b35eR9Id50b90eHE12b10eR9Id50b80eEcalIsoFilter
//  Unseeded: hltEG18TrackIso60Iso60CaloId15b35eR9Id50b90eHE12b10eR9Id50b80eTrackIsoUnseededFilter
//
//  Author: Rajdeep M Chatterjee  Date: 08/04/2026
//  Contributor: Santanu Mahata (added isolation branches: probe_trkSumPtHollowConeDR03,
//               probe_chargedHadIso, probe_pfRelIso03_chg_quad, tag_seed_hlt_*)
// =============================================================================

#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <limits>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "TTree.h"

// =============================================================================
class DiphotonTnPAnalyzer_OldString
    : public edm::one::EDAnalyzer<edm::one::SharedResources> {

public:
    explicit DiphotonTnPAnalyzer_OldString(const edm::ParameterSet&);
    ~DiphotonTnPAnalyzer_OldString() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
    void beginJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    void endJob() override {}

    void clearBranches();
    bool passesProbePreselection(const pat::Photon& pho) const;
    bool passesTagSelection(const pat::Photon&                               pho,
                            const std::vector<pat::TriggerObjectStandAlone>& objs) const;

    struct MatchResult {
        const pat::TriggerObjectStandAlone* obj;
        double                              dR;
    };
    MatchResult getClosestObject(
        const reco::Candidate::LorentzVector&            p4,
        const std::vector<pat::TriggerObjectStandAlone>& objs,
        const std::string&                               filterLabel) const;

    bool pathFired(const edm::TriggerResults& trigResults,
                   const edm::TriggerNames&   trigNames,
                   const std::string&         partialName) const;

    const edm::EDGetTokenT<edm::View<pat::Photon>>                 photonToken_;
    const edm::EDGetTokenT<edm::TriggerResults>                    trigResultsToken_;
    const edm::EDGetTokenT<pat::TriggerObjectStandAloneCollection>  trigObjToken_;
    const edm::EDGetTokenT<std::vector<reco::Vertex>>              vtxToken_;
    const edm::EDGetTokenT<double>                                 rhoToken_;

    const std::string diphotonPath_;
    const std::string seededFilter_;
    const std::string unseededFilter_;
    const std::string tagTriggerPath_;
    const std::string tagTriggerFilter_;
    const double      tagPtMin_;
    const double      tagEtaMax_;
    const double      tagMvaCut_;
    const double      probePtMin_;
    const double      probeEtaMax_;
    const double      massLow_;
    const double      massHigh_;
    const double      dRmatch_;
    const bool        isMC_;

    TTree* tree_;

    // Event
    ULong64_t b_run, b_lumi, b_event;
    int       b_nVtx;
    float     b_rho, b_weight;

    // Tag reco
    float b_tag_pt, b_tag_eta, b_tag_phi;
    float b_tag_r9, b_tag_full5x5_r9;
    float b_tag_sieie, b_tag_hoe, b_tag_mva;
    int   b_tag_isEB, b_tag_hasPixelSeed;

    // Tag HLT object (Ele30 WPTight match)
    float b_tag_hlt_pt, b_tag_hlt_eta, b_tag_hlt_phi, b_tag_hlt_e;
    float b_tag_hlt_dR;

    // Tag seeded-leg HLT object stored for ALL tags
    float b_tag_seed_hlt_pt,  b_tag_seed_hlt_eta;
    float b_tag_seed_hlt_phi, b_tag_seed_hlt_e;
    float b_tag_seed_hlt_dR;

    // Probe reco kinematics
    float b_probe_pt, b_probe_eta, b_probe_phi;
    int   b_probe_isEB, b_probe_hasPixelSeed;

    // Probe shower shapes
    float b_probe_r9, b_probe_full5x5_r9;
    float b_probe_sieie, b_probe_sieip, b_probe_s4;
    float b_probe_hoe, b_probe_hoe_bc;

    // Probe isolation
    // probe_ecalIso              : ecalPFClusterIso()
    // probe_hcalIso              : hcalPFClusterIso()
    // probe_trkIso               : trackIso() = trkSumPtSolidConeDR04 (kept for compatibility)
    // probe_trkSumPtHollowConeDR03: trkSumPtHollowConeDR03() = TrackerIso03 in preselection table
    // probe_phoIso               : photonIso() = Rho_PfPhoIso03 raw (pfPhoIso03 in NanoAOD)
    // probe_chargedHadIso        : chargedHadronIso() = pfChargedIso in NanoAOD
    // probe_pfRelIso03_chg_quad  : userFloat('PFIsoChgQuadratic')/pt = pfRelIso03_chg_quadratic
    float b_probe_ecalIso, b_probe_hcalIso;
    float b_probe_trkIso;                    // solid cone DR04 (legacy)
    float b_probe_trkSumPtHollowConeDR03;    // hollow cone DR03 -- preselection cut variable
    float b_probe_phoIso;                    // PF photon iso, raw (Rho_PfPhoIso03 before rho corr)
    float b_probe_chargedHadIso;             // PF charged hadron iso, raw
    float b_probe_pfRelIso03_chg_quad;       // rho-corrected relative charged iso (Winter22V1)

    // Probe MVA
    float b_probe_mva;

    // Pair
    float b_mass;

    // Trigger flags
    int b_probe_passSeed;
    int b_probe_passUnseed;
    int b_probe_passFullPath;

    // Seeded-leg HLT object stored for ALL probes
    float b_probe_seed_hlt_pt,  b_probe_seed_hlt_eta;
    float b_probe_seed_hlt_phi, b_probe_seed_hlt_e;
    float b_probe_seed_hlt_dR;

    // Unseeded-leg HLT object stored for ALL probes
    float b_probe_unseed_hlt_pt,  b_probe_unseed_hlt_eta;
    float b_probe_unseed_hlt_phi, b_probe_unseed_hlt_e;
    float b_probe_unseed_hlt_dR;

    // MC-only
    int   b_probe_isGenMatched, b_tag_isGenMatched;
    float b_probe_genPt, b_probe_genEta;
};

// =============================================================================
//  Constructor
// =============================================================================
DiphotonTnPAnalyzer_OldString::DiphotonTnPAnalyzer_OldString(const edm::ParameterSet& iConfig)
    : photonToken_     (consumes<edm::View<pat::Photon>>(
                            iConfig.getParameter<edm::InputTag>("photons")))
    , trigResultsToken_(consumes<edm::TriggerResults>(
                            iConfig.getParameter<edm::InputTag>("trigResults")))
    , trigObjToken_    (consumes<pat::TriggerObjectStandAloneCollection>(
                            iConfig.getParameter<edm::InputTag>("trigObjects")))
    , vtxToken_        (consumes<std::vector<reco::Vertex>>(
                            iConfig.getParameter<edm::InputTag>("vertices")))
    , rhoToken_        (consumes<double>(
                            iConfig.getParameter<edm::InputTag>("rho")))
    , diphotonPath_    (iConfig.getParameter<std::string>("diphotonPath"))
    , seededFilter_    (iConfig.getParameter<std::string>("seededFilter"))
    , unseededFilter_  (iConfig.getParameter<std::string>("unseededFilter"))
    , tagTriggerPath_  (iConfig.getParameter<std::string>("tagTriggerPath"))
    , tagTriggerFilter_(iConfig.getParameter<std::string>("tagTriggerFilter"))
    , tagPtMin_        (iConfig.getParameter<double>("tagPtMin"))
    , tagEtaMax_       (iConfig.getParameter<double>("tagEtaMax"))
    , tagMvaCut_       (iConfig.getParameter<double>("tagMvaCut"))
    , probePtMin_      (iConfig.getParameter<double>("probePtMin"))
    , probeEtaMax_     (iConfig.getParameter<double>("probeEtaMax"))
    , massLow_         (iConfig.getParameter<double>("massLow"))
    , massHigh_        (iConfig.getParameter<double>("massHigh"))
    , dRmatch_         (iConfig.getParameter<double>("dRmatch"))
    , isMC_            (iConfig.getParameter<bool>("isMC"))
    , tree_            (nullptr)
{
    usesResource("TFileService");
}

// =============================================================================
void DiphotonTnPAnalyzer_OldString::beginJob()
{
    edm::Service<TFileService> fs;
    tree_ = fs->make<TTree>("tnpPhoHLT",
        "Tag-and-Probe tree for HLT_Diphoton30_18 efficiency");

    // Event
    tree_->Branch("run",    &b_run,    "run/l");
    tree_->Branch("lumi",   &b_lumi,   "lumi/l");
    tree_->Branch("event",  &b_event,  "event/l");
    tree_->Branch("nVtx",   &b_nVtx,   "nVtx/I");
    tree_->Branch("rho",    &b_rho,    "rho/F");
    tree_->Branch("weight", &b_weight, "weight/F");

    // Tag reco
    tree_->Branch("tag_pt",           &b_tag_pt,           "tag_pt/F");
    tree_->Branch("tag_eta",          &b_tag_eta,          "tag_eta/F");
    tree_->Branch("tag_phi",          &b_tag_phi,          "tag_phi/F");
    tree_->Branch("tag_r9",           &b_tag_r9,           "tag_r9/F");
    tree_->Branch("tag_full5x5_r9",   &b_tag_full5x5_r9,   "tag_full5x5_r9/F");
    tree_->Branch("tag_sieie",        &b_tag_sieie,        "tag_sieie/F");
    tree_->Branch("tag_hoe",          &b_tag_hoe,          "tag_hoe/F");
    tree_->Branch("tag_mva",          &b_tag_mva,          "tag_mva/F");
    tree_->Branch("tag_isEB",         &b_tag_isEB,         "tag_isEB/I");
    tree_->Branch("tag_hasPixelSeed", &b_tag_hasPixelSeed, "tag_hasPixelSeed/I");

    // Tag HLT object
    tree_->Branch("tag_hlt_pt",  &b_tag_hlt_pt,  "tag_hlt_pt/F");
    tree_->Branch("tag_hlt_eta", &b_tag_hlt_eta, "tag_hlt_eta/F");
    tree_->Branch("tag_hlt_phi", &b_tag_hlt_phi, "tag_hlt_phi/F");
    tree_->Branch("tag_hlt_e",   &b_tag_hlt_e,   "tag_hlt_e/F");
    tree_->Branch("tag_hlt_dR",  &b_tag_hlt_dR,  "tag_hlt_dR/F");

    // Tag seeded-leg HLT object stored for ALL tags
    tree_->Branch("tag_seed_hlt_pt",  &b_tag_seed_hlt_pt,  "tag_seed_hlt_pt/F");
    tree_->Branch("tag_seed_hlt_eta", &b_tag_seed_hlt_eta, "tag_seed_hlt_eta/F");
    tree_->Branch("tag_seed_hlt_phi", &b_tag_seed_hlt_phi, "tag_seed_hlt_phi/F");
    tree_->Branch("tag_seed_hlt_e",   &b_tag_seed_hlt_e,   "tag_seed_hlt_e/F");
    tree_->Branch("tag_seed_hlt_dR",  &b_tag_seed_hlt_dR,  "tag_seed_hlt_dR/F");

    // Probe reco kinematics
    tree_->Branch("probe_pt",           &b_probe_pt,           "probe_pt/F");
    tree_->Branch("probe_eta",          &b_probe_eta,          "probe_eta/F");
    tree_->Branch("probe_phi",          &b_probe_phi,          "probe_phi/F");
    tree_->Branch("probe_isEB",         &b_probe_isEB,         "probe_isEB/I");
    tree_->Branch("probe_hasPixelSeed", &b_probe_hasPixelSeed, "probe_hasPixelSeed/I");

    // Probe shower shapes
    tree_->Branch("probe_r9",         &b_probe_r9,         "probe_r9/F");
    tree_->Branch("probe_full5x5_r9", &b_probe_full5x5_r9, "probe_full5x5_r9/F");
    tree_->Branch("probe_sieie",      &b_probe_sieie,      "probe_sieie/F");
    tree_->Branch("probe_sieip",      &b_probe_sieip,      "probe_sieip/F");
    tree_->Branch("probe_s4",         &b_probe_s4,         "probe_s4/F");
    tree_->Branch("probe_hoe",        &b_probe_hoe,        "probe_hoe/F");
    tree_->Branch("probe_hoe_bc",     &b_probe_hoe_bc,     "probe_hoe_bc/F");

    // Probe isolation
    tree_->Branch("probe_ecalIso",                 &b_probe_ecalIso,                 "probe_ecalIso/F");
    tree_->Branch("probe_hcalIso",                 &b_probe_hcalIso,                 "probe_hcalIso/F");
    tree_->Branch("probe_trkIso",                  &b_probe_trkIso,                  "probe_trkIso/F");
    tree_->Branch("probe_trkSumPtHollowConeDR03",  &b_probe_trkSumPtHollowConeDR03,  "probe_trkSumPtHollowConeDR03/F");
    tree_->Branch("probe_phoIso",                  &b_probe_phoIso,                  "probe_phoIso/F");
    tree_->Branch("probe_chargedHadIso",           &b_probe_chargedHadIso,           "probe_chargedHadIso/F");
    tree_->Branch("probe_pfRelIso03_chg_quad",     &b_probe_pfRelIso03_chg_quad,     "probe_pfRelIso03_chg_quad/F");

    // Probe MVA
    tree_->Branch("probe_mva", &b_probe_mva, "probe_mva/F");

    // Pair
    tree_->Branch("mass", &b_mass, "mass/F");

    // Trigger flags
    tree_->Branch("probe_passSeed",     &b_probe_passSeed,     "probe_passSeed/I");
    tree_->Branch("probe_passUnseed",   &b_probe_passUnseed,   "probe_passUnseed/I");
    tree_->Branch("probe_passFullPath", &b_probe_passFullPath, "probe_passFullPath/I");

    // Seeded-leg HLT object stored for ALL probes
    tree_->Branch("probe_seed_hlt_pt",  &b_probe_seed_hlt_pt,  "probe_seed_hlt_pt/F");
    tree_->Branch("probe_seed_hlt_eta", &b_probe_seed_hlt_eta, "probe_seed_hlt_eta/F");
    tree_->Branch("probe_seed_hlt_phi", &b_probe_seed_hlt_phi, "probe_seed_hlt_phi/F");
    tree_->Branch("probe_seed_hlt_e",   &b_probe_seed_hlt_e,   "probe_seed_hlt_e/F");
    tree_->Branch("probe_seed_hlt_dR",  &b_probe_seed_hlt_dR,  "probe_seed_hlt_dR/F");

    // Unseeded-leg HLT object stored for ALL probes
    tree_->Branch("probe_unseed_hlt_pt",  &b_probe_unseed_hlt_pt,  "probe_unseed_hlt_pt/F");
    tree_->Branch("probe_unseed_hlt_eta", &b_probe_unseed_hlt_eta, "probe_unseed_hlt_eta/F");
    tree_->Branch("probe_unseed_hlt_phi", &b_probe_unseed_hlt_phi, "probe_unseed_hlt_phi/F");
    tree_->Branch("probe_unseed_hlt_e",   &b_probe_unseed_hlt_e,   "probe_unseed_hlt_e/F");
    tree_->Branch("probe_unseed_hlt_dR",  &b_probe_unseed_hlt_dR,  "probe_unseed_hlt_dR/F");

    // MC-only
    if (isMC_) {
        tree_->Branch("probe_isGenMatched", &b_probe_isGenMatched, "probe_isGenMatched/I");
        tree_->Branch("probe_genPt",        &b_probe_genPt,        "probe_genPt/F");
        tree_->Branch("probe_genEta",       &b_probe_genEta,       "probe_genEta/F");
        tree_->Branch("tag_isGenMatched",   &b_tag_isGenMatched,   "tag_isGenMatched/I");
    }
}

// =============================================================================
//  analyze
// =============================================================================
void DiphotonTnPAnalyzer_OldString::analyze(const edm::Event&      iEvent,
                                             const edm::EventSetup& iSetup)
{
    edm::Handle<edm::View<pat::Photon>>                 photons;
    edm::Handle<edm::TriggerResults>                    trigResults;
    edm::Handle<pat::TriggerObjectStandAloneCollection> trigObjsRaw;
    edm::Handle<std::vector<reco::Vertex>>              vertices;
    edm::Handle<double>                                 rhoHandle;

    iEvent.getByToken(photonToken_,      photons);
    iEvent.getByToken(trigResultsToken_, trigResults);
    iEvent.getByToken(trigObjToken_,     trigObjsRaw);
    iEvent.getByToken(vtxToken_,         vertices);
    iEvent.getByToken(rhoToken_,         rhoHandle);

    if (vertices->empty()) return;
    const reco::Vertex& pv = vertices->front();
    if (pv.isFake()             ||
        pv.ndof()  < 4          ||
        std::fabs(pv.z()) > 24. ||
        pv.position().rho() > 2.) return;

    const edm::TriggerNames& trigNames = iEvent.triggerNames(*trigResults);

    std::vector<pat::TriggerObjectStandAlone> trigObjs;
    trigObjs.reserve(trigObjsRaw->size());
    for (auto obj : *trigObjsRaw) {
        obj.unpackNamesAndLabels(iEvent, *trigResults);
        trigObjs.push_back(std::move(obj));
    }

    if (!pathFired(*trigResults, trigNames, tagTriggerPath_)) return;
    const bool diphotonFired = pathFired(*trigResults, trigNames, diphotonPath_);

    std::vector<size_t> candIdx;
    candIdx.reserve(photons->size());
    for (size_t i = 0; i < photons->size(); ++i)
        if (passesProbePreselection((*photons)[i]))
            candIdx.push_back(i);

    if (candIdx.size() < 2) return;

    for (size_t iTag = 0; iTag < candIdx.size(); ++iTag) {
        const pat::Photon& tag = (*photons)[candIdx[iTag]];

        if (!passesTagSelection(tag, trigObjs)) continue;

        const MatchResult tagMatch =
            getClosestObject(tag.p4(), trigObjs, tagTriggerFilter_);
        const MatchResult tagSeedMatch =
            getClosestObject(tag.p4(), trigObjs, seededFilter_);

        for (size_t iProbe = 0; iProbe < candIdx.size(); ++iProbe) {
            if (iProbe == iTag) continue;

            const pat::Photon& probe = (*photons)[candIdx[iProbe]];

            const double mgg = (tag.p4() + probe.p4()).M();
            if (mgg < massLow_ || mgg > massHigh_) continue;

            const MatchResult seedMatch =
                getClosestObject(probe.p4(), trigObjs, seededFilter_);
            const MatchResult unseedMatch =
                getClosestObject(probe.p4(), trigObjs, unseededFilter_);

            clearBranches();

            b_run    = iEvent.id().run();
            b_lumi   = iEvent.id().luminosityBlock();
            b_event  = iEvent.id().event();
            b_nVtx   = static_cast<int>(vertices->size());
            b_rho    = static_cast<float>(*rhoHandle);
            b_weight = 1.0f;

            // Tag reco
            b_tag_pt           = static_cast<float>(tag.pt());
            b_tag_eta          = static_cast<float>(tag.eta());
            b_tag_phi          = static_cast<float>(tag.phi());
            b_tag_r9           = static_cast<float>(tag.r9());
            b_tag_full5x5_r9   = static_cast<float>(tag.full5x5_r9());
            b_tag_sieie        = static_cast<float>(tag.full5x5_sigmaIetaIeta());
            b_tag_hoe          = static_cast<float>(tag.hadTowOverEm());
            b_tag_isEB         = tag.isEB() ? 1 : 0;
            b_tag_hasPixelSeed = tag.hasPixelSeed() ? 1 : 0;
            {
                const std::string k = "PhotonMVAEstimatorRunIIIWinter22v1Values";
                b_tag_mva = tag.hasUserFloat(k)
                            ? static_cast<float>(tag.userFloat(k)) : -99.f;
            }

            b_tag_hlt_pt  = static_cast<float>(tagMatch.obj->pt());
            b_tag_hlt_eta = static_cast<float>(tagMatch.obj->eta());
            b_tag_hlt_phi = static_cast<float>(tagMatch.obj->phi());
            b_tag_hlt_e   = static_cast<float>(tagMatch.obj->energy());
            b_tag_hlt_dR  = static_cast<float>(tagMatch.dR);

            // Tag seeded-leg HLT object stored for ALL tags
            if (tagSeedMatch.obj != nullptr) {
                b_tag_seed_hlt_pt  = static_cast<float>(tagSeedMatch.obj->pt());
                b_tag_seed_hlt_eta = static_cast<float>(tagSeedMatch.obj->eta());
                b_tag_seed_hlt_phi = static_cast<float>(tagSeedMatch.obj->phi());
                b_tag_seed_hlt_e   = static_cast<float>(tagSeedMatch.obj->energy());
                b_tag_seed_hlt_dR  = static_cast<float>(tagSeedMatch.dR);
            }

            // Probe reco kinematics
            b_probe_pt           = static_cast<float>(probe.pt());
            b_probe_eta          = static_cast<float>(probe.eta());
            b_probe_phi          = static_cast<float>(probe.phi());
            b_probe_isEB         = probe.isEB() ? 1 : 0;
            b_probe_hasPixelSeed = probe.hasPixelSeed() ? 1 : 0;

            // Probe shower shapes
            b_probe_r9         = static_cast<float>(probe.r9());
            b_probe_full5x5_r9 = static_cast<float>(probe.full5x5_r9());
            b_probe_sieie      = static_cast<float>(probe.full5x5_sigmaIetaIeta());
            b_probe_sieip      = static_cast<float>(
                probe.full5x5_showerShapeVariables().sigmaIetaIphi);
            const float e5x5 = static_cast<float>(
                probe.full5x5_showerShapeVariables().e5x5);
            b_probe_s4 = (e5x5 > 0.f)
                ? static_cast<float>(probe.full5x5_showerShapeVariables().e2x2) / e5x5
                : -99.f;
            b_probe_hoe    = static_cast<float>(probe.hadTowOverEm());
            b_probe_hoe_bc = static_cast<float>(probe.hadronicOverEm());

            // Probe isolation
            b_probe_ecalIso               = static_cast<float>(probe.ecalPFClusterIso());
            b_probe_hcalIso               = static_cast<float>(probe.hcalPFClusterIso());
            b_probe_trkIso                = static_cast<float>(probe.trackIso());
            b_probe_trkSumPtHollowConeDR03 = static_cast<float>(probe.trkSumPtHollowConeDR03());
            b_probe_phoIso                = static_cast<float>(probe.photonIso());
            b_probe_chargedHadIso         = static_cast<float>(probe.chargedHadronIso());
            {
                const std::string k = "PFIsoChgQuadratic";
                b_probe_pfRelIso03_chg_quad = probe.hasUserFloat(k)
                    ? static_cast<float>(probe.userFloat(k) / probe.pt()) : -99.f;
            }

            // Probe MVA
            {
                const std::string k = "PhotonMVAEstimatorRunIIIWinter22v1Values";
                b_probe_mva = probe.hasUserFloat(k)
                              ? static_cast<float>(probe.userFloat(k)) : -99.f;
            }

            b_mass = static_cast<float>(mgg);

            b_probe_passSeed     = (seedMatch.dR   < dRmatch_) ? 1 : 0;
            b_probe_passUnseed   = (unseedMatch.dR  < dRmatch_) ? 1 : 0;
            b_probe_passFullPath = diphotonFired ? 1 : 0;

            if (seedMatch.obj != nullptr) {
                b_probe_seed_hlt_pt  = static_cast<float>(seedMatch.obj->pt());
                b_probe_seed_hlt_eta = static_cast<float>(seedMatch.obj->eta());
                b_probe_seed_hlt_phi = static_cast<float>(seedMatch.obj->phi());
                b_probe_seed_hlt_e   = static_cast<float>(seedMatch.obj->energy());
                b_probe_seed_hlt_dR  = static_cast<float>(seedMatch.dR);
            }

            if (unseedMatch.obj != nullptr) {
                b_probe_unseed_hlt_pt  = static_cast<float>(unseedMatch.obj->pt());
                b_probe_unseed_hlt_eta = static_cast<float>(unseedMatch.obj->eta());
                b_probe_unseed_hlt_phi = static_cast<float>(unseedMatch.obj->phi());
                b_probe_unseed_hlt_e   = static_cast<float>(unseedMatch.obj->energy());
                b_probe_unseed_hlt_dR  = static_cast<float>(unseedMatch.dR);
            }

            if (isMC_) {
                b_probe_isGenMatched = -1;
                b_probe_genPt        = -1.f;
                b_probe_genEta       = -99.f;
                b_tag_isGenMatched   = -1;
            }

            tree_->Fill();

        } // probe loop
    }     // tag loop
}

// =============================================================================
//  passesProbePreselection
// =============================================================================
bool DiphotonTnPAnalyzer_OldString::passesProbePreselection(const pat::Photon& pho) const
{
    if (pho.pt() < probePtMin_) return false;

    const double absEta = std::fabs(pho.superCluster()->eta());
    if (absEta > probeEtaMax_) return false;
    if (absEta > 1.4442 && absEta < 1.566) return false;

    if (!pho.superCluster().isNonnull())      return false;
    if (pho.superCluster()->rawEnergy() < 1.) return false;

    // Loose preselection: keep wide for NTuple storage.
    // Tight per-category cuts (H/E, sieie, R9, phoIso, trkIso) applied at fitter stage.
    if (pho.hadTowOverEm() > 0.15)           return false;
    if (pho.full5x5_sigmaIetaIeta() > 0.040) return false;

    return true;
}

// =============================================================================
//  passesTagSelection
// =============================================================================
bool DiphotonTnPAnalyzer_OldString::passesTagSelection(
    const pat::Photon&                               pho,
    const std::vector<pat::TriggerObjectStandAlone>& objs) const
{
    if (pho.pt() < tagPtMin_) return false;

    const double absEta = std::fabs(pho.superCluster()->eta());
    if (absEta > tagEtaMax_) return false;
    if (absEta > 1.4442 && absEta < 1.566) return false;

    if (!pho.hasPixelSeed()) return false;

    if (tagMvaCut_ > -1.5) {
        const std::string k = "PhotonMVAEstimatorRunIIIWinter22v1Values";
        if (!pho.hasUserFloat(k))          return false;
        if (pho.userFloat(k) < tagMvaCut_) return false;
    }

    const MatchResult m = getClosestObject(pho.p4(), objs, tagTriggerFilter_);
    return (m.obj != nullptr && m.dR < dRmatch_);
}

// =============================================================================
//  getClosestObject
// =============================================================================
DiphotonTnPAnalyzer_OldString::MatchResult
DiphotonTnPAnalyzer_OldString::getClosestObject(
    const reco::Candidate::LorentzVector&            p4,
    const std::vector<pat::TriggerObjectStandAlone>& objs,
    const std::string&                               filterLabel) const
{
    MatchResult result{nullptr, std::numeric_limits<double>::infinity()};

    for (const auto& obj : objs) {
        bool hasLabel = false;
        for (const auto& fl : obj.filterLabels()) {
            if (fl.find(filterLabel) != std::string::npos) {
                hasLabel = true; break;
            }
        }
        if (!hasLabel) continue;

        const double dEta = p4.eta() - obj.eta();
        const double dPhi = reco::deltaPhi(p4.phi(), obj.phi());
        const double dR   = std::sqrt(dEta*dEta + dPhi*dPhi);

        if (dR < result.dR) { result.dR = dR; result.obj = &obj; }
    }

    return result;
}

// =============================================================================
//  pathFired
// =============================================================================
bool DiphotonTnPAnalyzer_OldString::pathFired(const edm::TriggerResults& trigResults,
                                               const edm::TriggerNames&   trigNames,
                                               const std::string&         partialName) const
{
    for (unsigned i = 0; i < trigResults.size(); ++i) {
        if (trigNames.triggerName(i).find(partialName) != std::string::npos &&
            trigResults.accept(i))
            return true;
    }
    return false;
}

// =============================================================================
//  clearBranches
// =============================================================================
void DiphotonTnPAnalyzer_OldString::clearBranches()
{
    b_run = b_lumi = b_event = 0;
    b_nVtx = 0; b_rho = -1.f; b_weight = 1.f;

    b_tag_pt = b_tag_eta = b_tag_phi = -99.f;
    b_tag_r9 = b_tag_full5x5_r9 = b_tag_sieie = b_tag_hoe = b_tag_mva = -99.f;
    b_tag_isEB = b_tag_hasPixelSeed = -1;
    b_tag_hlt_pt = b_tag_hlt_eta = b_tag_hlt_phi = b_tag_hlt_e = b_tag_hlt_dR = -99.f;
    b_tag_seed_hlt_pt  = b_tag_seed_hlt_eta  = -99.f;
    b_tag_seed_hlt_phi = b_tag_seed_hlt_e    = -99.f;
    b_tag_seed_hlt_dR  = -99.f;

    b_probe_pt = b_probe_eta = b_probe_phi = -99.f;
    b_probe_isEB = b_probe_hasPixelSeed = -1;
    b_probe_r9 = b_probe_full5x5_r9 = -99.f;
    b_probe_sieie = b_probe_sieip = b_probe_s4 = -99.f;
    b_probe_hoe = b_probe_hoe_bc = -99.f;
    b_probe_ecalIso = b_probe_hcalIso = b_probe_trkIso = -99.f;
    b_probe_trkSumPtHollowConeDR03 = -99.f;
    b_probe_phoIso = b_probe_chargedHadIso = b_probe_pfRelIso03_chg_quad = -99.f;
    b_probe_mva = -99.f;

    b_mass = -99.f;

    b_probe_passSeed = b_probe_passUnseed = b_probe_passFullPath = -1;

    b_probe_seed_hlt_pt  = b_probe_seed_hlt_eta  = -99.f;
    b_probe_seed_hlt_phi = b_probe_seed_hlt_e    = -99.f;
    b_probe_seed_hlt_dR  = -99.f;

    b_probe_unseed_hlt_pt  = b_probe_unseed_hlt_eta  = -99.f;
    b_probe_unseed_hlt_phi = b_probe_unseed_hlt_e    = -99.f;
    b_probe_unseed_hlt_dR  = -99.f;

    b_probe_isGenMatched = b_tag_isGenMatched = -1;
    b_probe_genPt  = -1.f;
    b_probe_genEta = -99.f;
}

// =============================================================================
//  fillDescriptions
// =============================================================================
void DiphotonTnPAnalyzer_OldString::fillDescriptions(
    edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("photons",     edm::InputTag("slimmedPhotons"));
    desc.add<edm::InputTag>("trigResults", edm::InputTag("TriggerResults","","HLT"));
    desc.add<edm::InputTag>("trigObjects", edm::InputTag("slimmedPatTrigger"));
    desc.add<edm::InputTag>("vertices",    edm::InputTag("offlineSlimmedPrimaryVertices"));
    desc.add<edm::InputTag>("rho",         edm::InputTag("fixedGridRhoFastjetAll"));
    desc.add<std::string>("diphotonPath",
        "HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId");
    // OLD filter strings — valid for Run2023C-v1 and Run2023C-v2
    desc.add<std::string>("seededFilter",
        "hltEG30Iso60CaloId15b35eR9Id50b90eHE12b10eR9Id50b80eEcalIsoFilter");
    desc.add<std::string>("unseededFilter",
        "hltEG18TrackIso60Iso60CaloId15b35eR9Id50b90eHE12b10eR9Id50b80eTrackIsoUnseededFilter");
    desc.add<std::string>("tagTriggerPath",   "HLT_Ele30_WPTight_Gsf");
    desc.add<std::string>("tagTriggerFilter", "hltEle30WPTightGsfTrackIsoFilter");
    desc.add<double>("tagPtMin",   35.0);
    desc.add<double>("tagEtaMax",   2.1);
    desc.add<double>("tagMvaCut",   0.2);
    desc.add<double>("probePtMin",  15.0);
    desc.add<double>("probeEtaMax",  2.5);
    desc.add<double>("massLow",    60.0);
    desc.add<double>("massHigh",  120.0);
    desc.add<double>("dRmatch",     0.1);
    desc.add<bool>  ("isMC",      false);
    descriptions.addDefault(desc);
}

// =============================================================================
DEFINE_FWK_MODULE(DiphotonTnPAnalyzer_OldString);
