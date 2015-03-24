# GMS-PrismStructure-3D
3D-grid DLL for GMS.

This is the C++ source code for a 3-dimensional grid DLL for GameMaker. To use it you'll need to compile it as a DLL with the following project properties in VS C++ (preferably 2013):

- Config Properties For All Configurations:
 - Target Extension: .dll
 - Platform Toolset: v120
 - Config Type: Dynamic Library (.dll)
 - Character Set: Not Set
 - Whole Program Optimization: Link Time Code Generation
- C/C++ -> General:
 - Common Language RunTime Support: No
 - Multi-Processor Compilation: No
- C/C++ -> Optimization:
 - Optimization: Maximize Speed (/O2)
 - Enable Intrinsic Functions: Yes (/Oi)
 - Favor Size Or Speed: Favor fast code (/Ot)
 - Whole Program Optimization: Yes (/GL)
- C/C++ -> Code Generation:
 - Enable C++ Exceptions: Yes with Extern C functions (/EHs)
 - Runtime Library: Multi-threaded DLL (/MD)
