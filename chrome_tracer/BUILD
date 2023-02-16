cc_library(
    name = "chrome_tracer",
    hdrs = [
        "tracer.h",
        "event.h",
    ],
    srcs = [
        "tracer.cc",
    ],
    deps = [
        "@jsoncpp//:jsoncpp",
    ]
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