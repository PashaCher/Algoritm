#include "cocos2d.h"
#include "BonusObjects.h"

using namespace cocos2d;

const int MAX_NUMBONUSOBJECTS = 100;
#define BONUSTYPE_FLOWER 1
#define BONUSTYPE_BONE 2
#define BONUSTYPE_CACTUS 3
#define BONUSTYPE_PLAYER 5
#define BONUSTYPE_UNDEFINED 0

class Cat;

class MainScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	void menuCloseCallback(cocos2d::Ref* pSender);
	CREATE_FUNC(MainScene);

	Size visibleSizeText = Director::getInstance()->getVisibleSize();
	Vec2 originText = Director::getInstance()->getVisibleOrigin();

	void update(float dt);
	void CreateObjects();
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);

	void CharacteristicCat();
	void CharacteristicPlayer();

	void RandomMove();

	void AddBonusCat();
	void UpdateLabelCharacteristic();

	void ReadingFromTheFile();

	void ComparisonOfTheCharacteristic();
	void AlgorithmWin();
private:
	TMXTiledMap *_tileMap;
	TMXLayer *_background;

	Sprite *flower;
	Sprite *bone;
	Sprite *cactus;
	Sprite *myCat;
	Sprite *myPerson;

	Vector<BonusObject*> bonusObjects;
	int numBonusObjects;

	int numA;
	int iRand;
	Vec2 offsetх[4];
	MoveTo *moveTo;
	MoveBy *moveBy;
	int a[10][10];

	/*Cat*/
	Label *labelCharacteristicCatAtack;
	Label *labelCharacteristicCatDefense;
	Label *labelCharacteristicCatLives;

	int AttackCat = 0;
	int	DefenseCat = 0;
	int LivesCat = 0;

	/*Player*/
	int AttackPlayer = 0;
	int DefensePlayer = 0;
	int LivesPlayer = 0;
	Label *labelCharacteristicPlayerAtack;
	Label *labelCharacteristicPlayerDefense;
	Label *labelCharacteristicPlayerLives;

	int i;
	MainScene*_layer;
};
