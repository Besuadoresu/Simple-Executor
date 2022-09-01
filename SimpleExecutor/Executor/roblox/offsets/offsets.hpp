#pragma once
#include <cstdint>
#include <Windows.h>
#include <iostream>

/*
    Folders we use:
        - AST
        - Common
        - Compiler
        - VM
    
    Files used:
        - lapi.cpp
        - lobject.h
        - lstate.h
        - ltm.cpp
        - ltm.h
        - lua.h
*/

#define LUAVM_SHUFFLE_COMMA ,

#define LUAVM_SHUFFLE2( s, a0, a1 ) a0 s a1
#define LUAVM_SHUFFLE3( s, a0, a1, a2 ) a0 s a1 s a2
#define LUAVM_SHUFFLE4( s, a0, a1, a2, a3 ) a2 s a0 s a1 s a3
#define LUAVM_SHUFFLE5( s, a0, a1, a2, a3, a4 ) a0 s a1 s a4 s a3 s a2									//
#define LUAVM_SHUFFLE6( s, a0, a1, a2, a3, a4, a5 ) a1 s a5 s a3 s a0 s a4 s a2
#define LUAVM_SHUFFLE7( s, a0, a1, a2, a3, a4, a5, a6 ) a0 s a1 s a6 s a5 s a3 s a4 s a2
#define LUAVM_SHUFFLE8( s, a0, a1, a2, a3, a4, a5, a6, a7 ) a5 s a1 s a0 s a6 s a2 s a7 s a3 s a4		//

// lobject.h
#define tstring_hash vmvalue3_t //Set: Xor Get: Xor
#define tstring_len vmvalue4_t //Set: Neg Val on left Get: Add

#define userdata_metatable vmvalue4_t //Set: Neg Val on left  Get: Add

#define proto_member vmvalue2_t // 1 Setting is 100% adding
#define proto_source vmvalue2_t  //Setting is Neg val on the right Getting: Neg val on the right
#define proto_debugname vmvalue3_t //Getting is xor? Setting is ?? I thought setting was adding but I guess it's not?
#define proto_debuginsn vmvalue4_t //Getting is adding Setting is neg

#define cclosure_f vmvalue4_t //Setting is neg val on the left Getting is adding val on the right
#define cclosure_cont vmvalue2_t //Setting is neg val on the right Getting Is neg val on the right
#define cclosure_debugname vmvalue1_t //Setting is adding Getting is neg val on the left
#define lclosure_p vmvalue4_t //Setting is neg val on the left Getting is adding val on the right

#define table_metatable vmvalue4_t //Set: Neg Val on left  Get: Add
#define table_array vmvalue4_t //Getting is Adding val on right Has to be 4
#define table_node vmvalue4_t //Getting is Adding val on right Has to be 4

// lstate.h
#define gs_ttname vmvalue2_t //Setting is neg val on the right
#define gs_tmname vmvalue2_t //Setting is neg val on the right

#define ls_globalstate vmvalue2_t  //I HAVE NO IDEA HOW THIS IS 2 BUT IT DOESN'T CRASH WITH IT
#define ls_stacksize vmvalue2_t //getting is neg value on the right Setting is neg value on the right

// storage = whats inside the pointer, this = pointer
template <typename T> struct vmvalue1_t
{
public:
    operator const T() const
    {
        return (T)((uintptr_t)this - (uintptr_t)Storage);
    }

    void operator=(const T& value)
    {
        Storage = (T)((uintptr_t)value + (uintptr_t)this);
    }

    const T operator->() const
    {
        return operator const T();
    }
private:
    T Storage;
};

template <typename T> struct vmvalue2_t
{
public:
    operator const T() const
    {
        return (T)((uintptr_t)this - (uintptr_t)Storage);
    }

    void operator=(const T& value)
    {
        Storage = (T)((uintptr_t)this - (uintptr_t)value);
    }

    const T operator->() const
    {
        return operator const T();
    }
private:
    T Storage;
};

template <typename T> struct vmvalue3_t
{
public:
    operator const T() const
    {
        return (T)((uintptr_t)this ^ (uintptr_t)Storage);
    }

    void operator=(const T& value)
    {
        Storage = (T)((uintptr_t)this ^ (uintptr_t)value);
    }

    const T operator->() const
    {
        return operator const T();
    }
private:
    T Storage;
};

template <typename T> struct vmvalue4_t
{
public:
    operator const T() const
    {
        return (T)((uintptr_t)this + (uintptr_t)Storage);
    }

    void operator=(const T& value)
    {
        Storage = (T)((uintptr_t)value - (uintptr_t)this);
    }

    const T operator->() const
    {
        return operator const T();
    }
private:
    T Storage;
};

const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));

#define rebase(x) (x - 0x400000 + base)
#define unbase(x) (x + 0x400000 - base)

namespace executor::roblox::offsets
{
	namespace lua_state
	{
		inline std::uintptr_t get_lua_state(std::uintptr_t script_context)
		{
			return *reinterpret_cast<std::uintptr_t*>(script_context + 0x14C) - (script_context + 0x14C); // Running Script \"%s\"
		}
	}

	namespace taskscheduler // Need this to queue scripts for injection and to grab scriptcontext for luastate
	{
		const auto get_taskscheduler = reinterpret_cast<std::uintptr_t(__cdecl*)()>(rebase(0x1314CE0));

		constexpr std::uint16_t start_job_array = 0x134; // just get the address for whatever returns from calling get_taskscheduler and look through pointers until you find the start
		constexpr std::uint16_t end_job_array = 0x138; // start plus 4

		constexpr std::uint16_t waiting_script_job_scriptcontext = 0x130; // disect data structure and scroll till you find it
	}

    namespace execution
    {
        const auto luau_load = reinterpret_cast<std::uintptr_t(__fastcall*)(uintptr_t a1, const char* a2, const char* a3, int a4, int a5)>(rebase(0x188B1B0)); // syntax error: %s    function above has string called "watch" passed -> find a func in that func which is luau load
        const auto sc_resume = reinterpret_cast<std::uintptr_t(__thiscall*)(uintptr_t a1, uintptr_t** a2, int a3, bool a4, int a5)>(rebase(0x88A820));
		
		inline double Xor(double num)
		{
			uint64_t New = *reinterpret_cast<uint64_t*>(&num) ^ *reinterpret_cast<uint64_t*>(rebase(0x3B18790));
			return *reinterpret_cast<double*>(&New);
		}
	}

	namespace callcheck
	{
		const uintptr_t kernel32_CreateEventA = rebase(0x66839C);
		//const uintptr_t ret_test = rebase(0x64EAF3); // find this?
	}

	namespace memcheck
	{
		const uintptr_t silent_checker_array_start = rebase(0x6ADAF1); // find this later when working on memcheck
	}

	namespace retcheck
	{
		const std::uintptr_t callback = rebase(0x6F00C0); // lol any retcheck if statement just get the function it calls inside that if statement
	}

	namespace custom_function_utils
	{
        const std::uintptr_t dummynode rebase(0x25CF8A8);
        const std::uintptr_t luaO_nilobject rebase(0x25CF298);
		//const auto get_property = reinterpret_cast<std::uintptr_t(__thiscall*)(uintptr_t a1, uintptr_t a2)>(rebase(0x9320B0));
	}
}