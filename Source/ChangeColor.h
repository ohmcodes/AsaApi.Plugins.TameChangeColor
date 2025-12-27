

/* /cd 1-15,2-23,3-44 */
void TargetColorDinoCallBack(AShooterPlayerController* pc, FString* param, int, int)
{
    auto perms = GetPriorPermByEOSID(pc->GetEOSId());
    auto command = GetCommandString(perms.ToString(), "ColorDinoCMD");

    if (command.is_null() || (!command.is_null() && !command.value("Enabled", false)))
    {
        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PermErrorMSG", "You don't have permission to use this command.").c_str());

        return;
    }

    // points checking
    if (Points(pc->GetEOSId(), command.value("Cost", 0), true) == false)
    {
        if (TameChangeColor::config["Debug"].value("Points", false) == true)
        {
            Log::GetLog()->info("{} don't have points. Command: {}", pc->GetCharacterName().ToString(), __FUNCTION__);
        }

        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PointsErrorMSG", "Not enough points.").c_str());

        return;
    }

    ACharacter* character = pc->CharacterField().Get();
    APrimalCharacter* primalCharacter = static_cast<APrimalCharacter*>(character);

    AActor* actor = primalCharacter->GetAimedActor(ECollisionChannel::ECC_GameTraceChannel2, 0i64, 0.0, 0.0, 0i64, 0i64, false, false, false, 0i64);

    TArray<FString> parsed;
    param->ParseIntoArray(parsed, L" ", false);

    if (!parsed.IsValidIndex(1)) return;

    TArray<FString> colorArray;
    if (parsed[1].Contains(","))
    {
        parsed[1].ParseIntoArray(colorArray, L",", true);
    }
    else
    {
        colorArray.Add(parsed[1]);
    }

    if (!actor->IsA(APrimalDinoCharacter::GetPrivateStaticClass())) return;

    auto* dino = static_cast<APrimalDinoCharacter*>(actor);

    if (!dino) return;

    int dinoTeam = dino->TargetingTeamField();
    int playerTeam = pc->TargetingTeamField();

    if (dinoTeam != playerTeam) return;

    if (dino->IsDead()) return;

    bool affectedDino;
    for (auto color : colorArray)
    {
        if (color.Contains("-"))
        {

            // points deductions
            Points(pc->GetEOSId(), command.value("Cost", 0));

            TArray<FString> colorValues;
            color.ParseIntoArray(colorValues, L"-", true);

            //Log::GetLog()->info("Color Index {} Color Value {}", colorValues[0].ToString(), colorValues[1].ToString());

            dino->ForceUpdateColorSets(FCString::Atoi(*colorValues[0]), FCString::Atoi(*colorValues[1]));

            affectedDino = true;
        }
    }

    //dino->RefreshColorization(true);

    if (affectedDino)
    {
        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Green, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("ColorDinoMSG", "Dino color has been changed!").c_str());

        std::string msg = fmt::format("Player {} change dino {} color", pc->GetCharacterName().ToString(), dino->DescriptiveNameField().ToString());

        SendMessageToDiscord(msg);
    }
}

