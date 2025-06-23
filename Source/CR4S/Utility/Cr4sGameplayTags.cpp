// Fill out your copyright notice in the Description page of Project Settings.


#include "Cr4sGameplayTags.h"

namespace WeaponTags
{
	UE_DEFINE_GAMEPLAY_TAG(Melee,FName(TEXTVIEW("Weapon.Melee")));
	UE_DEFINE_GAMEPLAY_TAG(Ranged,FName(TEXTVIEW("Weapon.Ranged")));
	
	// Melee
	UE_DEFINE_GAMEPLAY_TAG(Chainsaw, FName(TEXT("Weapon.Melee.Chainsaw")));
	UE_DEFINE_GAMEPLAY_TAG(CrystalSword, FName(TEXT("Weapon.Melee.CrystalSword")));
	UE_DEFINE_GAMEPLAY_TAG(DemolitionGear, FName(TEXT("Weapon.Melee.DemolitionGear")));
	UE_DEFINE_GAMEPLAY_TAG(ShockBat, FName(TEXT("Weapon.Melee.ShockBat")));

	// Ranged
	UE_DEFINE_GAMEPLAY_TAG(CrystalShotgun, FName(TEXT("Weapon.Ranged.CrystalShotgun")));
	UE_DEFINE_GAMEPLAY_TAG(CrystalRifle, FName(TEXT("Weapon.Ranged.CrystalRifle")));
	UE_DEFINE_GAMEPLAY_TAG(CrystalBurstRifle, FName(TEXT("Weapon.Ranged.CrystalBurstRifle")));
	UE_DEFINE_GAMEPLAY_TAG(CrystalGatling, FName(TEXT("Weapon.Ranged.CrystalGatling")));
	UE_DEFINE_GAMEPLAY_TAG(CrystalSMG, FName(TEXT("Weapon.Ranged.CrystalSMG")));
	UE_DEFINE_GAMEPLAY_TAG(CrystalLauncher2, FName(TEXT("Weapon.Ranged.CrystalLauncher2")));
	UE_DEFINE_GAMEPLAY_TAG(CrystalHomingLauncher4, FName(TEXT("Weapon.Ranged.CrystalHomingLauncher4")));
	UE_DEFINE_GAMEPLAY_TAG(CrystalHomingHighSpeed4, FName(TEXT("Weapon.Ranged.CrystalHomingHighSpeed4")));
	UE_DEFINE_GAMEPLAY_TAG(Fireball, FName(TEXT("Weapon.Ranged.Fireball")));
	UE_DEFINE_GAMEPLAY_TAG(HomingFireball, FName(TEXT("Weapon.Ranged.HomingFireball")));
	UE_DEFINE_GAMEPLAY_TAG(IceShotgun, FName(TEXT("Weapon.Ranged.IceShotgun")));
	UE_DEFINE_GAMEPLAY_TAG(ThunderStrike, FName(TEXT("Weapon.Ranged.ThunderStrike")));
	UE_DEFINE_GAMEPLAY_TAG(Comet, FName(TEXT("Weapon.Ranged.Comet")));
}

namespace ToolTags
{
	UE_DEFINE_GAMEPLAY_TAG(Axe,FName(TEXT("Als.OverlayMode.Axe")));
	UE_DEFINE_GAMEPLAY_TAG(PickAxe,FName(TEXT("Als.OverlayMode.PickAxe")));
	UE_DEFINE_GAMEPLAY_TAG(Hammer,FName(TEXT("Als.OverlayMode.Hammer")));
}

namespace OverlayMode
{
	UE_DEFINE_GAMEPLAY_TAG(Mounted,FName(TEXT("Als.OverlayMode.Mounted")));
}

namespace RobotParts
{
	UE_DEFINE_GAMEPLAY_TAG(Core,FName(TEXT("RobotParts.Core")));
	UE_DEFINE_GAMEPLAY_TAG(Body,FName(TEXT("RobotParts.Body")));
	UE_DEFINE_GAMEPLAY_TAG(Arm,FName(TEXT("RobotParts.Arm")));
	UE_DEFINE_GAMEPLAY_TAG(Booster,FName(TEXT("RobotParts.Booster")));
	UE_DEFINE_GAMEPLAY_TAG(Leg,FName(TEXT("RobotParts.Leg")));
}

namespace ArmTags
{
	UE_DEFINE_GAMEPLAY_TAG(BalanceHeavy, FName(TEXT("RobotParts.Arm.Balance.Heavy")));
	UE_DEFINE_GAMEPLAY_TAG(MeleeLight, FName(TEXT("RobotParts.Arm.Melee.Light")));
	UE_DEFINE_GAMEPLAY_TAG(RangedLight, FName(TEXT("RobotParts.Arm.Ranged.Light")));
}

namespace BodyTags
{
	UE_DEFINE_GAMEPLAY_TAG(BalanceLight, FName(TEXT("RobotParts.Body.Balance.Light")));
	UE_DEFINE_GAMEPLAY_TAG(HumidityLight, FName(TEXT("RobotParts.Body.Humidity.Light")));
	UE_DEFINE_GAMEPLAY_TAG(TemperatureLight, FName(TEXT("RobotParts.Body.Temperature.Light")));
}

namespace BoosterTags
{
	UE_DEFINE_GAMEPLAY_TAG(Cyclic, FName(TEXT("RobotParts.Booster.Cyclic")));
	UE_DEFINE_GAMEPLAY_TAG(HighPower, FName(TEXT("RobotParts.Booster.HighPower")));
	UE_DEFINE_GAMEPLAY_TAG(LongRanged, FName(TEXT("RobotParts.Booster.LongRanged")));
	UE_DEFINE_GAMEPLAY_TAG(Recovery, FName(TEXT("RobotParts.Booster.Recovery")));
	UE_DEFINE_GAMEPLAY_TAG(Standard, FName(TEXT("RobotParts.Booster.Standard")));
	UE_DEFINE_GAMEPLAY_TAG(Sustained, FName(TEXT("RobotParts.Booster.Sustained")));
}

namespace CoreTags
{
	UE_DEFINE_GAMEPLAY_TAG(Tier1, FName(TEXT("RobotParts.Core.Tier1")));
	UE_DEFINE_GAMEPLAY_TAG(Tier2, FName(TEXT("RobotParts.Core.Tier2")));
	UE_DEFINE_GAMEPLAY_TAG(Tier3, FName(TEXT("RobotParts.Core.Tier3")));
	UE_DEFINE_GAMEPLAY_TAG(Tier4, FName(TEXT("RobotParts.Core.Tier4")));
}

namespace LegTags
{
	UE_DEFINE_GAMEPLAY_TAG(BipedalLight, FName(TEXT("RobotParts.Leg.Bipedal.Light")));
	UE_DEFINE_GAMEPLAY_TAG(QuadrupedalLight, FName(TEXT("RobotParts.Leg.Quadrupedal.Light")));
	UE_DEFINE_GAMEPLAY_TAG(ReverseBipedalLight, FName(TEXT("RobotParts.Leg.ReverseBipedal.Light")));
}
