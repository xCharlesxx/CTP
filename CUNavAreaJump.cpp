// Fill out your copyright notice in the Description page of Project Settings.


#include "CUNavAreaJump.h"

 UCUNavAreaJump::UCUNavAreaJump(const class FObjectInitializer& PCIP)
	 : Super(PCIP)
{
	FNavAreaHelper::Set(AreaFlags, ENavAreaFlag::Jump);
}