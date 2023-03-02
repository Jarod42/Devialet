#pragma once

#include <iostream>

#include "player.h"

namespace iplayer
{

	class Shell
	{
	public:
		explicit Shell(std::shared_ptr<Player> player) : player(std::move(player)) {}
		void run(std::istream& is, std::ostream& os);

	private:
		std::shared_ptr<Player> player;
	};
}
