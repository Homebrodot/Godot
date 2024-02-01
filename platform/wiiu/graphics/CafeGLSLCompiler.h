#pragma once
#include <stdint.h>

#if defined(__WUT__) || defined(__WIIU__)
#include <gx2/shaders.h>
#include <coreinit/dynload.h>
#include <coreinit/debug.h>

#define GLSL_COMPILER_CAFE_RPL // use compiler as rpl
#endif

#ifdef GLSL_COMPILER_CAFE_RPL
inline OSDynLoad_Module s_glslCompilerModule = nullptr;
#endif

enum GLSL_COMPILER_FLAG
{
    // reserved for future use
    GLSL_COMPILER_FLAG_NONE = 0,
    GLSL_COMPILER_FLAG_GENERATE_DISASSEMBLY = 1 << 0, // write disassembly to stderr
};

inline GX2VertexShader* (*GLSL_CompileVertexShader)(const char* shaderSource, char* infoLogOut, int infoLogMaxLength, GLSL_COMPILER_FLAG flags);
inline GX2PixelShader* (*GLSL_CompilePixelShader)(const char* shaderSource, char* infoLogOut, int infoLogMaxLength, GLSL_COMPILER_FLAG flags);
inline void (*GLSL_FreeVertexShader)(GX2VertexShader* shader);
inline void (*GLSL_FreePixelShader)(GX2PixelShader* shader);
inline void (*__GLSL_DestroyGLSLCompiler)();

#ifndef GLSL_COMPILER_CAFE_RPL
extern "C"
{
    void InitGLSLCompiler();
    void DestroyGLSLCompiler();
    GX2VertexShader* CompileVertexShader(const char* shaderSource, char* infoLogOut, int infoLogMaxLength, GLSL_COMPILER_FLAG flags);
    GX2PixelShader* CompilePixelShader(const char* shaderSource, char* infoLogOut, int infoLogMaxLength, GLSL_COMPILER_FLAG flags);
    void FreeVertexShader(GX2VertexShader* shader);
    void FreePixelShader(GX2PixelShader* shader);
};
#endif

static inline bool GLSL_Init()
{
    void (*_InitGLSLCompiler)() = nullptr;
#if defined(__WUT__) || defined(__WIIU__)
    if (s_glslCompilerModule != nullptr)
        return false;
    OSDynLoad_Error r = OSDynLoad_Acquire("glslcompiler", &s_glslCompilerModule);
    if(r != OS_DYNLOAD_OK) // try alternate path
        r = OSDynLoad_Acquire("~/wiiu/libs/glslcompiler.rpl", &s_glslCompilerModule);
    if (r != OS_DYNLOAD_OK)
    {
        OSReport("glslcompiler.rpl not found\n");
        return false;
    }
    // find exports
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "InitGLSLCompiler", (void**)&_InitGLSLCompiler);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "CompileVertexShader", (void**)&GLSL_CompileVertexShader);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "CompilePixelShader", (void**)&GLSL_CompilePixelShader);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "FreeVertexShader", (void**)&GLSL_FreeVertexShader);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "FreePixelShader", (void**)&GLSL_FreePixelShader);
    OSDynLoad_FindExport(s_glslCompilerModule, OS_DYNLOAD_EXPORT_FUNC, "DestroyGLSLCompiler", (void**)&__GLSL_DestroyGLSLCompiler);
#else
    _InitGLSLCompiler = InitGLSLCompiler;
    GLSL_CompileVertexShader = CompileVertexShader;
    GLSL_CompilePixelShader = CompilePixelShader;
    GLSL_FreeVertexShader = FreeVertexShader;
    GLSL_FreePixelShader = FreePixelShader;
    __GLSL_DestroyGLSLCompiler = DestroyGLSLCompiler;
#endif
    _InitGLSLCompiler();
    return true;
}

static inline bool GLSL_Shutdown()
{
#ifdef GLSL_COMPILER_CAFE_RPL
    if (s_glslCompilerModule == nullptr)
        return false;
    __GLSL_DestroyGLSLCompiler();
    OSDynLoad_Release(s_glslCompilerModule);
    s_glslCompilerModule = nullptr;
#else
    __GLSL_DestroyGLSLCompiler();
#endif
    return true;
}

