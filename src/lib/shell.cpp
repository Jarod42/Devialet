#include "shell.h"

#include <functional>
#include <map>
#include <sstream>
#include <string>

namespace
{
//------------------------------------------------------------------------------
void showHelp(iplayer::Player&, std::ostream&, std::istream&);

//------------------------------------------------------------------------------
void cd(iplayer::Player&, std::ostream& os, std::istream& is)
{
	os << "Current path was " << std::filesystem::current_path() << "\n";
	std::string directory;
	if (std::getline(is, directory)) {
		std::filesystem::current_path(directory);
	} else {
		os << "Invalid argument\n";
	}
	os << "Current path is " << std::filesystem::current_path() << "\n";
}


//------------------------------------------------------------------------------
void add_track(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	std::string filename;
	is >> filename;
	try {
		os << "Adding track " << filename << "\n";
		player.push_back(iplayer::openTrack(filename));
		os << "Added\n";
		player.info_track(os, player.getTrackCount() - 1);
	}
	catch (const std::exception& ex) {
		os << ex.what() << "\n";
		os << "Current path is " << std::filesystem::current_path() << "\n";
	}
}
//------------------------------------------------------------------------------
void move_track(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	std::size_t from;
	std::size_t to;
	if (is >> from >> to) {
		os << "Moving track from " << from << " to " << to << "\n";
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
		os << "Removing track " << pos << "\n";
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
void play(iplayer::Player& player, std::ostream& os, std::istream&)
{
	os << "Play\n";
	player.play();
	auto index = player.getSelectionIndex();
	if (index) {
		os << "Playing: " << player.getTrack(*index).title << "\n";
	} else {
		os << "Nothing to play\n";
	}
}
//------------------------------------------------------------------------------
void pause(iplayer::Player& player, std::ostream& os, std::istream&)
{
	os << "Pause\n";
	player.pause();
}
//------------------------------------------------------------------------------
void stop(iplayer::Player& player, std::ostream& os, std::istream&)
{
	os << "Stop\n";
	player.stop();
}
//------------------------------------------------------------------------------
void next(iplayer::Player& player, std::ostream&os, std::istream&)
{
	os << "Next\n";
	player.next();
	auto index = player.getSelectionIndex();
	if (index) {
		os << "selection: " << player.getTrack(*index).title << "\n";
	} else {
		os << "No selection\n";
	}
}
//------------------------------------------------------------------------------
void previous(iplayer::Player& player, std::ostream&os, std::istream&)
{
	os << "Previous\n";
	player.previous();
	auto index = player.getSelectionIndex();
	if (index) {
		os << "selection: " << player.getTrack(*index).title << "\n";
	} else {
		os << "No selection\n";
	}
}
//------------------------------------------------------------------------------
void select(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	std::size_t pos;
	if (is >> pos) {
		os << "Select " << pos << "\n";
		player.select(pos);
		auto index = player.getSelectionIndex();
		if (index) {
			os << "selection: " << player.getTrack(*index).title << "\n";
		} else {
			os << "No selection\n";
		}
	} else {
		os << "Invalid argument\n";
	}
}
//------------------------------------------------------------------------------
void set_repeat(iplayer::Player& player, std::ostream& os, std::istream& is)
{
	bool b;
	if (is >> b) {
		os << "Set repeat mode " << b << "\n";
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
		os << "Set random mode " << b << "\n";
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
	{"cd", {cd, " $directory"}},
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
	{"select", {select, " $pos"}},
	{"set_repeat", {set_repeat, " $bool"}},
	{"set_random", {set_random, " $bool"}},
};

//------------------------------------------------------------------------------
void showHelp(iplayer::Player&, std::ostream& os, std::istream&)
{
	os << "Commands are:\n";
	for (const auto& [key, value] : commands) {
		os << "- " << key << value.extraParam << "\n";
	}
	os << "- exit\n";
}

} // namespace

namespace iplayer
{

//------------------------------------------------------------------------------
void Shell::run(std::istream& is, std::ostream& os)
{
	os << "Imaginary player\n";
	{
		std::stringstream ss(std::string("../../data"));
		cd(*player, os, ss);
	}
	showHelp(*player, os, is);
	while (true) {
		os << "> ";
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
			it->second.func(*player, os, ss);
		} else {
			os << "invalid command\n";
		}
	}
}

} // namespace iplayer
