import FWCore.ParameterSet.Config as cms

config = cms.PSet(
    input_files = cms.vstring(),

    timestamp_min = cms.uint32(1451602800 + 24*3600*525),
    timestamp_max = cms.uint32(1451602800 + 24*3600*625),

    pixelsAvailable = cms.bool(True),

    stripRPIds = cms.vuint32(3, 103),
    pixelRPIds = cms.vuint32(23, 123),
)
