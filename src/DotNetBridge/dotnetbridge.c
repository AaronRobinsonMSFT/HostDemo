// Standard headers
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Provided by the AppHost NuGet package
#include <nethost.h>

// Header file copied from https://github.com/dotnet/core-setup
#include <hostfxr.h>

// Auto-generated header file from the Java compiler
#include <JavaHost.h>

#ifdef WINDOWS
#define STR(s) L ## s
#define CH(c) L ## c
#else
#define STR(s) s
#define CH(c) c
#endif

// Globals to hold hostfxr exports
hostfxr_initialize_for_runtime_config_fn init;
hostfxr_get_runtime_delegate_fn get_del;
hostfxr_close_fn close_fn;

// Function pointers returned from hostfxr
// These functions are defined in https://github.com/dotnet/core-setup
typedef int (NETHOST_CALLTYPE *load_assembly_and_get_function_pointer_fn)(
    const char_t *assembly_path,
    const char_t *type_name,
    const char_t *method_name,
    const char_t *delegate_type_name,
    void *reserved,
    void **delegate);
typedef int (NETHOST_CALLTYPE *managed_entry_point_fn)(void *arg, int argSize);

// Forward declarations
static bool load_hostfxr(void);
static load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *assembly);

/********************************************************************************************
 * Exports called by the hosting JVM
 ********************************************************************************************/

// Defined in jni.h - included via the auto-generated header
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
    if (!load_hostfxr())
    {
        assert(false && "Failure: load_hostfxr()");
        return -1; // Failed to load hostfxr
    }

    // Indicate the JNI version needed by the native library
    return JNI_VERSION_1_2;
}

// Defined in the auto-generated header file
JNIEXPORT void JNICALL Java_JavaHost_dotnetLibHello
  (JNIEnv *e, jclass klass)
{
    // Function pointer to managed delegate
    static managed_entry_point_fn hello = NULL;

    const char_t *dotnetlib_path = STR("DotNetLib.dll");
    const char_t *dotnet_type = STR("DotNetLib.Lib, DotNetLib");
    const char_t *dotnet_type_method = STR("Hello");

    if (hello == NULL)
    {
        load_assembly_and_get_function_pointer_fn load_and_get = NULL;
        load_and_get = get_dotnet_load_assembly(dotnetlib_path);
        assert(load_and_get != NULL && "Failure: get_dotnet_load_assembly()");

        int rc = load_and_get(dotnetlib_path, dotnet_type, dotnet_type_method, NULL, NULL, (void**)&hello);
        assert(rc == 0 && hello != NULL && "Failure: load_assembly_and_get_function_pointer()");
    }

    hello(0, 0);
}

/********************************************************************************************
 * Function used to load and activate .NET Core
 ********************************************************************************************/

// Forward declarations
static void *load_library(const char_t *);
static void *get_export(void *, const char *);

#ifdef WINDOWS
    #include <Windows.h>
    static void *load_library(const char_t *path)
    {
        HMODULE h = LoadLibraryW(path);
        assert(h != NULL);
        return (void*)h;
    }
    static void *get_export(void *h, const char *name)
    {
        void *f = GetProcAddress(h, name);
        assert(f != NULL);
        return f;
    }
#else
    #include <dlfcn.h>
    static void *load_library(const char_t *path)
    {
        void *h = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
        assert(h != NULL);
        return h;
    }
    static void *get_export(void *h, const char *name)
    {
        void *f = dlsym(h, name);
        assert(f != NULL);
        return f;
    }
#endif

#define RUNTIME_CONFIG_EXT      STR(".runtimeconfig.json")
#define RUNTIME_CONFIG_EXT_LEN  ((sizeof(RUNTIME_CONFIG_EXT) / sizeof(char_t)) - 1)

// Using the nethost library, discover the location of hostfxr and get exports
static bool load_hostfxr(void)
{
    // Pre-allocate a large buffer for the path to hostfxr
    char_t buffer[512];
    size_t buffer_size = sizeof(buffer) / sizeof(char_t);
    int rc = get_hostfxr_path(buffer, &buffer_size, NULL);
    if (rc != 0)
        return false;

    // Load hostfxr and get desired exports
    void *lib = load_library(buffer);
    init = (hostfxr_initialize_for_runtime_config_fn)get_export(lib, "hostfxr_initialize_for_runtime_config");
    get_del = (hostfxr_get_runtime_delegate_fn)get_export(lib, "hostfxr_get_runtime_delegate");
    close_fn = (hostfxr_close_fn)get_export(lib, "hostfxr_close");

    return (init && get_del && close_fn);
}

// Find the last character in the supplied string
static size_t str_find_last(const char_t *str, char_t c)
{
    size_t pos = (size_t)-1;
    for (const char_t *e = str; *e; ++e)
    {
        if (*e == c)
            pos = (e - str);
    }
    return pos;
}

// Load and initialize .NET Core and get desired function pointer for scenario
static load_assembly_and_get_function_pointer_fn get_dotnet_load_assembly(const char_t *assembly)
{
    // Build up path to .runtimeconfig.json file
    size_t asm_name_len = str_find_last(assembly, CH('.'));
    assert(asm_name_len != (size_t)-1);
    size_t cfg_len = asm_name_len + RUNTIME_CONFIG_EXT_LEN;
    char_t *cfg_path = (char_t *)calloc((cfg_len + 1), sizeof(char_t));
    memcpy(cfg_path, assembly, asm_name_len * sizeof(char_t));
    memcpy(cfg_path + asm_name_len, RUNTIME_CONFIG_EXT, RUNTIME_CONFIG_EXT_LEN * sizeof(char_t));

    // Load .NET Core
    void *load_and_get = NULL;
    hostfxr_handle cxt = NULL;
    int rc = init(cfg_path, NULL, &cxt);
    if (rc != 0 || cxt == NULL)
    {
        fprintf(stderr, "Init failed: %x\n", rc);
        goto done;
    }

    // Get the load assembly function pointer
    rc = get_del(cxt,
        hdt_load_assembly_and_get_function_pointer,
        &load_and_get);
    if (rc != 0 || load_and_get == NULL)
    {
        fprintf(stderr, "Get delegate failed: %x\n", rc);
    }

done:
    close_fn(cxt);
    free(cfg_path);
    return (load_assembly_and_get_function_pointer_fn)load_and_get;
}
