cmake_minimum_required(VERSION 2.8)

# Build the ivy file.
CONFIGURE_FILE(ivy.xml ivy-${VERSION_FULL}.xml)

# Filter the target.xml file.
SET(ARTIFACT_DIRECTORY    "${CMAKE_CURRENT_BINARY_DIR}")
SET(ARTIFACT_ARTIFACT_ID  "muhkuh_base_cli")
SET(ARTIFACT_GROUP_ID     "tools.muhkuh.org")
SET(ARTIFACT_VERSION      "${VERSION_FULL}")
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../artifacts.xml ${ARTIFACT_LISTS_OUTPUT}/artifacts_muhkuh_base_cli.xml)
