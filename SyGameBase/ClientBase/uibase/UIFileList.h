#pragma once

/**
 * ��Ϸ������
 */
#include "cocos2d.h"
#include "UIPanel.h"
#include "UIWindow.h"
#include "UIList.h"
NS_CC_BEGIN

/**
 * �ļ���Ŀ
 */
class UIFileItem:public BaseUIItem<UIFileItem>{
public:
	enum FILETYPE{
		__DIR__,
		__FILE_
	};
	void setFileType(FILETYPE type);
	FILETYPE nowType;
};
/**
 * �ļ��б�
 */
class UIFileList:public BaseDialog<UIFileList>{
public:

	/**
	 * ��ʼ��List �Ͱ��¼�
	 */
	virtual void doInitEvent();

	void show(const char *root);
};

NS_CC_END