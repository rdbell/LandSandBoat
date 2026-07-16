#pragma once

// Shared merit-table indexing used by CMeritPoints and native characterization.
#define GetMeritCategory(merit) (((merit) >> 6) - 1)
#define GetMeritID(merit)       (((merit) & 0x3F) >> 1)
