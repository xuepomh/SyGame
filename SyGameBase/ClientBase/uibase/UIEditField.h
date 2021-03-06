#pragma once
#include "cocos2d.h"
#include "UIBase.h"
#include "CursorTextField.h"
NS_CC_BEGIN
class UIEditField:public UIBase{
public:
	/**
	 * 创建编辑框
	 */
	static UIEditField * create(const CCPoint &position,const CCSize &size,const char *defaultText); 
	
	bool init(const char *defaultText);

	/**
	 * 设置位置
	 */
	virtual void setPosition(float x,float y);
	
	/**
	 * 设置大小
	 */
	virtual void setSize(float w,float h) ;
	 /** 
	 * 检查是否在区域里
	 */
	virtual bool touchDown(float x,float y);
	/**
	 * 更新位置
	 */
	virtual bool touchMove(float x,float y) ;
	/**
	 * 停止拖动
	 */
	virtual bool touchEnd(float x,float y);

	/**
	 * 设置为可编辑模式
	 */
	virtual void setEditable(bool tag);
	std::string getContent();
	void setContent(const char *content);

	static UIEditField * create();
	void beLoaded();
	/**
	 * 创建父节点下的子节点
	 */
	virtual TiXmlElement * makeNode(TiXmlElement *parent = NULL,const std::string &name="base");
private:
	bool _editable;
	bool _touchIn;
	CursorTextField *_field;
	CCPoint nowTouchPoint;
	std::string defaultContent;
	UIEditField()
	{
		_touchIn = false;
		_editable = false;
		_field = NULL;
		uiType = UIBase::UI_TEXT_FIELD;
	}
};

NS_CC_END