// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include "lua.h"
#include "lcommon.h"

/*
** Union of all collectible objects
*/
typedef union GCObject GCObject;

/*
** Common Header for all collectible objects (in macro form, to be included in other objects)
*/
// clang-format off
#define CommonHeader \
     LUAVM_SHUFFLE3(;, uint8_t tt, uint8_t marked, uint8_t memcat)
// clang-format on

/*
** Common header in struct form
*/
typedef struct GCheader
{
    CommonHeader;
} GCheader;

/*
** Union of all Lua values
*/
typedef union
{
    GCObject* gc;
    void* p;
    double n;
    int b;
    float v[2]; // v[0], v[1] live here; v[2] lives in TValue::extra
} Value;

/*
** Tagged Values
*/

typedef struct lua_TValue
{
    Value value;
    int extra[LUA_EXTRA_SIZE];
    int tt;
} TValue;

/* Macros to test type */
#define ttisnil(o) (ttype(o) == LUA_TNIL)
#define ttisnumber(o) (ttype(o) == LUA_TNUMBER)
#define ttisstring(o) (ttype(o) == LUA_TSTRING)
#define ttistable(o) (ttype(o) == LUA_TTABLE)
#define ttisfunction(o) (ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o) (ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o) (ttype(o) == LUA_TUSERDATA)
#define ttisthread(o) (ttype(o) == LUA_TTHREAD)
#define ttislightuserdata(o) (ttype(o) == LUA_TLIGHTUSERDATA)
#define ttisvector(o) (ttype(o) == LUA_TVECTOR)
#define ttisupval(o) (ttype(o) == LUA_TUPVAL)

/* Macros to access values */
#define ttype(o) ((o)->tt)
#define gcvalue(o) check_exp(iscollectable(o), (o)->value.gc)
#define pvalue(o) check_exp(ttislightuserdata(o), (o)->value.p)
#define nvalue(o) check_exp(ttisnumber(o), executor::roblox::offsets::execution::Xor((o)->value.n))
#define vvalue(o) check_exp(ttisvector(o), (o)->value.v)
#define tsvalue(o) check_exp(ttisstring(o), &(o)->value.gc->ts)
#define uvalue(o) check_exp(ttisuserdata(o), &(o)->value.gc->u)
#define clvalue(o) check_exp(ttisfunction(o), &(o)->value.gc->cl)
#define hvalue(o) check_exp(ttistable(o), &(o)->value.gc->h)
#define bvalue(o) check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o) check_exp(ttisthread(o), &(o)->value.gc->th)
#define upvalue(o) check_exp(ttisupval(o), &(o)->value.gc->uv)

#define l_isfalse(o) (ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))

/*
** for internal debug only
*/
#define checkconsistency(obj) LUAU_ASSERT(!iscollectable(obj) || (ttype(obj) == (obj)->value.gc->gch.tt))

#define checkliveness(g, obj) LUAU_ASSERT(!iscollectable(obj) || ((ttype(obj) == (obj)->value.gc->gch.tt) && !isdead(g, (obj)->value.gc)))

/* Macros to set values */
#define setnilvalue(obj) ((obj)->tt = LUA_TNIL)

#define setnvalue(obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.n = executor::roblox::offsets::execution::Xor(x); \
        i_o->tt = LUA_TNUMBER; \
    }

#if LUA_VECTOR_SIZE == 4
#define setvvalue(obj, x, y, z, w) \
    { \
        TValue* i_o = (obj); \
        float* i_v = i_o->value.v; \
        i_v[0] = (x); \
        i_v[1] = (y); \
        i_v[2] = (z); \
        i_v[3] = (w); \
        i_o->tt = LUA_TVECTOR; \
    }
#else
#define setvvalue(obj, x, y, z, w) \
    { \
        TValue* i_o = (obj); \
        float* i_v = i_o->value.v; \
        i_v[0] = (x); \
        i_v[1] = (y); \
        i_v[2] = (z); \
        i_o->tt = LUA_TVECTOR; \
    }
#endif

#define setpvalue(obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.p = (x); \
        i_o->tt = LUA_TLIGHTUSERDATA; \
    }

#define setbvalue(obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.b = (x); \
        i_o->tt = LUA_TBOOLEAN; \
    }

#define setsvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TSTRING; \
        checkliveness(L->global, i_o); \
    }

#define setuvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TUSERDATA; \
        checkliveness(L->global, i_o); \
    }

