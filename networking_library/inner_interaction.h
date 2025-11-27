#pragma once
#include "vector"
#include "optional"
#include "../parameters/controls.h"

std::optional<std::vector<number>> askInner(const SOCK& id, number num);
bool answerInner(const SOCK& id, const std::vector<number>& ans);
std::optional<number> getReqInner(const SOCK& id);