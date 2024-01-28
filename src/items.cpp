#include "items.h"

#include <stdexcept>

static std::vector<std::string> propertyNames;
static std::vector<std::string> itemNames;

static std::vector<std::vector<ItemDescriptor>> itemDescriptors;

static std::vector<Mesh> meshes;

void ItemDescriptor::setProperty(int propertyId, int level){
	for(auto& a : properties){
		if(a.propertyId == propertyId){
			a.level = level;
			return;
		}
	}
	properties.push_back({propertyId, level});
}


bool Item::operator==(const Item& rhs) const{
	return type == rhs.type && variation == rhs.variation;
}

bool Item::matches(const Item& rhs, const std::vector<Items::Modifier>& modifiers) const{
	//return type == rhs.type && (variation == rhs.variation || rhs.variation == -1);
	if(type != rhs.type) return false;
	if(variation == rhs.variation) return true;
	for(auto& m : modifiers){
		int level = getPropertyLevel(m.propertyId);
		switch(m.type){
			case Items::ModifierType::REQUIRE_LEVEL_OR_NONE:
				if(level == 0 || level >= m.propertyValue) return false;
				break;
			default:
				break;
		}
	}
	return true;
}

int Item::getPropertyLevel(int propertyId) const{
	ItemDescriptor& idesc = itemDescriptors.at(type).at(variation);
	if(idesc.type != type || idesc.variation != variation)
		throw std::runtime_error("ItemDescriptor [" + std::to_string(type) + "][" + std::to_string(variation) + "] is malplaced!");

	for(auto& a : idesc.properties){
		if(a.propertyId == propertyId) return a.level;
	}
	return 0;
}

ItemDescriptor Item::getDescriptor() const{
	return itemDescriptors.at(type).at(variation);
}

ItemStack::ItemStack(){
	type = 0;
	variation = 0;
	count = 0;
}

ItemStack::ItemStack(int type, int variation, int count){
	this->type = type;
	this->variation = variation;
	this->count = count;
}

ItemStack::ItemStack(Item item, int count){
	this->type = item.type;
	this->variation = item.variation;
	this->count = count;
}

int Items::getItemType(const std::string& name){
	for(decltype(itemNames.size()) i = 0; i < itemNames.size(); i++){
		if(itemNames[i] == name) return i;
	}
	throw std::runtime_error("Item " + name + " does not exist!");
	return -1;
}

std::string Items::getItemName(int type){
	return itemNames.at(type);
}

int Items::getPropertyId(const std::string& name){
	for(decltype(propertyNames.size()) i = 0; i < propertyNames.size(); i++){
		if(propertyNames[i] == name) return i;
	}
	throw std::runtime_error("Property " + name + " does not exist!");
	return -1;
}

std::string Items::getPropertyName(int id){
	return propertyNames.at(id);
}

int Items::addProperty(const std::string& name){
	propertyNames.push_back(name);
	return propertyNames.size()-1;
}

int Items::addItemType(const std::string& name){
	int type = itemNames.size();
	itemNames.push_back(name);
	itemDescriptors.push_back(std::vector<ItemDescriptor>());
	meshes.push_back(Mesh());
	return type;
}

int Items::addItemVariation(int type, const std::vector<ItemProperty>& properties){
	int variation = itemDescriptors.at(type).size();
	itemDescriptors.at(type).push_back(ItemDescriptor{type, variation, properties});
	return variation;
}

int Items::getOrAddVariation(int type, const std::vector<ItemProperty>& properties){
	for(auto& a : itemDescriptors.at(type)){
		if(a.properties.size() != properties.size()) continue;
		for(decltype(a.properties.size()) j = 0; j < a.properties.size(); j++){
			if(properties[j].propertyId != a.properties[j].propertyId || properties[j].level != a.properties[j].level)
				goto continue_outer;
		}
		return a.variation;
		continue_outer:
			continue;
	}
	return Items::addItemVariation(type, properties);
}

void Items::setMesh(int type, const Mesh& m){
	meshes.at(type) = m;
}
Mesh Items::getMesh(int type){
	return meshes.at(type);
}


