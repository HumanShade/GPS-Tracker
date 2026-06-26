# Pure-logic helper component, shared by the firmware (target) and the host tests
# (Linux target). Keep it free of hardware/RTOS dependencies so it stays host-testable.
idf_component_register(
    SRCS "banner.c"
    INCLUDE_DIRS "include"
)
