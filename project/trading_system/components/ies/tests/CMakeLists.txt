add_target_tests(
  TARGET ${COMPONENT_NAME}
  UNIT_TESTS
    unit_tests/phases/phase_scheduler_test.cpp
    unit_tests/phases/states_test.cpp
    unit_tests/phase_schedule_test.cpp
    unit_tests/tick_event_factory_test.cpp
    unit_tests/trading_phase_controller_test.cpp)