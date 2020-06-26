#ifndef LAYER_WEATHER_H
#define LAYER_WEATHER_H

#include"GameLayer.h"

//天气粒子效果
struct WeatherParticle{
	Point2D<float> position,moveSpeed;//粒子位置,粒子移动速度
	void move();//移动
};
/*天气层,用于显示天气特效*/
class Layer_Weather:public GameLayer{
	Array<WeatherParticle> allParticles;//所有粒子
public:
	Layer_Weather();
	~Layer_Weather();

	Point2D<float> particleSpeed,particleSpeedDec,particleSpeedInc;//粒子速度,及其变化波动
	ColorRGBA particleColor;//粒子颜色
	int particleLength;//粒子长度(线类型专用)

	enum ParticleStyle{
		StylePoint,//点
		StyleLine,//线
		StyleTexture,//纹理
		AmountOfParticleStyle
	};
	ParticleStyle particleStyle;//粒子风格

	void setParticleAmount(SizeType amount,ParticleStyle style);//设置粒子数量和风格
	void restartParticle(WeatherParticle &wp,const Rectangle2D<float> &rect);//重新初始化粒子

	virtual void consumeTimeSlice();//让粒子运动
	virtual void renderX()const;//渲染效果,决定如何渲染粒子
};
#endif