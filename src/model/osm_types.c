#include <stdio.h>
#include "osm_types.h"

void fprintOSM_Bounds(FILE* stream, const OSM_Bounds bounds){
	fprintf(stream,"OSM_Bounds:\n");
	fprintf(stream,"\tminlat: %f\n", bounds.minlat);
	fprintf(stream,"\tminlon: %f\n", bounds.minlon);
	fprintf(stream,"\tmaxlat: %f\n", bounds.maxlat);
	fprintf(stream,"\tmaxlon: %f\n", bounds.maxlon);
}

void fprintOSM_Node(FILE* stream, const OSM_Node node){
	fprintf(stream, "OSM_Node:\n");
	fprintf(stream, "\tid: %lu\n", node.id);
	fprintf(stream, "\tlat: %f\n", node.lat);
	fprintf(stream, "\tlon: %f\n", node.lon);
	fprintf(stream, "\tvisible: %d\n", node.visible);
	fprintf(stream, "\tnb_tag: %d\n", node.nb_tag);
}

void fprintOSM_Tag(FILE* stream, const OSM_Tag tag){
	fprintf(stream, "OSM_Tag:\n");
	fprintf(stream, "\tk: %s\n", tag.k);
	fprintf(stream, "\tv: %s\n", tag.v);
}

void fprintOSM_Way(FILE* stream, const OSM_Way way){
	fprintf(stream, "OSM_Way:\n");
	fprintf(stream, "\tid: %lu\n", way.id);
	fprintf(stream, "\tvisible: %d\n", way.visible);
	fprintf(stream, "\tnb_tag: %d\n", way.nb_tag);
	fprintf(stream, "\tnb_node: %d\n", way.nb_node);
}

void fprintOSM_Member(FILE* stream, const OSM_Member member){
	fprintf(stream, "OSM_Member:\n");
	switch(member.type & OSM_MEMBER_REF_ID_MASK)
	{
		case OSM_MEMBER_WAY_TYPE:
			fprintf(stream, "\ttype: way\n");
		break;
		case OSM_MEMBER_NODE_TYPE:
			fprintf(stream, "\ttype: node\n");
		break;
		case OSM_MEMBER_RELATION_TYPE:
			fprintf(stream, "\ttype: relation\n");
		break;
		default:
			fprintf(stream, "\ttype: undefined\n");
		break;
	}
	if(member.type & OSM_MEMBER_REF_ID_BIT)
			fprintf(stream, "\tref: id reference\n");
	else
			fprintf(stream, "\tref: struct reference\n");
	fprintf(stream, "\trole: %s\n", member.role );
}

void fprintOSM_Relation(FILE* stream, const OSM_Relation relation){
	fprintf(stream, "OSM_Relation:\n");
	fprintf(stream, "\tid: %lu\n", relation.id);
	fprintf(stream, "\tvisible: %d\n", relation.visible);
	fprintf(stream, "\tnb_member: %d\n", relation.nb_member);
	fprintf(stream, "\tnb_tag: %d\n", relation.nb_tag);
}


void fprintOSM_Data(FILE* stream, const OSM_Data data){
	fprintf(stream, "OSM_Data:\n");
	fprintf(stream, "\tnb_node: %d\n", data.nb_node);
	fprintf(stream, "\tnb_way: %d\n", data.nb_way);
}

void printOSM_Bounds(const OSM_Bounds bounds){
	fprintOSM_Bounds(stdout, bounds);
}

void printOSM_Node(const OSM_Node node){
	fprintOSM_Node(stdout, node);
}

void printOSM_Tag(const OSM_Tag tag){
	fprintOSM_Tag(stdout, tag);
}

void printOSM_Way(const OSM_Way way){
	fprintOSM_Way(stdout, way);
}

void printOSM_Member(const OSM_Member member) {
	fprintOSM_Member(stdout, member);
}


void printOSM_Relation(const OSM_Relation relation) {
	fprintOSM_Relation(stdout, relation);
}

void printOSM_Data(const OSM_Data data){
	fprintOSM_Data(stdout, data);
}
