
#include <fcntl.h>

#include "osmium.hpp"
#include "XMLParser.hpp"
#include "PBFParser.hpp"

/**
*
*  Parse OSM file and call callback functions.
*  This works for OSM XML files (suffix .osm) and OSM binary files (suffix .pbf).
*  Reads from STDIN if the filename is '-', in this case it assumes XML format.
*
*/
void parse_osmfile(char *osmfilename, struct callbacks *callbacks, Osmium::OSM::Node *node, Osmium::OSM::Way *way, Osmium::OSM::Relation *relation) {
    int fd = 0;
    if (osmfilename[0] == '-' && osmfilename[1] == '\0') {
        // fd is already 0, read STDIN
    } else {
        fd = open(osmfilename, O_RDONLY);
        if (fd < 0) {
            std::cerr << "Can't open osm file: " << strerror(errno) << '\n';
            exit(1);
        }
    }

    osm_file_format_t file_format;
    char *suffix = strrchr(osmfilename, '.');

    if (suffix == NULL) {
        file_format = xml;
    } else {
        if (!strcmp(suffix, ".osm")) {
            file_format = xml;
        } else if (!strcmp(suffix, ".pbf")) {
            file_format = pbf;
        } else {
            std::cerr << "Unknown file suffix: " << suffix << "\n";
            exit(1);
        }
    }

    if (callbacks->init) { callbacks->init(); }
    switch (file_format) {
        case xml:
            Osmium::XMLParser::parse(fd, callbacks, node, way, relation);
            break;
        case pbf:
            Osmium::PBFParser *pbf_parser = new Osmium::PBFParser(fd, callbacks);
            pbf_parser->parse(node, way, relation);
            break;
    }
    if (callbacks->final) { callbacks->final(); }

    close(fd);
}