#pragma once
#include <string>

namespace executor::roblox::structures
{
	namespace taskscheduler
	{
		struct job_t
		{
			std::uintptr_t* functions;
			std::uint8_t pad0[12];
			std::string name;
			//std::uint8_t pad1[16];
			//double time;
			//std::uint8_t pad2[16];
			//double time_spend;
			//std::uint8_t pad3[8];
			//std::uintptr_t state;
		};
	}
}