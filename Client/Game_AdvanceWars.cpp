#include"Game_AdvanceWars.h"
#include"Layer_Conversation.h"

//所有对话框
#define ALL_DIALOGS(MACRO)\
MACRO(NewMap)

//声明
ALL_SCENES(GAME_SCENE_DECLARE)
ALL_DIALOGS(GAME_DIALOG_DECLARE)

Game_AdvanceWars::Game_AdvanceWars(){
	gameSettings=&settings;//配置
	//战场数据
	battleField.corpsList=&mCorpsList;
	battleField.troopsList=&mTroopsList;
	battleField.terrainsList=&mTerrainCodesList;
	//战役数据
	campaign.battleField=&battleField;
	campaign.commandersList=&mCommandersList;
	campaign.weathersList=&mWeathersList;
	//错误处理函数
	battleField.whenError = luaState.whenError = whenError;
}
Game_AdvanceWars::~Game_AdvanceWars(){
	//删除场景
	ALL_SCENES(GAME_DELETE_SCENE)
	ALL_DIALOGS(GAME_DELETE_DIALOG);
	//清除数据
	mCorpsList.clear();
	mCommandersList.clear();
	mTerrainCodesList.clear();
	mTroopsList.clear();
	mWeathersList.clear();
	//清除纹理
	clearAllTextureCache();
}

Game* Game::newGame(){
	game=new Game_AdvanceWars();
	return game;
}
Game_AdvanceWars* Game_AdvanceWars::currentGame(){
	return dynamic_cast<Game_AdvanceWars*>(game);
}
string Game_AdvanceWars::gameName()const{return"AdvanceWars";}

void Game_AdvanceWars::restart(){
	//进入场景
	auto scene=gotoScene_Logo();
	scene->reset();
	scene->logoText.setString("AdvanceWars_LifeTime",true);
	scene->whenLogoOver=[&](){gotoScene_Main(true);};
	//gotoScene_Main(true);
}

//场景跳转
#define SCENE_DEFINE(Name) GAME_GOTOSCENE_DEFINE(Game_AdvanceWars,Name)
ALL_SCENES(SCENE_DEFINE)
#undef SCENE_DEFINE
//对话框
GAME_SHOWDIALOG_DEFINE(Game_AdvanceWars,NewMap)

#define AW_LOAD_LUA(mList,dataName) if(mList.size()==0)mList.loadFile_lua(settings.dataName,whenError);
bool Game_AdvanceWars::loadAllConfigData(){
	AW_LOAD_LUA(mTerrainCodesList,dataTerrainCodes)//地形表
	AW_LOAD_LUA(mCorpsList,dataCorps)//兵种表
	AW_LOAD_LUA(mTroopsList,dataTroops)//势力表
	AW_LOAD_LUA(mCommandersList,dataCommanders)//CO表
	AW_LOAD_LUA(mWeathersList,dataWeathers)//天气表
	//规则数据
	if(!campaign.luaState){
		campaign.luaState=&luaState;
		campaign.registerDefaultScriptFunction();
		damageCaculator.luaState=&luaState;
		luaState.doFile(settings.ruleMove);//移动规则
		luaState.doFile(settings.ruleLoadUnit);//装载单位规则
		luaState.doFile(settings.ruleBuild);//装载建造规则
		luaState.doFile(settings.ruleCommanders);//指挥官规则
		luaState.doFile(settings.ruleDamage);//损伤规则
	}
	//引用传递
	damageCaculator.campaign=&campaign;
	campaign.damageCaculator=&damageCaculator;
	return true;
}
bool Game_AdvanceWars::loadAllTextures(){
	loadTerrainsTextures();
	loadCorpsTextures(corpsIconsArray,settings.imagesPathCorps);
	loadCorpsTextures(corpsImagesArray,settings.imagesPathCorpsImages);
	loadTroopsTextures();
	loadCommandersTextures();
	loadAllIconsTextures();
	//关联小图标
	loadMapEditMenuTextures();
	laodCampaignMenuTextures();
	loadCorpMenuTextures();
	//数字
	numbersTextures.setSize(10,true);
	for(SizeType i=0;i<numbersTextures.size();++i){
		char ch[2];
		sprintf(ch,"%d",(int)i);
		auto tex=numbersTextures.data(i);
		tex->deleteTexture();
		tex->texImage2D_FilePNG(settings.imagesPathNumbers+"/"+ch+".png",whenError);
	}
	return true;
}