#define setthvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TTHREAD; \
        checkliveness(L->global, i_o); \
    }

#define setclvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TFUNCTION; \
        checkliveness(L->global, i_o); \
    }

#define sethvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TTABLE; \
        checkliveness(L->global, i_o); \
    }

#define setptvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TPROTO; \
        checkliveness(L->global, i_o); \
    }

#define setupvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TUPVAL; \
        checkliveness(L->global, i_o); \
    }

#define setobj(L, obj1, obj2) \
    { \
        const TValue* o2 = (obj2); \
        TValue* o1 = (obj1); \
        *o1 = *o2; \
        checkliveness(L->global, o1); \
    }

/*
** different types of sets, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s setobj
/* to stack (not from same stack) */
#define setobj2s setobj
#define setsvalue2s setsvalue
#define sethvalue2s sethvalue
#define setptvalue2s setptvalue
/* from table to same table */
#define setobjt2t setobj
/* to table */
#define setobj2t setobj
/* to new object */
#define setobj2n setobj
#define setsvalue2n setsvalue

#define setttype(obj, tt) (ttype(obj) = (tt))

#define iscollectable(o) (ttype(o) >= LUA_TSTRING)

typedef TValue* StkId; /* index to stack elements */

/*
** String headers for string table
*/
typedef struct TString
{
    CommonHeader;
    // 1 byte padding

    int16_t atom;
    // 2 byte padding

    TString* next; // next string in the hash table bucket or the string buffer linked list

    LUAVM_SHUFFLE2(;,
        tstring_hash<unsigned int> hash, // 12
        tstring_len<unsigned int> len // 16
    );

    char data[1]; // string data is allocated right after the header
} TString;

#define getstr(ts) (ts)->data
#define svalue(o) getstr(tsvalue(o))

typedef struct Udata
{
    CommonHeader;

    uint8_t tag;

    LUAVM_SHUFFLE2(;,
        int len,
        userdata_metatable<struct Table*> metatable
    );

    union
    {
        char data[1];      // userdata is allocated right after the header
        L_Umaxalign dummy; // ensures maximum alignment for data
    };
} Udata;

/*
** Function Prototypes
*/
// clang-format off
typedef struct Proto
{
    CommonHeader;

    LUAVM_SHUFFLE8(;,
        proto_member<TValue*> k, // 4             /* constants used by the function */
        proto_member<Instruction*> code, // 8     /* function bytecode */
        proto_member<struct Proto**> p,  // 12     /* functions defined inside the function */
        proto_member<uint8_t*> lineinfo, // 16     /* for each instruction, line number as a delta from baseline */
        proto_member<int*> abslineinfo,  // 20     /* baseline line info, one entry for each 1<<linegaplog2 instructions; allocated after lineinfo */
        proto_member<struct LocVar*> locvars, // 24 /* information about local variables */
        proto_member<TString**> upvalues, // 28     /* upvalue names */
        proto_source<TString*> source // 32
    );

    LUAVM_SHUFFLE2(;,
        proto_debugname<TString*> debugname, // 36
        proto_debuginsn<uint8_t*> debuginsn // 40 // a copy of code[] array with just opcodes
    );

    GCObject* gclist; // 44

    LUAVM_SHUFFLE8(;,
        int sizecode, // 48
        int sizep, // 52
        int sizelocvars, // 56
        int sizeupvalues, // 60
        int sizek, // 64
        int sizelineinfo, // 68
        int linegaplog2, // 72
        int linedefined // 76
    );

    LUAVM_SHUFFLE4(;,
        uint8_t nups, // 80 /* number of upvalues */
        uint8_t numparams, // 81
        uint8_t is_vararg, // 82
        uint8_t maxstacksize // 83
    );
} Proto;
// clang-format on

typedef struct LocVar
{
    TString* varname;
    int startpc; /* first point where variable is active */
    int endpc;   /* first point where variable is dead */
    uint8_t reg; /* register slot, relative to base, where variable is stored */
} LocVar;

/*
** Upvalues
*/

typedef struct UpVal
{
    CommonHeader;
    // 1 (x86) or 5 (x64) byte padding
    TValue* v; /* points to stack or to its own value */
    union
    {
        TValue value; /* the value (when closed) */
        struct
        {
            /* global double linked list (when open) */
            LUAVM_SHUFFLE2(;,
                struct UpVal* prev,
                struct UpVal* next
            );

            /* thread double linked list (when open) */
            struct UpVal* threadnext;
            /* note: this is the location of a pointer to this upvalue in the previous element that can be either an UpVal or a lua_State */
            struct UpVal** threadprev;
        } l;
    } u;
} UpVal;

