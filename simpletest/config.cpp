#include "config.h"
#include <iostream>

Config::Config() : m_filename("config.xml") {}

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::loadConfig() {
    std::lock_guard<std::mutex> lock(m_mutex);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError result = doc.LoadFile(m_filename.c_str());
    if (result != tinyxml2::XML_SUCCESS) {
        std::cerr << "Error loading XML file: " << doc.ErrorIDToName(result) << std::endl;
        return false;
    }

    tinyxml2::XMLElement* root = doc.RootElement();
    if (!root) {
        std::cerr << "No root element in XML file." << std::endl;
        return false;
    }

    parseThreads(root);
    return true;
}

void Config::parseThreads(tinyxml2::XMLElement* root) {
    tinyxml2::XMLElement* threadsElement = root->FirstChildElement("threads");
    if (!threadsElement) {
        std::cerr << "No Threads element in XML file." << std::endl;
        return;
    }

    m_threads.clear();

    for (tinyxml2::XMLElement* thread = threadsElement->FirstChildElement(); thread != nullptr; thread = thread->NextSiblingElement()) {
        int priority = thread->IntAttribute("priority");
        int target = thread->IntAttribute("target");

        m_threads.emplace_back(ThreadConfig{priority, target});
    }
}

const std::vector<ThreadConfig>& Config::getThreads() const {
    return m_threads;
}
