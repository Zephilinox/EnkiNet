#include "Scenegraph.hpp"

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
	entities[info.ID] = builders.at(info.type)(info);
	return entities[info.ID].get();
}

Entity* Scenegraph::getEntity(uint32_t entityID)
{
	return entities.at(entityID).get();
}

/*Entity* Scenegraph::getParent(uint32_t entityID)
{
	return getEntity(getEntity(entityID)->info.parentID);
}

std::vector<Entity*> Scenegraph::getChildren(uint32_t entityID)
{
	auto ids = parents.at(entityID).children;
	std::vector<Entity*> children;
	children.reserve(ids.size());

	for (int i = 0; i < ids.size(); ++i)
	{
		children.push_back(getEntity(ids[i]));
	}

	return children;
}

std::vector<Entity*> Scenegraph::getChildrenRecursively(uint32_t entityID)
{
	return std::vector<Entity*>();
}*/
