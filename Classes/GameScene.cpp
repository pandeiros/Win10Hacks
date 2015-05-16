#include "GameScene.h"
#include "GameObject.h"
#define DEBUG_PHYSICS 17892
USING_NS_CC;

// on "init" you need to initialize your instance
bool GameScene::initWithPhysics()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::initWithPhysics())
    {
        return false;
    }

		auto contactListener = EventListenerPhysicsContact::create();
		contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

		getPhysicsWorld()->setGravity(Vec2(0, 0));

#ifdef DEBUG_PHYSICS 
		getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
#endif


		
		mapLayers[GState::MENU] = MenuLayer::create();
    mapLayers[GState::INIT] = InitLayer::create();
    mapLayers[GState::SETTINGS] = SettingsLayer::create();
    mapLayers[GState::CREDITS] = CreditsLayer::create();
    mapLayers[GState::LEADERBOARDS] = LeaderboardLayer::create();
    mapLayers[GState::ACHIEVEMENTS] = AchievementsLayer::create();
    mapLayers[GState::ENDGAME] = EndGameLayer::create();
    mapLayers[GState::PAUSE] = PauseLayer::create();
    mapLayers[GState::GAME] = GameLayer::create();

    for (unsigned int i = GState::INIT; i < GState::__END; ++i)
    {
        this->addChild(mapLayers[static_cast<GState>(i)]);
        mapLayers[static_cast<GState>(i)]->setOpacity(0);
        mapLayers[static_cast<GState>(i)]->setPosition(Vec2(visibleSize.width, 0));
        mapLayers[static_cast<GState>(i)]->setCascadeOpacityEnabled(true);
        mapLayers[static_cast<GState>(i)]->setVisible(true);
    }

    mapLayers[GameSceneDefines::activeState]->setOpacity(255);
    mapLayers[GameSceneDefines::activeState]->setPosition(Vec2(0, 0));

    this->scheduleUpdate();

    return true;
}

void GameScene::update(float dt)
{
    cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    // Check for scene change.
    if (GDef::activeState != GDef::queuedState)
    {
        auto moveRight = MoveTo::create(0.5, Vec2(visibleSize.width, 0));
        auto moveIn = MoveTo::create(0.5, Vec2(0, 0));
        auto moveLeft = MoveTo::create(0.5, Vec2(-visibleSize.width, 0));
        auto fadeIn = FadeIn::create(0.5);
        auto fadeOut = FadeOut::create(0.5);
        auto fadeTo = FadeTo::create(0.5, 200);

        if (GDef::activeState == GState::INIT)
        {
            mapLayers[GDef::activeState]->runAction(Sequence::create(fadeOut, moveRight, nullptr));
            mapLayers[GDef::queuedState]->setPosition(0, 0);
            mapLayers[GDef::queuedState]->runAction(fadeTo);
            mapLayers[GState::GAME]->setPosition(0, 0);
            mapLayers[GState::GAME]->runAction(fadeIn);
        }
        else if (GDef::activeState == GState::GAME)
        {
            mapLayers[GDef::queuedState]->setPosition(0, 0);
            mapLayers[GDef::queuedState]->runAction(Spawn::create(fadeIn,
                moveIn, nullptr));
        }
        else
        {
            mapLayers[GDef::activeState]->runAction(Spawn::create(fadeOut,
                rand() % 2 == 0 ? moveLeft : moveRight, nullptr));
            mapLayers[GDef::queuedState]->runAction(Spawn::create(fadeIn,
                moveIn, nullptr));
           /* mapLayers[GDef::activeState]->runAction(moveRight);
            mapLayers[GDef::queuedState]->setPosition(0, 0);
            mapLayers[GDef::queuedState]->setOpacity(255);*/
        }


        GDef::activeState = GDef::queuedState;
        mapLayers[GDef::activeState]->onActive();
    }
}


void GameScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

GameScene* GameScene::createWithPhysics()
{
    GameScene *ret = new (std::nothrow) GameScene();
    if (ret && ret->initWithPhysics())
    {
        ret->autorelease();
        return ret;
    }
    else
    {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
}


bool GameScene::onContactBegin(PhysicsContact& contact)
{
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();

	int tagA = contact.getShapeA()->getBody()->getTag();
	int tagB = contact.getShapeB()->getBody()->getTag();

	Player * player;
	GameObject * other;
	if (nodeA && nodeB)
	{
	CCLOG("KONTAKT");

		if (tagA == 0)
		{
			player = dynamic_cast<Player*>(nodeA);
			other = dynamic_cast<GameObject*>(nodeB);

			if (tagB == 1)
			{
				player->endGame();
				//end game screen
			}
			else if (tagB == 2)
			{
				CCLOG("PRZY�PIESZ KURWA");
				player->acceleration += 100;
				other->removeFromParent();
			}
		}
		else if (tagB == 0)
		{
			other = dynamic_cast<GameObject*>(nodeA);
			player = dynamic_cast<Player*>(nodeB);

			if (tagA == 1)
			{
				player->endGame();
				//end game screen
			}
			else if (tagA == 2)
			{
				CCLOG("PRZY�PIESZ KURWA");
				player->acceleration += 100;
				other->removeFromParent();
			}
		}
	}

	return true;
}