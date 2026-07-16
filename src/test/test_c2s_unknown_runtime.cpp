#include "test_c2s_unknown_runtime.h"
#include <cstdint>
namespace { struct Diagnostic { uint16_t unknown00{}, padding00{}; uint32_t unknown01{}; }; auto diagnostic(uint16_t a,uint16_t b,uint32_t c)->Diagnostic{return {a,b,c};} }
auto runC2SUnknownRuntimeSelfTests() -> bool { const auto d=diagnostic(1,2,3); return d.unknown00==1&&d.padding00==2&&d.unknown01==3; }
