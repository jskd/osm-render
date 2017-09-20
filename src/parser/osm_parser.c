#include "osm_parser.h"
#include "osm_bind.h"
#include "../model/osm_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

parser_error_t open_OSM_ParserFile(const char* filename, osmParserFilePtr* fileOut)
{
	osmParserFilePtr	file	= NULL;
	xmlDocPtr					doc		= NULL;

	assert(filename);
	assert(fileOut);

	// Load XML document
	doc = xmlParseFile(filename);
	if (doc == NULL) 
		return(PARSER_ERROR_PARSE_FILE);

	// Save osmParserFile
	file= (osmParserFilePtr) malloc(sizeof(struct osmParserFile));
	file->doc= doc;
	*fileOut = file;

	return(PARSER_SUCESS);
}

void close_OSM_ParserFile(osmParserFilePtr file)
{
	xmlFreeDoc(file->doc); 
	free(file);
}

void freeDataSet(osmParserDataSetPtr data)
{
	assert(data);

	if(data->xpathObj != NULL) 
		xmlXPathFreeObject(data->xpathObj);

	xmlXPathFreeContext(data->xpathCtx);

	free(data);
}


parser_error_t execute_xpath(osmParserFilePtr file, const xmlChar* xpathExpr, osmParserDataSetPtr* dataOut)
{
	osmParserDataSetPtr	data	= NULL;
	data= (osmParserDataSetPtr) malloc(sizeof(struct osmParserDataSet));

	// Create xpath evaluation context
	data->xpathCtx = xmlXPathNewContext(file->doc);
	if(data->xpathCtx == NULL) 
		return(PARSER_ERROR_XPATH_CONTEXT);

	// Execute xpath evaluation
	data->xpathObj = xmlXPathEvalExpression(xpathExpr, data->xpathCtx);
	if(data->xpathObj == NULL) 
		return(PARSER_ERROR_EVALUATE_XPATH);

	*dataOut = data;
	return PARSER_SUCESS;
}

int countChildrenElementByName(xmlNodePtr node, const char* name)
{
	xmlNodePtr currentNode= NULL;
	int count = 0;
	for(currentNode = node->children; currentNode; currentNode=currentNode->next)
		if(currentNode->type == XML_ELEMENT_NODE) 
			if(xmlStrEqual(currentNode->name, xmlCharStrdup(name)))
				count++;
	return count;
}

int countChildrenTag(xmlNodePtr node)
{
	return countChildrenElementByName(node, "tag");
}

int countChildrenNode(xmlNodePtr node)
{
	return countChildrenElementByName(node, "nd");
}

int countChildrenMember(xmlNodePtr node)
{
	return countChildrenElementByName(node, "member");
}

xmlNodePtr getXmlNodeByIndex( osmParserDataSetPtr osmDataSet, int index)
{
	return osmDataSet->xpathObj->nodesetval->nodeTab[index];
}

parser_error_t getOSM_Bounds(osmParserFilePtr file, osmParserDataSetPtr* dataOut)
{
  parser_error_t error= execute_xpath( file, (xmlChar*)"/osm/bounds",  dataOut);
	if(error != PARSER_SUCESS)
		return error;
	return(PARSER_SUCESS);
}

parser_error_t getOSM_Node(osmParserFilePtr file, osmParserDataSetPtr* dataOut)
{
  parser_error_t error= execute_xpath( file, (xmlChar*)"/osm/node",  dataOut);
	if(error != PARSER_SUCESS)
		return error;
	return(PARSER_SUCESS);
}

parser_error_t getOSM_Way(osmParserFilePtr file, osmParserDataSetPtr* dataOut)
{
  parser_error_t error= execute_xpath( file, (xmlChar*)"/osm/way",  dataOut);
	if(error != PARSER_SUCESS)
		return error;
	return(PARSER_SUCESS);
}

parser_error_t getOSM_Relation(osmParserFilePtr file, osmParserDataSetPtr* dataOut)
{
  parser_error_t error= execute_xpath( file, (xmlChar*)"/osm/relation",  dataOut);
	if(error != PARSER_SUCESS)
		return error;
	return(PARSER_SUCESS);
}

int getDataSet_lenght(osmParserDataSetPtr data)
{
	xmlNodeSetPtr node_set=  data->xpathObj->nodesetval;
	return (node_set) ? node_set->nodeNr : 0;
}

parser_error_t getOSM_data(const char* filename, OSM_Data** dataOut)
{
	osmParserFile* 		osmFile;
	osmParserDataSet* osmDataSet;
	parser_error_t 		error;

	OSM_Data* data= (OSM_Data*) malloc(sizeof(OSM_Data));
	data->abr_node = NULL;
	data->abr_way = NULL;
	data->abr_relation = NULL;

	error= open_OSM_ParserFile(filename, &osmFile);
	if(error != PARSER_SUCESS)
		return error;

	// === Bounds ===
	error= getOSM_Bounds(osmFile, &osmDataSet);
	if(error != PARSER_SUCESS)
		return error;
	data->bounds = malloc( sizeof(OSM_Bounds));
	*data->bounds = bind_OSM_Bounds( getXmlNodeByIndex(osmDataSet, 0));
	freeDataSet(osmDataSet);

	// === Nodes ===
	error= getOSM_Node(osmFile, &osmDataSet);
	if(error != PARSER_SUCESS)
		return error;
	data->nb_node = getDataSet_lenght(osmDataSet);
	data->nodes = getNodeList(osmDataSet);
	freeDataSet(osmDataSet);

	// === Nodes ABR === 
	for(int i= 0; i < data->nb_node; i++){
		addNode(&data->abr_node, data->nodes[i].id, &data->nodes[i]);
	}

	// === Ways ===
	error= getOSM_Way(osmFile, &osmDataSet);
	if(error != PARSER_SUCESS)
		return error;
	data->nb_way = getDataSet_lenght(osmDataSet);
	data->ways = getWayList(data, osmDataSet);
	freeDataSet(osmDataSet);

	// === Ways ABR === 
	for(int i= 0; i < data->nb_way; i++){
		addNode(&data->abr_way, data->ways[i].id, &data->ways[i]);
	}

	// === Relation ===
	error= getOSM_Relation(osmFile, &osmDataSet);
	if(error != PARSER_SUCESS)
		return error;
	data->nb_relation = getDataSet_lenght(osmDataSet);
	data->relations = getRelationList(data, osmDataSet);
	freeDataSet(osmDataSet);

	// === Relation ABR === 
	for(int i= 0; i < data->nb_relation; i++){
		addNode(&data->abr_relation, data->relations[i].id, &data->relations[i]);
	}

	linkRelationMembers(data);

	*dataOut = data;

	close_OSM_ParserFile(osmFile);
	return(PARSER_SUCESS);
}

void freeOSM_data(OSM_Data* data)
{
	free(data->bounds);

	for(int i=0; i< data->nb_node; i++)
		free(data->nodes[i].tags);
	free(data->nodes);

	for(int i=0; i< data->nb_way; i++)
	{
		free(data->ways[i].tags);
		free(data->ways[i].nodes);
	}
	free(data->ways);

	clearTree(data->abr_node);
	clearTree(data->abr_way);

	free(data);
}


