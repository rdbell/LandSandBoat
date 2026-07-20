describe('Gouging Branch mob skill',function()
 it('uses its captured three-hit piercing plan and damages only after processing',function()
  local branch=require('scripts/actions/mobskills/gouging_branch');local move,process=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage;local params,damage=nil,nil;local mob={getWeaponDmg=function()return 77 end};local target={takeDamage=function(_,...)damage={...}end};xi.mobskills.mobPhysicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.PIERCING}end;xi.mobskills.processDamage=function()return false end
  assert(branch.onMobSkillCheck(target,mob,{})==0 and branch.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.numHits==3 and params.fTP[1]==1 and params.fTP[2]==1 and params.fTP[3]==1 and params.attackType==xi.attackType.PHYSICAL and params.damageType==xi.damageType.PIERCING and params.shadowBehavior==xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage==nil);xi.mobskills.processDamage=function()return true end;branch.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage=move,process;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.PHYSICAL and damage[4]==xi.damageType.PIERCING)
 end)
end)
