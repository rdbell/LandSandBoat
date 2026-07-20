describe('Great Wheel mob skill',function()
 it('uses its physical plan and resets enmity only for processed non-Trust targets',function()
  local wheel=require('scripts/actions/mobskills/great_wheel');local move,process=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage;local params,damage,reset=nil,nil,nil;local mob={getWeaponDmg=function()return 77 end,resetEnmity=function(_,target)reset=target end};local trusted=false;local target={takeDamage=function(_,...)damage={...}end,isTrust=function()return trusted end};xi.mobskills.mobPhysicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.SLASHING}end;xi.mobskills.processDamage=function()return false end
  assert(wheel.onMobSkillCheck(target,mob,{})==0 and wheel.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.numHits==1 and params.fTP[1]==3 and params.fTP[2]==3 and params.fTP[3]==3 and params.attackType==xi.attackType.PHYSICAL and params.damageType==xi.damageType.SLASHING and params.shadowBehavior==xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage==nil and reset==nil);xi.mobskills.processDamage=function()return true end;wheel.onMobWeaponSkill(mob,target,{},{})
  assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.PHYSICAL and damage[4]==xi.damageType.SLASHING and reset==target);reset=nil;trusted=true;wheel.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage=move,process;assert(reset==nil)
 end)
end)
