//
// Created by Giyoung Lee on 5/12/22.
//

#include "AutoIndex.hpp"
#include <dirent.h>

AutoIndex::AutoIndex(const std::string &config) : _config(config), _err(false){}

bool & AutoIndex::makeHTML() {
    std::string     body;
    struct dirent   *ent;
    DIR             *dir;

    body = "<pre>\n";
    dir = opendir(_config.c_str());
    if (dir != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (std::string(ent->d_name) == "." || std::string(ent->d_name) == "..")
                continue;
            body += ent->d_name;
            body += "\n";
        }
        closedir (dir);
    } else {
        _err = true;
    }
    body += "</pre>";
    _res.body = body;
    return _err;
}

const ResponseMessage &AutoIndex::getRes() const {
    return _res;
}
