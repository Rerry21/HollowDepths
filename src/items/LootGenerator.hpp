#pragma once

#include <optional>

#include "generation/Rng.hpp"
#include "items/Item.hpp"

std::optional<Item> rollEnemyDrop(Rng & rng);