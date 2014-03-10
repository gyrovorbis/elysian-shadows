# Copyright 2012 Jake Petroules
# via: https://github.com/petroules/solar-cmake

function(group_by_folder relative_to_folder files)
	foreach(core_SOURCE ${files})
		# Get the path of the file relative to the current source directory
		file(RELATIVE_PATH core_SOURCE_relative "${relative_to_folder}" "${core_SOURCE}")

		# Get the relative folder path
		get_filename_component(core_SOURCE_dir "${core_SOURCE_relative}" PATH)

		# Convert forward slashes to backslashes to get source group identifiers
		string(REPLACE "/" "\\" core_SOURCE_group "${core_SOURCE_dir}")

		source_group("${core_SOURCE_group}" FILES "${core_SOURCE}")
	endforeach()
endfunction()
