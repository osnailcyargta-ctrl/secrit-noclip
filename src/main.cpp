#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

// Label global buat teks "secrit no way"
static CCLabelBMFont* s_secritLabel = nullptr;
static float s_labelTimer = 0.f;
static bool s_noclipActive = false;

class $modify(SecritPlayLayer, PlayLayer) {

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        s_secritLabel = nullptr;
        s_labelTimer = 0.f;
        s_noclipActive = false;

        // Bikin label "secrit no way"
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

        if (!Mod::get()->getSettingValue<bool>("noclip-enabled")) return;
        if (!Mod::get()->getSettingValue<bool>("show-text")) return;
        if (!s_secritLabel) return;

        // Fade out label kalau lagi keliatan
        if (s_labelTimer > 0.f) {
            s_labelTimer -= dt;
            float alpha = (s_labelTimer / 1.8f) * 255.f;
            s_secritLabel->setOpacity((GLubyte)std::clamp(alpha, 0.f, 255.f));
        } else {
            s_secritLabel->setOpacity(0);
        }
    }

    // Hook playerDied - kalau noclip aktif, jangan mati
    void playerDied(PlayerObject* player) {
        if (!Mod::get()->getSettingValue<bool>("noclip-enabled")) {
            PlayLayer::playerDied(player);
            return;
        }

        // Noclip aktif: skip kematian, tampilin teks
        s_noclipActive = true;

        if (Mod::get()->getSettingValue<bool>("show-text") && s_secritLabel) {
            s_secritLabel->setOpacity(255);
            s_labelTimer = 1.8f;

            // Efek scale pop
            s_secritLabel->stopAllActions();
            s_secritLabel->setScale(0.6f);
            s_secritLabel->runAction(
                CCSequence::create(
                    CCEaseBackOut::create(CCScaleTo::create(0.15f, 0.85f)),
                    nullptr
                )
            );
        }

        // Reset posisi player biar gak stuck (opsional, komentarin kalau mau beneran tembus)
        // player->setPosition(player->getPosition());
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        s_noclipActive = false;
        s_labelTimer = 0.f;
        if (s_secritLabel) s_secritLabel->setOpacity(0);
    }
};

// Hook PlayerObject buat disable collision detection saat noclip
class $modify(SecritPlayerObject, PlayerObject) {

    void collidedWithObject(float dt, GameObject* obj, CCRect rect, bool idk) {
        if (!Mod::get()->getSettingValue<bool>("noclip-enabled")) {
            PlayerObject::collidedWithObject(dt, obj, rect, idk);
            return;
        }
        // Skip collision — player tembus objek
    }
};
