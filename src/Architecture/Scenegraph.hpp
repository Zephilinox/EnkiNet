#pragma once

//STD
#include <vector>
#include <map>
#include <functional>

//SELF
#include "Entity.hpp"

class Scenegraph
{
public:
	void update(float dt);
	void draw(sf::RenderWindow& window) const;

	void registerBuilder(std::string type, std::function<std::unique_ptr<Entity>(EntityInfo)> builder);

	Entity* createEntity(EntityInfo info);

	Entity* getEntity(uint32_t entityID);

private:
	std::map<uint32_t, std::unique_ptr<Entity>> entities;
	std::map<std::string, std::function<std::unique_ptr<Entity>(EntityInfo)>> builders;
};