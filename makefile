all: distributions make_ratios

distributions: distributions.cc command_line_tools.h data_formats.h
	g++ -O3 -Wall -Wextra -Wno-attributes --std=c++11\
		`root-config --libs` -lMinuit -lMinuit2 `root-config --cflags` \
		-I"$(CMSSW_BASE)/src" \
		-I"$(CMSSW_RELEASE_BASE)/src" \
		-L"$(CMSSW_BASE)/lib/slc6_amd64_gcc630" \
		-L"$(CMSSW_RELEASE_BASE)/lib/slc6_amd64_gcc630" \
		-lDataFormatsFWLite \
		-lDataFormatsCommon \
		-lDataFormatsCTPPSDetId \
			distributions.cc -o distributions

make_ratios: make_ratios.cc command_line_tools.h data_formats.h
	g++ `root-config --libs` `root-config --cflags` \
		make_ratios.cc -o make_ratios
