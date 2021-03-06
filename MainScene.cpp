#include "MainScene.h"
#include "Cat.h"

#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <fstream>

USING_NS_CC;

Scene* MainScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MainScene::create();
	scene->addChild(layer);
	return scene;
}

Cat *myCat;

bool MainScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_tileMap = TMXTiledMap::create("Map.tmx");
	this->addChild(_tileMap);
	_background = _tileMap->getLayer("Background");

	//_batchNode = SpriteBatchNode::create("CatTexture.png");
	//this->addChild(_batchNode);
	//SpriteFrameCache::getInstance()->addSpriteFramesWithFile("primer.plist");

	myCat = Cat::createWithLayer(this);
	this->addChild(myCat, 4);

	MainScene::CreateObjects();
	MainScene::CharacteristicCat();
	MainScene::CharacteristicPlayer();
	numBonusObjects = 0;

	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch *touch, Event *event)
	{
		Point touchLocation = _tileMap->convertTouchToNodeSpace(touch);
		myCat->moveToward(touchLocation);
		AlgorithmWin();
		return true;
	};
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	auto listenerkey = EventListenerKeyboard::create();
	listenerkey->onKeyPressed = CC_CALLBACK_2(MainScene::onKeyPressed, this);
	listenerkey->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerkey, this);

	this->scheduleUpdate();
	return true;
}

void MainScene::CreateObjects()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	std::ifstream ifs("UploadObjects.txt");
	int k = 0;
	for (int i = 0; i < 10; ++i)
	for (int j = 0; j < 10; ++j)
	{
		ifs >> a[i][j];
		switch (a[i][j])
		{
		case BONUSTYPE_FLOWER:
			bObjTmp = (BonusObject*)Sprite::create("flower.png");
			bObjTmp->setPosition(j * 32 + 16, i * 32 + 16);
			bObjTmp->setType(BONUSTYPE_FLOWER);
			bonusObjects.pushBack(bObjTmp);
			this->addChild(bObjTmp, 1);
			break;
		case BONUSTYPE_BONE:
			bObjTmp = (BonusObject*)Sprite::create("bone.png");
			bObjTmp->setPosition(j * 32 + 16, i * 32 + 16);
			bObjTmp->setType(BONUSTYPE_BONE);
			bonusObjects.pushBack(bObjTmp);
			this->addChild(bObjTmp, 2);
			break;
		case BONUSTYPE_CACTUS:
			bObjTmp = (BonusObject*)Sprite::create("cactus.png");
			bObjTmp->setPosition(j * 32 + 16, i * 32 + 16);
			bObjTmp->setType(BONUSTYPE_CACTUS);
			bonusObjects.pushBack(bObjTmp);
			this->addChild(bObjTmp, 3);
			break;
		case 4:
			myCat->setPosition(j * 32 + 16, i * 32 + 16);
			break;
		case 5:
			cObjTmp = (BonusObject*)Sprite::create("Player.png");
			cObjTmp->setPosition(j * 32 + 16, i * 32 + 16);
			cObjTmp->setType(BONUSTYPE_PLAYER);
			bonusObjects.pushBack(cObjTmp);
			RandomMove();
			this->addChild(cObjTmp, 5);

			/*myPerson = Sprite::create("Player.png");
			myPerson->setPosition(j * 32 + 16, i * 32 + 16);
			RandomMove();
			this->addChild(myPerson, 5);*/
			break;
		default: break;
		}
	}
}

void MainScene::update(float dt)
{
	for (i = 0; i < bonusObjects.size(); i++)
	{
		if (myCat->getBoundingBox().intersectsRect(bonusObjects.at(i)->getBoundingBox()))
		{
			removeChild(bonusObjects.at(i));
			AddBonusCat();
			ComparisonOfTheCharacteristic();
			//AlgorithmWin();
		}
	}
}

void MainScene::AddBonusCat()
{
	BonusObject *bObj = bonusObjects.at(i);

	if (bObj && bObj->getType() == BONUSTYPE_FLOWER)
	{
		bObj->setType(BONUSTYPE_UNDEFINED);

		AttackCat++;
	}

	if (bObj && bObj->getType() == BONUSTYPE_BONE)
	{
		bObj->setType(BONUSTYPE_UNDEFINED);

		DefenseCat++;
	}

	if (bObj && bObj->getType() == BONUSTYPE_CACTUS)
	{
		bObj->setType(BONUSTYPE_UNDEFINED);

		LivesCat++;
	}

	UpdateLabelCharacteristic();
}

void MainScene::UpdateLabelCharacteristic()
{
	char strAtack[20];
	sprintf(strAtack, "Atack Cat: %d", AttackCat);
	labelCharacteristicCatAtack->setString(strAtack);

	char strDefense[20];
	sprintf(strDefense, "Defense Cat: %d", DefenseCat);
	labelCharacteristicCatDefense->setString(strDefense);

	char strLives[20];
	sprintf(strLives, "Lives Cat: %d", LivesCat);
	labelCharacteristicCatLives->setString(strLives);
}

