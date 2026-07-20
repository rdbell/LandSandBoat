describe('Frozen Mist mob skill', function()
    it('uses its Ice plan, gates Terror on damage processing, and always resets its aura', function()
        local mist=require('scripts/actions/mobskills/frozen_mist');local move,process,status=xi.mobskills.mobMagicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove;local params,damage,terror,aura=nil,nil,nil,nil
        local mob={getMainLvl=function()return 75 end,delStatusEffectSilent=function(_,e)aura.removed=e end,addStatusEffect=function(_,e,o)aura.added={e,o}end};local target={takeDamage=function(_,...)damage={...}end};local skill={setFinalAnimationSub=function(_,v)aura={final=v}end}
        xi.mobskills.mobMagicalMove=function(_,_,_,_,value)params=value;return {damage=123,attackType=xi.attackType.MAGICAL,damageType=xi.damageType.ICE}end;xi.mobskills.processDamage=function()return false end;xi.mobskills.mobStatusEffectMove=function(...)terror={...}end
        assert(mist.onMobSkillCheck(target,mob,skill)==0 and mist.onMobWeaponSkill(mob,target,skill,{})==123)
        assert(params.baseDamage==77 and params.fTP[1]==1.5 and params.fTP[2]==1.5 and params.fTP[3]==1.5 and params.element==xi.element.ICE and params.attackType==xi.attackType.MAGICAL and params.damageType==xi.damageType.ICE and params.shadowBehavior==xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage==nil and terror==nil and aura.final==1 and aura.removed==xi.effect.STONESKIN and aura.added[1]==xi.effect.STONESKIN and aura.added[2].duration==180 and aura.added[2].origin==mob and aura.added[2].subType==1 and aura.added[2].subPower==1500)
        xi.mobskills.processDamage=function()return true end;mist.onMobWeaponSkill(mob,target,skill,{})
        xi.mobskills.mobMagicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove=move,process,status;assert(damage[1]==123 and terror[3]==xi.effect.TERROR and terror[4]==1 and terror[5]==0 and terror[6]==30)
    end)
end)
