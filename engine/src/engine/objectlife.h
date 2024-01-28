#ifndef OBJECTLIFE_H
#define OBJECTLIFE_H

#include <memory>

class ObjectLifeEntity;

class ObjectLife{
	friend class ObjectLifeEntity;
protected:
	ObjectLife();
	virtual ~ObjectLife();
	void createLife();
	void maybeDestroy();
	void reset();
	virtual void destroy() = 0;

public:
	static bool isCleaned();

private:
	bool destroyChecked = false;
	//int objectId = 0;
	std::shared_ptr<ObjectLifeEntity> lifeEntity = nullptr;
	//ObjectLifeEntity* pointer = nullptr;
};

class ObjectLifeEntity{
public:
	ObjectLifeEntity();
	~ObjectLifeEntity();
};

#endif // OBJECTLIFE_H