void MainScene::RandomMove()
{
	srand(time(0));
	offsetх[0] = Vec2(32, 32);
	offsetх[1] = Vec2(-32, 32);
	offsetх[2] = Vec2(32, -32);
	offsetх[3] = Vec2(-32, -32);
	iRand = (rand() % 4);
	moveTo = MoveTo::create(1.0f, cObjTmp->getPosition() + offsetх[iRand]);
	cObjTmp->runAction(Sequence::create(moveTo, DelayTime::create(2.0f), CallFunc::create(CC_CALLBACK_0(MainScene::RandomMove, this)), NULL));
}

void MainScene::ReadingFromTheFile()
{
	FILE *pFile = fopen("Characteristic.txt", "r");
	fscanf_s(pFile, "Cat: [Attack: %d, Defense: %d, Lives: %d]", &AttackCat, &DefenseCat, &LivesCat);
	fscanf_s(pFile, "\nMonster: [Attack: %d, Defense: %d, Lives: %d]", &AttackPlayer, &DefensePlayer, &LivesPlayer);
	fclose(pFile);
}

void MainScene::CharacteristicCat()
{
	ReadingFromTheFile();

	labelCharacteristicCatAtack = Label::createWithTTF("fonts/arial.ttf", StringUtils::format("Atack Cat: %d", AttackCat).c_str());
	labelCharacteristicCatAtack->setScale(1.3F);
	labelCharacteristicCatAtack->setPosition(Vec2(originText.x + visibleSizeText.width / 5, originText.y + visibleSizeText.height / 1.05));
	this->addChild(labelCharacteristicCatAtack);

	labelCharacteristicCatDefense = Label::createWithTTF("fonts/arial.ttf", StringUtils::format("Defense Cat: %d", DefenseCat).c_str());
	labelCharacteristicCatDefense->setScale(1.3F);
	labelCharacteristicCatDefense->setPosition(Vec2(originText.x + visibleSizeText.width / 5, originText.y + visibleSizeText.height / 1.10));
	this->addChild(labelCharacteristicCatDefense);

	labelCharacteristicCatLives = Label::createWithTTF("fonts/arial.ttf", StringUtils::format("Lives Cat: %d", LivesCat).c_str());
	labelCharacteristicCatLives->setScale(1.3F);
	labelCharacteristicCatLives->setPosition(Vec2(originText.x + visibleSizeText.width / 5, originText.y + visibleSizeText.height / 1.15));
	this->addChild(labelCharacteristicCatLives);
}

void MainScene::CharacteristicPlayer()
{
	ReadingFromTheFile();

	labelCharacteristicPlayerAtack = Label::createWithTTF("fonts/arial.ttf", StringUtils::format("Atack Player: %d", AttackPlayer).c_str());
	labelCharacteristicPlayerAtack->setScale(1.3F);
	labelCharacteristicPlayerAtack->setPosition(Vec2(originText.x + visibleSizeText.width / 1.3, originText.y + visibleSizeText.height / 1.05));
	this->addChild(labelCharacteristicPlayerAtack);

	labelCharacteristicPlayerDefense = Label::createWithTTF("fonts/arial.ttf", StringUtils::format("Defense Player: %d", DefensePlayer).c_str());
	labelCharacteristicPlayerDefense->setScale(1.3F);
	labelCharacteristicPlayerDefense->setPosition(Vec2(originText.x + visibleSizeText.width / 1.3, originText.y + visibleSizeText.height / 1.10));
	this->addChild(labelCharacteristicPlayerDefense);

	labelCharacteristicPlayerLives = Label::createWithTTF("fonts/arial.ttf", StringUtils::format("Lives Player: %d", LivesPlayer).c_str());
	labelCharacteristicPlayerLives->setScale(1.3F);
	labelCharacteristicPlayerLives->setPosition(Vec2(originText.x + visibleSizeText.width / 1.3, originText.y + visibleSizeText.height / 1.15));
	this->addChild(labelCharacteristicPlayerLives);
}

void MainScene::ComparisonOfTheCharacteristic()
{
	BonusObject *cObjTmp = bonusObjects.at(i);

	if (LivesCat + DefenseCat > AttackPlayer)
	{
		if (cObjTmp && cObjTmp->getType() == 5)
		{
			auto winSize = Director::getInstance()->getWinSize();
			char message[10] = "You Win";
			auto labelwin = Label::createWithTTF("fonts/arial.ttf", message);
			labelwin->setScale(5.0F);
			labelwin->setPosition(winSize.width / 2, winSize.height / 2);
			this->addChild(labelwin);
		}
	}

	if (LivesCat + DefenseCat < AttackPlayer)
	{
		if (cObjTmp && cObjTmp->getType() == 5)
		{
			auto winSize = Director::getInstance()->getWinSize();
			char message[12] = "You Louser";
			auto labellouser = Label::createWithTTF("fonts/arial.ttf", message);
			labellouser->setScale(5.0F);
			labellouser->setPosition(winSize.width / 2, winSize.height / 2);
			this->addChild(labellouser);
		}
	}

	if (LivesCat + DefenseCat == AttackPlayer)
	{
		if (cObjTmp && cObjTmp->getType() == 5)
		{
			auto winSize = Director::getInstance()->getWinSize();
			char message[12] = "Equally";
			auto labelequally = Label::createWithTTF("fonts/arial.ttf", message);
			labelequally->setScale(5.0F);
			labelequally->setPosition(winSize.width / 2, winSize.height / 2);
			this->addChild(labelequally);
		}
	}
}