/*
** Closures
*/

typedef struct Closure
{
    CommonHeader;

    uint8_t isC;
    uint8_t nupvalues;
    uint8_t stacksize;
    uint8_t preload;

    GCObject* gclist; // 8
    struct Table* env; // 12

    union
    {
        struct
        {
            cclosure_f<lua_CFunction> f; // 16
            cclosure_cont<lua_Continuation> cont; // 20
            cclosure_debugname<const char*> debugname; // 24
            TValue upvals[1]; // 28
        } c;

        struct
        {
            lclosure_p<struct Proto*> p; // 16
            TValue uprefs[1]; // 20
        } l;
    };
} Closure;

#define iscfunction(o) (ttype(o) == LUA_TFUNCTION && clvalue(o)->isC)
#define isLfunction(o) (ttype(o) == LUA_TFUNCTION && !clvalue(o)->isC)

/*
** Tables
*/

typedef struct TKey
{
    ::Value value;
    int extra[LUA_EXTRA_SIZE];
    unsigned tt : 4;
    int next : 28; /* for chaining */
} TKey;

typedef struct LuaNode
{
    TValue val;
    TKey key;
} LuaNode;

/* copy a value into a key */
#define setnodekey(L, node, obj) \
    { \
        LuaNode* n_ = (node); \
        const TValue* i_o = (obj); \
        n_->key.value = i_o->value; \
        memcpy(n_->key.extra, i_o->extra, sizeof(n_->key.extra)); \
        n_->key.tt = i_o->tt; \
        checkliveness(L->global, i_o); \
    }

/* copy a value from a key */
#define getnodekey(L, obj, node) \
    { \
        TValue* i_o = (obj); \
        const LuaNode* n_ = (node); \
        i_o->value = n_->key.value; \
        memcpy(i_o->extra, n_->key.extra, sizeof(i_o->extra)); \
        i_o->tt = n_->key.tt; \
        checkliveness(L->global, i_o); \
    }

// clang-format off
typedef struct Table
{
    CommonHeader;

    LUAVM_SHUFFLE5(;,
        uint8_t tmcache, //3   /* 1<<p means tagmethod(p) is not present */
        uint8_t readonly, //4.  /* sandboxing feature to prohibit writes to table */
        uint8_t safeenv,  //5  /* environment doesn't share globals with other scripts */
        uint8_t lsizenode, //6. /* log2 of size of `node' array */
        uint8_t nodemask8 //7  /* (1<<lsizenode)-1, truncated to 8 bits */
    );

    int sizearray; //8 /* size of `array' array */
    union
    {
        int lastfree;  /* any free position is before this position */
        int aboundary; /* negated 'boundary' of `array' array; iff aboundary < 0 */
    };

    LUAVM_SHUFFLE4(;,
        table_metatable<struct Table*> metatable, // 16.
        table_array<TValue*> array,  //20 /* array part */
        table_node<LuaNode*> node, //24 .
        GCObject* gclist // 28
    );
} Table;
// clang-format on

/*
** `module' operation for hashing (size is always a power of 2)
*/
#define lmod(s, size) (check_exp((size & (size - 1)) == 0, (cast_to(int, (s) & ((size)-1)))))

#define twoto(x) ((int)(1 << (x)))
#define sizenode(t) (twoto((t)->lsizenode))

//#define luaO_nilobject (&luaO_nilobject_)
#define luaO_nilobject (TValue*)executor::roblox::offsets::custom_function_utils::luaO_nilobject

LUAI_DATA const TValue luaO_nilobject_;

#define ceillog2(x) (luaO_log2((x)-1) + 1)

LUAI_FUNC int luaO_log2(unsigned int x);
LUAI_FUNC int luaO_rawequalObj(const TValue* t1, const TValue* t2);
LUAI_FUNC int luaO_rawequalKey(const TKey* t1, const TValue* t2);
LUAI_FUNC int luaO_str2d(const char* s, double* result);
LUAI_FUNC const char* luaO_pushvfstring(lua_State* L, const char* fmt, va_list argp);
LUAI_FUNC const char* luaO_pushfstring(lua_State* L, const char* fmt, ...);
LUAI_FUNC void luaO_chunkid(char* out, const char* source, size_t len);
