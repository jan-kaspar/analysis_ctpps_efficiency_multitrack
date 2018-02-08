import sys 
import os
import FWCore.ParameterSet.Config as cms

sys.path.append(os.path.relpath("./"))
sys.path.append(os.path.relpath("../../../../"))

from config_base import config
from input_files import input_files

config.input_files = input_files
