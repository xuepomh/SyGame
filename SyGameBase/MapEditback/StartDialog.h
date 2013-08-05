#pragma once
/**
 * ��Ϸ������
 */
#include "cocos2d.h"
#include "UIPanel.h"
#include "UIItem.h"
#include "UIChoiceList.h"
#include "UIBag.h"
#include "UILabel.h"
#include "UIEditField.h"
#include "UIWindow.h"
NS_CC_BEGIN

/**
 * �������Button �� sysui
 */
class MainDialog:public BaseDialog<MainDialog>{
public:
	virtual void doInitEvent();

	void openMap(UIBase *base);
	void saveMap(UIBase *base);
	void createCartoon(UIBase *base);
	
	void createImage(UIBase *base);
	
	void createBigImage(UIBase *base);

	void showMapProp(UIBase *base);

	void editBlock(UIBase *base);

	void showControlMonster(UIBase *base);
};
NS_CC_END