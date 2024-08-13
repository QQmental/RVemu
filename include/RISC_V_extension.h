#pragma once

#ifndef RISC_V_EXT_ZICSR
static_assert(0, "macro RISC_V_EXT_ZICSR should be defined as either 0 or 1");
#else
static_assert(RISC_V_EXT_ZICSR == 0 || RISC_V_EXT_ZICSR == 1);
#endif