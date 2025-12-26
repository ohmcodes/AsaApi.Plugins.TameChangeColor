

void AddOrRemoveCommands(bool addCmd = true)
{
	const FString TargetColorDino = TameChangeColor::config["Commands"]["ColorDinoCMD"].get<std::string>().c_str();
	if (!TargetColorDino.IsEmpty())
	{
		if (addCmd)
		{
			AsaApi::GetCommands().AddChatCommand(TargetColorDino, &TargetColorDinoCallBack);
		}
		else
		{
			AsaApi::GetCommands().RemoveChatCommand(TargetColorDino);
		}
	}

	const FString AoeColorDino = TameChangeColor::config["Commands"]["ColorDinoAOECMD"].get<std::string>().c_str();
	if (!AoeColorDino.IsEmpty())
	{
		if (addCmd)
		{
			AsaApi::GetCommands().AddChatCommand(AoeColorDino, &AoeColorDinoCallBack);
		}
		else
		{
			AsaApi::GetCommands().RemoveChatCommand(AoeColorDino);
		}
	}
	
	const FString TargetCopyColorDino = TameChangeColor::config["Commands"]["CopyColorDinoCMD"].get<std::string>().c_str();
	if (!TargetCopyColorDino.IsEmpty())
	{
		if (addCmd)
		{
			AsaApi::GetCommands().AddChatCommand(TargetCopyColorDino, &TargetCopyColorDinoCallBack);
		}
		else
		{
			AsaApi::GetCommands().RemoveChatCommand(TargetCopyColorDino);
		}
	}

	const FString TargetApplyCopyColorDino = TameChangeColor::config["Commands"]["ApplyCopiedColorDinoCMD"].get<std::string>().c_str();
	if (!TargetApplyCopyColorDino.IsEmpty())
	{
		if (addCmd)
		{
			AsaApi::GetCommands().AddChatCommand(TargetApplyCopyColorDino, &TargetApplyColorDinoCallBack);
		}
		else
		{
			AsaApi::GetCommands().RemoveChatCommand(TargetApplyCopyColorDino);
		}
	}

	const FString AoeApplyCopyColorDino = TameChangeColor::config["Commands"]["ApplyCopiedColorDinoAOECMD"].get<std::string>().c_str();
	if (!AoeApplyCopyColorDino.IsEmpty())
	{
		if (addCmd)
		{
			AsaApi::GetCommands().AddChatCommand(AoeApplyCopyColorDino, &AoeApplyColorDinoCallBack);
		}
		else
		{
			AsaApi::GetCommands().RemoveChatCommand(AoeApplyCopyColorDino);
		}
	}

}