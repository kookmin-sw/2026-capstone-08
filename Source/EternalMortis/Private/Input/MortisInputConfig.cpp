// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/MortisInputConfig.h"

UInputAction* UMortisInputConfig::FindNativeInputActionByTag(const FGameplayTag& InputTagToFind) const
{
    for (const FMortisInputActionConfig& InputActionConfig : NativeInputActions)
    {
        if (InputActionConfig.InputAction && InputActionConfig.InputTag == InputTagToFind)
            return InputActionConfig.InputAction;
    }

    return nullptr;
}
