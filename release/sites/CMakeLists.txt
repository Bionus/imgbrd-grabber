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
	DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/package.json"
	COMMAND npm install
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Installing npm packages..."
)

add_custom_command(
	OUTPUT JavaScript_sites
	COMMAND npm run build
	DEPENDS NPM_modules ${SOURCES}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Transpiling TypeScript sources into JavaScript..."
)

add_custom_target(sites ALL DEPENDS JavaScript_sites)
