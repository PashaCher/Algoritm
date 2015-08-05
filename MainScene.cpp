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
	bool bRet = false;
	do
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

		MainScene::CreateObjects();
		MainScene::CharacteristicCat();
		MainScene::CharacteristicPlayer();

		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [this](Touch *touch, Event *event)
		{
			//Point touchLocation = _tileMap->convertTouchToNodeSpace(touch);
			//myCat->moveToward(touchLocation);
			MainScene::AlgorithmWin();
			return true;
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
		this->scheduleUpdate();

		auto listenerkey = EventListenerKeyboard::create();
		listenerkey->onKeyPressed = CC_CALLBACK_2(MainScene::onKeyPressed, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerkey, this);

		numBonusObjects = 0;

		bRet = true;
	} while (0);
	return bRet;
}

BonusObject *cObjTmp;
BonusObject *bObjTmp;

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
			myCat = /*(Cat*)*/Sprite::create("Cat.png");
			myCat->setPosition(j * 32 + 16, i * 32 + 16);
			this->addChild(myCat, 4);
			/*myCat->_layer = this;*/
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
			AlgorithmWin();
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

void MainScene::AlgorithmWin()
{
	int x, y;
	if (LivesCat + DefenseCat < AttackPlayer)
	{
		for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
		{
			std::ifstream ifs("UploadObjects.txt");
			ifs >> a[i][j];
			switch (a[i][j])
			{
			case BONUSTYPE_BONE:
				x = j * 32 + 16;
				y = i * 32 + 16;
				break;
			case BONUSTYPE_CACTUS:
				x = j * 32 + 16;
				y = i * 32 + 16;
				break;
			default: break;
			}
		}
		Action* moveTo = MoveTo::create(3, Vec2(x, y));
		myCat->runAction(moveTo);
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
		AlgorithmWin();
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
