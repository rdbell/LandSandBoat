-----------------------------------
-- Global, independent functions for counter calculations.
-- Pure Seigan counter inject dual-wired to OmegaXI internal/seigancounter (slice 6692 / 0905).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.counter = xi.combat.counter or {}
-----------------------------------

xi.combat.counter.baseCounterRate = 25
xi.combat.counter.facingCone = 64

-- Pure checkSeiganCounter once status/facing/engaged/weapon/mod/hit-rate/roll are injected.
-- params: hasThirdEye, hasSeigan, isFacing64, isEngaged, isPC, isWeaponTwoHanded,
--   thirdEyeCounterRateMod, hitRateFactor, roll1to100
-- Succeeds when roll1to100 <= (25 + mod) * hitRateFactor (parity with d100).
xi.combat.counter.checkSeiganCounterFromParams = function(params)
    if not params.hasThirdEye then
        return false
    end

    if not params.hasSeigan then
        return false
    end

    if not params.isFacing64 then
        return false
    end

    if not params.isEngaged then
        return false
    end

    -- PC without 2H weapon cannot Seigan-counter.
    if params.isPC and not params.isWeaponTwoHanded then
        return false
    end

    local baseCounterRate = xi.combat.counter.baseCounterRate + (params.thirdEyeCounterRateMod or 0)
    local hitRateFactor = params.hitRateFactor or 0
    local roll = params.roll1to100 or 0

    -- Original: if random(1,100) > base * hitRate then false else true
    -- ⇔ succeeds when roll <= base * hitRate
    if roll > baseCounterRate * hitRateFactor then
        return false
    end

    return true
end

-- Entity host: inject status/facing/equip/mod/hit-rate/roll → pure.
xi.combat.counter.checkSeiganCounter = function(attacker, defender)
    local isPC = defender:isPC()

    return xi.combat.counter.checkSeiganCounterFromParams({
        hasThirdEye            = defender:hasStatusEffect(xi.effect.THIRD_EYE),
        hasSeigan              = defender:hasStatusEffect(xi.effect.SEIGAN),
        isFacing64             = defender:isFacing(attacker, xi.combat.counter.facingCone),
        isEngaged              = defender:isEngaged(),
        isPC                   = isPC,
        isWeaponTwoHanded      = defender:isWeaponTwoHanded(),
        thirdEyeCounterRateMod = defender:getMod(xi.mod.THIRD_EYE_COUNTER_RATE),
        hitRateFactor          = xi.combat.physicalHitRate.getPhysicalHitRate(
            defender, attacker, 0, xi.attackAnimation.RIGHT_ATTACK, false
        ),
        roll1to100             = math.random(1, 100),
    })
end
