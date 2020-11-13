#include "gpxparser.h"

#include "file_utils.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <limits>
#include <memory>
#include <mutex>
#include <sstream>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
 
using namespace xercesc;

namespace internal
{

bool parseFile(const std::string& gpxFileData, Container<TrackPoint>& stream);

class XercesPlatform
{
public:
    XercesPlatform()
    {
        reference();
    }
    ~XercesPlatform()
    {
        release();
    }
protected:
    static void reference()
    {
        std::unique_lock lock(s_mutex);
        assert(s_ref_count >= 0);
        if (s_ref_count++ == 0)
        {
            XMLPlatformUtils::Initialize();
        }
    }
    static void release()
    {
        std::unique_lock lock(s_mutex);
        assert(s_ref_count > 0);
        if (--s_ref_count == 0)
        {
            // TODO: Does this just need to be terminated when the program exits?
            //       Would it be better to just have a simple glocal RAII object (singleton)?
            XMLPlatformUtils::Terminate();
        }
    }
    static std::mutex s_mutex;
    static int s_ref_count;
};

std::mutex XercesPlatform::s_mutex{};
int XercesPlatform::s_ref_count{};

} // namespace internal


bool GpxParser::canParseActivityData(const std::string& activityData) const
{
    if (!apperasToBeXmlFileData(activityData))
    {
        return false;
    }

    // TODO: verify that it's a GPX file
    return true;
}

bool GpxParser::parseActivityData(const std::string& activityData, Container<TrackPoint> &output) const
{
    internal::XercesPlatform platform;

    return internal::parseFile(activityData, output);
}

