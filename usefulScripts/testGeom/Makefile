all:
	g++ `root-config --cflags --glibs` `xml2-config --cflags` -lxml2 testGeom.cpp DetectorGeometryTools.cpp DetectorGeometry.cpp DetectorGeometryNSW.cpp Sector.cpp Tools.cpp

clean:
	rm a.out *.o
