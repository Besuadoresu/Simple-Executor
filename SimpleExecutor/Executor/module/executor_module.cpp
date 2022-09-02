#include "executor_module.hpp"
#include "../roblox/custom_functions/custom_functions.hpp"
#include "../execution/execution.hpp"
#include "../luau/lualib.h"

//#define USE_ROBLOX_LUAU_LOAD

executor::module::executor_module_t::executor_module_t()
{

}

//void stackdump_g(lua_State* l)
//{
//    int i;
//    int top = lua_gettop(l);
//
//    printf("total in stack %d\n", top);
//
//    for (i = 1; i <= top; i++)
//    {  /* repeat for each level */
//        int t = lua_type(l, i);
//        switch (t) {
//        case LUA_TSTRING:  /* strings */
//            printf("string: '%s'\n", lua_tostring(l, i));
//            break;
//        case LUA_TBOOLEAN:  /* booleans */
//            printf("boolean %s\n", lua_toboolean(l, i) ? "true" : "false");
//            break;
//        case LUA_TNUMBER:  /* numbers */
//            printf("number: %g\n", lua_tonumber(l, i));
//            break;
//        default:  /* other values */
//            printf("%s\n", lua_typename(l, t));
//            break;
//        }
//        printf("  ");  /* put a separator */
//    }
//    printf("\n");  /* end the listing */
//}

bool executor::module::executor_module_t::compileScript(lua_State* thread, const std::string& script)
{
    struct : Luau::BytecodeEncoder
    {
        uint8_t encodeOp(const std::uint8_t op) override
        {
            return op * 227;
        }
    } bytecode_encoder;

	const auto bytecode = Luau::compile(script, { 2, 1, 0 }, { }, &bytecode_encoder);

#ifdef USE_ROBLOX_LUAU_LOAD
    return roblox::offsets::execution::luau_load(reinterpret_cast<std::uintptr_t>(thread), utilities::rand_gen::generate_random(15).c_str(), bytecode.c_str(), bytecode.size(), 0) != 1;
#else
    return luau_load(thread, utilities::rand_gen::generate_random(15).c_str(), bytecode.c_str(), bytecode.size(), 0) != 1;
#endif
}

void executor::module::executor_module_t::bypass_checks()
{
	using namespace executor::roblox::checks;

	console::bypass("Simple Executor");

    memcheck::bypass();

    DWORD old;
    uintptr_t test_v = rebase(0x18D0050); // do something here later
    printf("test_v: 0x%X\n", test_v);

    VirtualProtect(reinterpret_cast<void*>(test_v), 8, PAGE_EXECUTE_READWRITE, &old);
    memset(reinterpret_cast<void*>(test_v), 0x90, 8);
    VirtualProtect(reinterpret_cast<void*>(test_v), 8, old, &old);
    
}

void executor::module::executor_module_t::push_custom_functions()
{
    roblox::custom_functions::push_functions(state);
}

void executor::module::executor_module_t::initialize_scheduler()
{
    set_state();
    std::thread(executor::execution::initialize_scheduler2).detach();
}

void executor::module::executor_module_t::set_state()
{
    const auto waiting_script_job = roblox::taskscheduler::get_job_by_name("WaitingHybridScriptsJob");

    printf("waiting_script_job: 0x%X\n", reinterpret_cast<std::uintptr_t>(waiting_script_job.value()));

    if (!waiting_script_job.has_value())
        throw std::exception("Unable to find WaitingHybridScriptsJob");

    const auto script_context = *reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(waiting_script_job.value()) + roblox::offsets::taskscheduler::waiting_script_job_scriptcontext);

    printf("script_context: 0x%X\n", script_context);

    state = reinterpret_cast<lua_State*>(roblox::offsets::lua_state::get_lua_state(script_context));
    luaL_sandboxthread(state); // is this redundant?
    state->userdata->context_level = 8; // is this redundant?

    printf("lua_state: 0x%p\n", state);
}