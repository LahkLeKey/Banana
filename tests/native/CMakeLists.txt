if(WIN32)
  set(DEMO_FRAME_OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../artifacts/native/032-demo-frame-qa/runs")
  set(DEMO_FRAME_RUN_LABEL "dx12-poc-autotest")
  set(DEMO_FRAME_VALIDATE_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/feedback/validate_demo_frame_bundle.sh")
  set(DEMO_FRAME_SMOKE_CMD "\"$<TARGET_FILE:banana_engine_win32_dx12_poc>\" && bash \"${DEMO_FRAME_VALIDATE_SCRIPT}\" --run-label \"${DEMO_FRAME_RUN_LABEL}\" --output-dir \"${DEMO_FRAME_OUTPUT_DIR}\"")

  add_test(NAME banana_engine_win32_dx12_poc_demo_frame_smoke
    COMMAND ${CMAKE_COMMAND} -E env
      BANANA_DX12_POC_AUTOTEST=1
      BANANA_DX12_POC_AUTOTEST_SECONDS=2
      BANANA_DX12_POC_SCENE_VARIANT=0
      BANANA_DEMO_FRAME_EXPORT=1
      BANANA_DEMO_FRAME_OUTPUT_DIR=${DEMO_FRAME_OUTPUT_DIR}
      BANANA_DEMO_FRAME_RUN_LABEL=${DEMO_FRAME_RUN_LABEL}
      bash -lc "${DEMO_FRAME_SMOKE_CMD}"
  )
endif()
