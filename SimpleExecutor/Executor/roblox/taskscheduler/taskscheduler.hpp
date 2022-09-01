#pragma once
#include "vector"
#include "string"
#include <optional>
#include "../structures/structures.hpp"
#include "../offsets/offsets.hpp"

namespace executor::roblox::taskscheduler
{
	std::vector<structures::taskscheduler::job_t*> get_all_jobs();
	std::optional<structures::taskscheduler::job_t*> get_job_by_name(const std::string& job_name);
}