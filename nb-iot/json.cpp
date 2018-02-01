/*
 * JSON utility functions
 * Copyright (c) 2018 IOTA Foundation
 */

#if __linux
#include <json/json.h>
#endif

#include "json.h"

bool JsonObj::getAttr(const char *name, jsonAttr &attr) {
#if __linux
	json_object *jobj = (json_object *)priv;
	json_object *attrObj = json_object_object_get(jobj, name);

	if (!attrObj) {
		return false;
	}
	switch (json_object_get_type(attrObj)) {
	case json_type_string:
		attr.type = jsonTypeString;
		attr.val.str = json_object_get_string(attrObj);
		break;
	default:
		return false;
	}
	return true;
#else
	return false;
#endif
}

JsonObj jsonParse(char *buf)
{
	JsonObj jsonObj;

#if __linux
	json_object *jobj = json_tokener_parse(buf);
	if (jobj) {
		jsonObj.valid = true;
		jsonObj.priv = jobj;
	}
#endif
	return jsonObj;
}
