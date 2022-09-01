#include "execution.hpp"
#include "../module/executor_module.hpp"
#include "../luau/lualib.h"

struct live_thread_ref
{
	int unknown; // 0
	lua_State* thread;
	int thread_id;
	int unknown2; // 0
};

std::queue<std::string> executor::execution::waiting_scripts = {};

void executor::execution::initialize_scheduler2()
{
	while (true)
	{
		if (!waiting_scripts.empty())
		{
			try
			{
				lua_State* thread = lua_newthread(module::global_module->return_state());
				luaL_sandboxthread(thread);
				thread->userdata->context_level = 8;

				// create live thread ref
				live_thread_ref* live_thread = new live_thread_ref;
				live_thread->unknown = 0;
				live_thread->thread = thread;

				lua_pushthread(thread);
				live_thread->thread_id = lua_ref(thread, -1);
				lua_pop(thread, 1);

				live_thread->unknown2 = 0;

				if (module::global_module->compileScript(thread, waiting_scripts.front()))
				{
					roblox::offsets::execution::sc_resume(thread->userdata->script_context, (uintptr_t**)(&live_thread), 0, false, 0);
				}

				lua_pop(thread, 1);
			}
			catch (const std::exception& e)
			{
				printf("error: %s\n", e.what());
			}

			waiting_scripts.pop();
		}

		Sleep(10);
	}
}
