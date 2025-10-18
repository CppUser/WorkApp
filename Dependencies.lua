function firstToUpper(str)
	return (str:gsub("^%l", string.upper))
end

function __genOrderedIndex(t)
	local orderedIndex = {}
	for key in pairs(t) do
		table.insert(orderedIndex, key)
	end
	table.sort(orderedIndex)
	return orderedIndex
end

function getTableSize(t)
	local count = 0
	for _, __ in pairs(t) do
		count = count + 1
	end
	return count
end

function orderedNext(t, state)
	local key = nil
	if state == nil then
		t.__orderedIndex = __genOrderedIndex(t)
		key = t.__orderedIndex[1]
	else
		for i = 1, getTableSize(t) do
			if t.__orderedIndex[i] == state then
				key = t.__orderedIndex[i + 1]
			end
		end
	end

	if key then
		return key, t[key]
	end

	t.__orderedIndex = nil
	return
end

function orderedPairs(t)
	return orderedNext, t, nil
end

-- ======================
-- DEPENDENCIES
-- ======================

Dependencies = {
	-- Windows platform system libs
	WS2 = { Windows = { LibName = "Ws2_32" } },
	Dbghelp = { Windows = { LibName = "Dbghelp" } },
	OpenGL = { Windows = { LibName = "opengl32" } },

	-- GLFW
	GLFW = {
		IncludeDir = "%{wks.location}/Core/tplibs/glfw/include",
		Windows = {
			LibDir = "%{wks.location}/Core/tplibs/glfw/lib",
			LibName = "glfw3",
		}
	},

	-- GLAD
	Glad = {
		IncludeDir = "%{wks.location}/Core/tplibs/glad/include",
		Src = "%{wks.location}/Core/tplibs/glad/src/glad.c"
	},

	-- IMGUI
	ImGui = {
		IncludeDir = "%{wks.location}/Core/tplibs/imgui",
		BackendDir = "%{wks.location}/Core/tplibs/imgui/backends",
		Src = {
			"%{wks.location}/Core/tplibs/imgui/imgui.cpp",
			"%{wks.location}/Core/tplibs/imgui/imgui_draw.cpp",
			"%{wks.location}/Core/tplibs/imgui/imgui_demo.cpp",
			"%{wks.location}/Core/tplibs/imgui/imgui_tables.cpp",
			"%{wks.location}/Core/tplibs/imgui/imgui_widgets.cpp",
			"%{wks.location}/Core/tplibs/imgui/backends/imgui_impl_glfw.cpp",
			"%{wks.location}/Core/tplibs/imgui/backends/imgui_impl_opengl3.cpp",
		}
	},

	-- SPDLOG
	Spdlog = {
		IncludeDir = "%{wks.location}/Core/tplibs/spdlog/include"
	},

	-- TDLib
	TdLib = {
		IncludeDir = "%{wks.location}/Core/tplibs/tdlib/include",
		Windows = {
			LibDir = "%{wks.location}/Core/tplibs/tdlib/lib",
			LibName = "tdjson.lib",
			BinDir = "%{wks.location}/Core/tplibs/tdlib/bin"
		}
	}
}

-- ======================
-- FUNCTIONS
-- ======================

function LinkDependency(table, is_debug, target)
	if table.LibDir ~= nil then
		libdirs { table.LibDir }
	end

	local libraryName = nil
	if table.LibName ~= nil then
		libraryName = table.LibName
	end

	if table.DebugLibName ~= nil and is_debug and target == "Windows" then
		libraryName = table.DebugLibName
	end

	if libraryName ~= nil then
		links { libraryName }
		return true
	end

	return false
end

function AddDependencyIncludes(table)
	if table.IncludeDir ~= nil then
		externalincludedirs { table.IncludeDir }
	end
	if table.BackendDir ~= nil then
		externalincludedirs { table.BackendDir }
	end
end

function ProcessDependencies(config_name)
	local target = firstToUpper(os.target())

	for key, libraryData in orderedPairs(Dependencies) do
		local matchesConfiguration = true
		if config_name ~= nil and libraryData.Configurations ~= nil then
			matchesConfiguration = string.find(libraryData.Configurations, config_name)
		end

		local isDebug = config_name == "Debug"

		if matchesConfiguration then
			local continueLink = true

			if libraryData[target] ~= nil then
				continueLink = not LinkDependency(libraryData[target], isDebug, target)
				AddDependencyIncludes(libraryData[target])
			end

			if continueLink then
				LinkDependency(libraryData, isDebug, target)
			end

			AddDependencyIncludes(libraryData)
		end
	end
end

function IncludeDependencies(config_name)
	local target = firstToUpper(os.target())
	for key, libraryData in orderedPairs(Dependencies) do
		local matchesConfiguration = true
		if config_name ~= nil and libraryData.Configurations ~= nil then
			matchesConfiguration = string.find(libraryData.Configurations, config_name)
		end
		if matchesConfiguration then
			AddDependencyIncludes(libraryData)
			if libraryData[target] ~= nil then
				AddDependencyIncludes(libraryData[target])
			end
		end
	end
end
