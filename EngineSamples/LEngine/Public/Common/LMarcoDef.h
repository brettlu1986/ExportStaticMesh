#pragma once

#include "pch.h"

#define SAFE_DELETE(x) if(x) { delete x; x = nullptr; }
#define SAFE_DESTROY(x) if(x) { x->Destroy(); delete x; x = nullptr;}