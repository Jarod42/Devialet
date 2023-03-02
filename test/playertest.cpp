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
	void play() override {
		inPause = false;
		elapsedTime = 1s;
	}
	void setElapsedTime(const std::chrono::seconds& s) override { elapsedTime = s; }
	std::chrono::seconds getElapsedTime() override { return elapsedTime; }
	void setOnMusicFinished(std::function<void()> f) { onMusicFinished = f; }

	std::function<void()> onMusicFinished;
	std::filesystem::path path;
	std::chrono::seconds elapsedTime;
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
TEST_CASE("Pause")
{
	auto mock = std::make_shared<MockMusicPlayer>();
	iplayer::Player player{mock, buildPlaylist({0, 1, 2, 3})};

	player.select(1);
	CHECK_EQ(makeTrack(1).filename, mock->path);
	CHECK(mock->inPause);
	mock->setElapsedTime(1s);
	player.next();
	CHECK_EQ(makeTrack(2).filename, mock->path);
	CHECK(mock->inPause);
	CHECK_EQ(0s, mock->elapsedTime);
	mock->setElapsedTime(1s);
	player.previous();
	CHECK_EQ(makeTrack(1).filename, mock->path);
	CHECK(mock->inPause);
	CHECK_EQ(0s, mock->elapsedTime);

	player.play();
	CHECK_EQ(makeTrack(1).filename, mock->path);
	CHECK(!mock->inPause);
	player.next();
	CHECK_EQ(makeTrack(2).filename, mock->path);
	CHECK(!mock->inPause);
	player.previous();
	CHECK_EQ(makeTrack(1).filename, mock->path);
	CHECK(!mock->inPause);
}
