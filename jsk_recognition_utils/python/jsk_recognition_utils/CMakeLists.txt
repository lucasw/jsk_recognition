if(NOT DEFINED Numpy_INCLUDE_DIRS)
  # Get Numpy include directories
  execute_process(
    COMMAND python$ENV{ROS_PYTHON_VERSION} -c "import sys, numpy; sys.stdout.write(numpy.get_include())"
    OUTPUT_VARIABLE Numpy_INCLUDE_DIRS
    RESULT_VARIABLE retcode)
  if(NOT ${retcode} EQUAL 0)
    message(FATAL_ERROR "Failed to get Numpy include dirs by numpy.get_include(). Exit code: ${retcode}")
  endif()
endif()
# Compile nms.pyx
include_directories(${Numpy_INCLUDE_DIRS})
cython_add_module(nms nms.pyx)
set_target_properties(nms PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CATKIN_DEVEL_PREFIX}/${CATKIN_PACKAGE_PYTHON_DESTINATION})
install(TARGETS nms
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_PYTHON_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_PYTHON_DESTINATION}
)
