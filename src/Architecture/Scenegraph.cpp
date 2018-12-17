#include "Scenegraph.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>

void Scenegraph::update(float dt)
{
	for (auto& ent : entities)
	{
		ent.second->update(dt);
	}
}

void Scenegraph::draw(sf::RenderWindow& window) const
{
	for (const auto& ent : entities)
	{
		ent.second->draw(window);
	}
}

void Scenegraph::registerBuilder(std::string type, std::function<std::unique_ptr<Entity>(EntityInfo)> builder)
{
	builders[type] = builder;
}

Entity* Scenegraph::createEntity(EntityInfo info)
{
	static int ID = 0;
	info.ID = ID++;
	entities[info.ID] = builders.at(info.type)(info);
	return entities[info.ID].get();
}

Entity* Scenegraph::getEntity(uint32_t entityID)
{
	return entities.at(entityID).get();
}