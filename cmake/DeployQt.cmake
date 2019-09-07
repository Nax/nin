function(DeployQt _path)
	find_program(_prg windeployqt
		PATHS $ENV{QTDIR}/bin/
    )
	if(_prg)
		message(STATUS "Found ${_prg}")
	else()
		message(FATAL_ERROR "Unable to find a suitable Qt deployment tool")
	endif()

	execute_process(COMMAND "${_prg}" "--release" "${_path}" "--no-compiler-runtime" "--no-opengl-sw" "--no-system-d3d-compiler")
endfunction()
