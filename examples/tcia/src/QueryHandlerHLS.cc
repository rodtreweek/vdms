#include "QueryHandler.h"
#include "AthenaDemoHLS.h"
#include "opencv2/opencv.hpp"
#include <algorithm>

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>
#include "VCL.h"
#include "chrono/Chrono.h"

#include "util.h"
#include "neighbor.h"

using namespace athena;
using namespace Jarvis;

// AthenaDemoHLS demodb("/mnt/optane/tcia_pmgd_1000", AthenaDemoHLS::EXT4, "./images_hls/pngs/");
AthenaDemoHLS demodb("tools/tciaLoader/tcia_pmgd_1", AthenaDemoHLS::EXT4, "./images_hls/pngs/");

// Defined at the bottom
Json::Value handleHLSQuery(protobufs::queryMessage& query,
                           protobufs::queryMessage& res);
std::string metadataQuery(const Json::Value);

QueryHandler::QueryHandler(Jarvis::Graph *db, std::mutex *mtx)
:_pmgd_qh(db, mtx)
{
}

void QueryHandler::process_connection(comm::Connection *c)
{
    CommandHandler handler(c);

    Json::FastWriter fastWriter;
    bool failed;
    while (true) {

        failed = false;

        ChronoCpu total("totaltime");
        ChronoCpu comm("commTime");
        ChronoCpu parsing("parTime");
        ChronoCpu queryTime("querTime");
        try {
            total.tic();
            comm.tic();
            protobufs::queryMessage cmd = handler.get_command();
            protobufs::queryMessage response;
            comm.tac();
            // TODO: Need a shutdown command for the thread

            // std::cout << cmd.json() << std::endl;
            // std::cout << "Blob size: " << cmd.blobs().size()/1024 << std::endl;

            queryTime.tic();
            Json::Value json_res = handleHLSQuery(cmd, response);
            queryTime.tac();
            // std::cout << str_res << std::endl;

            total.tac();
            Json::Value timing;
            timing["totalTime[ms]"] = total.getTotalTime_ms();
            timing["queryTime[ms]"] = queryTime.getTotalTime_ms();
            timing["commuTime[ms]"] = comm.getTotalTime_ms();
            json_res.append(timing);

            parsing.tic();
            response.set_json(fastWriter.write(json_res));
            parsing.tac();

            comm.tic();
            handler.send_response(response);
            comm.tac();

        } catch (comm::ExceptionComm e) {
            print_exception(e);
            break;
        } catch (VCL::Exception e) {
            print_exception(e);
            failed = true;
        } catch (Jarvis::Exception e) {
            print_exception(e);
            failed = true;
        }

        if (failed) {
            protobufs::queryMessage response;
            Json::Value error;
            error["return"] = "failed!";
            response.set_json(fastWriter.write(error));
            handler.send_response(response);
        }

        // total.printTotalTime_ms();
        // comm.printTotalTime_ms();
        // queryTime.printTotalTime_ms();
        // parsing.printTotalTime_ms();
    }
}

void run_operations(VCL::Image& vclimg, const Json::Value& op)
{
    std::string type = op["type"].asString();

    if (type == "threshold"){
        vclimg.threshold(op["value"].asInt());
    }
    if (type == "resize"){
        vclimg.resize(op["height"].asInt(),
                      op["width" ].asInt());
    }
    if (type == "crop"){
        vclimg.crop(VCL::Rectangle (
                        op["x"].asInt(),
                        op["y"].asInt(),
                        op["height"].asInt(),
                        op["width" ].asInt()));
    }
}

