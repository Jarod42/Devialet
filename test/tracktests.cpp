#include "track.h"

#include <doctest.h>
#include <sstream>

using namespace std::literals;

//------------------------------------------------------------------------------
TEST_CASE("open track")
{
	const iplayer::Track expected{.filename = "c:/somepath/track.txt", .title = "Some title", .duration = 42s};

	std::stringstream ss;
	ss << std::quoted(expected.title) << " " << expected.duration.count();
	
	const iplayer::Track track = iplayer::openTrack(expected.filename, ss);
	CHECK_EQ(expected, track);
}

//------------------------------------------------------------------------------
TEST_CASE("info")
{
	const iplayer::Track track{.filename = "c:/somepath/track.txt", .title = "Some title", .duration = 42s};
	const std::string expected = R"(Filename: track.txt
Title: Some title
Duration: 42s
)";
	std::ostringstream ss;

	iplayer::infoTrack(ss, track);
	CHECK_EQ(expected, ss.str());
}

//------------------------------------------------------------------------------
TEST_CASE("data")
{
	// working dir is at solution/$buildsystem/
	const std::filesystem::path dataDir = "../../data";

	CHECK_NOTHROW(iplayer::openTrack(dataDir / "track1.txt"));
	CHECK_NOTHROW(iplayer::openTrack(dataDir / "track2.txt"));
	CHECK_NOTHROW(iplayer::openTrack(dataDir / "track3.txt"));
	CHECK_NOTHROW(iplayer::openTrack(dataDir / "track4.txt"));

	CHECK_THROWS(iplayer::openTrack(dataDir / "not-exist.txt"));
	CHECK_THROWS(iplayer::openTrack(dataDir / "invalid1.txt"));
	CHECK_THROWS(iplayer::openTrack(dataDir / "invalid2.txt"));
}
