#pragma once
#include <Windows.h>
#include <iostream>
#include <memory>
#include <array>
#include <thread>
#include <TlHelp32.h>
#include <DbgHelp.h>
#pragma comment( lib, "DbgHelp.lib" )

namespace executor::utilities::structures
{
	struct segment
	{
		std::string_view name;
		std::uintptr_t start_addr = 0;
		std::uintptr_t end_addr = 0;
		std::size_t size = 0;

		segment(std::string_view name_s, HMODULE mod = GetModuleHandle(nullptr)) : name{ name_s }
		{
			const auto nt = ImageNtHeader(mod);
			auto section = reinterpret_cast<IMAGE_SECTION_HEADER*>(nt + 1);

			for (auto iteration = 0u; iteration < nt->FileHeader.NumberOfSections; ++iteration, ++section)
			{
				const auto segment_name = reinterpret_cast<const char*>(section->Name);

				if (name == segment_name)
				{
					start_addr = reinterpret_cast<std::uintptr_t>(mod) + section->VirtualAddress;
					size = section->Misc.VirtualSize;
					end_addr = start_addr + size;

					break;
				}
			}
		}
	};
}