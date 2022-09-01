#pragma once
#include <iostream>
#include <thread>
#include <string>
#include "../roblox/checks/checks.hpp"
#include "../utilities/utilities.hpp"
#include "../execution/execution.hpp"
#include "../luau/lstate.h"
#include "../luau/Luau/Compiler.h"
#include "../luau/Luau/BytecodeBuilder.h"
#include "../roblox/offsets/offsets.hpp"

namespace executor::module
{
	class executor_module_t
	{
		lua_State* state{};
	public:
		explicit executor_module_t();

		bool compileScript(lua_State* state, const std::string& script);

		void bypass_checks();

		void push_custom_functions();

		void initialize_scheduler();

		void execute(const std::string& script)
		{
			executor::execution::waiting_scripts.push(script);
		}

		void set_state();

		void set_state(lua_State* new_state)
		{
			state = new_state;
		}

		lua_State* return_state()
		{
			return state;
		}

		bool console_bypassed = false;
		bool memcheck_disabled = false;
	}; 

	extern std::shared_ptr<executor::module::executor_module_t> global_module;
}