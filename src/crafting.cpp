#include "crafting.h"

#include <stdexcept>

static std::vector<Recipe> recipes;

int Crafting::addRecipe(const Recipe& recipe){
	int id = recipes.size();
	recipes.push_back(recipe);
	return id;
}

Recipe Crafting::getRecipe(int id){
	return recipes.at(id);
}
std::vector<Recipe>& Crafting::getRecipes(){
	return recipes;
}

bool Crafting::canCraft(const Inventory& inventory, const Recipe& recipe){
	for(auto& a : recipe.ingredients){
		int count = inventory.countItems(a, recipe.modifiers);
		if(count < a.count || (a.count == 0 && count < 1)) return false;
	}
	return true;
}

void Crafting::craft(Inventory& inventory, const Recipe& recipe){
	std::vector<ItemStack> itemsTaken;
	for(auto& a : recipe.ingredients){
		int count = 0;
		do{
			itemsTaken.push_back(inventory.take(ItemStack(a.type, a.variation, a.count-count), recipe.modifiers));
			count += itemsTaken.back().count;
		}
		while((count < a.count && itemsTaken.back().type != -1));
		if(itemsTaken.back().type == -1) itemsTaken.pop_back();
		if(count < a.count){
			throw std::runtime_error("Not enough items to craft item!");
		}
	}
	for(auto& a : recipe.result){
		ItemDescriptor idesc = a.getDescriptor();
		for(auto& m : recipe.modifiers){
			if(m.type == Items::ModifierType::INHERIT_HIGHEST){
				int highestLevel = 0;
				for(auto& a : itemsTaken){
					int level = a.getPropertyLevel(m.propertyId);
					if(level > highestLevel) highestLevel = level;
				}
				idesc.setProperty(m.propertyId, highestLevel);
			}
		}
		int variation = Items::getOrAddVariation(idesc.type, idesc.properties);
		inventory.give(ItemStack{a.type, variation, a.count});
	}
}

