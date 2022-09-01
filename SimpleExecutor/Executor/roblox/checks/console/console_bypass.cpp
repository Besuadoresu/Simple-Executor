#include "console_bypass.hpp"
#include "../../../module/executor_module.hpp"

void executor::roblox::checks::console::bypass(const char* console_title)
{
	if (const auto console_api_lib = GetModuleHandle("api-ms-win-core-console-l1-1-0.dll"))
	{
		if (const auto free_console = GetProcAddress(console_api_lib, "FreeConsole"))
		{
			DWORD old;

			VirtualProtect(free_console, 1, PAGE_EXECUTE_READWRITE, &old);

			*reinterpret_cast<std::uint8_t*>(free_console) = 0xC3; 

			VirtualProtect(free_console, 1, old, &old);
		}
	}

	AllocConsole();
	
	FILE* f;
	freopen_s(&f, "CONIN$", "r", stdin);
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONOUT$", "w", stderr);

	SetConsoleTitleA(console_title);

	module::global_module->console_bypassed = true;
}