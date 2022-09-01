#include "memory.hpp"

std::uintptr_t executor::utilities::memory::tramp_hook(std::uintptr_t func, std::uintptr_t new_func, std::size_t inst_size)
{
	constexpr auto extra_size = 5;

	auto clone = reinterpret_cast<std::uintptr_t>(VirtualAlloc(nullptr, inst_size + extra_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));

	if (!clone)
		return 0;

	std::memmove(reinterpret_cast<void*>(clone), reinterpret_cast<void*>(func), inst_size);

	const auto jmp_pos = (func - clone - extra_size);

	*reinterpret_cast<std::uint8_t*>(clone + inst_size) = 0xE9;
	*reinterpret_cast<std::uintptr_t*>(clone + inst_size + 1) = jmp_pos;

	DWORD old_protect;

	VirtualProtect(reinterpret_cast<void*>(func), inst_size, 0x40, &old_protect);

	std::memset(reinterpret_cast<void*>(func), 0x90, inst_size);

	const auto rel_location = (new_func - func - extra_size);
	*reinterpret_cast<std::uint8_t*>(func) = 0xE9;
	*reinterpret_cast<std::uintptr_t*>(func + 1) = rel_location;

	VirtualProtect(reinterpret_cast<void*>(func), inst_size, old_protect, &old_protect);

	return clone;
}

std::uintptr_t executor::utilities::memory::clone_section(std::uintptr_t section_start)
{
	MEMORY_BASIC_INFORMATION mbi;

	VirtualQuery(reinterpret_cast<LPCVOID>(section_start), &mbi, sizeof(mbi));

	auto new_mem = VirtualAlloc(nullptr, mbi.RegionSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	std::memmove(new_mem, reinterpret_cast<void*>(section_start), mbi.RegionSize);

	return reinterpret_cast<std::uintptr_t>(new_mem);
}

std::uintptr_t executor::utilities::memory::calculate_function_size(std::uintptr_t originaladdress)
{
	auto bytes = reinterpret_cast<std::uint8_t*>(originaladdress);

	do
		bytes += 0x10;
	while (!(bytes[0] == 0x55 && bytes[1] == 0x8B && bytes[2] == 0xEC));

	return reinterpret_cast<std::uintptr_t>(bytes) - originaladdress;
}

std::uintptr_t executor::utilities::memory::clone_function(std::uintptr_t original_address, std::size_t function_size)
{
	if (!function_size)
		function_size = calculate_function_size(original_address);

	auto clone = VirtualAlloc(nullptr, function_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	std::memmove(reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(clone) + 0x50), reinterpret_cast<void*>(original_address), function_size);

	return reinterpret_cast<std::uintptr_t>(clone) + 0x50;
}