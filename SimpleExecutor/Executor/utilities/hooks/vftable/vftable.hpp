#pragma once

#include <iostream>
#include <stdint.h>
#include <vector>

#include <Windows.h>

namespace executor::utilities::hooks::vftable
{
	template<std::size_t vftable_index>
	class vftable_replace
	{
		std::uintptr_t* new_vftable = nullptr;
		std::uintptr_t* vftable_backup = nullptr;
		std::uintptr_t* callback = nullptr;
		std::size_t function_count = 0;

	public:

		vftable_replace(std::uintptr_t* address, std::uintptr_t* callback) /* constructor */
			: callback(callback)
		{
			vftable_backup = *reinterpret_cast<std::uintptr_t**>(address);

			while (reinterpret_cast<uintptr_t*>(*address)[function_count])
				function_count++;

			const int vt_size = function_count * 0x4 + 0x4;

			new_vftable = new std::uintptr_t[function_count + 1];

			memcpy(new_vftable, &vftable_backup[-1], vt_size);
			*reinterpret_cast<std::uintptr_t**>(address) = &new_vftable[1];
		}

		~vftable_replace() /* destructor */
		{
			delete[] new_vftable;
		}

		[[nodiscard]] std::uintptr_t get_old_func() const
		{
			return this->vftable_backup[vftable_index];
		}

		void enable() const
		{
			new_vftable[vftable_index + 1] = reinterpret_cast<std::uintptr_t>(callback);
		}

		void disable() const
		{
			new_vftable[vftable_index + 1] = vftable_backup[vftable_index];
		}
	};

}