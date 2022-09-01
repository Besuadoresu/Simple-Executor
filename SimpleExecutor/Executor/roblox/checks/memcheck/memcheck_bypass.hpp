#pragma once
#include <Windows.h>
#include <iostream>
#include "../../../utilities/utilities.hpp"

namespace executor::roblox::checks::memcheck
{
	using silent_checker_t = std::uint32_t(*)(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
	static silent_checker_t silent_checker;

	static std::uintptr_t base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));

	std::uint32_t spoof(std::uint32_t a1);
	std::uint32_t silent_checker_hook(std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4);
	std::uintptr_t __cdecl get_spoofed_address(std::uintptr_t address);

	void bypass();

	int __fastcall memcheck_job_hook(void* ecx, void* edx, int rando);

	static utilities::hooks::vftable::vftable_replace<5>* hook; // 5th function calls step function
}