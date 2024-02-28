#include "logger.h"


void PrintVector3(Vector3 v)
{
	char *var = GET_VARIABLE_NAME(v);
	TraceLog(LOG_INFO, "Vector3 %s, {x:%f, y:%f, z:%f}", var, v.x, v.y, v.z);
}

void PrintVector2(Vector2 v)
{
	char *var = GET_VARIABLE_NAME(v);
	TraceLog(LOG_INFO, "Vector2 %s, {x:%f, y:%f}", var, v.x, v.y);
}