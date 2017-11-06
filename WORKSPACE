bind(
    name = "nanopb",
    actual = "//third_party/nanopb",
)

bind(
    name = "libssl",
    actual = "@boringssl//:ssl",
)

bind(
    name = "zlib",
    actual = "@submodule_zlib//:z",
)

bind(
    name = "protobuf",
    actual = "@com_google_protobuf//:protobuf",
)

bind(
    name = "protobuf_clib",
    actual = "@com_google_protobuf//:protoc_lib",
)

bind(
    name = "protobuf_headers",
    actual = "@com_google_protobuf//:protobuf_headers",
)

bind(
    name = "protocol_compiler",
    actual = "@com_google_protobuf//:protoc",
)

bind(
    name = "cares",
    actual = "@submodule_cares//:ares",
)

bind(
    name = "gtest",
    actual = "@submodule_gtest//:gtest",
)

bind(
    name = "gmock",
    actual = "@submodule_gtest//:gmock",
)

bind(
    name = "benchmark",
    actual = "@submodule_benchmark//:benchmark",
)

bind(
    name = "gflags",
    actual = "@com_github_gflags_gflags//:gflags",
)

local_repository(
    name = "boringssl",
    path = "third_party/boringssl-with-bazel",
)

new_local_repository(
    name = "submodule_zlib",
    build_file = "third_party/zlib.BUILD",
    path = "third_party/zlib",
)

new_local_repository(
    name = "com_google_protobuf",
    build_file = "third_party/protobuf/BUILD",
    path = "third_party/protobuf",
)

new_local_repository(
    name = "submodule_gtest",
    build_file = "third_party/gtest.BUILD",
    path = "third_party/googletest",
)

local_repository(
    name = "com_github_gflags_gflags",
    path = "third_party/gflags",
)

new_local_repository(
    name = "submodule_benchmark",
    path = "third_party/benchmark",
    build_file = "third_party/benchmark.BUILD",
)

new_local_repository(
    name = "submodule_cares",
    path = "third_party/cares",
    build_file = "third_party/cares/cares.BUILD",
)

local_repository(
    name = "com_google_absl",
    path = "third_party/abseil-cpp",
)

http_archive(
    name = "bazel_toolchains",
    urls = [
        "http://mirror.bazel.build/github.com/bazelbuild/bazel-toolchains/archive/9dbd803ad3b9447430a296810197b09b3a710956.tar.gz",
        "https://github.com/bazelbuild/bazel-toolchains/archive/9dbd803ad3b9447430a296810197b09b3a710956.tar.gz",
    ],
    strip_prefix = "bazel-toolchains-9dbd803ad3b9447430a296810197b09b3a710956",
    sha256 = "0799aa12db5260a499beb40f81744e760c59d055bfc5d271dd2c2ed4d5419faa",
)
