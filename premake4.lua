solution "Befunge98 to C Compiler"
	configurations { "debug", "release" }

project "b98toc"
	kind "ConsoleApp"
	language "C++"
	files { "src/**" }
	objdir "build/obj"
	buildoptions { "-std=c++11" }

	configuration "debug"
		flags { "Symbols", "ExtraWarnings" }

	configuration "release"
		flags { "Optimize" }

