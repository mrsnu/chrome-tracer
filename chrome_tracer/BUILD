package(
    licenses = ["notice"],  # Apache 2.0
)

config_setting(
    name = "linux",
    constraint_values = [
        "@platforms//os:linux",
    ],
)

config_setting(
    name = "android",
    constraint_values = [
        "@platforms//os:android",
    ],
)

config_setting(
    name = "windows",
    constraint_values = [
        "@platforms//os:windows",
    ],
)

cc_library(
    name = "utils",
    hdrs = [
        "utils.h",
    ],
)

cc_library(
    name = "chrome_tracer",
    hdrs = [
        "tracer_interface.h",
        "tracer.h",
        "stream_tracer.h"
        "event.h",
    ],
    srcs = [
        "tracer.cc",
        "stream_tracer.cc",
    ],
    deps = [
        ":utils",
    ]
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "test_main",
    srcs = [
        "test_main.cc",
    ],
    deps = [
        ":chrome_tracer",
    ],
)

cc_binary(
    name = "test_thread_main",
    srcs = [
        "test_thread_main.cc",
    ],
    linkopts = select({
        "//chrome_tracer:linux": ["-lpthread"],
        "//conditions:default": [],
    }),
    deps = [
        ":chrome_tracer",
    ],
)
