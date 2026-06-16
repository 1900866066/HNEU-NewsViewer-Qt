include("D:/Mycode/Python/HNEU-NewsViewer-Qt/HNEU-Viewer/build/Desktop_Qt_6_8_3_MinGW_64_bit-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/HNEU-Viewer-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE D:/Mycode/Python/HNEU-NewsViewer-Qt/HNEU-Viewer/build/Desktop_Qt_6_8_3_MinGW_64_bit-Debug/HNEU-Viewer.exe
    GENERATE_QT_CONF
)
