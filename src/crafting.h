#ifndef CRAFTING_H
#define CRAFTING_H

#include "items.h"
#include "inventory.h"

#include <vector>

struct Recipe{
	std::vector<ItemStack> ingredients;
	std::vector<ItemStack> result;
	std::vector<Items::Modifier> modifiers;
};

namespace Crafting{

	int addRecipe(const Recipe& recipe);
	Recipe getRecipe(int id);
	std::vector<Recipe>& getRecipes();
	bool canCraft(const Inventory& inventory, const Recipe& recipe);
	void craft(Inventory& inventory, const Recipe& recipe);

};

#endif // CRAFTING_H
