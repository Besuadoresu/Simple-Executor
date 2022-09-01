#include "memcheck_bypass.hpp"
#include "../../../module/executor_module.hpp"

executor::utilities::structures::segment text_seg = executor::utilities::structures::segment{ ".text" };
executor::utilities::structures::segment rdata_seg = executor::utilities::structures::segment{ ".rdata" };
executor::utilities::structures::segment vmpx_seg = executor::utilities::structures::segment{ ".vmpx" };
executor::utilities::structures::segment vmp0_seg = executor::utilities::structures::segment{ ".vmp0" };
executor::utilities::structures::segment vmp1_seg = executor::utilities::structures::segment{ ".vmp1" };

std::uintptr_t text_clone = executor::utilities::memory::clone_section(text_seg.start_addr);
std::uintptr_t rdata_clone = executor::utilities::memory::clone_section(rdata_seg.start_addr);
std::uintptr_t vmpx_clone = executor::utilities::memory::clone_section(vmpx_seg.start_addr);
std::uintptr_t vmp0_clone = executor::utilities::memory::clone_section(vmp0_seg.start_addr);
std::uintptr_t vmp1_clone = executor::utilities::memory::clone_section(vmp1_seg.start_addr);
std::uintptr_t core_hasher_start = executor::utilities::pattern_scan::scan("\x8B\x03\x03\xC3\x69\xC0\x2D\xFE\x94\x15\x03\xC7\xC1\xC0\x13", "xxxxxxxxxxxxxxx").back();
std::uintptr_t core_hasher_end = (core_hasher_start + 107);

std::uintptr_t old_address;
std::uintptr_t esp_backup;
std::uintptr_t spoofed;

std::uintptr_t __cdecl get_spoofed_address(std::uintptr_t address)
{
	if (address >= text_seg.start_addr && address <= text_seg.start_addr + text_seg.size)
		return address - text_seg.start_addr + text_clone;
	else if (address >= rdata_seg.start_addr && address <= rdata_seg.start_addr + rdata_seg.size)
		return address - rdata_seg.start_addr + rdata_clone;
	else if (address >= vmpx_seg.start_addr && address <= vmpx_seg.start_addr + vmpx_seg.size)
		return address - vmpx_seg.start_addr + vmpx_clone;
	else if (address >= vmp1_seg.start_addr && address <= vmp1_seg.start_addr + vmp1_seg.size)
		return address - vmp1_seg.start_addr + vmp1_clone;

	return address;
}

__declspec(naked) void main_hasher_loop()
{
	__asm
	{
		mov esp_backup, esp;
		sub esp, 0x300;

		pushad;

		push ebx;
		call get_spoofed_address;
		add esp, 4;

		mov spoofed, eax;

		popad;

		add esp, 0x300;
		mov eax, spoofed;
		mov esp, eax;

	hasher_start:

		mov eax, [esp];
		add eax, ebx;
		imul eax, 0x1594FE2D;
		add eax, edi;
		rol eax, 0x13;
		imul edi, eax, 0x0CBB4ABF7;

		lea eax, [ebx + 4];
		sub eax, [esp + 4];
		add ebx, 8;
		add esp, 8;
		imul eax, 0x344B5409;
		add eax, [ebp - 0x10];
		rol eax, 0x11;
		imul eax, 0x1594FE2D;
		mov[ebp - 0x10], eax;

		mov eax, [esp];
		xor eax, ebx;
		add ebx, 4;
		add esp, 4;
		imul eax, 0x1594FE2D;
		add eax, [ebp - 0xC];
		rol eax, 0xD;
		imul eax, 0x0CBB4ABF7;
		mov[ebp - 0xC], eax;

		mov eax, [esp];
		sub eax, ebx;
		add ebx, 4;
		add esp, 4;
		imul eax, 0x344B5409;
		add eax, esi;
		rol eax, 0xF;
		imul esi, eax, 0x1594FE2D;

		cmp ebx, ecx;
		jb hasher_start;

		mov esp, esp_backup;

		jmp[core_hasher_end];
	}
}

std::uint32_t executor::roblox::checks::memcheck::spoof(std::uint32_t a1)
{
	const auto& ref = (a1 >= text_seg.start_addr && a1 <= text_seg.end_addr) ? text_clone : vmp0_clone;

	if (a1 >= vmp0_seg.start_addr && a1 <= vmp0_seg.end_addr || a1 >= text_seg.start_addr && a1 <= text_seg.end_addr)
		a1 = a1 - base + ref - 0x1000;

	return a1;
}

std::uint32_t executor::roblox::checks::memcheck::silent_checker_hook(std::uint32_t a1, std::uint32_t a2, std::uint32_t a3, std::uint32_t a4)
{
	/*if (a1 == base + 0x1000)
		printf("Silent Checker Fired: 0x%X, 0x%X, 0x%X, 0x%X\n", a1, a2, a3, a4);*/

	return silent_checker(spoof(a1), a2, a3, a4);
}

int __fastcall executor::roblox::checks::memcheck::memcheck_job_hook(void* ecx, void* edx, int rando)
{
	std::vector<std::uintptr_t> silent_checkers;

	for (uintptr_t index = offsets::memcheck::silent_checker_array_start; index < offsets::memcheck::silent_checker_array_start + (7 * 16); index += 7)
	{
		uintptr_t addr_thing;
		memcpy(&addr_thing, (uintptr_t*)(index + 3), 4); // C7 45 EC F0 7A 82 00 -> C7 45 EC are all common bytes so we add 3 bytes to get the function address

		silent_checkers.push_back(addr_thing);
	}

	const auto cloned_checker = utilities::memory::clone_function(silent_checkers[6]);

	std::memset(reinterpret_cast<void*>(cloned_checker + 0x12), 0x90, 0x6);

	silent_checker = reinterpret_cast<silent_checker_t>(cloned_checker);

	for (const auto checker : silent_checkers)
		utilities::memory::tramp_hook(checker, reinterpret_cast<std::uintptr_t>(silent_checker_hook), 6);

	utilities::memory::tramp_hook(core_hasher_start, reinterpret_cast<std::uintptr_t>(main_hasher_loop), 5);

	const auto return_value = reinterpret_cast<int(__fastcall*)(void*, void*, int)>(old_address)(ecx, edx, rando);

	hook->disable();

	module::global_module->memcheck_disabled = true;

	return return_value;
}

__declspec(noinline) void executor::roblox::checks::memcheck::bypass()
{
	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	} while (roblox::taskscheduler::get_job_by_name("US14116") == nullptr);

	auto memcheckjob = roblox::taskscheduler::get_job_by_name("US14116");

	const auto mc_vftable = reinterpret_cast<std::uintptr_t>(memcheckjob.value());

	hook = new utilities::hooks::vftable::vftable_replace<5>(reinterpret_cast<std::uintptr_t*>(mc_vftable), reinterpret_cast<std::uintptr_t*>(memcheck_job_hook));

	old_address = hook->get_old_func();

	hook->enable();
}