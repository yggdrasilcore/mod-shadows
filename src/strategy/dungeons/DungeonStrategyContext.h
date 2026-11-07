#ifndef _SHADOW_DUNGEONSTRATEGYCONTEXT_H
#define _SHADOW_DUNGEONSTRATEGYCONTEXT_H

#include "Strategy.h"
#include "wotlk/utgardekeep/UtgardeKeep.h"
#include "wotlk/nexus/Nexus.h"
#include "wotlk/azjolnerub/AzjolNerub.h"
#include "wotlk/oldkingdom/OldKingdom.h"
#include "wotlk/draktharonkeep/DrakTharonKeep.h"
#include "wotlk/violethold/VioletHold.h"
#include "wotlk/gundrak/Gundrak.h"
#include "wotlk/hallsofstone/HallsOfStone.h"
#include "wotlk/hallsoflightning/HallsOfLightning.h"
#include "wotlk/oculus/Oculus.h"
#include "wotlk/utgardepinnacle/UtgardePinnacle.h"
#include "wotlk/cullingofstratholme/CullingOfStratholme.h"
#include "wotlk/forgeofsouls/ForgeOfSouls.h"
#include "wotlk/pitofsaron/PitOfSaron.h"
#include "wotlk/trialofthechampion/TrialOfTheChampion.h"

/*
Full list/TODO:

Trial of the Champion - ToC
Alliance Champions: Deathstalker Visceri, Eressea Dawnsinger, Mokra the Skullcrusher, Runok Wildmane, Zul'tore
Horde Champions: Ambrose Boltspark, Colosos, Jacob Alerius, Jaelyne Evensong, Lana Stouthammer
Argent Champion: Argent Confessor Paletress/Eadric the Pure
The Black Knight
Halls of Reflection - HoR
Falric, Marwyn, The Lich King
Pit of Saron - PoS
Forgemaster Garfrost, Krick & Ick, Scourgelord Tyrannus
The Forge of Souls - FoS
Bronjahm, Devourer of Souls

*/



class DungeonStrategyContext : public NamedObjectContext<Strategy>
{
    public:
        DungeonStrategyContext() : NamedObjectContext<Strategy>(false, true)
        {
            // Vanilla
            // ...

            // Burning Crusade
            // ...

            // Wrath of the Lich King
            creators["wotlk-uk"] = &DungeonStrategyContext::wotlk_uk;       // Utgarde Keep
            creators["wotlk-nex"] = &DungeonStrategyContext::wotlk_nex;     // The Nexus
            creators["wotlk-an"] = &DungeonStrategyContext::wotlk_an;       // Azjol-Nerub
            creators["wotlk-ok"] = &DungeonStrategyContext::wotlk_ok;       // Ahn'kahet: The Old Kingdom
            creators["wotlk-dtk"] = &DungeonStrategyContext::wotlk_dtk;     // Drak'Tharon Keep
            creators["wotlk-vh"] = &DungeonStrategyContext::wotlk_vh;       // The Violet Hold
            creators["wotlk-gd"] = &DungeonStrategyContext::wotlk_gd;       // Gundrak
            creators["wotlk-hos"] = &DungeonStrategyContext::wotlk_hos;     // Halls of Stone
            creators["wotlk-hol"] = &DungeonStrategyContext::wotlk_hol;     // Halls of Lightning
            creators["wotlk-occ"] = &DungeonStrategyContext::wotlk_occ;     // The Oculus
            creators["wotlk-up"] = &DungeonStrategyContext::wotlk_up;       // Utgarde Pinnacle
            creators["wotlk-cos"] = &DungeonStrategyContext::wotlk_cos;     // The Culling of Stratholme
            creators["wotlk-toc"] = &DungeonStrategyContext::wotlk_toc;     // Trial of the Champion
            creators["wotlk-hor"] = &DungeonStrategyContext::wotlk_hor;     // Halls of Reflection
            creators["wotlk-pos"] = &DungeonStrategyContext::wotlk_pos;     // Pit of Saron
            creators["wotlk-fos"] = &DungeonStrategyContext::wotlk_fos;     // The Forge of Souls
        }
    private:
        static Strategy* wotlk_uk(ShadowAI* botAI) { return new WotlkDungeonUKStrategy(botAI); }
        static Strategy* wotlk_nex(ShadowAI* botAI) { return new WotlkDungeonNexStrategy(botAI); }
        static Strategy* wotlk_an(ShadowAI* botAI) { return new WotlkDungeonANStrategy(botAI); }
        static Strategy* wotlk_ok(ShadowAI* botAI) { return new WotlkDungeonOKStrategy(botAI); }
        static Strategy* wotlk_dtk(ShadowAI* botAI) { return new WotlkDungeonDTKStrategy(botAI); }
        static Strategy* wotlk_vh(ShadowAI* botAI) { return new WotlkDungeonVHStrategy(botAI); }
        static Strategy* wotlk_gd(ShadowAI* botAI) { return new WotlkDungeonGDStrategy(botAI); }
        static Strategy* wotlk_hos(ShadowAI* botAI) { return new WotlkDungeonHoSStrategy(botAI); }
        static Strategy* wotlk_hol(ShadowAI* botAI) { return new WotlkDungeonHoLStrategy(botAI); }
        static Strategy* wotlk_occ(ShadowAI* botAI) { return new WotlkDungeonOccStrategy(botAI); }
        static Strategy* wotlk_up(ShadowAI* botAI) { return new WotlkDungeonUPStrategy(botAI); }
        static Strategy* wotlk_cos(ShadowAI* botAI) { return new WotlkDungeonCoSStrategy(botAI); }
        static Strategy* wotlk_fos(ShadowAI* botAI) { return new WotlkDungeonFoSStrategy(botAI); }
        static Strategy* wotlk_pos(ShadowAI* botAI) { return new WotlkDungeonPoSStrategy(botAI); }
        static Strategy* wotlk_toc(ShadowAI* botAI) { return new WotlkDungeonToCStrategy(botAI); }
        // NYI from here down
        static Strategy* wotlk_hor(ShadowAI* botAI) { return new WotlkDungeonUKStrategy(botAI); }


};

#endif
