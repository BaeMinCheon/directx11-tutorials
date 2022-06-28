
#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE CurrentInstanceHandle, HINSTANCE PreviousInstanceHandle, PSTR CommandLine, int ShowOption)
{
	int Result = 1;

	auto System = new SystemClass;
	if (System)
	{
		if (System->Initialize())
		{
			System->Run();
			System->Shutdown();
			Result = 0;
		}

		delete System;
		System = nullptr;
	}

	return Result;
}