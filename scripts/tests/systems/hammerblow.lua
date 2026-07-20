describe('Hammerblow mob skill',function()
 it('uses its captured blunt plan and damages only after processing',function()
  local blow=require('scripts/actions/mobskills/hammerblow');local move,process=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage;local params,damage=nil,nil;local mob={getWeaponDmg=function()return 77 end};local target={takeDamage=function(_,...)damage={...}end};xi.mobskills.mobPhysicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.BLUNT}end;xi.mobskills.processDamage=function()return false end
  assert(blow.onMobSkillCheck(target,mob,{})==0 and blow.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.numHits==1 and params.fTP[1]==2.8 and params.fTP[2]==2.8 and params.fTP[3]==2.8 and params.attackType==xi.attackType.PHYSICAL and params.damageType==xi.damageType.BLUNT and params.shadowBehavior==xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage==nil);xi.mobskills.processDamage=function()return true end;blow.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage=move,process;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.PHYSICAL and damage[4]==xi.damageType.BLUNT)
 end)
end)
