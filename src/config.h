#ifdef BOREALIS_BUILD_DLL
	#if defined(_WIN32) || defined(_WIN64)
		#define BOREALIS_API __declspec(dllexport)
	#else	
		#define BOREALIS_API __attribute__((visibility("default")))
	#endif
#else
	#if defined(_WIN32) || defined(_WIN64)
		#define BOREALIS_API __declspec(dllimport)
	#else	
		#define BOREALIS_API __attribute__((visibility("default")))
	#endif
#endif // BOREALIS_BUILD_DLL
