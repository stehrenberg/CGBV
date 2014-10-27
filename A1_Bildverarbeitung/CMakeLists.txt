
SET(PROJECT_NAME A1_Bildverarbeitung)

set(SOURCES ImageProcessing.cpp
	FragmentShaderBewMit.glsl
	VertexShader.glsl
	FragmentShaderBrightness_Contrast.glsl
	FragmentShaderDilatation.glsl
	FragmentShaderErosion.glsl
	FragmentShaderGauss3x3.glsl
	FragmentShaderGauss5x5.glsl
	FragmentShaderGauss7x7.glsl
	FragmentShaderLaplace.glsl
	FragmentShaderPassthrough.glsl
	FragmentShaderSharpen.glsl
	FragmentShaderSobel.glsl
	Timer.h
	Timer.cpp
 )

set(LIBRARIES GLTools AntTweakBar freeglut ImageLoader)

project(${PROJECT_NAME} )

add_executable(${PROJECT_NAME} ${SOURCES})
create_target_launcher(${PROJECT_NAME}  WORKING_DIRECTORY ${${PROJECT_NAME}_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Aufgaben")
install_sample(${PROJECT_NAME} ${SOURCES})