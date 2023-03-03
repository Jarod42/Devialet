#pragma once

#include <iostream>

#include "player.h"

namespace iplayer
{

	class Shell
	{
	public:
		Shell(std::shared_ptr<Player>, std::istream&, std::ostream&);
		void run();

	private:
		std::shared_ptr<Player> player;
		std::istream& is;
		std::ostream& os;
	};
}