/* /acd Rex 1-15,2-25,3-44 5000 */
void AoeColorDinoCallBack(AShooterPlayerController* pc, FString* param, int, int)
{
    auto perms = GetPriorPermByEOSID(pc->GetEOSId());
    auto command = GetCommandString(perms.ToString(), "ColorDinoAOECMD");

    if (command.is_null() || (!command.is_null() && !command.value("Enabled", false)))
    {
        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PermErrorMSG", "You don't have permission to use this command.").c_str());

        return;
    }

    // points checking
    if (Points(pc->GetEOSId(), command.value("Cost", 0), true) == false)
    {
        if (TameChangeColor::config["Debug"].value("Points", false) == true)
        {
            Log::GetLog()->info("{} don't have points. Command: {}", pc->GetCharacterName().ToString(), __FUNCTION__);
        }

        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PointsErrorMSG", "Not enough points.").c_str());

        return;
    }

    FVector pc_location = AsaApi::GetApiUtils().GetPosition(pc);

    //float aoe_max_range = TameChangeColor::config["General"].value("CryoMaxRange", 2000);
    float aoe_max_range = static_cast<float>(command.value("AoeMaxRange", TameChangeColor::config["General"].value("DefaultAoeMaxRange", 5000.0f)));

    TArray<FString> parsed;
    param->ParseIntoArray(parsed, L" ", false);

    if (!parsed.IsValidIndex(2)) return;

    TArray<FString> colorArray;
    if (parsed[2].Contains(","))
    {
        parsed[2].ParseIntoArray(colorArray, L",", true);
    }
    else
    {
        colorArray.Add(parsed[2]);
    }

    //float param_aoe_range = parsed.IsValidIndex(4) ? FCString::Atoi(*parsed[4]) : 2000;
    int param_index = 3;
    float param_aoe_range = parsed.IsValidIndex(param_index) && !parsed[param_index].IsEmpty() ? static_cast<float>(FCString::Atoi(*parsed[param_index])) : TameChangeColor::config["General"].value("DefaultAoeMaxRange", 5000.0f);

    float aoe_range = (aoe_max_range >= param_aoe_range) ? param_aoe_range : aoe_max_range;

    TArray<AActor*> dino_in_range = AsaApi::GetApiUtils().GetAllActorsInRange(pc_location, aoe_range, EServerOctreeGroup::Type::DINOPAWNS_TAMED);

    int coloredDino = 0;
    for (AActor* actor : dino_in_range)
    {
        auto* dino = static_cast<APrimalDinoCharacter*>(actor);

        if (!dino) continue;

        int dinoTeam = dino->TargetingTeamField();
        int playerTeam = pc->TargetingTeamField();

        if (dinoTeam != playerTeam) continue;

        if (dino->IsDead()) continue;

        /*FARKDinoData dinoData;
        dino->GetDinoData(&dinoData);
        FString dinoName = dinoData.DinoName.ToLower();*/
        FString dinoName = dino->DescriptiveNameField().ToLower();

        //Log::GetLog()->info(dinoData.DinoNameField().ToString());

        if (parsed.IsValidIndex(1))
        {
            //Log::GetLog()->info(parsed[1].ToString());

            if (!dinoName.ToLower().Contains(parsed[1].ToLower()))
            {
                continue;
            }
        }

        for (auto color : colorArray)
        {
            if (color.Contains("-"))
            {
                // points deductions
                Points(pc->GetEOSId(), command.value("Cost", 0));

                TArray<FString> colorValues;
                color.ParseIntoArray(colorValues, L"-", true);

                //Log::GetLog()->info("Color Index {} Color Value {}", colorValues[0].ToString(), colorValues[1].ToString());

                dino->ForceUpdateColorSets(FCString::Atoi(*colorValues[0]), FCString::Atoi(*colorValues[1]));

                coloredDino++;
            }
        }
    }

    if (coloredDino > 0)
    {
        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Green, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("ColorDinoAoeMSG", "Dino in range color has been changed!").c_str());

        std::string msg = fmt::format("Player {} change dino color in range", pc->GetCharacterName().ToString());

        SendMessageToDiscord(msg);
    }
}

