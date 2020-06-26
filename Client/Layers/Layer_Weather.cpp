#include "Layer_Weather.h"
#include"Game_AdvanceWars.h"

//实验性静态变量
#define PARTICLE_AMOUNT 1000
static ShapeRenderer sr;

Layer_Weather::Layer_Weather():particleSpeed(-8,-8),particleSpeedDec(-1,-1),particleSpeedInc(1,1),
particleColor(ColorRGBA::White),particleLength(8),particleStyle(StylePoint){
	GAME_AW
	size.setXY(Game::resolution.x,Game::resolution.y);
	//粒子运动相关
	game->timeSliceList.pushTimeSlice(this,20,40);
}
Layer_Weather::~Layer_Weather(){}

//天气粒子
void WeatherParticle::move(){position=position+moveSpeed;}

void Layer_Weather::setParticleAmount(SizeType amount,ParticleStyle style){
	allParticles.setSize(amount,true);
	particleStyle=style;
	//初始化粒子
	auto rect=rectF();
	allParticles.forEach([&](WeatherParticle &wp){
		restartParticle(wp,rect);
	});
}
void Layer_Weather::restartParticle(WeatherParticle &wp,const Rectangle2D<float> &rect){
	//初始化速度
	auto &spd(wp.moveSpeed);
	spd=particleSpeed;
	spd.x += Number::randomFloat(particleSpeedDec.x,particleSpeedInc.x);
	spd.y += Number::randomFloat(particleSpeedDec.y,particleSpeedInc.y);
	if(spd.x==spd.y && spd.x==0){//极端情况
		spd.setXY(-1,-1);
	}
	//根据速度调整出现位置
	auto w(rect.width()),h(rect.height());
	auto len=0.0;
	bool nearVertical=abs(spd.x)<abs(spd.y);
	if(nearVertical){//速度方向偏向纵向
		len = w + h*abs(spd.x/spd.y);
	}else{//速度方向偏向横向
		len = h + w*abs(spd.y/spd.x);
	}
	auto result=Number::randomFloat(0,len);
	if(nearVertical){
		if(result<w){//出现在上或下
			wp.position.setXY(rect.left()+result,spd.y>0 ? rect.bottom() : rect.top());
		}else{//出现在左或右
			wp.position.setXY(spd.x>0 ? rect.left() : rect.right(),rect.bottom()+(result-w)*abs(spd.y/spd.x));
		}
	}else{
		if(result<h){//出现在左或右
			wp.position.setXY(spd.x>0 ? rect.left() : rect.right(),rect.bottom()+result);
		}else{//出现在上或下
			wp.position.setXY(rect.left()+(result-h)*abs(spd.x/spd.y),spd.y>0 ? rect.bottom() : rect.top());
		}
	}
}

void Layer_Weather::consumeTimeSlice(){
	//粒子移动
	auto rect=rectF();
	allParticles.forEach([&](WeatherParticle &wp){
		wp.move();//粒子运动
		if(!rect.containPoint(wp.position)){//超出范围
			restartParticle(wp,rect);//重置粒子
		}
	});
}
void Layer_Weather::renderX()const{
	sr.edgeColor=&particleColor;
	auto sz=allParticles.size();
	WeatherParticle *wp=nullptr;
#define FOREACH_PARTICLE(code) \
for(SizeType i=0;i<sz;++i){\
	wp=allParticles.data(i);\
	code;\
}
	switch(particleStyle){
		case StylePoint:
			FOREACH_PARTICLE(sr.drawPoint(wp->position))
		break;
		case StyleLine:{
			Point2D<float> p;
			auto speed=(float)particleSpeed.distance();
			if(speed>0 && particleLength){
				FOREACH_PARTICLE(
					p=wp->moveSpeed*particleLength;
					p=p/speed;
					sr.drawLine(wp->position,wp->position + p)
				)
			}
		}break;
		case StyleTexture:break;
		default:;
	}
}