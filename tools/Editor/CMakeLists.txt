SET(LIB_NAME "Editor")

SET(HEADER_PATH ${INCLUDE_PATH}/${LIB_NAME})

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/glfw/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/glew/include)

IF(NOT BUILD_OPENGL_ES)
    ADD_DEFINITIONS(-DGLEW_STATIC)
ENDIF()

SET(LIGHTMASS_LIST 
	HemisphereWeight.h
	LightMass.h
	LightMass.cpp
	LightMassAmbientOcclusion.h
	LightMassAmbientOcclusion.cpp
	LightMassBaking.h
	LightMassBaking.cpp
	LightMassGlobalIllumination.h
	LightMassGlobalIllumination.cpp
	LightMassListener.h
	LightMassListener.cpp
	LightMassParams.h
	LightMassParams.cpp
	LightMassTypes.h
)
SOURCE_GROUP("LightMass" FILES ${LIGHTMASS_LIST})

SET(LIGHTMAPPACK_LIST 
	LightMapListener.h
	LightMapListener.cpp
	LightMapPack.h
	LightMapPack.cpp
	LightMapTypes.h
	trianglepacker.hpp
)
SOURCE_GROUP("LightMapPack" FILES ${LIGHTMAPPACK_LIST})

SET(APP_LIST 
	main.cpp
	modpmx.h
	modpmx.cpp
)
SOURCE_GROUP("Common" FILES ${APP_LIST})

SET(COMPONENT_LIST 
	UIView.h
	UIView.cpp
	UIParams.h
	UIParams.cpp
	UIController.h
	UIController.cpp
	UITexts.h
	UITexts.cpp
	first_person_camera.cpp
	first_person_camera.h
)
SOURCE_GROUP("Components" FILES ${COMPONENT_LIST})

FILE(GLOB HLSLPARSE_LIST "HLSLParser/*.*")
SOURCE_GROUP("HLSLParser" FILES ${HLSLPARSE_LIST})

FILE(GLOB IESLOADER_LIST "IESLoader/*.*")
SOURCE_GROUP("IESLoader" FILES ${IESLOADER_LIST})

SET(NATIVE_FILED_DIALOG_LIST 
	nativefiledialog/nfd.h
	nativefiledialog/nfd_common.c
	nativefiledialog/nfd_win.cpp
)
SOURCE_GROUP("NativeFiledDialog" FILES ${NATIVE_FILED_DIALOG_LIST})

ADD_EXECUTABLE(${LIB_NAME} ${APP_LIST} ${LIGHTMASS_LIST} ${LIGHTMAPPACK_LIST} ${COMPONENT_LIST} ${HLSLPARSE_LIST} ${IESLOADER_LIST} ${NATIVE_FILED_DIALOG_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME} "ray-c")
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE glew)
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE opengl32)