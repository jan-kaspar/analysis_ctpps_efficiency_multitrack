all: parse

parse: parse.cc
	g++ `root-config --libs` `root-config --cflags` \
		parse.cc -o parse