void TargetCopyColorDinoCallBack(AShooterPlayerController* pc, FString* param, int, int)
{
    auto perms = GetPriorPermByEOSID(pc->GetEOSId());
    auto command = GetCommandString(perms.ToString(), "CopyColorDinoCMD");

    if (command.is_null() || (!command.is_null() && !command.value("Enabled", false)))
    {
        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PermErrorMSG", "You don't have permission to use this command.").c_str());

        return;
    }

    // points checking
    if (Points(pc->GetEOSId(), command.value("Cost", 0), true) == false)
    {
        if (TameChangeColor::config["Debug"].value("Points", false) == true)
        {
            Log::GetLog()->info("{} don't have points. Command: {}", pc->GetCharacterName().ToString(), __FUNCTION__);
        }

        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PointsErrorMSG", "Not enough points.").c_str());

        return;
    }

    ACharacter* character = pc->CharacterField().Get();
    APrimalCharacter* primalCharacter = static_cast<APrimalCharacter*>(character);

    AActor* actor = primalCharacter->GetAimedActor(ECollisionChannel::ECC_GameTraceChannel2, 0i64, 0.0, 0.0, 0i64, 0i64, false, false, false, 0i64);

    if (!actor->IsA(APrimalDinoCharacter::GetPrivateStaticClass())) return;

    auto* dino = static_cast<APrimalDinoCharacter*>(actor);

    if (!dino) return;

    //Log::GetLog()->info("-----------------------------");

    FieldArray<unsigned __int8, 6> csif = dino->ColorSetIndicesField();

    TameChangeColor::CopiedColor.Empty();
    TameChangeColor::CopiedColorDino = dino;

    for (size_t i = 0; i < csif.GetSize(); ++i)
    {
        int c = static_cast<int>(csif()[i]);
        //Log::GetLog()->info("ColorSetIndicesField {}", c);

        TameChangeColor::CopiedColor.Add(i, c);
    }

    // points deductions
    Points(pc->GetEOSId(), command.value("Cost", 0));

    AsaApi::GetApiUtils().SendNotification(pc, FColorList::Green, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("CopyColorDinoMSG", "Color dino has been copied.").c_str());

    std::string msg = fmt::format("Player {} copy dino {} color", pc->GetCharacterName().ToString(), dino->DescriptiveNameField().ToString());

    SendMessageToDiscord(msg);
}

void TargetApplyColorDinoCallBack(AShooterPlayerController* pc, FString* param, int, int)
{
    auto perms = GetPriorPermByEOSID(pc->GetEOSId());
    auto command = GetCommandString(perms.ToString(), "ApplyCopiedColorDinoCMD");

    if (command.is_null() || (!command.is_null() && !command.value("Enabled", false)))
    {
        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PermErrorMSG", "You don't have permission to use this command.").c_str());

        return;
    }

    // points checking
    if (Points(pc->GetEOSId(), command.value("Cost", 0), true) == false)
    {
        if (TameChangeColor::config["Debug"].value("Points", false) == true)
        {
            Log::GetLog()->info("{} don't have points. Command: {}", pc->GetCharacterName().ToString(), __FUNCTION__);
        }

        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PointsErrorMSG", "Not enough points.").c_str());

        return;
    }

    ACharacter* character = pc->CharacterField().Get();
    APrimalCharacter* primalCharacter = static_cast<APrimalCharacter*>(character);

    AActor* actor = primalCharacter->GetAimedActor(ECollisionChannel::ECC_GameTraceChannel2, 0i64, 0.0, 0.0, 0i64, 0i64, false, false, false, 0i64);

    if (!actor->IsA(APrimalDinoCharacter::GetPrivateStaticClass())) return;

    auto* dino = static_cast<APrimalDinoCharacter*>(actor);

    if (!dino) return;

    int dinoTeam = dino->TargetingTeamField();
    int playerTeam = pc->TargetingTeamField();

    if (dinoTeam != playerTeam) return;

    if (dino->IsDead()) return;

    if (!TameChangeColor::CopiedColorDino) return;

    if (TameChangeColor::CopiedColorDino == dino)
    {
        AsaApi::GetApiUtils().SendServerMessage(pc, FColorList::Red, "Copied Dino is different color indices");
        return;
    }

    for (auto color : TameChangeColor::CopiedColor)
    {
        // points deductions
        Points(pc->GetEOSId(), command.value("Cost", 0));

        dino->ForceUpdateColorSets(color.Key, color.Value);
    }

    AsaApi::GetApiUtils().SendNotification(pc, FColorList::Green, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("ApplyCopiedColorDinoMSG", "Copied color applied.").c_str());

    std::string msg = fmt::format("Player {} applied color to dino {}", pc->GetCharacterName().ToString(), dino->DescriptiveNameField().ToString());

    SendMessageToDiscord(msg);
}