void cmd_addImage(const Json::Value& aImg, Json::Value& response,
                  const std::string& blob)
{
    std::cout << "DO NOT!" << std::endl;
    return;
    Jarvis::Transaction tx(*demodb.db(), Jarvis::Transaction::ReadWrite);

    Json::Value res;

    ChronoCpu ch("addImage");
    ch.tic();

    Jarvis::Node &im_node = demodb.db()->add_node("AT:IMAGE");

    static uint32_t name_count = 0;
    std::string im_name;

    if (aImg.isMember("name")) {
        im_name = aImg["name"].asString();
        im_node.set_property("name", aImg["name"].asCString() );
    }
    else{
        im_name = "no_name_" + std::to_string(name_count);
        ++name_count;
    }

    if (aImg.isMember("category")) {
        // will get the JL node for that category.
        // here we check if the category is defined,
        // if not, we create a new category.
        // std::cout << aImg["category"]["name"].asString()
        //           << std::endl;

        std::string cat = aImg["category"]["name"].asString();

        Jarvis::NodeIterator i = demodb.db()->get_nodes(cat.c_str());

        if (i) {
            // ChronoCpu addEdge("addEdge");
            // addEdge.tic();
            Jarvis::Edge &e = demodb.db()->add_edge(im_node, *i,
                            "AT:IMAGE_EDGE");
            // addEdge.tac();
            // addEdge.printLastTime_us();
        }
        else {
            // ChronoCpu addNode("addNode");
            // addNode.tic();
            Jarvis::Node &cat_node = demodb.db()->add_node(cat.c_str());
            // addNode.tac();
            // addNode.printLastTime_ms();

            std::cout <<  " { \"warning\": \"No such category, adding\"}\n";
            Jarvis::Edge &e = demodb.db()->add_edge(im_node, cat_node,
                            "AT:IMAGE_EDGE");
        }
    }

    if (aImg.isMember("link")) {
        Json::Value link = aImg["link"];
        // will get the JL node for that the entity,
        // which can be a generic entity.
        // For the HLS case, this entity can be a Patient.
        // Some unique ID must be given in that case.

        std::string entity   = link["entity"].asString();
        std::string prop_id  = link["prop_id"].asString();
        std::string prop_val = link["prop_value"].asString();

        // Here we need a swtich based on the type? absolutely horrible

        Jarvis::PropertyPredicate pps1(prop_id.c_str(),
                                    Jarvis::PropertyPredicate::Eq,
                                    prop_val);

        Jarvis::NodeIterator i = demodb.db()->get_nodes(entity.c_str(),
                                                       pps1);

        if (i) {
            Jarvis::Edge &e = demodb.db()->add_edge(im_node, *i, "AT:IMAGE_EDGE");
        }
        else {
            Json::Value error;
            error["addImage"] = "error: No such entity";
            std::cout << "No such entity: " << prop_val << std::endl;
            response.append(error);
            return;
        }
    }

    ChronoCpu ch_ops("addImage");
    ch_ops.tic();

    // VCL::Image vclimg((void*)blob.data(), blob.size());

    // if (aImg.isMember("operation")) {
    //     run_operations(vclimg, aImg["operation"]);
    // }

    ChronoCpu write_time("write_time");

    std::replace( im_name.begin(), im_name.end(), '.', '-');
    std::string img_root;
    VCL::ImageFormat vcl_format;

    if (aImg.isMember("format")) {
        std::string format = aImg["format"].asString();

        if (format == "png") {
            vcl_format = VCL::PNG;
            // img_root = "/mnt/optane/png/";
            img_root = "/mnt/ssd_400/png/";
            im_name += ".png";
        }
        else if (format == "tdb") {
            vcl_format = VCL::TDB;
            img_root = "/ssd_400/imagestdb/hls/";
            im_name += ".tdb";
        }
        else {
            std::cout << "Format Not Implemented" << std::endl;
            Json::Value error;
            error["Format"] = format + " Not implemented";
            response.append(error);
            return;
        }
    }
    else { // TileDB by default
        vcl_format = VCL::TDB;
        img_root = "/ssd_400/imagestdb/hls/";
        im_name += ".tdb";
    }

    // write_time.tic();
    // vclimg.store(img_root + im_name , vcl_format);
    // write_time.tac();

    ch_ops.tac();

    im_node.set_property("imgPath", img_root + im_name);
    std::cout << "imgPath: " << img_root + im_name << std::endl;

    tx.commit();
    ch.tac();

    Json::Value addImage;
    addImage["return"] = "success :)";
    addImage["name"] = aImg["name"].asString();

    if (aImg.isMember("timing")) {
        Json::Value timing;
        timing["addImage[us]"]  = ch.getAvgTime_us();
        timing["imageTotal[%]"] = ch_ops.getAvgTime_us()*100 /
                                  ch.getAvgTime_us();
        timing["imageTotal[us]"] = ch_ops.getAvgTime_us();
        timing["write_image[us]"] = write_time.getAvgTime_us();

        addImage["timing"] = timing;
    }

    res["addImage"] = addImage;
    response.append(res);
}

