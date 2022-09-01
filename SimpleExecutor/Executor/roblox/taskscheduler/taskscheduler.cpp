#include "taskscheduler.hpp"

std::vector<executor::roblox::structures::taskscheduler::job_t*> executor::roblox::taskscheduler::get_all_jobs()
{
	const auto taskscheduler = offsets::taskscheduler::get_taskscheduler();

	auto job_start = *reinterpret_cast<std::uintptr_t**>(taskscheduler + offsets::taskscheduler::start_job_array);
	auto job_end = *reinterpret_cast<std::uintptr_t**>(taskscheduler + offsets::taskscheduler::end_job_array);

	std::vector<executor::roblox::structures::taskscheduler::job_t*> jobs;

	for (auto job = job_start; job != job_end; job += 2)
	{	
		jobs.push_back(*reinterpret_cast<executor::roblox::structures::taskscheduler::job_t**>(job));
	}

	return jobs;
}

std::optional<executor::roblox::structures::taskscheduler::job_t*> executor::roblox::taskscheduler::get_job_by_name(const std::string& job_name)
{
	for (const auto jobs = get_all_jobs(); auto job : jobs)
	{
		if (job->name == job_name)
			return job;
	}
	
	return std::nullopt;
}