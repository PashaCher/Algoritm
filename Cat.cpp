#include "Cat.h"
#include "MainScene.h"

#define MOVEBONUS 1000

USING_NS_CC;

Cat::ShortestPathStep::ShortestPathStep() : _position(Point::ZERO), _gScore(0), _hScore(0), _parent(nullptr)
{
}

Cat::ShortestPathStep::~ShortestPathStep()
{
}

Cat::ShortestPathStep *Cat::ShortestPathStep::createWithPosition(const Point &pos)
{
	ShortestPathStep *pRet = new ShortestPathStep();
	if (pRet && pRet->initWithPosition(pos))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool Cat::ShortestPathStep::initWithPosition(const Point &pos)
{
	bool bRet = false;
	do
	{
		this->setPosition(pos);
		bRet = true;
	} while (0);

	return bRet;
}

int Cat::ShortestPathStep::getFScore() const
{
	return this->getGScore() + this->getHScore();
}

bool Cat::ShortestPathStep::isEqual(const Cat::ShortestPathStep *other) const
{
	return this->getPosition() == other->getPosition();
}

std::string Cat::ShortestPathStep::getDescription() const
{
	return StringUtils::format("pos=[%.0f;%.0f]  g=%d  h=%d  f=%d", this->getPosition().x, this->getPosition().y, this->getGScore(), this->getHScore(), this->getFScore());
}

Cat::Cat() 
{
}

Cat::~Cat()
{

}

void Cat::insertInOpenSteps(Cat::ShortestPathStep *step)
{
	int stepFScore = step->getFScore();
	ssize_t count = _spOpenSteps.size();
	ssize_t i = 0;
	for (; i < count; ++i)
	{
		if (stepFScore <= _spOpenSteps.at(i)->getFScore())
		{
			break;
		}
	}
	_spOpenSteps.insert(i, step);
}

int Cat::computeHScoreFromCoordToCoord(const Point &fromCoord, const Point &toCoord)
{
	return abs(toCoord.x - fromCoord.x) + abs(toCoord.y - fromCoord.y);
}

int Cat::costToMoveFromStepToAdjacentStep(const ShortestPathStep *fromStep, const ShortestPathStep *toStep)
{
	//if (_layer->bObjTmp(toStep->getPosition()))
		return MOVEBONUS;
	return 1;
}

Cat *Cat::createWithLayer(MainScene *layer)
{
	Cat *pRet = new Cat();
	if (pRet && pRet->initWithLayer(layer))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool Cat::initWithLayer(MainScene *layer)
{
	bool bRet = false;
	do
	{
		CC_BREAK_IF(!this->initWithFile("Cat.png"));
		bRet = true;
		_layer = layer;

	} while (0);
	return bRet;
}

void Cat::moveToward(const Point &target)
{
	Point fromTileCoord = _layer->tileCoordForPosition(this->getPosition());
	Point toTileCoord = _layer->tileCoordForPosition(target);
	//ssize_t count = _spOpenSteps.size();
	if (fromTileCoord == toTileCoord)
	{
		CCLOG("You're already there! :P");
		return;
	}
	if (!_layer->isValidTileCoord(toTileCoord) || _layer->isWallAtTileCoord(toTileCoord))
	{
		return;
	}
	CCLOG("From: %f, %f", fromTileCoord.x, fromTileCoord.y);
	CCLOG("To: %f, %f", toTileCoord.x, toTileCoord.y);
	_spOpenSteps.clear();
	_spClosedSteps.clear();
	this->insertInOpenSteps(ShortestPathStep::createWithPosition(fromTileCoord));
	do
	{
		ShortestPathStep *currentStep = _spOpenSteps.at(0);
		_spClosedSteps.pushBack(currentStep);
		_spOpenSteps.erase(0);
		if (currentStep->getPosition() == toTileCoord)
		{
			this->constructPathAndStartAnimationFromStep(currentStep);
			_spOpenSteps.clear();
			_spClosedSteps.clear();
			break;
		}
		PointArray *adjSteps = _layer->walkableAdjacentTilesCoordForTileCoord(currentStep->getPosition());
		for (ssize_t i = 0; i < adjSteps->count(); ++i)
		{
			ShortestPathStep *step = ShortestPathStep::createWithPosition(adjSteps->getControlPointAtIndex(i));
			if (this->getStepIndex(_spClosedSteps, step) != -1)
			{
				continue;
			}
			int moveCost = this->costToMoveFromStepToAdjacentStep(currentStep, step);
			ssize_t index = this->getStepIndex(_spOpenSteps, step);
			if (index == -1)
			{
				step->setParent(currentStep);
				step->setGScore(currentStep->getGScore() + moveCost);
				step->setHScore(this->computeHScoreFromCoordToCoord(step->getPosition(), toTileCoord));
				this->insertInOpenSteps(step);
			}
			else
			{
				step = _spOpenSteps.at(index);
				if ((currentStep->getGScore() + moveCost) < step->getGScore())
				{
					step->setGScore(currentStep->getGScore() + moveCost);
					step->retain();
					_spOpenSteps.erase(index);
					this->insertInOpenSteps(step);
					step->release();
				}
			}
		}
	} while (_spOpenSteps.size() > 0);
	if (_shortestPath.empty())
	{
	}
}

void Cat::constructPathAndStartAnimationFromStep(Cat::ShortestPathStep *step)
{
	_shortestPath.clear();
	do
	{
		if (step->getParent())
		{
			_shortestPath.insert(0, step);
		}
		step = step->getParent();
	} while (step);
	for (const ShortestPathStep *s : _shortestPath)
	{
		CCLOG("%s", s->getDescription().c_str());
	}
	this->popStepAndAnimate();
}

void Cat::popStepAndAnimate()
{
	if (_shortestPath.size() == 0)
	{
		return;
	}
	ShortestPathStep *s = _shortestPath.at(0);
	MoveTo *moveAction = MoveTo::create(0.4f, _layer->positionForTileCoord(s->getPosition()));
	CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(Cat::popStepAndAnimate, this));
	_shortestPath.erase(0);
	this->runAction(Sequence::create(moveAction, moveCallback, nullptr));
}

ssize_t Cat::getStepIndex(const cocos2d::Vector<Cat::ShortestPathStep *> &steps, const Cat::ShortestPathStep *step)
{
	for (ssize_t i = 0; i < steps.size(); ++i)
	{
		if (steps.at(i)->isEqual(step))
		{
			return i;
		}
	}
	return -1;
}
