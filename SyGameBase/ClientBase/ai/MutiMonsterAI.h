#pragma once 

/**
 * 怪物AI
 */

#include "scriptLib.h"
#include "cocos2d.h"
#include <bitset>
NS_CC_BEGIN
class MutiMonster;
class MutiAIStub;
class MonsterAILib;
class MutiEvent{
public:
	int tapTime; // 间隔时间
	int execCount; // 将触发的次数
	int execMaxCount;
	cc_timeval startTime; // 开启时间
	script::tixmlCodeNode* code; // 执行代码
	MutiEvent()
	{
		startTime.tv_sec = 0;
		code = NULL;
		tapTime = 0;
		execCount = 0;
		execMaxCount = 0;
	}
	bool checkTimeOut(cc_timeval & nowTime);
};
class MutiAI{
public:
	MutiAI()
	{
		id = 0;
	}
	DWORD id;
	/**
 	 * 执行一个事件
 	 * */
	bool action(MutiAIStub * stub,int event);
	/**
	 * 定时执行事件
	 */
	bool timer(MutiAIStub * stub,int event);
	enum{
		DEATH = 0, // 死亡
		BIRTH = 1, // 出生
		TARGET_ENTER = 2, // 物体进入视野
		TARGET_LEAVE = 3, // 物体离开视野
		ATTACK_ME = 4, // 被人攻击
		IDLE_ACTION = 5, // 空闲时间段
		MEET_TARGET = 6, // 对象在攻击范围内 
		HAD_TARGET = 7, // 有目标的状态
		HAD_TARGET_LEAVE = 8, // 有对象离开
		MOVE_TO_DESTIONATION = 9,// 移动到目的地
		TOUCH_TIME_OUT = 10, // 连续攻击时间过长
		ACTION_END = 11, // 攻击技能结束
	};
	/**
 	 * 增加一个code
 	 * \param code 代码节点
 	 * \param name 代码名称
 	 * \return ture 成功 false 失败
 	 * */
	bool addCode(script::tixmlCodeNode* code,script::tixmlCodeNode *info);

	void bindEvent(const std::string &name,script::tixmlCodeNode *code);

	void clearEvent(const std::string &name);

	unsigned int getEventIdByName(const std::string &name);
private:
	std::vector<script::tixmlCodeNode*> events;	
	std::vector<std::vector<MutiEvent> > timeEvts; // 带时间的事件 触发后会多次执行 事件在有效期内不可重入
};
typedef int (MonsterAILib::*AIAction)(MutiAIStub *,script::tixmlCodeNode *);

//执行点
class AILibAction{
public:
	int type;
	AIAction action;
};
class MutiMonsterRefrence
{
public:
	MutiMonster * monster;
	int uniqueId; // monster 唯一编号
	cc_timeval touchStartTime; // 刚进入持续targets时间
	int weight; // 权值
	bool isValid(){return true;} // 当前引用是否有效
	bool hadMeet; // 是否遇到
	MutiMonsterRefrence()
	{
		CCTime::gettimeofdayCocos2d(&touchStartTime,NULL);
		monster = NULL;
		uniqueId  = -1;
		weight = 0;
		hadMeet = false;
	}
	bool checkTimeOut(int timeout);

	MutiMonsterRefrence & operator=(const MutiMonsterRefrence &ref)
	{
		monster = ref.monster;
		uniqueId = ref.uniqueId;
		touchStartTime = ref.touchStartTime;
		weight = ref.weight;
		return *this;
	}
	std::bitset<32> notifyStates;
	bool checkNotify(int aiEvent)
	{
		return notifyStates.test(aiEvent);
	}
	void setNotify(int aiEvent)
	{
		notifyStates.set(aiEvent);
	}
	void resetNotify(int aiEvent)
	{
		notifyStates.reset(aiEvent);
	}
	
};
struct stTimeInfo{
public:
	cc_timeval startTime; // 开始时间
	stTimeInfo()
	{
		reset();
	}
	void reset()
	{
		 CCTime::gettimeofdayCocos2d(&startTime,NULL);
	}

	bool checkTimeOut(int timeout)
	{
		cc_timeval nowTime;
		CCTime::gettimeofdayCocos2d(&nowTime,NULL);
		float disTime = CCTime::timersubCocos2d(&startTime,&nowTime) / 1000;
		if (disTime >= timeout)
		{
			return true;
		}
		return false;
	}
};
/**
 * ai 的执行者
 * */
class MutiAIStub{
public:
	MutiMonsterRefrence tempRef; // 目前临时引用
	MutiMonster *npc;
	std::vector<MutiMonsterRefrence> targetPool; // 对象池 0 号对象为默认处理池
	typedef std::vector<MutiMonsterRefrence>::iterator TARGETPOOL_ITER;
	MutiAIStub()
	{
		npc = NULL;
		ai = NULL;
	}
	MutiAIStub(MutiMonster *npc)
	{
		this->npc = npc;
		ai = NULL;
	}
	void removeTarget();
	int getTargetCount(); // 当前对象的数量
	MutiMonster * getTarget(); // 获取当前对象
	MutiMonsterRefrence * getTargetRef(); // 获取当前对象池
	void addTarget(MutiMonster *monster); // 增加对象
	void pickSuitTarget(); // 挑选合适的对象
	std::bitset<32> notifyStates;
	bool checkNotify(int aiEvent)
	{
		return notifyStates.test(aiEvent);
	}
	void setNotify(int aiEvent)
	{
		notifyStates.set(aiEvent);
	}
	void resetNotify(int aiEvent)
	{
		notifyStates.reset(aiEvent);
	}
	stTimeInfo * getTimer(int id)
	{
		std::map<int,stTimeInfo>::iterator iter = times.find(id);
		if (iter != times.end())
		{
			return &iter->second;
		}
		return NULL;
	}
	void addTimer(int id)
	{
		stTimeInfo *timeInfo = getTimer(id);
		if (timeInfo)
		{
			timeInfo->reset();
		}
		else
		{
			times[id] = stTimeInfo();
		}
	}
	void delTimer(int id)
	{
		std::map<int,stTimeInfo>::iterator iter = times.find(id);
		if (iter != times.end())
		{
			 times.erase(iter);
		}
	}
	MutiAI *ai;
	std::map<int,stTimeInfo> times;
};
/**
 * 执行库
 * 加载代码库 和 事件池
 **/
