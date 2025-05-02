#pragma once

#include "Locale_inc.h"

const char* LocaleService_GetName();
const char*	LocaleService_GetLocaleName();
const char*	LocaleService_GetLocalePath();
const char*	LocaleService_GetSecurityKey();
BOOL		LocaleService_IsLeadByte( const char chByte );
int			LocaleService_StringCompareCI( LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength );

unsigned	LocaleService_GetLastExp(int level);
int			LocaleService_GetSkillPower(unsigned level);
