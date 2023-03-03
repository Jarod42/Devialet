local Root = path.getabsolute("..")

if (_ACTION == nil) then
	return
end

local LocationDir = path.join(Root, "solution", _ACTION)

workspace "Project"
	location(LocationDir)
	configurations {"Debug", "Release"}

	objdir(path.join(LocationDir, "obj")) -- premake adds $(configName)/$(AppName)
	targetdir(path.join(LocationDir, "bin", "%{cfg.buildcfg}"))
	-- startproject "app"
	startproject "test"

group "3rd"
	project "doctest"
		kind "None"
		files { path.join(Root, "3rd/doctest/**.*") }
group ""

project "app"
	kind "ConsoleApp"
	cppdialect "C++20"

	targetname("iplayer")
	files { path.join(Root, "src/app/**.*") }
	includedirs { path.join(Root, "src/lib") }
	links { "lib" }

project "lib"
	kind "StaticLib"
	cppdialect "C++20"

	targetname("iplayerlib")
	files {path.join(Root, "src/lib/**.*")}

project "test"
	kind "ConsoleApp"
	cppdialect "C++20"

	targetname("test")
	files { path.join(Root, "test/**.*") }
	includedirs { path.join(Root, "src/lib") }
	includedirs { path.join(Root, "test/") }
	includedirs { path.join(Root, "3rd/doctest") }
	links { "lib" }
	debugdir (LocationDir)

project "data"
	kind "None"

	files { path.join(Root, "data/**.*") }