Jarvis::NodeIterator get_img_iterator(const Json::Value& qImg, Json::Value& response)
{
    if (qImg.isMember("name")) {

        std::string im_name = qImg["name"].asString();

        Jarvis::PropertyPredicate pps1("name",
                                    Jarvis::PropertyPredicate::Eq,
                                    im_name);

        return demodb.db()->get_nodes("AT:IMAGE", pps1);
    }
    else if (qImg.isMember("link")) {
        Json::Value link = qImg["link"];
        // will get the JL node for that the entity,
        // which can be a generic entity.
        // For the HLS case, this entity can be a Patient.
        // Some unique ID must be given in that case.

        std::string entity   = link["entity"].asString();
        std::string prop_id  = link["prop_id"].asString();
        std::string prop_val = link["prop_value"].asString();

        // Here we need a swtich based on the type? absolutely horrible

        Jarvis::PropertyPredicate pps1(prop_id.c_str(),
                                    Jarvis::PropertyPredicate::Eq,
                                    prop_val);

        Jarvis::NodeIterator i = demodb.db()->get_nodes(entity.c_str(),
                                                       pps1);

        return get_neighbors(*i, Jarvis::Any);
    }
}

void cmd_FindImage(const Json::Value& qImg, Json::Value& response,
                    protobufs::queryMessage& proto_res)
{
    Json::Value res;
    Jarvis::Transaction tx(*demodb.db(), Jarvis::Transaction::ReadWrite);

    int counter = 0; // Max Returned images
    int max_counter = 1000; // Max Returned images

    if (qImg.isMember("count")) {
        max_counter =  qImg["count"].asInt();
    }

    static ChronoCpu jarvisQuery("jarvisQuery");
    ChronoCpu copyProto("copyProto");
    ChronoCpu readOperate("read&Operate");

    jarvisQuery.tic();
    Jarvis::NodeIterator it = get_img_iterator(qImg, response);
    jarvisQuery.tac();

    if (!it) {
        Json::Value error;
        error["FindImage"] = "Not found";
        response.append(error);
        return;
    }

    for (; it && counter < max_counter; it.next(), counter++) {
        // We only load images, should be carefull with this
        if (it->get_tag().name() != "AT:IMAGE"){
            continue;
        }
        // I am seeing a single imaged per patient, which makes no sense
        std::string im_path =
                    it->get_property("imgPath").string_value();

        // size_t pos = im_path.rfind('/');
        // im_path = im_path.substr(pos+1, im_path.length());
        // im_path = "/mnt/ssd_400/tdb_database_240/hls/" + im_path;
        // im_path += ".tdb";
        // std::cout << im_path << std::endl;
        readOperate.tic();

        std::vector<unsigned char> img_enc;
        // try {
        //     VCL::Image vclimg(im_path);

        //     if (qImg.isMember("operation")) {
        //         run_operations(vclimg, qImg["operation"]);
        //     }

        //     img_enc = vclimg.get_encoded_image(VCL::PNG);
        // } catch (VCL::Exception e) {
        //     print_exception(e);
        // }

        // std::vector<char> img_enc;
        // std::ifstream file(im_path,
        //         std::ios::in | std::ios::binary | std::ios::ate);
        // img_enc.resize(file.tellg());
        // file.seekg(0, std::ios::beg);
        // if( !file.read(&img_enc[ 0 ], img_enc.size()))
        //     std::cout << "error" << std::endl;

        readOperate.tac();

        if (!img_enc.empty()) {
            copyProto.tic();
            std::string img((const char*)img_enc.data(), img_enc.size());
            proto_res.add_blobs(img);
            copyProto.tac();
        }

        Json::Value success;
        success["FindImage"] = "Success!";
        Json::Value image;
        image["name"] = im_path;
        success["metadata"] = image;
        response.append(success);
    }
    // printf("%d\n", counter);

    tx.commit();

    if (qImg.isMember("timing")) {
        Json::Value timing;
        timing["jarvisQuery[us]"] = jarvisQuery.getLastTime_us();
        std::cout << jarvisQuery.getLastTime_us() << ",";
        std::cout << jarvisQuery.getAvgTime_us() << ",";
        // timing["jarvisQuerystd[us]"] = jarvisQuery.getSTD_us();
        std::cout << jarvisQuery.getSTD_us() << "\n";
        timing["copyProto[us]"]   = copyProto.getTotalTime_us();
        timing["readOperate[us]"] = readOperate.getTotalTime_us();
        response.append(timing);
    }
}

