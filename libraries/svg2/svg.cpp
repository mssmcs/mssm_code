#include "svg.h"
#include <iostream>
#include <ostream>

using namespace tinyxml2;

Svg::Svg(const std::string& filename)
{
    if (!filename.empty())
    {
        load(filename);
    }
}

SvgAttr::SvgAttr()
{
    xmlns_dc="http://purl.org/dc/elements/1.1/";
    xmlns_cc="http://creativecommons.org/ns#";
    xmlns_rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#";
    xmlns_svg="http://www.w3.org/2000/svg";
    xmlns="http://www.w3.org/2000/svg";
    width="210mm";
    height="297mm";
    viewBox="0 0 744.09448819 1052.3622047";
    id="svg2";
    version="1.1";
}

void Svg::init()
{

}

void Svg::save(const std::string& filename)
{
    tinyxml2::XMLDocument doc;

    // Create root element
    tinyxml2::XMLElement* root = doc.NewElement("svg");
    doc.InsertFirstChild(root);

    // Add attributes to root element
    root->SetAttribute("xmlns:dc", headerInfo.xmlns_dc.c_str());
    root->SetAttribute("xmlns:cc", headerInfo.xmlns_cc.c_str());
    root->SetAttribute("xmlns:rdf", headerInfo.xmlns_rdf.c_str());
    root->SetAttribute("xmlns:svg", headerInfo.xmlns_svg.c_str());
    root->SetAttribute("xmlns", headerInfo.xmlns.c_str());
    root->SetAttribute("width", headerInfo.width.c_str());
    root->SetAttribute("height", headerInfo.height.c_str());
    root->SetAttribute("viewBox", headerInfo.viewBox.c_str());
    root->SetAttribute("id", headerInfo.id.c_str());
    root->SetAttribute("version", headerInfo.version.c_str());

    // Add defs element
    tinyxml2::XMLElement* defs = doc.NewElement("defs");
    defs->SetAttribute("id", "defs4");
    root->InsertEndChild(defs);

    // Add metadata element
    tinyxml2::XMLElement* metadata = doc.NewElement("metadata");
    metadata->SetAttribute("id", "metadata7");
    root->InsertEndChild(metadata);

    // Add rdf:RDF and cc:Work elements
    tinyxml2::XMLElement* rdfRDF = doc.NewElement("rdf:RDF");
    tinyxml2::XMLElement* ccWork = doc.NewElement("cc:Work");
    ccWork->SetAttribute("rdf:about", "");

    // Add dc:format element
    tinyxml2::XMLElement* dcFormat = doc.NewElement("dc:format");
    dcFormat->SetText("image/svg+xml");
    ccWork->InsertEndChild(dcFormat);

    // Add dc:type element
    tinyxml2::XMLElement* dcType = doc.NewElement("dc:type");
    dcType->SetAttribute("rdf:resource", "http://purl.org/dc/dcmitype/StillImage");
    ccWork->InsertEndChild(dcType);

    // Add dc:title element
    tinyxml2::XMLElement* dcTitle = doc.NewElement("dc:title");
    ccWork->InsertEndChild(dcTitle);

    // Append cc:Work to rdf:RDF and rdf:RDF to metadata
    rdfRDF->InsertEndChild(ccWork);
    metadata->InsertEndChild(rdfRDF);

    // Add SVG elements
    for (SvgElementPtr element : elements) {
        element->saveAsChildOf(root);
    }

    // Save or print the document
    doc.SaveFile(filename.c_str());



    // zen::XmlDoc doc("svg");

    // zen::XmlOut out(doc);

    // out.attribute("xmlns:dc", headerInfo.xmlns_dc);
    // out.attribute("xmlns:cc", headerInfo.xmlns_cc);
    // out.attribute("xmlns:rdf", headerInfo.xmlns_rdf);
    // out.attribute("xmlns:svg", headerInfo.xmlns_svg);
    // out.attribute("xmlns", headerInfo.xmlns);
    // out.attribute("width", headerInfo.width);
    // out.attribute("height", headerInfo.height);
    // out.attribute("viewBox", headerInfo.viewBox);
    // out.attribute("id", headerInfo.id);
    // out.attribute("version", headerInfo.version);

    // out["defs"].attribute("id","defs4");

    // out["metadata"].attribute("id","metadata7");
    // auto wrk = out["metadata"]["rdf:RDF"]["cc:Work"];
    // wrk.attribute("rdf:about","");
    // wrk["dc:format"]("image/svg+xml");
    // wrk["dc:type"].attribute("rdf:resource","http://purl.org/dc/dcmitype/StillImage");
    // wrk["dc:title"];

    // for (SvgElementPtr element : elements)
    // {
    //     element->saveAsChildOf(doc.root());
    // }

/*
    doc

    <metadata
       id="metadata7">
      <rdf:RDF>
        <cc:Work
           rdf:about="">
          <dc:format>image/svg+xml</dc:format>
          <dc:type
             rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
          <dc:title />
        </cc:Work>
      </rdf:RDF>
    </metadata>

*/
       /*
      <defs
         id="defs4" />
      <metadata
         id="metadata7">
        <rdf:RDF>
          <cc:Work
             rdf:about="">
            <dc:format>image/svg+xml</dc:format>
            <dc:type
               rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
            <dc:title></dc:title>
          </cc:Work>
        </rdf:RDF>
      </metadata>
      <g
         style="display:inline"
         id="layer2">
        <path
           id="path4640"
           style="fill:#1b1918;fill-rule:evenodd;stroke:#000000;stroke-width:0.1;stroke-miterlimit:4;stroke-dasharray:none"
           d="m 357.02507,426.67697 -0.0917,-0.7335 -0.4584,-0.6418 -0.6418,-0.4584 -0.8252,-0.1834 -0.8251,0.1834 -0.6418,0.4584 -0.3667,0.6418 -0.1834,0.7335 0.1834,0.8252 0.3667,0.6418 0.6418,0.4584 0.8251,0.1833 0.8252,-0.1833 0.6418,-0.4584 0.4584,-0.6418 0.0917,-0.8252 z" />
        <path
           id="path4642"
           */

    // try
    // {
    //     zen::save(doc, filename); //throw zen::XmlFileError
    // }
    // catch (const zen::XmlFileError& e)
    // {
    //     /* handle error */
    // }

}

