#pragma once
#include <Windows.h>
#include <iostream>

namespace executor::utilities::memory
{
	std::uintptr_t tramp_hook(std::uintptr_t func, std::uintptr_t new_func, std::size_t inst_size);

	std::uintptr_t clone_section(std::uintptr_t section_start);

	std::uintptr_t calculate_function_size(std::uintptr_t originaladdress);

	std::uintptr_t clone_function(std::uintptr_t original_address, std::size_t function_size = 0);

	const auto is_prologue = [](const std::uintptr_t address) -> bool
	{
		return
			not std::memcmp(reinterpret_cast<void*>(address), "\x55\x8B\xEC", 3) or
			not std::memcmp(reinterpret_cast<void*>(address), "\x53\x8B\xDC", 3) or
			not std::memcmp(reinterpret_cast<void*>(address), "\x56\x8B\xF4", 3);
	};

	const auto get_prologue = [](const std::uintptr_t function_address) -> const std::uintptr_t
	{
		auto address_copy = function_address;

		while (!is_prologue(address_copy))
		{
			address_copy--;
		}

		return address_copy;
	};
}