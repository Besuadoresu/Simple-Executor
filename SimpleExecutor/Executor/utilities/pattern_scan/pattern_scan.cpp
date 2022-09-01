#pragma once
#include "pattern_scan.hpp"

std::vector<std::uintptr_t> executor::utilities::pattern_scan::scan(const std::string_view& pattern, const std::string_view& mask)
{
	std::vector<std::uintptr_t> results;

	utilities::structures::segment text{ ".text" };

	for (auto at = text.start_addr; at < text.end_addr; ++at)
	{
		const auto is_same = [&]() -> bool
		{
			for (auto i = 0u; i < mask.length(); ++i)
			{
				if (*reinterpret_cast<std::uint8_t*>(at + i) != static_cast<std::uint8_t>(pattern[i]) && mask[i] == 'x')
				{
					return false;
				}
			}

			return true;
		};

		if (is_same()) 
			results.push_back(at); 
	}

	return results;
}