Json::Value handleHLSQuery(protobufs::queryMessage& query,
                           protobufs::queryMessage& proto_res)
{
    Json::Value response;
    Json::Value root;

    Json::Reader reader;
    bool parsingSuccessful = reader.parse( query.json().c_str(), root );

    if ( !parsingSuccessful ) {
        std::cout << "Error parsing!" << std::endl;
        Json::Value error;
        error["return"] = "Server error - parsing";
        response.append(error);
        return response;
    }

    unsigned blob_count = 0;

    // std::cout << "Blobls: " << query.blobs_size() << std::endl;

    for (const Json::Value& element : root){
        for(Json::Value::ArrayIndex i=0; i < element.size(); ++i) {

            std::string cmd = element.getMemberNames()[i];

            if (cmd == "hlsQuery") {
                Json::Value res;
                // res["return"] = "hlsQuery not supported";
                // response.append(res);
                res["result"] = metadataQuery(element["hlsQuery"]);
                response.append(res);

            }
            else if (cmd == "FindImage") {
                cmd_FindImage(element["FindImage"], response, proto_res);
            }
            else if (cmd == "addImage") {
                if(query.blobs_size() <= blob_count)
                    printf("this cannot happen, wtf\n");
                cmd_addImage(element["addImage"], response,
                             query.blobs(blob_count));
                ++blob_count;
            }

            else {
                std::cout << "Undefined command" << std::endl;
                Json::Value error;
                error["Undefined"] = "Undefined Command";
                response.append(error);
            }
        }
    }

    return response;
}

std::string metadataQuery(const Json::Value root)
{
    const Json::Value params = root["params"];

    switch (root.get("queryId",99).asInt()) {
        case 1: {
            if (params.size() != 1){
                return "{ \
                        \"error\" = \
                        \"Specify treatmentType \
                        [drug | radiation]\" }";
            }
            std::cout << std::endl << "Running Query 1..." << std::endl;
            return demodb.runQuery1(params["treatmentType"].asString());
            break;
        }
        case 2: {
            if (params.size() != 2){
                return "{ \
                        \"error\" = \"Specify ageMin and ageMin\" }";
            }
            std::cout << std::endl << "Running Query 2..." << std::endl;
            return demodb.runQuery2(params.get("ageMin",0).asInt(),
                                   params.get("ageMax",0).asInt());
            break;
        }
        case 3: {
            std::string drugName = params.get("drugName","x").asString();
            if (drugName == "x"){
                return "{ \
                        \"error\" = \"Specify drugName\" }";
            }
            std::cout << std::endl << "Running Query 3..." << std::endl;
            return demodb.runQuery3(drugName);
            break;
        }

        default: {
            return "UNKINDLY CHECK THE QUERY ID";
        }
    }
}
