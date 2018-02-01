/*
 * JSON utility functions
 * Copyright (c) 2018 IOTA Foundation
 */

#ifndef JSON_H_
#define JSON_H_

#include <stdlib.h>

enum jsonType {
	jsonTypeString,
};

struct jsonAttr {
	enum jsonType type;
	union {
		const char *str;
	} val;
};

class JsonObj {
public:
	JsonObj() : valid(false), priv(NULL) {}
	bool valid;
	bool getAttr(const char *name, jsonAttr &attr);
private:
	void *priv;

	friend JsonObj jsonParse(char *buf);
};

JsonObj jsonParse(char *buf);

#endif /* JSON_H_ */