class MonsterAILib:public script::Lib<AILibAction,MutiAIStub,MonsterAILib>
{
public:
	static MonsterAILib & getMe()
	{
		static MonsterAILib lib;
		return lib;
	}
	void initWithFile(const char *fileName);
    /**
     * 只有在scope="stub" 的时候 type 才有效
     */
    int var(MutiAIStub* stub,script::tixmlCodeNode * node)
    {
        return script::Lib<AILibAction,MutiAIStub,MonsterAILib>::var(stub,node);
    }
    /**
     * 输出
     */
    int print(MutiAIStub* stub,script::tixmlCodeNode * node)
    {
    	return script::Lib<AILibAction,MutiAIStub,MonsterAILib>::print(stub,node);
    }
    /**
     * 计算变量
     **/
    int calc(MutiAIStub* stub,script::tixmlCodeNode * node)
    {
    	return script::Lib<AILibAction,MutiAIStub,MonsterAILib>::calc(stub,node);
    }
    /**
     * 执行其他节点
     **/
    int exec(MutiAIStub* stub,script::tixmlCodeNode * node)
    {
        execCode(stub,node->getAttr("name"));
        return 0;
    }
    void bindActions();
    void takeNode(script::tixmlCodeNode *node);
    /**
     * 清除资源
     * */
    void destroy();
    /**
     * 执行事件
     * \param npcAIID ai 号
     * \param npc 怪物
     * \param event 事件
     * */
    void execEvent(DWORD npcAIID,MutiAIStub *stub,int event);
	void tapExecEvent(DWORD npcAIID,MutiAIStub *stub,int event);
    std::vector<MutiAI*> npcAis;
/////////////////////////////////////////////////////////////////////////    
//			相关扩展
////////////////////////////////////////////////////////////////////////
    /**
     * 触发技能
     * */    
    int putskill(MutiAIStub* stub,script::tixmlCodeNode * node);

	/**
	 * 行走到 目标
	 * <movetotarget/>
	 **/
	int movetotarget(MutiAIStub* stub,script::tixmlCodeNode * node);
	/**
	 * 设定距离最短的为当前攻击对象 重新设定lock对象
	 * <lockmindistacetarget/>
	 */
	int lockmindistacetarget(MutiAIStub* stub,script::tixmlCodeNode * node);
	/**
	 * 将权值最大者锁定 重新设定
	 * <locksuittarget/>
	 **/
	int locksuittarget(MutiAIStub* stub,script::tixmlCodeNode * node);
	/**
	 * 行走到目的地
	 * <move targetx="" targety=""/>
	 */
	int move(MutiAIStub* stub,script::tixmlCodeNode * node);
	/**
	 * 沿着目标绕圈
	 * <moverandarround/>
	 */
	int moverandarround(MutiAIStub* stub,script::tixmlCodeNode * node);
	
	/**
	 * 清除当前移动路径
	 */
	int clearmovepath(MutiAIStub* stub,script::tixmlCodeNode * node);
	
	/**
	 * 设置当前攻击时间
	 */
	int reset_touch_lasttime(MutiAIStub* stub,script::tixmlCodeNode * node);
	/**
	 * 检查攻击的持续时间
	 */
	int check_touch_lasttime(MutiAIStub* stub,script::tixmlCodeNode * node);

	/**
	 * 检查当前的位置
	 */
	int checknowposition(MutiAIStub* stub,script::tixmlCodeNode * node);

	/**
	 * 更改事件响应函数
	 **/
	int bind_event(MutiAIStub* stub,script::tixmlCodeNode * node);
	/**
	 * 清除事件
	 **/
	int clear_event(MutiAIStub* stub,script::tixmlCodeNode * node);
	/**
	 * 检查当前的目标
	 */
	int checknowtarget(MutiAIStub* stub,script::tixmlCodeNode * node);


	/**
	 * 重置事件触发 由于事件触发后不再进行 触发了 需要有此机制
	 **/
	int reset_notify(MutiAIStub* stub,script::tixmlCodeNode * node);
	
	/**
	 * 增加时间
	 */
	int add_timer(MutiAIStub* stub,script::tixmlCodeNode * node);

	/**
	 * 检查时间
	 **/
	int check_timer(MutiAIStub* stub,script::tixmlCodeNode * node);

	/**
	 * 重置时间
	 */
	int reset_timer(MutiAIStub* stub,script::tixmlCodeNode * node);

	/**
	 * 删除时间
	 */
	int del_timer(MutiAIStub* stub,script::tixmlCodeNode * node);

	/**
	 * 检查怪物当前行为
	 */
	int check_now_monster_action(MutiAIStub* stub,script::tixmlCodeNode * node);
};

#define theAILib MonsterAILib::getMe()

NS_CC_END
