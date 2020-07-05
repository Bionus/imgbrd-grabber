project(sites)

# Get all TS files to transpile
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/**.ts")

# Exclude type definitions and node_modules from the list
include(ListFilterRegex)
listFilterRegex(SOURCES ".*.d.ts$")
listFilterRegex(SOURCES "node_modules")

# Install NPM dependencies
add_custom_command(
	OUTPUT NPM_modules
	COMMAND npm install
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Installing npm packages..."
)
add_custom_target(sites_modules DEPENDS NPM_modules)

# Transpiling TypeScript files into JavaScript
add_custom_command(
	OUTPUT JavaScript_sites
	COMMAND npm run build
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Transpiling TypeScript sources into JavaScript..."
)

add_custom_target(sites DEPENDS JavaScript_sites)
add_dependencies(sites sites_modules)

add_test(
	NAME sites
	COMMAND npm run test
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
