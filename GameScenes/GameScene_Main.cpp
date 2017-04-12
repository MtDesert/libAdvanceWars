#include"GameScene_Main.h"

#define MIN_TIME_SLICE 1000
#define SLICE_FADEIN  1000000
#define SLICE_DELAY   2000000
#define SLICE_FADEOUT 1000000

enum Status{
	FadeIn,Delay,FadeOut,StatusOver
};

GameScene_Main::GameScene_Main():status(0),countDown(0),contentBrightness(0){
	//主菜单
	menuItemNames.push_back("单机模式");
	menuItemNames.push_back("联网模式");
	menuItemNames.push_back("军事资料");//查看所有资料
	menuItemNames.push_back("军事部署");
	menuItemNames.push_back("军事记录");
	menuItemNames.push_back("游戏设定");
	menuItemNames.push_back("关于本作");
}
static void singleMode(){//单机模式
	//剧情模式
	//训练任务(单图模式,多图模式)
	//自由训练(单图模式,多图模式)
	//生存模式(金钱,回合,时间)
	//厮杀模式
}
static void networkMode(){//联网模式
	//玩家注册(密码认证,设备认证)
	//玩家登陆(密码认证,设备认证)
	//玩家信息(修改登陆密码,昵称,头像,全身像,邮箱)
	//查看留言(基于玩家列表)
	//查看在线玩家
	//玩家间联机(以后做)
	
	//开战(默认所有玩家,后期考虑可向局部玩家宣战)
	//参战(从宣战列表中找出可以让自己参战的宣战)
	//观战(查看别人的战斗,雾战禁止查看,后期可以考虑授权给好友查看雾战中己方的情况,但好友泄密的后果由玩家自己负责)
	//联赛(可以随时参加和退出,从所有参加者中匹配实力比较相近的对手玩家,所有玩家选择好CO后,系统随机选择地图来参战)
	//玩家战局(当前玩家的战局)
}

static void militaryFile(){//军事资料
	//兵种资料(AWDS和AWDOR兵种资料,可能有FamicomWars的兵种)
	//CO资料(包含AW1,AW2,AWDS,AWDOR的CO数据(可能会包含FamicomWars的CO),会出现同一个人物在不同版本中的能力差异,这很正常,后期可以加入网友自创的CO(需要进行技术实现难度的评估))
	//地形资料(AWDS和AWDOR地形资料,可能有黑洞发明的设施和FamicomWars的地形)
	//部队资料(官方有橙星,蓝月,绿地,黄彗,黑洞,加上AWDOR的势力和AWBW的势力,以及中立势力共20个)
	//损伤计算器(基于官方的损伤值,如果混合了其它版本的兵种,则可能含有近似的损伤值)
	//损伤计算器(基于民间的武器护甲公式,这个数值平衡不好调....)
	
	//音频资料(可不做)
	//图像资料(可不做)
}
static void militaryDeploy(){//军事部署
	//绘制地图
	//CO换装
}

void GameScene_Main::consumeTimeSlice(){
	switch(status){
		case FadeIn://1秒显示内容
			countDown+=MIN_TIME_SLICE;
			contentBrightness=countDown*255/SLICE_FADEIN;
			if(countDown>=SLICE_FADEIN){//状态切换
				countDown=SLICE_DELAY;
				status=Delay;
				printf("Fade in over\n");
			}
		break;
		case Delay://2秒停顿
			countDown-=MIN_TIME_SLICE;
			if(countDown<=0){
				countDown=SLICE_FADEOUT;
				status=FadeOut;
				printf("Delay over\n");
			}
		break;
		case FadeOut://1秒消失
			countDown-=MIN_TIME_SLICE;
			contentBrightness=countDown*255/SLICE_FADEOUT;
			if(countDown<0){
				countDown=0;
				status=StatusOver;
				printf("Fade out over\n");
			}
		break;
		default:;
	}
}