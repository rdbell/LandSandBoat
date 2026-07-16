#pragma once
namespace mobwidescanhelpers
{
inline auto IsWideScannable(bool baseWideScannable, bool noWideScan) -> bool { return baseWideScannable && !noWideScan; }
}