void AoeApplyColorDinoCallBack(AShooterPlayerController* pc, FString* param, int, int)
{
    auto perms = GetPriorPermByEOSID(pc->GetEOSId());
    auto command = GetCommandString(perms.ToString(), "ApplyCopiedColorDinoAOECMD");

    if (command.is_null() || (!command.is_null() && !command.value("Enabled", false)))
    {
        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PermErrorMSG", "You don't have permission to use this command.").c_str());

        return;
    }

    // points checking
    if (Points(pc->GetEOSId(), command.value("Cost", 0), true) == false)
    {
        if (TameChangeColor::config["Debug"].value("Points", false) == true)
        {
            Log::GetLog()->info("{} don't have points. Command: {}", pc->GetCharacterName().ToString(), __FUNCTION__);
        }

        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Red, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("PointsErrorMSG", "Not enough points.").c_str());

        return;
    }

    FVector pc_location = AsaApi::GetApiUtils().GetPosition(pc);

    //float aoe_max_range = TameChangeColor::config["General"].value("CryoMaxRange", 2000);
    float aoe_max_range = static_cast<float>(command.value("AoeMaxRange", TameChangeColor::config["General"].value("DefaultAoeMaxRange", 5000.0f)));

    TArray<FString> parsed;
    param->ParseIntoArray(parsed, L" ", false);

    //float param_aoe_range = parsed.IsValidIndex(4) ? FCString::Atoi(*parsed[2]) : 2000;
    int param_index = 4;
    float param_aoe_range = parsed.IsValidIndex(param_index) && !parsed[param_index].IsEmpty() ? static_cast<float>(FCString::Atoi(*parsed[param_index])) : TameChangeColor::config["General"].value("DefaultAoeMaxRange", 5000.0f);

    float aoe_range = (aoe_max_range >= param_aoe_range) ? param_aoe_range : aoe_max_range;

    TArray<AActor*> dino_in_range = AsaApi::GetApiUtils().GetAllActorsInRange(pc_location, aoe_range, EServerOctreeGroup::Type::DINOPAWNS_TAMED);

    int coloredDino = 0;
    for (AActor* actor : dino_in_range)
    {
        auto* dino = static_cast<APrimalDinoCharacter*>(actor);

        if (!dino) continue;

        int dinoTeam = dino->TargetingTeamField();
        int playerTeam = pc->TargetingTeamField();

        if (dinoTeam != playerTeam) continue;

        if (dino->IsDead()) continue;

        /*FARKDinoData dinoData;
        dino->GetDinoData(&dinoData);
        FString dinoName = dinoData.DinoName.ToLower();*/

        FString dinoName = dino->DescriptiveNameField().ToLower();

        //Log::GetLog()->info(dinoData.DinoNameField().ToString());

        if (parsed.IsValidIndex(1))
        {
            //Log::GetLog()->info(parsed[1].ToString());

            if (!dinoName.ToLower().Contains(parsed[1].ToLower()))
            {
                continue;
            }
        }

        if (!TameChangeColor::CopiedColorDino) continue;

        if (TameChangeColor::CopiedColorDino == dino) continue;

        for (auto color : TameChangeColor::CopiedColor)
        {
            // points deductions
            Points(pc->GetEOSId(), command.value("Cost", 0));

            dino->ForceUpdateColorSets(color.Key, color.Value);
        }

        coloredDino++;
    }

    if (coloredDino > 0)
    {
        AsaApi::GetApiUtils().SendNotification(pc, FColorList::Green, 1.3f, 15.0f, nullptr, TameChangeColor::config["Messages"].value("ApplyCoppiedColorAoeMSG", "Copied color applied to all dino in range.").c_str());

        std::string msg = fmt::format("Player {} applied dino color in range", pc->GetCharacterName().ToString());

        SendMessageToDiscord(msg);
    }
}