// Copyright 2014 Joseph Riedel, All Rights Reserved.
// See LICENSE for licensing terms.

#pragma once

#include "../../../CefRuntime/API/CEFJavaScriptAPI.hpp"

class FJavaScriptHelper
{
public:

	static void ExecuteHook(UObject* Receiver, const FString& HookName, ICefRuntimeVariantList* Arguments);
	static ICefRuntimeVariantList* CreateVariantList(UStruct* Class, void *Container, ICefRuntimeVariantFactory* VariantFactory);

};