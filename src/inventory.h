#ifndef INVENTORY_H
#define INVENTORY_H

#include "items.h"

#include <vector>

class Inventory{
public:
	Inventory();
	~Inventory();

	void give(const ItemStack& item);
	void give(const Item& item);
	void take(const ItemStack& item);
	ItemStack take(const ItemStack& item, const std::vector<Items::Modifier>& modifiers);

	int countItems(const Item& item) const;
	int countItems(const Item& item, const std::vector<Items::Modifier>& modifiers) const;

	std::vector<ItemStack> items;
protected:

};

#endif // INVENTORY_H
