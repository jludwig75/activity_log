#include "catch.hpp"

#include "../file_utils.h"


const std::string simplestXmlFile =
"<root></root>";

const std::string upperCaseXMLFile =
"<?XML version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<ROOT>"
"<ROOT>\n";

const std::string testGpxXml = 
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
"  </trkseg>\n"
" </trk>\n"
"</gpx>";

const std::string allWhiteSpace =
"\n \t\n \n \t \t \n \n";

const std::string xmlComment =
"<!-- This is a commend -->\n";

const std::string nonXmlLime =
"Joe was here!\n";


TEST_CASE( "apperasToBeXmlFileData", "" ) {
    SECTION( "Basic XML file" )
    {
        REQUIRE(apperasToBeXmlFileData(simplestXmlFile));
    }

    SECTION( "XML file that starts with a comment" )
    {
        REQUIRE(apperasToBeXmlFileData(xmlComment + simplestXmlFile));
    }

    SECTION( "XML file that starts with a non-XML data is not XML" )
    {
        REQUIRE(!apperasToBeXmlFileData(nonXmlLime + simplestXmlFile));
    }

    SECTION( "XML file that ends with a non-XML data is not XML" )
    {
        REQUIRE(!apperasToBeXmlFileData(simplestXmlFile + nonXmlLime));
    }

    SECTION( "All whitespace is not XML" )
    {
        REQUIRE(!apperasToBeXmlFileData(allWhiteSpace));
    }

    SECTION( "Uppercase XML file" )
    {
        REQUIRE(apperasToBeXmlFileData(upperCaseXMLFile));
    }

    SECTION( "Basic XML file with white space at beginning" )
    {
        REQUIRE(apperasToBeXmlFileData(allWhiteSpace + simplestXmlFile));
    }

    SECTION( "Basic XML file with white space at end" )
    {
        REQUIRE(apperasToBeXmlFileData(simplestXmlFile + allWhiteSpace));
    }

    SECTION( "Basic GPX file" )
    {
        REQUIRE(apperasToBeXmlFileData(testGpxXml));
    }

    SECTION( "Basic GPX file with white space at beginning" )
    {
        REQUIRE(apperasToBeXmlFileData(allWhiteSpace + testGpxXml));
    }

    SECTION( "Basic GPX file with white space at end" )
    {
        REQUIRE(apperasToBeXmlFileData(testGpxXml + allWhiteSpace));
    }
}