template<typename I>
I begin(std::pair<I, I> collection)
{
    return collection.first;
}

std::string fixZero(const char* str)
{
    return str ? std::string(str) : "";
}

void Svg::load(const std::string& filename)
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
        std::cerr << "Failed to load SVG file: " << filename << std::endl;
        return;
    }

    // Retrieve root element
    tinyxml2::XMLElement* root = doc.RootElement();
    if (!root || std::string(root->Name()) != "svg") {
        std::cerr << "Invalid SVG root element." << std::endl;
        return;
    }

    // Read attributes from root element
    headerInfo.xmlns_dc = fixZero(root->Attribute("xmlns:dc"));
    headerInfo.xmlns_cc = fixZero(root->Attribute("xmlns:cc"));
    headerInfo.xmlns_rdf = fixZero(root->Attribute("xmlns:rdf"));
    headerInfo.xmlns_svg = fixZero(root->Attribute("xmlns:svg"));
    headerInfo.xmlns = fixZero(root->Attribute("xmlns"));
    headerInfo.width = fixZero(root->Attribute("width"));
    headerInfo.height = fixZero(root->Attribute("height"));
    headerInfo.viewBox = fixZero(root->Attribute("viewBox"));
    headerInfo.id = fixZero(root->Attribute("id"));
    headerInfo.version = fixZero(root->Attribute("version"));

    // Handle metadata, defs, and other child elements
    tinyxml2::XMLElement* metadata = root->FirstChildElement("metadata");
    if (metadata) {
        tinyxml2::XMLElement* rdfRDF = metadata->FirstChildElement("rdf:RDF");
        if (rdfRDF) {
            tinyxml2::XMLElement* ccWork = rdfRDF->FirstChildElement("cc:Work");
            if (ccWork) {
                // Read attributes and child elements of cc:Work
                //const char* about = ccWork->Attribute("rdf:about");
                tinyxml2::XMLElement* dcFormat = ccWork->FirstChildElement("dc:format");
                if (dcFormat) {
                    //const char* format = dcFormat->GetText();
                }
                tinyxml2::XMLElement* dcType = ccWork->FirstChildElement("dc:type");
                if (dcType) {
                    //const char* resource = dcType->Attribute("rdf:resource");
                }
                //tinyxml2::XMLElement* dcTitle = ccWork->FirstChildElement("dc:title");
                // Handle dc:title if necessary
            }
        }
    }

    SvgStructElement::load(root);

    // // Parse child elements like groups or paths
    // for (tinyxml2::XMLElement* child = root->FirstChildElement("g"); child; child = child->NextSiblingElement("g")) {
    //     auto group = createGroup();
    //     group->load(child);
    //     elements.push_back(group);
    // }

    // // Additional parsing for specific elements
    // for (SvgElementPtr element : elements) {
    //     element->loadFromXml(root);
    // }
    //
    //     /*
    //     auto kids = doc.root().getChildren("g");

    //     for (auto gi = kids.first; gi != kids.second; ++gi)
    //     {
    //         // load groups
    //         auto group = createGroup();
    //         group->load(*gi);
    //     }
    //     */
    //        /*
    //       <defs
    //          id="defs4" />
    //       <metadata
    //          id="metadata7">
    //         <rdf:RDF>
    //           <cc:Work
    //              rdf:about="">
    //             <dc:format>image/svg+xml</dc:format>
    //             <dc:type
    //                rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
    //             <dc:title></dc:title>
    //           </cc:Work>
    //         </rdf:RDF>
    //       </metadata>
    //       <g
    //          style="display:inline"
    //          id="layer2">
    //         <path
    //            id="path4640"
    //            style="fill:#1b1918;fill-rule:evenodd;stroke:#000000;stroke-width:0.1;stroke-miterlimit:4;stroke-dasharray:none"
    //            d="m 357.02507,426.67697 -0.0917,-0.7335 -0.4584,-0.6418 -0.6418,-0.4584 -0.8252,-0.1834 -0.8251,0.1834 -0.6418,0.4584 -0.3667,0.6418 -0.1834,0.7335 0.1834,0.8252 0.3667,0.6418 0.6418,0.4584 0.8251,0.1833 0.8252,-0.1833 0.6418,-0.4584 0.4584,-0.6418 0.0917,-0.8252 z" />
    //         <path
    //            id="path4642"
    //            */
    // }
    // catch (zen::XmlFileError &err)
    // {

    // }
    // catch (zen::XmlParsingError &err)
    // {

    // }
}
