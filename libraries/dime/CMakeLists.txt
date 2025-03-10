set(NAME "dime")

# include(${PROJECT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake)

CPMAddPackage(
    NAME  dime
    GIT_REPOSITORY  https://github.com/coin3d/dime.git
    GIT_TAG dime-0.9.1
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL TRUE
    DOWNLOAD_ONLY   TRUE
)

add_library(dime STATIC
${dime_SOURCE_DIR}/src/Base.cpp 
${dime_SOURCE_DIR}/src/Basic.cpp 
${dime_SOURCE_DIR}/src/Input.cpp 
${dime_SOURCE_DIR}/src/Layer.cpp 
${dime_SOURCE_DIR}/src/Model.cpp 
${dime_SOURCE_DIR}/src/Output.cpp 
${dime_SOURCE_DIR}/src/RecordHolder.cpp 
${dime_SOURCE_DIR}/src/State.cpp 
${dime_SOURCE_DIR}/src/classes/Class.cpp 
${dime_SOURCE_DIR}/src/classes/UnknownClass.cpp 
${dime_SOURCE_DIR}/src/convert/3dfaceconvert.cpp 
${dime_SOURCE_DIR}/src/convert/arcconvert.cpp 
${dime_SOURCE_DIR}/src/convert/circleconvert.cpp 
${dime_SOURCE_DIR}/src/convert/convert.cpp 
${dime_SOURCE_DIR}/src/convert/ellipseconvert.cpp 
${dime_SOURCE_DIR}/src/convert/layerdata.cpp 
${dime_SOURCE_DIR}/src/convert/lineconvert.cpp 
${dime_SOURCE_DIR}/src/convert/linesegment.cpp 
${dime_SOURCE_DIR}/src/convert/lwpolylineconvert.cpp 
${dime_SOURCE_DIR}/src/convert/pointconvert.cpp 
${dime_SOURCE_DIR}/src/convert/polylineconvert.cpp 
${dime_SOURCE_DIR}/src/convert/solidconvert.cpp 
${dime_SOURCE_DIR}/src/convert/traceconvert.cpp 
${dime_SOURCE_DIR}/src/entities/3DFace.cpp 
${dime_SOURCE_DIR}/src/entities/Arc.cpp 
${dime_SOURCE_DIR}/src/entities/Block.cpp 
${dime_SOURCE_DIR}/src/entities/Circle.cpp 
${dime_SOURCE_DIR}/src/entities/Ellipse.cpp 
${dime_SOURCE_DIR}/src/entities/Entity.cpp 
${dime_SOURCE_DIR}/src/entities/ExtrusionEntity.cpp 
${dime_SOURCE_DIR}/src/entities/FaceEntity.cpp 
${dime_SOURCE_DIR}/src/entities/Insert.cpp 
${dime_SOURCE_DIR}/src/entities/LWPolyline.cpp 
${dime_SOURCE_DIR}/src/entities/Line.cpp 
${dime_SOURCE_DIR}/src/entities/Point.cpp 
${dime_SOURCE_DIR}/src/entities/Polyline.cpp 
${dime_SOURCE_DIR}/src/entities/Solid.cpp 
${dime_SOURCE_DIR}/src/entities/Spline.cpp 
${dime_SOURCE_DIR}/src/entities/Text.cpp 
${dime_SOURCE_DIR}/src/entities/Trace.cpp 
${dime_SOURCE_DIR}/src/entities/UnknownEntity.cpp 
${dime_SOURCE_DIR}/src/entities/Vertex.cpp 
${dime_SOURCE_DIR}/src/objects/Object.cpp 
${dime_SOURCE_DIR}/src/objects/UnknownObject.cpp 
${dime_SOURCE_DIR}/src/records/DoubleRecord.cpp 
${dime_SOURCE_DIR}/src/records/FloatRecord.cpp 
${dime_SOURCE_DIR}/src/records/HexRecord.cpp 
${dime_SOURCE_DIR}/src/records/Int16Record.cpp 
${dime_SOURCE_DIR}/src/records/Int32Record.cpp 
${dime_SOURCE_DIR}/src/records/Int8Record.cpp 
${dime_SOURCE_DIR}/src/records/Record.cpp 
${dime_SOURCE_DIR}/src/records/StringRecord.cpp 
${dime_SOURCE_DIR}/src/sections/BlocksSection.cpp 
${dime_SOURCE_DIR}/src/sections/ClassesSection.cpp 
${dime_SOURCE_DIR}/src/sections/EntitiesSection.cpp 
${dime_SOURCE_DIR}/src/sections/HeaderSection.cpp 
${dime_SOURCE_DIR}/src/sections/ObjectsSection.cpp 
${dime_SOURCE_DIR}/src/sections/Section.cpp 
${dime_SOURCE_DIR}/src/sections/TablesSection.cpp 
${dime_SOURCE_DIR}/src/sections/UnknownSection.cpp 
${dime_SOURCE_DIR}/src/tables/LayerTable.cpp 
${dime_SOURCE_DIR}/src/tables/Table.cpp 
${dime_SOURCE_DIR}/src/tables/TableEntry.cpp 
${dime_SOURCE_DIR}/src/tables/UCSTable.cpp 
${dime_SOURCE_DIR}/src/tables/UnknownTable.cpp 
${dime_SOURCE_DIR}/src/util/Array.cpp 
${dime_SOURCE_DIR}/src/util/BSPTree.cpp 
${dime_SOURCE_DIR}/src/util/Box.cpp 
${dime_SOURCE_DIR}/src/util/Dict.cpp 
${dime_SOURCE_DIR}/src/util/Linear.cpp 
${dime_SOURCE_DIR}/src/util/MemHandler.cpp 
${dime_SOURCE_DIR}/include/dime/Base.h 
${dime_SOURCE_DIR}/include/dime/Basic.h 
${dime_SOURCE_DIR}/include/dime/Input.h 
${dime_SOURCE_DIR}/include/dime/Layer.h 
${dime_SOURCE_DIR}/include/dime/Model.h 
${dime_SOURCE_DIR}/include/dime/Output.h 
${dime_SOURCE_DIR}/include/dime/RecordHolder.h 
${dime_SOURCE_DIR}/include/dime/State.h 
${dime_SOURCE_DIR}/include/dime/classes/Class.h 
${dime_SOURCE_DIR}/include/dime/classes/UnknownClass.h 
${dime_SOURCE_DIR}/include/dime/convert/convert.h 
${dime_SOURCE_DIR}/include/dime/convert/layerdata.h 
${dime_SOURCE_DIR}/include/dime/entities/3DFace.h 
${dime_SOURCE_DIR}/include/dime/entities/Arc.h 
${dime_SOURCE_DIR}/include/dime/entities/Block.h 
${dime_SOURCE_DIR}/include/dime/entities/Circle.h 
${dime_SOURCE_DIR}/include/dime/entities/Ellipse.h 
${dime_SOURCE_DIR}/include/dime/entities/Entity.h 
${dime_SOURCE_DIR}/include/dime/entities/ExtrusionEntity.h 
${dime_SOURCE_DIR}/include/dime/entities/FaceEntity.h 
${dime_SOURCE_DIR}/include/dime/entities/Insert.h 
${dime_SOURCE_DIR}/include/dime/entities/LWPolyline.h 
${dime_SOURCE_DIR}/include/dime/entities/Line.h 
${dime_SOURCE_DIR}/include/dime/entities/Point.h 
${dime_SOURCE_DIR}/include/dime/entities/Polyline.h 
${dime_SOURCE_DIR}/include/dime/entities/Solid.h 
${dime_SOURCE_DIR}/include/dime/entities/Spline.h 
${dime_SOURCE_DIR}/include/dime/entities/Text.h 
${dime_SOURCE_DIR}/include/dime/entities/Trace.h 
${dime_SOURCE_DIR}/include/dime/entities/UnknownEntity.h 
${dime_SOURCE_DIR}/include/dime/entities/Vertex.h 
${dime_SOURCE_DIR}/include/dime/objects/Object.h 
${dime_SOURCE_DIR}/include/dime/objects/UnknownObject.h 
${dime_SOURCE_DIR}/include/dime/records/DoubleRecord.h 
${dime_SOURCE_DIR}/include/dime/records/FloatRecord.h 
${dime_SOURCE_DIR}/include/dime/records/HexRecord.h 
${dime_SOURCE_DIR}/include/dime/records/Int16Record.h 
${dime_SOURCE_DIR}/include/dime/records/Int32Record.h 
${dime_SOURCE_DIR}/include/dime/records/Int8Record.h 
${dime_SOURCE_DIR}/include/dime/records/Record.h 
${dime_SOURCE_DIR}/include/dime/records/StringRecord.h 
${dime_SOURCE_DIR}/include/dime/sections/BlocksSection.h 
${dime_SOURCE_DIR}/include/dime/sections/ClassesSection.h 
${dime_SOURCE_DIR}/include/dime/sections/EntitiesSection.h 
${dime_SOURCE_DIR}/include/dime/sections/HeaderSection.h 
${dime_SOURCE_DIR}/include/dime/sections/ObjectsSection.h 
${dime_SOURCE_DIR}/include/dime/sections/Section.h 
${dime_SOURCE_DIR}/include/dime/sections/TablesSection.h 
${dime_SOURCE_DIR}/include/dime/sections/UnknownSection.h 
${dime_SOURCE_DIR}/include/dime/tables/LayerTable.h 
${dime_SOURCE_DIR}/include/dime/tables/Table.h 
${dime_SOURCE_DIR}/include/dime/tables/TableEntry.h 
${dime_SOURCE_DIR}/include/dime/tables/UCSTable.h 
${dime_SOURCE_DIR}/include/dime/tables/UnknownTable.h 
${dime_SOURCE_DIR}/include/dime/util/Array.h 
${dime_SOURCE_DIR}/include/dime/util/BSPTree.h 
${dime_SOURCE_DIR}/include/dime/util/Box.h 
${dime_SOURCE_DIR}/include/dime/util/Dict.h 
${dime_SOURCE_DIR}/include/dime/util/Linear.h 
${dime_SOURCE_DIR}/include/dime/util/MemHandler.h 
${dime_SOURCE_DIR}/src/convert/convert_funcs.h 
${dime_SOURCE_DIR}/src/convert/linesegment.h 
)

target_compile_definitions(dime PUBLIC DIME_NOT_DLL)

target_include_directories(
    dime
    PUBLIC
    $<BUILD_INTERFACE:${dime_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${dime_SOURCE_DIR}/include>
)

set_target_properties(dime PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
