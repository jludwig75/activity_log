# Proto file
get_filename_component(activity_proto "${CMAKE_CURRENT_SOURCE_DIR}/activity_log.proto" ABSOLUTE)
get_filename_component(activity_proto_path "${activity_proto}" PATH)

# Generated sources
set(activity_proto_srcs "${CMAKE_CURRENT_SOURCE_DIR}/activity_log.pb.cc")
set(activity_proto_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/activity_log.pb.h")
set(activity_grpc_srcs "${CMAKE_CURRENT_SOURCE_DIR}/activity_log.grpc.pb.cc")
set(activity_grpc_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/activity_log.grpc.pb.h")
add_custom_command(
      OUTPUT "${activity_proto_srcs}" "${activity_proto_hdrs}" "${activity_grpc_srcs}" "${activity_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_SOURCE_DIR}"
        --cpp_out "${CMAKE_CURRENT_SOURCE_DIR}"
        -I "${activity_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${activity_proto}"
      DEPENDS "${activity_proto}")

add_library(activity_proto STATIC
            ${activity_proto_srcs}
            ${activity_grpc_srcs})

target_link_libraries(activity_proto
            ${_REFLECTION}
            ${_GRPC_GRPCPP}
            ${_PROTOBUF_LIBPROTOBUF})

set_target_properties(activity_proto PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
  