#include "catch.hpp"

#include "../parser.h"


const std::string simpleGpxFileData = 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<gpx creator=\"StravaGPX\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\" version=\"1.1\" xmlns=\"http://www.topografix.com/GPX/1/1\">\n"
" <metadata>\n"
"  <time>2020-11-07T14:26:52Z</time>\n"
" </metadata>\n"
" <trk>\n"
"  <name>Fun Long View, Three Falls, Forbidden Loop</name>\n"
"  <type>1</type>\n"
"  <trkseg>\n"
"   <trkpt lat=\"40.4645690\" lon=\"-111.8047420\">\n"
"    <ele>1591.6</ele>\n"
"    <time>2020-11-07T14:26:52Z</time>\n"
"   </trkpt>\n"
"   <trkpt lat=\"40.4647140\" lon=\"-111.8049490\">\n"
"    <ele>1592.6</ele>\n"
"    <time>2020-11-07T14:27:13Z</time>\n"
"   </trkpt>\n"
"   <trkpt lat=\"40.4648990\" lon=\"-111.8049670\">\n"
"    <ele>1592.7</ele>\n"
"    <time>2020-11-07T14:27:19Z</time>\n"
"   </trkpt>\n"
"  </trkseg>\n"
"  <trkseg>\n"
"   <trkpt lat=\"40.4650730\" lon=\"-111.8048700\">\n"
"    <ele>1591.4</ele>\n"
"    <time>2020-11-07T14:27:25Z</time>\n"
"   </trkpt>\n"
"   <trkpt lat=\"40.4652790\" lon=\"-111.8047500\">\n"
"    <ele>1590.5</ele>\n"
"    <time>2020-11-07T14:27:29Z</time>\n"
"   </trkpt>\n"
"  </trkseg>\n"
" </trk>\n"
"</gpx>\n";


TEST_CASE( "gpxparser", "" ) {
    SECTION( "parser simple GPX file" )
    {
        Parser parser;
        Container<TrackPoint> output;
        REQUIRE(parser.parseActivityData(simpleGpxFileData, output));

        REQUIRE(output.size() == 5);

        REQUIRE(output[0].activityName == "Fun Long View, Three Falls, Forbidden Loop");

        REQUIRE(output[0].latitude == Approx(40.4645690));
        REQUIRE(output[0].longitude == Approx(-111.8047420));
        REQUIRE(output[0].altitude == Approx(1591.6));
        REQUIRE(output[0].startOfSegement);

        REQUIRE(output[1].latitude == Approx(40.4647140));
        REQUIRE(output[1].longitude == Approx(-111.8049490));
        REQUIRE(output[1].altitude == Approx(1592.6));
        REQUIRE(!output[1].startOfSegement);
        REQUIRE(output[1].activityName.empty());

        REQUIRE(output[2].latitude == Approx(40.4648990));
        REQUIRE(output[2].longitude == Approx(-111.8049670));
        REQUIRE(output[2].altitude == Approx(1592.7));
        REQUIRE(!output[2].startOfSegement);
        REQUIRE(output[2].activityName.empty());

        REQUIRE(output[3].latitude == Approx(40.4650730));
        REQUIRE(output[3].longitude == Approx(-111.8048700));
        REQUIRE(output[3].altitude == Approx(1591.4));
        REQUIRE(output[3].startOfSegement);
        REQUIRE(output[3].activityName.empty());

        REQUIRE(output[4].latitude == Approx(40.4652790));
        REQUIRE(output[4].longitude == Approx(-111.8047500));
        REQUIRE(output[4].altitude == Approx(1590.5));
        REQUIRE(!output[4].startOfSegement);
        REQUIRE(output[4].activityName.empty());
    }
}

