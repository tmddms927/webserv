//
// Created by Giyoung Lee on 5/12/22.
//

#include "AutoIndex.hpp"
#include <dirent.h>

AutoIndex::AutoIndex(const std::vector<servers> &config) : _config(config){}

void AutoIndex::makeHTML() {
    std::string     body;
    struct dirent   *ent;
    DIR             *dir;

    dir = opendir ("./");
    if (dir != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            printf ("%s\n", ent->d_name);
        }
        closedir (dir);
    } else {
        perror ("");
    }
    body = "<pre>\n";
    body += "</pre>";
    /*
     * <a href="+ + "></a>
     */
}