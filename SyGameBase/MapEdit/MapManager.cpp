#include "MapManager.h"
#include "MutiMonster.h"
NS_CC_BEGIN
UIWindow * MapManager::getWindow()
{
	return window;
}
MutiMap *MapManager::getMap()
{
	if (!nowMap) return mapRoot;
	return nowMap;
}
CCScene *MapManager::getScene()
{
	return scene;
}
struct stAddBg:public stExecEachBackgroud{
	void exec(MutiMap *map)
	{
		mapList.push_back(map);
	}
	stAddBg(std::list<MutiMap *>& mapList):mapList(mapList)
	{
	
	}
	std::list<MutiMap *>& mapList;
};
void MapManager::replaceMap(MutiMap *map)
{
	if (this->mapRoot)
	{
		mapRoot->save();
		mapRoot->removeAllChildrenWithCleanup(true);
		scene->removeChild(this->mapRoot,true);
	}
	this->mapRoot = map;
	if (scene)
		scene->addChild(map,-2);
	mapList.clear();
	stAddBg exec(mapList);
	map->execEachBg(&exec);
}
void MapManager::addMap(MutiMap *map)
{
	if (nowMap) nowMap->setVisible(false); 
	nowMap = map;
	mapList.push_back(map);
}
void MapManager::choiceMap(MutiMap *map)
{
	nowMap = map;
	bool had = false;
	for (MAPLIST_ITER iter = mapList.begin(); iter != mapList.end();++iter)
	{
		if (*iter == map)
		{
			had = true;
		}
		//(*iter)->hide();
	}
	if (!had) mapList.push_back(map);
	nowMap->show();
}
void MapManager::doTouch(int touchType,const CCPoint &touchPoint)
{
	MutiMap *map = getMap();
	if (!map) return;
	switch(touchType)
	{
		case UIBase::TOUCH_DOWN:
		{
			if (map->touchDown(touchPoint.x,touchPoint.y))
			{
				return;
			}
			if (isSetBlock)
			{
				map->setBlockByTouchPoint(touchPoint,blockValue);
				map->showGrids();
				return;
			}
			nowObject = map->pickObject(touchPoint);
			nowTouchPoint = touchPoint;
			if (nowObject && nowObject->objectType == MutiObject::MOSTER_TYPE)
			{
				nowMonster = (MutiMonster*) nowObject;
				nowMonster->map->clearBlock(nowMonster->getNowIndex(),GridIndex::MONSTER_BLOCK);
			}
			
		}break;
		case UIBase::TOUCH_MOVE:
		{
			if (map->touchMove(touchPoint.x,touchPoint.y)) return;
			if (isSetBlock) return;
			if (nowObject)
			{
				CCPoint nowPoint = nowObject->getPosition();
				nowObject->setPosition(ccp(nowPoint.x + touchPoint.x - nowTouchPoint.x,
                                  nowPoint.y + touchPoint.y - nowTouchPoint.y));
				nowTouchPoint = touchPoint;
			
				nowObject->freshBlock();
				return;
			}
			else
			{
				CCPoint nowPoint = map->getPosition();
				map->setPosition(ccp(nowPoint.x + touchPoint.x - nowTouchPoint.x,
                                  nowPoint.y + touchPoint.y - nowTouchPoint.y));
				nowTouchPoint = touchPoint;
				return;
			}
			
		}break;
		case UIBase::TOUCH_END:
		{
			if (nowMonster)
			{
				//nowMonster->map->setBlock(nowMonster->getNowIndex(),GridIndex::MONSTER_BLOCK);
				nowMonster->freshBlock();
			}
			map->touchEnd(touchPoint.x,touchPoint.y);
			nowObject = NULL;
		}break;
	}
}
NS_CC_END