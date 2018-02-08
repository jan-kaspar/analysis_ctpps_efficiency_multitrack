import FWCore.ParameterSet.Config as cms

config = cms.PSet(
    input_files = cms.vstring(),

    pixelsAvailable = cms.bool(False),

    stripRPIds = cms.vuint32(3, 2, 102, 103),
    pixelRPIds = cms.vuint32(),
)
