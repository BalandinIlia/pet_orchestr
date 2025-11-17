#pragma once
#include "vector"
#include "../parameters/controls.h"

std::vector<number> askInner(SOCKET id, number num);
void answerInner(SOCKET id, std::vector<number> ans);
number getReqInner(SOCKET id);