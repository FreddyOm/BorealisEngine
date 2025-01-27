#ifdef BOREALIS_BUILD_DLL
#define BOREALIS_API __declspec(dllexport)
#else
#define BOREALIS_API __declspec(dllimport)
#endif // BOREALIS_BUILD_DLL
