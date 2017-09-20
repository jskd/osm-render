#ifndef _OSM_PARSER_H_ 
#define _OSM_PARSER_H_

#include "error.h"
#include "../model/osm_types.h"


#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

struct osmParserFile {
	xmlDocPtr	doc;
};

struct osmParserDataSet {
	xmlXPathContextPtr	xpathCtx; 
	xmlXPathObjectPtr		xpathObj;
	xmlNodePtr					current_node;
};

typedef struct osmParserFile osmParserFile;
typedef osmParserFile* osmParserFilePtr;

typedef struct osmParserDataSet osmParserDataSet;
typedef osmParserDataSet* osmParserDataSetPtr;



parser_error_t open_OSM_ParserFile(const char* filename, osmParserFilePtr* fileOut);
void close_OSM_ParserFile(osmParserFilePtr file);

void freeDataSet(osmParserDataSetPtr data);
parser_error_t execute_xpath(osmParserFilePtr file, const xmlChar* xpathExpr, osmParserDataSetPtr* dataOut);

int countChildrenElementByName(xmlNodePtr node, const char* name);

int countChildrenTag(xmlNodePtr node);

int countChildrenNode(xmlNodePtr node);

int countChildrenMember(xmlNodePtr node);

xmlNodePtr getXmlNodeByIndex( osmParserDataSetPtr osmDataSet, int index);

parser_error_t getOSM_Bounds(osmParserFilePtr file, osmParserDataSetPtr* dataOut);

parser_error_t getOSM_Node(osmParserFilePtr file, osmParserDataSetPtr* dataOut);

parser_error_t getOSM_Way(osmParserFilePtr file, osmParserDataSetPtr* dataOut);


parser_error_t getOSM_Relation(osmParserFilePtr file, osmParserDataSetPtr* dataOut);

int getDataSet_lenght(osmParserDataSetPtr data);

parser_error_t getOSM_data(const char* filename, OSM_Data** dataOut);

void freeOSM_data(OSM_Data* data);

#endif /* _OSM_PARSER_H_ */
