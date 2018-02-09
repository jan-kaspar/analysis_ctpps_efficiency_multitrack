import FWCore.ParameterSet.Config as cms

config = cms.PSet(
    input_files = cms.vstring(),

    timestamp_min = cms.uint32(1451602800 + 24*3600*100),
    timestamp_max = cms.uint32(1451602800 + 24*3600*300),

    pixelsAvailable = cms.bool(False),

    stripRPIds = cms.vuint32(3, 2, 102, 103),
    pixelRPIds = cms.vuint32(),
)
