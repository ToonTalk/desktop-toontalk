// Copyright (c) 1992-2004.  Ken Kahn, Animated Programs, All rights reserved

#if !defined(__TT_ENTITY_H)

#define __TT_ENTITY_H

#if !defined(__TT_DEFS_H)   
#include "defs.h"
#endif

class Entity {
   // new on 061102 -- for generalizing the handling of XML to all ToonTalk entities
   public:
#if TT_XML
		virtual xml_document *xml_new_document(boolean include_explanation=FALSE);
		virtual void top_level_xml(xml_document *document, xml_element *parent) { // boolean generate_toontalk_object=TRUE) {
			// changed to not return anything on 200804
//			return(NULL); // only Sprite and Background use this - otherwise something wrong
		};
		HGLOBAL xml_global_handle();
      virtual boolean handle_xml_tag(Tag tag, xml_node *node);
#endif
		virtual SpriteType entity_type() { // new on 040803
			return(NONE_GIVEN);
		};
};

#endif