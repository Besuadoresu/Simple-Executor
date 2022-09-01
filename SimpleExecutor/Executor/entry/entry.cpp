#include "entry.hpp"

bool __stdcall DllMain(HINSTANCE, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		std::thread(executor::executor_main).detach();
	}

	return true;
}