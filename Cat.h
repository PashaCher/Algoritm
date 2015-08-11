#ifndef __CAT_SPRITE_H__
#define __CAT_SPRITE_H__

#include "cocos2d.h"
USING_NS_CC;

class MainScene;

class Cat : public cocos2d::Sprite
{
	class ShortestPathStep : public cocos2d::Object
	{
	public:
		ShortestPathStep();
		~ShortestPathStep();
		static ShortestPathStep *createWithPosition(const cocos2d::Point &pos);
		bool initWithPosition(const cocos2d::Point &pos);
		int getFScore() const;
		bool isEqual(const ShortestPathStep *other) const;
		std::string getDescription() const;
		CC_SYNTHESIZE(cocos2d::Point, _position, Position);
		CC_SYNTHESIZE(int, _gScore, GScore);
		CC_SYNTHESIZE(int, _hScore, HScore);
		CC_SYNTHESIZE(ShortestPathStep*, _parent, Parent);
	};
public:
	Cat();
	~Cat();
	static Cat *createWithLayer(MainScene *layer);
	bool initWithLayer(MainScene *layer);
	void moveToward(const cocos2d::Point &target);
	void popStepAndAnimate();
	cocos2d::Vector<ShortestPathStep*> _shortestPath;
	cocos2d::Animation *createCatAnimation(const char *animType);
	cocos2d::Vector<ShortestPathStep*> _spOpenSteps;
	cocos2d::Vector<ShortestPathStep*> _spClosedSteps;
	void insertInOpenSteps(Cat::ShortestPathStep *step);
	int computeHScoreFromCoordToCoord(const Point& fromCoord, const Point &toCoord);
	int costToMoveFromStepToAdjacentStep(const ShortestPathStep *fromStep, const ShortestPathStep *toStep);
	ssize_t getStepIndex(const cocos2d::Vector<Cat::ShortestPathStep *> &steps, const Cat::ShortestPathStep *step);
	void constructPathAndStartAnimationFromStep(Cat::ShortestPathStep *step);
	MainScene *_layer;
};

#endif
