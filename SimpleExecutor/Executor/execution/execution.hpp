#pragma once

#include <string>
#include <queue>
#include "../roblox/taskscheduler/taskscheduler.hpp"

namespace executor::execution
{
	extern std::queue<std::string> waiting_scripts;

	int __fastcall waiting_script_job(void* ecx, void* edx);

	void initialize_scheduler2();
}