import FWCore.ParameterSet.Config as cms

config = cms.PSet(
    input_files = cms.vstring(),

    pixelsAvailable = cms.bool(True),

    stripRPIds = cms.vuint32(3, 103),
    pixelRPIds = cms.vuint32(23, 123),
)
