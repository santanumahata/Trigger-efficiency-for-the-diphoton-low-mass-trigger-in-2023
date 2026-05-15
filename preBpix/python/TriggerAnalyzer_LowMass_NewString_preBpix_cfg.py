import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

# Run2023C-v3 and Run2023C-v4 — NEW HLT filter strings (changed from C-v3 onwards)
# Seeded  : hltEG30Iso60b40eCaloId11b32eHE10b9eR9Id50b90eEcalIsoFilter
# Unseeded: hltDiEG18TrackIso60Iso60b40eCaloId11b32eHE10b9eR9Id50b90eTrackIsoUnseededFilter
# Contributor: Santanu Mahata (added isolation/seed-match branches)

options = VarParsing('analysis')
try:
    options.register('isMC', False,
        VarParsing.multiplicity.singleton, VarParsing.varType.bool, "True for MC")
    options.register('globalTag', '',
        VarParsing.multiplicity.singleton, VarParsing.varType.string, "Override global tag")
    options.register('outputFile', 'tnpTree_preBpix_Cv3v4_newString.root',
        VarParsing.multiplicity.singleton, VarParsing.varType.string, "Output file name")
except RuntimeError:
    pass
options.parseArguments()

process = cms.Process("TnPAna")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
gt = options.globalTag if options.globalTag else \
     ("130X_mcRun3_2023_realistic_v14" if options.isMC else "130X_dataRun3_v2")
process.GlobalTag = GlobalTag(process.GlobalTag, gt, '')

process.load("Configuration.Geometry.GeometryDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.inputFiles if options.inputFiles else [
        # Run2023C-v4 test file (EGamma0) — Rajdeep's verified new-string file
        'root://xrootd-cms.infn.it//store/data/Run2023C/EGamma0/MINIAOD/22Sep2023_v4-v1/40000/00016571-eb40-4d14-8fc0-e37d4feab672.root',
    ]),
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents if options.maxEvents != -1 else -1)
)

process.TFileService = cms.Service("TFileService",
    fileName      = cms.string(options.outputFile),
    closeFileFast = cms.untracked.bool(True),
)

# LumiList not applied in cfg — CRAB handles lumi masking via cfg.Data.lumiMask

SEEDED_FILTER   = "hltEG30Iso60b40eCaloId11b32eHE10b9eR9Id50b90eEcalIsoFilter"
UNSEEDED_FILTER = "hltDiEG18TrackIso60Iso60b40eCaloId11b32eHE10b9eR9Id50b90eTrackIsoUnseededFilter"

process.diphotonTnP = cms.EDAnalyzer("DiphotonTnPAnalyzer_NewString",
    photons     = cms.InputTag("slimmedPhotons"),
    trigResults = cms.InputTag("TriggerResults", "", "HLT"),
    trigObjects = cms.InputTag("slimmedPatTrigger"),
    vertices    = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho         = cms.InputTag("fixedGridRhoFastjetAll"),
    diphotonPath   = cms.string("HLT_Diphoton30_18_R9IdL_AND_HE_AND_IsoCaloId"),
    seededFilter   = cms.string(SEEDED_FILTER),
    unseededFilter = cms.string(UNSEEDED_FILTER),
    tagTriggerPath   = cms.string("HLT_Ele30_WPTight_Gsf"),
    tagTriggerFilter = cms.string("hltEle30WPTightGsfTrackIsoFilter"),
    tagPtMin  = cms.double(35.0),
    tagEtaMax = cms.double(2.1),
    tagMvaCut = cms.double(0.2),
    probePtMin  = cms.double(15.0),
    probeEtaMax = cms.double(2.5),
    massLow  = cms.double(60.0),
    massHigh = cms.double(120.0),
    dRmatch  = cms.double(0.1),
    isMC     = cms.bool(options.isMC),
)

process.p = cms.Path(process.diphotonTnP)
process.schedule = cms.Schedule(process.p)