void Game_AdvanceWars::clearAllTextureCache(){
	commandersHeadTextures.clearCache();
	commandersBodyTextures.clearCache();
	terrainsTexturesArray.clearCache();
	corpsIconsArray.clearCache();
	troopsTextures.clearCache();
	allIconsTextures.clearCache();
	numbersTextures.clearCache();
	corpMenuTextures.clearCache();
}

#define FORCE_LOAD_CHECK(texArray) \
if(forceReload)texArray.clearCache();\
if(texArray.size())return;

void Game_AdvanceWars::loadCorpsTextures(TextureCacheArray &corpsTexArray,const string &imagePath,bool forceReload){
	//强行重新加载一定要清除数据
	FORCE_LOAD_CHECK(corpsTexArray)
	//未加载,重新加载
	corpsTexArray.setSize(mCorpsList.size(),true);
	int corpIndex=0;
	for(auto &corp:mCorpsList){
		auto texArr=corpsTexArray.data(corpIndex);
		texArr->setSize(mTroopsList.size(),true);
		//开始用png调色
		FilePNG filePng;
		filePng.loadFile(imagePath+"/"+corp.name+".png",whenError);
		filePng.parseData();
		auto plte=filePng.findPLTE();
		if(plte){
			auto i=0;
			for(auto &troop:mTroopsList){//根据部队配色表来进行调色
				for(int i=0;i<4;++i){
					plte->setColor(6+i,troop.colors[i]);
				}
				//完成配色,生成纹理
				auto tex=texArr->data(i);
				tex->texImage2D(filePng);
				//下一个
				++i;
			}
		}
		//调色完毕,下一个
		filePng.memoryFree();
		++corpIndex;
	}
}
void Game_AdvanceWars::loadCommandersTextures(bool forceReload){
	FORCE_LOAD_CHECK(commandersHeadTextures)
	//开始加载
	commandersHeadTextures.setSize(mCommandersList.size(),true);
	commandersBodyTextures.setSize(mCommandersList.size(),true);
	auto coID=0;
	for(auto &commander:mCommandersList){
		//头像纹理
		auto tex=commandersHeadTextures.data(coID);
		if(tex){
			tex->texImage2D_FilePNG(settings.headImagePath+"/"+commander.name+".png",whenError);
		}
		//身体纹理
		tex=commandersBodyTextures.data(coID);
		if(tex){
			tex->texImage2D_FilePNG(settings.bodyImagePath+"/"+commander.name+".png",whenError);
		}
		//下一个coID
		++coID;
	}
}
void Game_AdvanceWars::loadTroopsTextures(bool forceReload){
	FORCE_LOAD_CHECK(troopsTextures)
	//开始加载
	troopsTextures.setSize(mTroopsList.size(),true);
	auto troopIndex=0;
	for(auto &troop:mTroopsList){
		auto tex=troopsTextures.data(troopIndex);
		if(tex){
			tex->texImage2D_FilePNG(settings.imagesPathTroops+"/"+troop.name+".png",whenError);
		}
		//下一个
		++troopIndex;
	}
}
void Game_AdvanceWars::loadTerrainsTextures(bool forceReload){
	FORCE_LOAD_CHECK(terrainsTexturesArray)
	//未加载,重新加载
	terrainsTexturesArray.setSize(mTerrainCodesList.size(),true);
	int trnIndex=0;
	for(auto &trnCode:mTerrainCodesList){
		auto texArr=terrainsTexturesArray.data(trnIndex);
		if(trnCode.capturable){//据点
			texArr->setSize(mTroopsList.size(),true);
			//开始用png调色
			FilePNG filePng;
			filePng.loadFile(settings.imagesPathTerrainCodes+"/"+trnCode.name+".png",whenError);
			filePng.parseData();
			//开始调色
			auto plte=filePng.findPLTE();
			if(plte){
				auto i=0;
				for(auto &troop:mTroopsList){//根据部队配色表来进行调色
					for(int i=0;i<4;++i){
						plte->setColor(2+i,troop.colors[i]);
						ColorRGBA tmp;
						tmp.fromBGRA(troop.colors[i]);
					}
					//完成配色,生成纹理
					auto tex=texArr->data(i);
					tex->texImage2D(filePng);
					//下一个
					++i;
				}
			}
			//调色完毕
			filePng.memoryFree();
		}else if(trnCode.has4direction){//有方向性的图块
			auto amount=16;
			char num[3];
			texArr->setSize(amount,true);
			for(int i=0;i<amount;++i){
				sprintf(num,"%X",i);
				auto tex=texArr->data(i);
				tex->texImage2D_FilePNG(settings.imagesPathTerrainCodes+"/"+trnCode.name+"s/"+trnCode.name+"_"+num+".png",whenError);
			}
		}else{//其他地形
			texArr->setSize(1,true);
			auto tex=texArr->data(0);
			tex->texImage2D_FilePNG(settings.imagesPathTerrainCodes+"/"+trnCode.name+".png",whenError);
		}
		//下一个
		++trnIndex;
	}
}

