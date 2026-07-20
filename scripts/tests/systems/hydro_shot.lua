describe('Hydro Shot mob skill',function()
 it('uses its hand-to-hand plan and resets enmity only after processing',function()
  local shot=require('scripts/actions/mobskills/hydro_shot');local move,process=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage;local params,damage,reset=nil,nil,nil;local mob={getWeaponDmg=function()return 77 end,resetEnmity=function(_,target)reset=target end};local target={takeDamage=function(_,...)damage={...}end};xi.mobskills.mobPhysicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.HAND_TO_HAND}end;xi.mobskills.processDamage=function()return false end
  assert(shot.onMobSkillCheck(target,mob,{})==0 and shot.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.numHits==1 and params.fTP[1]==2.5 and params.fTP[2]==2.5 and params.fTP[3]==2.5 and params.attackType==xi.attackType.PHYSICAL and params.damageType==xi.damageType.HAND_TO_HAND and params.shadowBehavior==xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage==nil and reset==nil);xi.mobskills.processDamage=function()return true end;shot.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage=move,process;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.PHYSICAL and damage[4]==xi.damageType.HAND_TO_HAND and reset==target)
 end)
end)
