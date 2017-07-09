#include <iostream>
#include <json/json.h>
#include <json/writer.h>
#include <fstream>

int main () {
    std::ifstream in;
    in.open("./purejson", std::ios_base::in);
    if (!in.is_open()) {
        std::cerr << "open purejson error." << std::endl;
        return -1;
    }
    Json::Reader reader;
    Json::FastWriter write;
    Json::Value root;

    if (!reader.parse(in, root)) {
        std::cerr << "parse purejson error." << std::endl;
        return -1;
    }
    if (root.isMember("seatbid") && root["seatbid"].isArray()) {
        int index = 0;
        root = root["seatbid"][index];
        if (root.isMember("bid") && root["bid"].isArray()) {
            if (root["bid"].size() <= 0) {
                std::cerr << "bid array is empty." << std::endl;
                return -1;
            }
            std::cout << "bid array size:"  << root["bid"].size() << std::endl;
            Json::Value bid = root["bid"][index];
            
            if (bid.isMember("adm") && bid["adm"].isObject()) {
                std::cout << "adm:" << write.write(bid["adm"]) << std::endl; 
            } else {
                if (!bid.isMember("adm")) {
                    std::cerr << "adm is not adm member." << std::endl;
                } else {
                    std::cerr << "bid[adm] value is not object." << std::endl;
                    std::cout << "adm:" << write.write(bid["adm"]) << std::endl; 
                }
            }
        }
    } else {
        std::cerr << "can't find seatbid member or seatbid is null." << std::endl;
        return -1;
    }
}