#define COPY_ICONS_TEX(menu,prefix,name) \
texA=menu.data(prefix##name);\
texB=allIconsTextures.value(#name);\
if(texA && texB){*texA=*texB;}

void Game_AdvanceWars::loadMapEditMenuTextures(bool forceReload){
	FORCE_LOAD_CHECK(mapEditMenuTextures)
	mapEditMenuTextures.setSize(Scene_BattleField::AmountOfEnumMapEditCommand,true);
	Texture *texA=nullptr,*texB=nullptr;
	//根据定义进行加载
#define MAKE_ICON(name) COPY_ICONS_TEX(mapEditMenuTextures,Scene_BattleField::MapEdit_,name)
	BATTLEFIELD_EDIT_MAP_MENU(MAKE_ICON)
#undef MAKE_ICON
}
void Game_AdvanceWars::laodCampaignMenuTextures(bool forceReload){
	FORCE_LOAD_CHECK(campaignMenuTextures)
	campaignMenuTextures.setSize(Scene_BattleField::AmountOfEnumCampaignCommand,true);
	Texture *texA=nullptr,*texB=nullptr;
	//根据定义进行加载
#define MAKE_ICON(name) COPY_ICONS_TEX(campaignMenuTextures,Scene_BattleField::Campaign_,name)
	BATTLEFIELD_CAMPAIGN_MENU(MAKE_ICON)
#undef MAKE_ICON
}
void Game_AdvanceWars::loadCorpMenuTextures(bool forceReload){
	FORCE_LOAD_CHECK(corpMenuTextures)
	corpMenuTextures.setSize(Campaign::AmountOfCorpEnumMenu,true);
	Texture *texA=nullptr,*texB=nullptr;
	//根据定义进行加载
#define MAKE_ICON(name) COPY_ICONS_TEX(corpMenuTextures,Campaign::Menu_,name)
	CAMPAIGN_CORPMENU(MAKE_ICON)
#undef MAKE_ICON
}
void Game_AdvanceWars::loadAllIconsTextures(bool forceReload){
	FORCE_LOAD_CHECK(allIconsTextures)
	Directory dir;
	if(dir.changeDir(settings.imagesPathIcons,whenError)){
		for(auto &entry:dir.direntList){//依次加载图片
			if(!entry.isRegularFile())continue;//只加载正常文件
			Texture tex;
			tex.texImage2D_FilePNG(settings.imagesPathIcons+"/"+entry.name(),whenError);
			auto name=entry.name();
			auto nm=name.substr(0,name.find_last_of("."));//去掉扩展名
			allIconsTextures.insert(nm,tex);
		}
	}
}

void Game_AdvanceWars::notDone(){
	whenError("尚未实现");
}
