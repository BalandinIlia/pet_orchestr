#pragma once
#include "vector"
#include "../parameters/controls.h"

std::optional<std::vector<number>> askInner(SOCKET id, number num);
bool answerInner(SOCKET id, const std::vector<number>& ans);
std::optional<number> getReqInner(SOCKET id);