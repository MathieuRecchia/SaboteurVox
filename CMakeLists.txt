cmake_minimum_required(VERSION 3.20)
project(SaboteurVox VERSION 1.0.0)

include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        9.0.1
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(SaboteurVox
    COMPANY_NAME "Saboteur"
    PLUGIN_MANUFACTURER_CODE "SbtR"
    PLUGIN_CODE "SbVx"
    FORMATS VST3
    PRODUCT_NAME "Saboteur Vox"
    NEEDS_JUCE_HEADER TRUE
)

target_sources(SaboteurVox PRIVATE
    Source/SaboteurVoxProcessor.cpp
    Source/SaboteurVoxEditor.cpp
)

target_compile_features(SaboteurVox PRIVATE cxx_std_20)
target_compile_definitions(SaboteurVox PUBLIC JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(SaboteurVox PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)