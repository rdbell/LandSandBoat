describe('Heat Wave mob skill',function()
 it('uses its captured Burn power curves and status-effect host',function()
  local wave=require('scripts/actions/mobskills/heat_wave');local execute=xi.combat.action.executeMobskillStatusEffect;local power=nil;local level,id=1,0;local mob={getMainLvl=function()return level end};local target={};local skill={getID=function()return id end};xi.combat.action.executeMobskillStatusEffect=function(_,_,_,effects,options)power=effects[1].power;assert(effects[1].effectId==xi.effect.BURN and effects[1].tick==3 and effects[1].duration==180 and effects[1].tier==1 and next(options)==nil);return 456 end
  assert(wave.onMobSkillCheck(target,mob,skill)==0 and wave.onMobWeaponSkill(mob,target,skill,{})==456 and power==1);level=6;wave.onMobWeaponSkill(mob,target,skill,{});assert(power==2);level=80;wave.onMobWeaponSkill(mob,target,skill,{});assert(power==19);level=99;wave.onMobWeaponSkill(mob,target,skill,{});assert(power==19);id=xi.mobSkill.HEAT_WAVE_2;level=4;wave.onMobWeaponSkill(mob,target,skill,{});assert(power==1);level=9;wave.onMobWeaponSkill(mob,target,skill,{});xi.combat.action.executeMobskillStatusEffect=execute;assert(power==2)
 end)
end)
