#ifndef _OSM_BIND_H_ 
#define _OSM_BIND_H_

#include "../model/osm_types.h"

OSM_Bounds		bind_OSM_Bounds		(xmlNodePtr node);
OSM_Tag 			bind_OSM_Tag			(xmlNodePtr node);
OSM_Node 			bind_OSM_Node			(xmlNodePtr node);
OSM_Way 			bind_OSM_Way			(OSM_Data* osm_data, xmlNodePtr node);
OSM_Member		bind_OSM_Member		(OSM_Data* osm_data, xmlNodePtr node);
OSM_Relation 	bind_OSM_Relation	(OSM_Data* osm_data, xmlNodePtr node);


OSM_Node*			bind_Ref_Node			(OSM_Data* osm_data, xmlNodePtr node);


OSM_Node*			getNodeList				(osmParserDataSetPtr data);

OSM_Way*			getWayList				(OSM_Data* osm_data, osmParserDataSetPtr data);

OSM_Relation*	getRelationList		(OSM_Data* osm_data, osmParserDataSetPtr data);



OSM_Node**		getRefNodeList		(OSM_Data* osm_data, xmlNodePtr node);
OSM_Member*  	getMemberList			(OSM_Data* osm_data, xmlNodePtr node);
OSM_Tag* 			getTagList				(xmlNodePtr node);

void linkRelationMembers(OSM_Data* osm_data);

#endif /* _OSM_BIND_H_ */
