#ifndef ITEMS_H
#define ITEMS_H

#include "engine/mesh.h"

#include <vector>
#include <string>

struct ItemProperty{
	int propertyId;
	int level;
};

struct ItemDescriptor{
	int type;
	int variation;
	std::vector<ItemProperty> properties;
	void setProperty(int propertyId, int level);
};

namespace Items{
	enum class ModifierType{ REQUIRE_LEVEL_OR_NONE, REQUIRE_LEVEL, INHERIT_LOWEST, INHERIT_HIGHEST, INCREMENT };
	struct Modifier{
		ModifierType type;
		int propertyId;
		int propertyValue;
	};
};

struct Item{
	int type = -1;
	int variation;

	bool operator==(const Item& rhs) const;
	bool matches(const Item& rhs, const std::vector<Items::Modifier>& modifiers) const;

	int getPropertyLevel(int propertyId) const;
	ItemDescriptor getDescriptor() const;
};

struct ItemStack : public Item{
	ItemStack();
	ItemStack(int type, int variation, int count);
	ItemStack(Item item, int count);
	int count;
};

namespace Items{

	int getItemType(const std::string& name);
	std::string getItemName(int type);

	int getPropertyId(const std::string& name);
	std::string getPropertyName(int id);

	int addProperty(const std::string& name);
	int addItemType(const std::string& name);
	int addItemVariation(int type, const std::vector<ItemProperty>& properties);

	int getOrAddVariation(int type, const std::vector<ItemProperty>& properties);

	void setMesh(int type, const Mesh& m);
	Mesh getMesh(int type);

};

#endif
