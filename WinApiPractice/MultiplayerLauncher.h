#pragma once
#include "SharedStorage.h"

class MultiplayerLauncher
{
public:
	explicit MultiplayerLauncher();

	void LaunchClient() const;

private:
	struct ClientsCounter
	{
		int clients = 0;
	};
	
	SharedStorage storage;
	ClientsCounter* counter = nullptr;
};
