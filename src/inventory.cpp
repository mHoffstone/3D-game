#include "inventory.h"

Inventory::Inventory(){}
Inventory::~Inventory(){}

void Inventory::give(const ItemStack& item){
	for(decltype(items.size()) i = 0; i < items.size(); i++){
		if(items[i] == item){
			items[i].count += item.count;
			return;
		}
	}
	items.push_back(item);
}
void Inventory::give(const Item& item){
	return give(ItemStack(item, 1));
}


void Inventory::take(const ItemStack& item){
	int itemsTaken = 0;
	for(decltype(items.size()) i = 0; i < items.size(); i++){
		if(items[i] == item){
			int itemsToTake = item.count-itemsTaken;
			if(itemsToTake > items[i].count) itemsToTake = items[i].count;
			items[i].count -= itemsToTake;
			itemsTaken += itemsToTake;
			if(items[i].count == 0){
				items.erase(items.begin()+i);
				i--;
			}
		}
	}
}

ItemStack Inventory::take(const ItemStack& item, const std::vector<Items::Modifier>& modifiers){
	ItemStack stack{-1, -1, 0};
	for(decltype(items.size()) i = 0; i < items.size(); i++){
		if(items[i].matches(item, modifiers)){
			int itemsToTake = item.count;
			if(itemsToTake > items[i].count) itemsToTake = items[i].count;
			items[i].count -= itemsToTake;
			stack.type = items[i].type;
			stack.variation = items[i].variation;
			stack.count = itemsToTake;
			if(items[i].count <= 0){
				items.erase(items.begin()+i);
				i--;
			}
			break;
		}
	}
	return stack;
}

int Inventory::countItems(const Item& item) const{
	int count = 0;
	for(auto& a : items){
		if(a == item) count += a.count;
	}
	return count;
}

int Inventory::countItems(const Item& item, const std::vector<Items::Modifier>& modifiers) const{
	int count = 0;
	for(auto& a : items){
		if(a.matches(item, modifiers)) count += a.count;
	}
	return count;
}

