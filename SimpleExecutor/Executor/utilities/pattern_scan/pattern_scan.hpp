#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

#include "../structures/structures.hpp"

namespace executor::utilities::pattern_scan
{
	std::vector<std::uintptr_t> scan(const std::string_view& pattern, const std::string_view& mask);
}