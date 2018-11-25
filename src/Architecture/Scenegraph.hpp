#pragma once

//STD
#include <vector>
#include <map>
#include <functional>

//SELF
#include "Entity.hpp"

//todo: rethink this stuff
//don't want an entity to define parent and children due to potential outdated info
//don't want to rebuild list of all children each time they're requested
//have scene manager keep track of both?
//struct parent
//{
//	std::vector<uint32_t> children;
//};

class Scenegraph
{
public:
	void update(float dt);
	void draw(sf::RenderWindow& window) const;

	void registerBuilder(std::string type, std::function<std::unique_ptr<Entity>(EntityInfo)> builder);

	Entity* createEntity(EntityInfo info);

	Entity* getEntity(uint32_t entityID);
	//Entity* getParent(uint32_t entityID);
	//std::vector<Entity*> getChildren(uint32_t entityID);
	//std::vector<Entity*> getChildrenRecursively(uint32_t entityID);

private:
	std::map<uint32_t, std::unique_ptr<Entity>> entities;
	std::map<std::string, std::function<std::unique_ptr<Entity>(EntityInfo)>> builders;
	//std::map<uint32_t, parent> parents;
};