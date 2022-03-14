workspace "AppSystem"
    architecture "x64"
    startproject "AppImGuiShell"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
	--flags
	--{
	--	"MultiProcessorCompile"
	--}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["AppImGuiCore"] = "AppImGuiCore/src"

group "Dependencies"
	include "vendor/GLFW"

group ""

project "MCore"
	location "MCore"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
	}

	filter "system:windows"
		cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"
		defines
		{
            "DM_PLATFORM_WINDOWS",
			"DM_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
        }
		postbuildcommands { ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/AppImGuiShell/\"") }
        
	filter "configurations:Debug"
		defines "DM_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "DM_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "DM_DIST"
		buildoptions "/MD"
		optimize "on"

project "MContracts"
	location "MContracts"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/src",
	}

	filter "system:windows"
		cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"
		defines
		{
            "DM_PLATFORM_WINDOWS",
			"DM_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
        }
		postbuildcommands { ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/AppImGuiShell/\"") }
        
	filter "configurations:Debug"
		defines "DM_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "DM_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "DM_DIST"
		buildoptions "/MD"
		optimize "on"

project "AppImGuiCore"
	location "AppImGuiCore"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	includedirs
	{
		"%{prj.name}/src"
	}

	filter "system:windows"
		cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"
		defines
		{
            "DM_PLATFORM_WINDOWS",
			"DM_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
        }
		--postbuildcommands { ("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/AppSystem/\"") }
        
	filter "configurations:Debug"
		defines "DM_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "DM_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "DM_DIST"
		buildoptions "/MD"
		optimize "on"

project "AppImGuiShell"
	location "AppImGuiShell"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"vendor/imgui/**.h",
		"vendor/imgui/**.cpp"
	}

	includedirs
	{
		"vendor/imgui",
		"vendor/GLFW/include",
		"C:/VulkanSDK/1.3.204.1/Include",
		"%{IncludeDir.AppImGuiCore}"
	}
	libdirs
	{
		"AppImGuiCore",
		"vendor/GLFW/bin/Debug-windows-x86_64/GLFW",
		"C:/VulkanSDK/1.3.204.1/Lib"
	}
	links
	{
		"GLFW.lib",
		"vulkan-1.lib"
	}

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "off"
		systemversion "latest"
		--postbuildcommands { "{COPY} %{cfg.buildtarget.relpaht} ../bin/" .. outputdir .. "/DemonGame" }
		defines
		{
            "DM_PLATFORM_WINDOWS"
		}
		
	filter "configurations:Debug"
		defines "DM_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "DM_RELEASE"
		buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "DM_DIST"
		buildoptions "/MD"
		optimize "on"