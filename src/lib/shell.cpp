#include "shell.h"

#include <functional>
#include <map>
#include <sstream>
#include <string>

namespace
{
//------------------------------------------------------------------------------
void showHelp(iplayer::Player&, std::ostream& os, std::istream&);

//------------------------------------------------------------------------------
void add_track(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	std::string filename;
	is >> filename;
	try {
		player.push_back(iplayer::openTrack(filename));
	}
	catch (const std::exception& ex) {
		os << ex.what();
	}
}
//------------------------------------------------------------------------------
void move_track(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	std::size_t from;
	std::size_t to;
	if (is >> from >> to) {
		player.move(from, to);
	} else {
		os << "Invalid argument\n";
	}
}
//------------------------------------------------------------------------------
void remove_track(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	std::size_t pos;
	if (is >> pos) {
		player.remove(pos);
	} else {
		os << "Invalid argument\n";
	}
}
//------------------------------------------------------------------------------
void info_track(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	std::size_t pos;
	if (is >> pos) {
		player.info_track(os, pos);
	} else {
		os << "Invalid argument\n";
	}
}
//------------------------------------------------------------------------------
void info_tracks(iplayer::Player& player, std::ostream&os, std::istream&)
{
	player.info_tracks(os);
}


//------------------------------------------------------------------------------
void remove_duplicate(iplayer::Player& player, std::ostream&, std::istream&)
{
	player.removeDuplicate();
}
//------------------------------------------------------------------------------
void play(iplayer::Player& player, std::ostream&, std::istream&)
{
	player.play();
}
//------------------------------------------------------------------------------
void pause(iplayer::Player& player, std::ostream&, std::istream&)
{
	player.pause();
}
//------------------------------------------------------------------------------
void stop(iplayer::Player& player, std::ostream&, std::istream&)
{
	player.pause();
}
//------------------------------------------------------------------------------
void next(iplayer::Player& player, std::ostream&, std::istream&)
{
	player.next();
}
//------------------------------------------------------------------------------
void previous(iplayer::Player& player, std::ostream&, std::istream&)
{
	player.previous();
}
//------------------------------------------------------------------------------
void select(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	std::size_t pos;
	if (is >> pos) {
		player.select(pos);
	} else {
		os << "Invalid argument\n";
	}
}
//------------------------------------------------------------------------------
void set_repeat(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	bool b;
	if (is >> b) {
		player.setRepeatMode(b);
	} else {
		os << "Invalid argument\n";
	}
}
//------------------------------------------------------------------------------
void set_random(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	bool b;
	if (is >> b) {
		player.setRandomMode(b);
	} else {
		os << "Invalid argument\n";
	}
}

//------------------------------------------------------------------------------
struct Command
{
	std::function<void(iplayer::Player&, std::ostream&, std::istream&)> func;
	std::string extraParam;
};

//------------------------------------------------------------------------------
const static std::map<std::string, Command> commands{
	{"help", {showHelp}},
	{"add_track", {add_track, " $file ($pos)"}},
	{"move_track", {move_track, " $from $to"}},
	{"remove_track", {remove_track, " $pos"}},
	{"info_track", {info_track, " $pos"}},
	{"info_tracks", {info_tracks}},
	{"remove_duplicate", {remove_duplicate}},
	{"play", {play}},
	{"pause", {pause}},
	{"stop", {stop}},
	{"next", {next}},
	{"previous", {previous}},
	{"select", {select, "$pos"}},
	{"set_repeat", {set_repeat, "$bool"}},
	{"set_random", {set_random, "$bool"}},
};

//------------------------------------------------------------------------------
void showHelp(iplayer::Player&, std::ostream& os, std::istream&)
{
	os << "Commands are:\n";
	for (const auto& [key, value] : commands) {
		os << "- " << key << value.extraParam << "\n";
	}
}

} // namespace

namespace iplayer
{

//------------------------------------------------------------------------------
void Shell::run(std::istream& is, std::ostream& os)
{
	os << "Imaginary player\n";
	showHelp(*player, os, is);
	while (true) {
		std::string line;
		std::getline(is, line);
		std::stringstream ss(line);

		std::string command;
		ss >> command;

		if (command == "exit") {
			os << "Bye.\n";
			break;
		}
		auto it = commands.find(command);
		if (it != commands.end()) {
			it->second.func(*player, os, is);
		} else {
			os << "invalid command";
		}
	}
}

} // namespace iplayer
