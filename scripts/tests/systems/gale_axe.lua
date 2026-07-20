describe('Gale Axe mob skill',function()
 it('uses its slashing plan and Chokes only after processing',function()
  local axe=require('scripts/actions/mobskills/gale_axe');local move,process,status=xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove;local params,damage,choke=nil,nil,nil;local mob={getWeaponDmg=function()return 77 end};local target={takeDamage=function(_,...)damage={...}end};xi.mobskills.mobPhysicalMove=function(_,_,_,_,v)params=v;return {damage=123,attackType=xi.attackType.PHYSICAL,damageType=xi.damageType.SLASHING}end;xi.mobskills.processDamage=function()return false end;xi.mobskills.mobStatusEffectMove=function(...)choke={...}end
  assert(axe.onMobSkillCheck(target,mob,{})==0 and axe.onMobWeaponSkill(mob,target,{},{})==123);assert(params.baseDamage==77 and params.numHits==1 and params.fTP[1]==1 and params.fTP[2]==1 and params.fTP[3]==1 and params.attackType==xi.attackType.PHYSICAL and params.damageType==xi.damageType.SLASHING and params.shadowBehavior==xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage==nil and choke==nil)
  xi.mobskills.processDamage=function()return true end;axe.onMobWeaponSkill(mob,target,{},{})
  xi.mobskills.mobPhysicalMove,xi.mobskills.processDamage,xi.mobskills.mobStatusEffectMove=move,process,status;assert(damage[1]==123 and damage[2]==mob and damage[3]==xi.attackType.PHYSICAL and damage[4]==xi.damageType.SLASHING and choke[3]==xi.effect.CHOKE and choke[4]==5 and choke[5]==3 and choke[6]==60)
 end)
end)