PointArray *MainScene::walkableAdjacentTilesCoordForTileCoord(const Point &tileCoord)
{
	PointArray *tmp = PointArray::create(8);
	Point p(tileCoord.x, tileCoord.y - 1);
	if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	p.setPoint(tileCoord.x - 1, tileCoord.y);
	if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	p.setPoint(tileCoord.x, tileCoord.y + 1);
	if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	p.setPoint(tileCoord.x + 1, tileCoord.y);
	if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	//
	p.setPoint(tileCoord.x + 1, tileCoord.y + 1);
	if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	p.setPoint(tileCoord.x + 1, tileCoord.y - 1);
	if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	p.setPoint(tileCoord.x - 1, tileCoord.y - 1);
	if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	p.setPoint(tileCoord.x - 1, tileCoord.y + 1);
	if (this->isValidTileCoord(p) && !this->isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	//
	return tmp;
}

bool MainScene::isWallAtTileCoord(const Point &tileCoord)
{
	return (this->isPropAtTileCoordForLayer("Wall", tileCoord, _background));
}

Point MainScene::tileCoordForPosition(const Point &position)
{
	int x = position.x / _tileMap->getTileSize().width;
	int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
	return Point(x, y);
}

Point MainScene::positionForTileCoord(const Point &tileCoord)
{
	int x = (tileCoord.x * _tileMap->getTileSize().width) + _tileMap->getTileSize().width / 2;
	int y = (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - (tileCoord.y * _tileMap->getTileSize().height) - _tileMap->getTileSize().height / 2;
	return Point(x, y);
}

void MainScene::removeObjectAtTileCoord(const Point &tileCoord)
{

}

bool MainScene::isPropAtTileCoordForLayer(const char *prop, const Point &tileCoord, TMXLayer *layer)
{
	if (!this->isValidTileCoord(tileCoord))
	{
		return false;
	}
	int gid = layer->getTileGIDAt(tileCoord);
	Value properties = _tileMap->getPropertiesForGID(gid);
	if (properties.isNull())
	{
		return false;
	}
	return properties.asValueMap().find(prop) != properties.asValueMap().end();
}

bool MainScene::isValidTileCoord(const Point &tileCoord)
{
	if (tileCoord.x < 0 || tileCoord.y < 0 || tileCoord.x >= _tileMap->getMapSize().width || tileCoord.y >= _tileMap->getMapSize().height)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void MainScene::setViewpointCenter(const Point &position)
{
	Size winSize = Director::getInstance()->getWinSize();
	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	x = MIN(x, (_tileMap->getMapSize().width * _tileMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height / 2);
	Point actualPosition(x, y);
	Point centerOfView(winSize.width / 2, winSize.height / 2);
	Point viewPoint = centerOfView - actualPosition;
	_tileMap->setPosition(viewPoint);
}

void MainScene::AlgorithmWin()
{
	if (LivesCat + DefenseCat < AttackPlayer)
	{
		auto winSize = Director::getInstance()->getWinSize();
		char message[12] = "Start";
		auto labellouser = Label::createWithTTF("fonts/arial.ttf", message);
		labellouser->setScale(5.0F);
		labellouser->setPosition(winSize.width / 2, winSize.height / 2);
		this->addChild(labellouser);
		auto fadeOut = FadeOut::create(6.0f);
		labellouser->runAction(fadeOut);
	}
}

void MainScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_W == keyCode)
	{
		myCat->setPosition(myCat->getPosition() + Point(0, 32));
	}

	if (EventKeyboard::KeyCode::KEY_S == keyCode)
	{
		myCat->setPosition(myCat->getPosition() + Point(0, -32));
	}

	if (EventKeyboard::KeyCode::KEY_A == keyCode)
	{
		myCat->setPosition(myCat->getPosition() + Point(-32, 0));
	}

	if (EventKeyboard::KeyCode::KEY_D == keyCode)
	{
		myCat->setPosition(myCat->getPosition() + Point(32, 0));
	}

	if (EventKeyboard::KeyCode::KEY_SPACE == keyCode)
	{
	}

	if (EventKeyboard::KeyCode::KEY_Q == keyCode)
	{
	}

	if (EventKeyboard::KeyCode::KEY_ESCAPE == keyCode)
	{
		exit(0);
	}
}

void MainScene::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
