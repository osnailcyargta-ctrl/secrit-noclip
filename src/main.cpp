#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

static CCLabelBMFont* s_secritLabel = nullptr;
static float s_labelTimer = 0.f;

class $modify(SecritPlayLayer, PlayLayer) {

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        s_secritLabel = nullptr;
        s_labelTimer = 0.f;

        auto label = CCLabelBMFont::create("secrit no way", "bigFont.fnt");
        label->setScale(0.85f);
        label->setOpacity(0);
        label->setColor({ 255, 255, 255 });
        label->setZOrder(999);

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        label->setPosition({ winSize.width / 2.f, winSize.height * 0.72f });

        this->addChild(label);
        s_secritLabel = label;

        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        if (!s_secritLabel) return;

        if (s_labelTimer > 0.f) {
            s_labelTimer -= dt;
            float alpha = (s_labelTimer / 1.8f) * 255.f;
            s_secritLabel->setOpacity((GLubyte)std::clamp(alpha, 0.f, 255.f));
        } else {
            s_secritLabel->setOpacity(0);
        }
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (!Mod::get()->getSettingValue<bool>("noclip-enabled")) {
            PlayLayer::destroyPlayer(player, obj);
            return;
        }

        // Noclip: skip kematian, tampilkan teks
        if (Mod::get()->getSettingValue<bool>("show-text") && s_secritLabel) {
            s_secritLabel->stopAllActions();
            s_secritLabel->setOpacity(255);
            s_secritLabel->setScale(0.6f);
            s_secritLabel->runAction(
                CCEaseBackOut::create(CCScaleTo::create(0.15f, 0.85f))
            );
            s_labelTimer = 1.8f;
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        s_labelTimer = 0.f;
        if (s_secritLabel) s_secritLabel->setOpacity(0);
    }
};
