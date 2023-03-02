#include "player.h"

#include "testutils.h"

#include <doctest.h>

using namespace std::literals;

struct MockMusicPlayer : iplayer::IMusicPlayer
{
	bool openMusic(const std::filesystem::path& path) override
	{
		this->path = path;
		return true;
	}
	void pause() override { inPause = true; }
	void play() override
	{
		inPause = false;
		elapsedTime = 1s;
	}
	void setElapsedTime(const std::chrono::seconds& s) override { elapsedTime = s; }
	std::chrono::seconds getElapsedTime() override { return elapsedTime; }
	void setOnMusicFinished(std::function<void()> f) { onMusicFinished = f; }

	std::function<void()> onMusicFinished;
	std::filesystem::path path;
	std::chrono::seconds elapsedTime = 0s;
	bool inPause = true;
};

//------------------------------------------------------------------------------
TEST_CASE("Next")
{
	auto mock = std::make_shared<MockMusicPlayer>();
	iplayer::Player player{mock, buildPlaylist({0, 1, 2})};

	CHECK(mock->path.empty());
	CHECK(mock->inPause);
	player.next();
	CHECK_EQ(makeTrack(0).filename, mock->path);
	player.next();
	CHECK_EQ(makeTrack(1).filename, mock->path);
	player.next();
	CHECK_EQ(makeTrack(2).filename, mock->path);
	player.next();
	CHECK_EQ(makeTrack(2).filename, mock->path);
	player.setRepeatMode(true);
	player.next();
	CHECK_EQ(makeTrack(0).filename, mock->path);
}

//------------------------------------------------------------------------------
TEST_CASE("Previous")
{
	auto mock = std::make_shared<MockMusicPlayer>();
	iplayer::Player player{mock, buildPlaylist({0, 1, 2})};

	CHECK(mock->path.empty());
	CHECK(mock->inPause);
	player.select(0);
	CHECK_EQ(makeTrack(0).filename, mock->path);
	player.previous();
	CHECK_EQ(makeTrack(0).filename, mock->path);
	player.setRepeatMode(true);
	player.previous();
	CHECK_EQ(makeTrack(2).filename, mock->path);
	player.previous();
	CHECK_EQ(makeTrack(1).filename, mock->path);
}

//------------------------------------------------------------------------------
TEST_CASE("OnMusicFinished")
{
	auto mock = std::make_shared<MockMusicPlayer>();
	iplayer::Player player{mock, buildPlaylist({0, 1, 2, 3})};

	CHECK(mock->path.empty());
	CHECK(mock->inPause);
	player.play();
	CHECK_EQ(makeTrack(0).filename, mock->path);
	CHECK(!mock->inPause);
	mock->onMusicFinished();
	CHECK_EQ(makeTrack(1).filename, mock->path);
	CHECK(!mock->inPause);
}

//------------------------------------------------------------------------------
TEST_CASE("Random")
{
	auto mock = std::make_shared<MockMusicPlayer>();
	iplayer::Player player{mock, buildPlaylist({0, 1, 2, 3})};

	player.setRandomMode(true);
	player.select(0);

	std::vector<std::filesystem::path> paths;
	for (std::size_t i = 0; i != 4; ++i) {
		paths.push_back(mock->path);
		player.next();
	}
	for (std::size_t i = 4; i != 0; --i) {
		CHECK_EQ(paths[i - 1], mock->path);
		player.previous();
	}
	CHECK_EQ(paths[0], mock->path);
}

//------------------------------------------------------------------------------
TEST_CASE("Playlist::insertAt")
{
	auto mock = std::make_shared<MockMusicPlayer>();
	iplayer::Player player{mock, buildPlaylist({0, 2, 3})};

	player.select(2);
	REQUIRE_EQ(makeTrack(3).filename, mock->path);
	player.insertAt(1, makeTrack(1));
	player.previous();
	REQUIRE_EQ(makeTrack(2).filename, mock->path);
}

//------------------------------------------------------------------------------
TEST_CASE("Playlist::remove")
{
	auto mock = std::make_shared<MockMusicPlayer>();
	iplayer::Player player{mock, buildPlaylist({0, 1, 2, 3})};

	player.select(2);
	REQUIRE_EQ(makeTrack(2).filename, mock->path);
	player.remove(1);
	player.previous();
	REQUIRE_EQ(makeTrack(0).filename, mock->path);
}

//------------------------------------------------------------------------------
TEST_CASE("Playlist::move")
{
	auto mock = std::make_shared<MockMusicPlayer>();
	iplayer::Player player{mock, buildPlaylist({0, 1, 2, 3})};

	player.select(2);
	REQUIRE_EQ(makeTrack(2).filename, mock->path);
	player.move(1, 2); // 0, 2, 1, 3
	player.previous();
	REQUIRE_EQ(makeTrack(0).filename, mock->path);
	player.next();
	REQUIRE_EQ(makeTrack(2).filename, mock->path);

	player.move(0, 3); // 2, 1, 3, 0
	player.next();
	REQUIRE_EQ(makeTrack(1).filename, mock->path);
	player.previous();
	REQUIRE_EQ(makeTrack(2).filename, mock->path);

	player.move(0, 3); // 1, 3, 0, 2
	player.previous();
	REQUIRE_EQ(makeTrack(0).filename, mock->path);
	player.next();
	REQUIRE_EQ(makeTrack(2).filename, mock->path);


}
