#ifndef _OSM_TYPES_H_ 
#define _OSM_TYPES_H_

#include "OSM_ABR.h"

#define OSM_IS_PRINTED									0x01
#define OSM_NOT_PRINTED									0x00

#define OSM_MEMBER_WAY_TYPE							0x01
#define OSM_MEMBER_NODE_TYPE						0x02
#define OSM_MEMBER_RELATION_TYPE				0x03
#define OSM_MEMBER_UNDEFINED_TYPE				0x08

#define OSM_MEMBER_WAY_STR 							"way"
#define OSM_MEMBER_NODE_STR							"node" 
#define OSM_MEMBER_RELATION_STR					"relation" 

#define OSM_MEMBER_REF_ID_BIT						(0x80)
#define OSM_MEMBER_REF_ID_MASK					(~OSM_MEMBER_REF_ID_BIT)

#define OSM_MEMBER_WAY_REF_STRUCT				(OSM_MEMBER_WAY_TYPE 			& OSM_MEMBER_REF_ID_MASK	)
#define OSM_MEMBER_WAY_REF_ID 					(OSM_MEMBER_WAY_TYPE 			| OSM_MEMBER_REF_ID_BIT 	)
#define OSM_MEMBER_NODE_REF_STRUCT			(OSM_MEMBER_NODE_TYPE 		& OSM_MEMBER_REF_ID_MASK	)
#define OSM_MEMBER_NODE_REF_ID 					(OSM_MEMBER_NODE_TYPE			| OSM_MEMBER_REF_ID_BIT 	)
#define OSM_MEMBER_RELATION_REF_STRUCT	(OSM_MEMBER_RELATION_TYPE	& OSM_MEMBER_REF_ID_MASK	)
#define OSM_MEMBER_RELATION_REF_ID			(OSM_MEMBER_RELATION_TYPE	| OSM_MEMBER_REF_ID_BIT 	)

typedef unsigned long int osm_element_id_t; 

typedef struct{
	double minlat;
	double minlon;
	double maxlat;
	double maxlon;
} OSM_Bounds;

typedef struct{
	char *k;
	char *v;
} OSM_Tag;

typedef struct{
	osm_element_id_t 	id;
	double 						lat;
	double 						lon;
	unsigned char 		visible;
	unsigned char 		printed;
	unsigned int 			nb_tag;
	OSM_Tag *					tags;
} OSM_Node;

typedef struct{
	osm_element_id_t 	id;
	unsigned char  		visible;
	unsigned char 		printed;
	unsigned int 			nb_node;
	OSM_Node** 				nodes;
	unsigned int 			nb_tag;
	OSM_Tag* 					tags;
} OSM_Way;

typedef struct{
	void* 				ref;
	unsigned char type;
	char* 				role;
} OSM_Member;

typedef struct{
	osm_element_id_t 	id;
	unsigned char 		visible;
	unsigned char 		printed;
	unsigned int 			nb_member;
	OSM_Member* 			members;
	unsigned int 			nb_tag;
	OSM_Tag* 					tags;
} OSM_Relation;

typedef struct{
	OSM_Bounds* bounds;
	// nodes
	unsigned int 	nb_node;
	OSM_Node* 		nodes;
	ABR_Node* 		abr_node;
	// ways
	unsigned int	nb_way;
	OSM_Way* 			ways;
	ABR_Node* 		abr_way;
	// relations
	unsigned int 	nb_relation;
	OSM_Relation*	relations;
	ABR_Node* 		abr_relation;

}	OSM_Data;

extern void fprintOSM_Bounds(FILE* stream, const OSM_Bounds bounds);
extern void fprintOSM_Node(FILE* stream, const OSM_Node node);
extern void fprintOSM_Tag(FILE* stream, const OSM_Tag tag);
extern void fprintOSM_Way(FILE* stream, const OSM_Way way);
extern void fprintOSM_Data(FILE* stream, const OSM_Data data);
extern void fprintOSM_Member(FILE* stream, const OSM_Member way);
extern void fprintOSM_Relation(FILE* stream, const OSM_Relation data);


extern void printOSM_Bounds(const OSM_Bounds bounds);
extern void printOSM_Node(const OSM_Node node);
extern void printOSM_Tag(const OSM_Tag tag);
extern void printOSM_Way(const OSM_Way way);
extern void printOSM_Data(const OSM_Data data);
extern void printOSM_Member(const OSM_Member member);
extern void printOSM_Relation(const OSM_Relation relation);

#endif /* _OSM_TYPES_H_ */
