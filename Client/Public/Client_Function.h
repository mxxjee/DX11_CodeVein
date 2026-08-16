
NS_BEGIN(Client)

inline _bool Is_Playable_Level(LEVEL _level)
{
	switch (_level)
	{
	case LEVEL::MAIN:
		return true;
	case LEVEL::SAMPLE:
		return true;
	}

	return false;
}

inline _bool Is_Playable_Level(_uint _level)
{
	LEVEL level = CAST(LEVEL)(_level);

	return Is_Playable_Level(level);
}





NS_END