namespace internal
{

template<typename ValueType>
ValueType fromString(const std::string& str)
{
    std::stringstream ss(str);
    ValueType v;
    ss >> v;
    assert(!ss.bad() && ss.eof());
    return v;
}

std::time_t parseTimeString(const std::string& timeStr)
{
    std::tm tm = {};
    std::stringstream ss(timeStr);
    // 2020-10-31T16:35:15Z
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return timegm(&tm);
}


bool parseTrackPoint(DOMElement* trackPointElement, Container<TrackPoint>& stream, bool startOfSegment, const std::string& activityName, bool& activityNameSet)
{
    double latitude = std::numeric_limits<double>::infinity();
    double longitude = std::numeric_limits<double>::infinity();
    double altitude = std::numeric_limits<double>::infinity();
    std::time_t time = 0;

    auto TAG_lat = XMLString::transcode("lat");
    auto TAG_lon = XMLString::transcode("lon");
    auto TAG_ele = XMLString::transcode("ele");
    auto TAG_time = XMLString::transcode("time");

    DOMNodeList* trackPointChildren = trackPointElement->getChildNodes();
    const  XMLSize_t trackPointChildrenCount = trackPointChildren->getLength();
    for( XMLSize_t t = 0; t < trackPointChildrenCount; ++t )
    {
        DOMNode* trackPointChildNode = trackPointChildren->item(t);
        if( trackPointChildNode->getNodeType())  // true is not NULL
        {
            auto nodeType = trackPointChildNode->getNodeType();
            if (trackPointChildNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
            {
                // Found node which is an Element. Re-cast node as element
                DOMElement* trackSegmentChildElement
                            = dynamic_cast< xercesc::DOMElement* >( trackPointChildNode );
                if( XMLString::equals(trackSegmentChildElement->getTagName(), TAG_ele))
                {
                    DOMNode *node = trackSegmentChildElement->getFirstChild();
                    std::string value = XMLString::transcode(node->getNodeValue());
                    altitude = fromString<double>(value);
                }
                else if( XMLString::equals(trackSegmentChildElement->getTagName(), TAG_time))
                {
                    DOMNode *node = trackSegmentChildElement->getFirstChild();
                    std::string timeStr = XMLString::transcode(node->getNodeValue());
                    time = parseTimeString(timeStr);
                }
            }
        }
    }

    auto* attrs = trackPointElement->getAttributes();

    auto* latAttr = attrs->getNamedItem(TAG_lat);
    assert(latAttr != NULL);
    std::string latStr = XMLString::transcode(latAttr->getNodeValue());
    latitude = fromString<double>(latStr);

    auto* lonAttr = attrs->getNamedItem(TAG_lon);
    assert(lonAttr != NULL);
    std::string lonStr = XMLString::transcode(lonAttr->getNodeValue());
    longitude = fromString<double>(lonStr);

    assert(latitude != std::numeric_limits<double>::infinity() &&
            longitude != std::numeric_limits<double>::infinity() &&
            altitude != std::numeric_limits<double>::infinity() &&
            time != 0);

    // TODO: parse heart reate
    auto trackPoint = TrackPoint(std::chrono::system_clock::from_time_t(time), latitude, longitude, altitude, 0, startOfSegment);
    if (!activityNameSet && !activityName.empty())
    {
        trackPoint.activityName = activityName;
        activityNameSet = true;
    }
    stream.push(std::move(trackPoint));
    return true;
}

bool parseTrackSegment(DOMElement* trackSegmentElement, Container<TrackPoint>& stream, const std::string& activityName, bool& activityNameSet)
{
    auto TAG_trackPt = XMLString::transcode("trkpt");

    bool startOfSegment = true;
    DOMNodeList* trackSegmentChildren = trackSegmentElement->getChildNodes();
    const  XMLSize_t trackSgementChildrenCount = trackSegmentChildren->getLength();
    for( XMLSize_t t = 0; t < trackSgementChildrenCount; ++t )
    {
        DOMNode* trackSegmentChildNode = trackSegmentChildren->item(t);
        if( trackSegmentChildNode->getNodeType() &&  // true is not NULL
            trackSegmentChildNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* trackSegmentChildElement
                        = dynamic_cast< xercesc::DOMElement* >( trackSegmentChildNode );
            if( XMLString::equals(trackSegmentChildElement->getTagName(), TAG_trackPt))
            {
                if (!parseTrackPoint(trackSegmentChildElement, stream, startOfSegment, activityName, activityNameSet))
                {
                    return false;
                }
                startOfSegment = false;
            }
        }
    }

    return true;
}

bool parseTrack(DOMElement* trackElement, Container<TrackPoint>& stream, std::string& activityName, bool& activityNameSet)
{
    auto TAG_trackSeg = XMLString::transcode("trkseg");
    auto TAG_name = XMLString::transcode("name");
    auto TAG_type = XMLString::transcode("type");

    unsigned trackType = UINT_MAX;

    DOMNodeList* trackChildren = trackElement->getChildNodes();
    const  XMLSize_t trackCount = trackChildren->getLength();
    for( XMLSize_t t = 0; t < trackCount; ++t )
    {
        DOMNode* trackChildNode = trackChildren->item(t);
        if( trackChildNode->getNodeType() &&  // true is not NULL
            trackChildNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* trackChildElement
                        = dynamic_cast< xercesc::DOMElement* >( trackChildNode );
            if( XMLString::equals(trackChildElement->getTagName(), TAG_trackSeg))
            {
                if (!parseTrackSegment(trackChildElement, stream, activityName, activityNameSet))
                {
                    return false;
                }
            }
            else if( XMLString::equals(trackChildElement->getTagName(), TAG_name))
            {
                DOMNode *node = trackChildElement->getFirstChild();
                if (!activityNameSet)
                {
                    assert(activityName.empty());
                    activityName = XMLString::transcode(node->getNodeValue());
                }
            }
            else if( XMLString::equals(trackChildElement->getTagName(), TAG_type))
            {
                DOMNode *node = trackChildElement->getFirstChild();
                std::string value = XMLString::transcode(node->getNodeValue());
                trackType = fromString<unsigned>(value);
            }
        }
    }

    return true;
}

bool parseFile(const std::string& gpxFileData, Container<TrackPoint>& stream)
{
    auto TAG_root = XMLString::transcode("gpx");
    auto TAG_track = XMLString::transcode("trk");
    auto TAG_metadata = XMLString::transcode("metadata");
    auto TAG_time = XMLString::transcode("time");

    auto parser = std::make_unique<XercesDOMParser>();
    // Configure DOM parser.
    parser->setValidationScheme( XercesDOMParser::Val_Never );
    parser->setDoNamespaces( false );
    parser->setDoSchema( false );
    parser->setLoadExternalDTD( false );

    MemBufInputSource dataSource((const XMLByte*)gpxFileData.data(), gpxFileData.length(), "");
    parser->parse(dataSource);

    auto xmlDoc = parser->getDocument();
    auto elementRoot = xmlDoc->getDocumentElement();
    DOMNodeList* children = elementRoot->getChildNodes();
    const  XMLSize_t nodeCount = children->getLength();

    std::string activityName;
    bool activityNameSet = false;

    // For all nodes, children of "root" in the XML tree.
    for( XMLSize_t xx = 0; xx < nodeCount; ++xx )
    {
        DOMNode* currentNode = children->item(xx);
        if( currentNode->getNodeType() &&  // true is not NULL
            currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
        {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement
                        = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if( XMLString::equals(currentElement->getTagName(), TAG_track))
            {
                if (!parseTrack(currentElement, stream, activityName, activityNameSet))
                {
                    // TODO: Use RAII
                    stream.done_pushing();
                    return false;
                }
            }
            else if( XMLString::equals(currentElement->getTagName(), TAG_metadata))
            {
                DOMNodeList* metadatChildren = currentElement->getChildNodes();
                const  XMLSize_t metadatChildrenCount = metadatChildren->getLength();
                for( XMLSize_t t = 0; t < metadatChildrenCount; ++t )
                {
                    DOMNode* metadatChildNode = metadatChildren->item(t);
                    if( metadatChildNode->getNodeType())  // true is not NULL
                    {
                        if (metadatChildNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
                        {
                            // Found node which is an Element. Re-cast node as element
                            DOMElement* metadatChildElement
                                        = dynamic_cast< xercesc::DOMElement* >( metadatChildNode );
                            if( XMLString::equals(metadatChildElement->getTagName(), TAG_time))
                            {
                                DOMNode *node = metadatChildElement->getFirstChild();
                                std::string timeStr = XMLString::transcode(node->getNodeValue());
                                std::time_t time = parseTimeString(timeStr);
                            }
                        }
                    }
                }
            }
        }
    }

    // TODO: Use RAII
    stream.done_pushing();
    return true;
}

} // namespace internal
