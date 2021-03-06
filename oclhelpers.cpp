#include "oclhelpers.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>

namespace oclhelpers {
std::vector<cl::Platform> get_platforms() {
  std::vector<cl::Platform> platforms;
  cl::Platform::get(&platforms);
  if (!platforms.size()) {
    throw OCLHelpersException("No platform found!");
  }
  return platforms;
}

cl::Platform get_platform_matching(std::string_view s) {
  auto platforms = get_platforms();
  for (auto &p : platforms) {
    auto pname = p.getInfo<CL_PLATFORM_NAME>();
    if (pname.find(s) != std::string::npos)
      return p;
  }
}

cl::Platform get_default_platform() { return get_platforms()[0]; }

std::vector<cl::Device> get_devices(const cl::Platform &p, int type) {
  std::vector<cl::Device> devices;
  p.getDevices(type, &devices);
  return devices;
}

std::vector<cl::Device> get_gpus(const cl::Platform &p) {
  return get_devices(p, CL_DEVICE_TYPE_GPU);
}

std::vector<cl::Device> get_cpus(const cl::Platform &p) {
  return get_devices(p, CL_DEVICE_TYPE_CPU);
}

cl::Device get_default_device(const cl::Platform &p) { return get_gpus(p)[0]; }

// fixme
cl::Device get_default_gpu(const cl::Platform &p) { return get_gpus(p)[0]; }
cl::Device get_default_cpu(const cl::Platform &p) { return get_cpus(p)[0]; }

std::string read_kernel_from_file(const std::string &filename) {
  std::ifstream is(filename);
  return std::string(std::istreambuf_iterator<char>(is),
                     std::istreambuf_iterator<char>());
}

cl::Program make_program_from_file(cl::Context &ctx,
                                   const std::string &filename) {
  cl::Program::Sources sources;
  auto kernel_code = read_kernel_from_file(filename);
  if (kernel_code.empty()) {
    throw OCLHelpersException("Kernel code is empty!");
  }
  return {ctx, kernel_code};
}

void build(cl::Program &program, const cl::Device &device) {
  if (program.build({device}) != CL_SUCCESS) {
    std::stringstream ss;
    ss << "Building failed: "
       << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
    throw OCLHelpersException(ss.str());
  }
}

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with(const std::string &filename, const cl::Platform &platform,
                  const cl::Device &device) {
  cl::Context ctx({device});
  auto program = make_program_from_file(ctx, filename);
  build(program, device);
  return std::make_tuple(platform, device, ctx, program);
}

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_defaults(const std::string &filename) {
  auto platform = get_default_platform();
  auto device = get_default_device(platform);

  return compile_file_with(filename, platform, device);
}

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_default_cpu(const std::string &filename) {
  for (auto &p : get_platforms()) {
    if (get_cpus(p).size()) {
      return compile_file_with(filename, p, get_default_cpu(p));
    }
  }

  throw OCLHelpersException("No cpu found!");
}

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_default_cpu(cl::Platform &platform,
                              const std::string &filename) {
  auto device = get_default_cpu(platform);
  return compile_file_with(filename, platform, device);
}

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_default_gpu(const std::string &filename) {
  for (auto &p : get_platforms()) {
    if (get_gpus(p).size()) {
      return compile_file_with(filename, p, get_default_gpu(p));
    }
  }

  throw OCLHelpersException("No gpu found!");
}

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_default_gpu(cl::Platform &platform,
                              const std::string &filename) {
  auto device = get_default_gpu(platform);
  return compile_file_with(filename, platform, device);
}

std::string get_error_string(int error) {
  // source:
  // https://stackoverflow.com/questions/24326432/convenient-way-to-show-opencl-error-codes
  switch (error) {
  // run-time and JIT compiler errors
  case 0:
    return "CL_SUCCESS";
  case -1:
    return "CL_DEVICE_NOT_FOUND";
  case -2:
    return "CL_DEVICE_NOT_AVAILABLE";
  case -3:
    return "CL_COMPILER_NOT_AVAILABLE";
  case -4:
    return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
  case -5:
    return "CL_OUT_OF_RESOURCES";
  case -6:
    return "CL_OUT_OF_HOST_MEMORY";
  case -7:
    return "CL_PROFILING_INFO_NOT_AVAILABLE";
  case -8:
    return "CL_MEM_COPY_OVERLAP";
  case -9:
    return "CL_IMAGE_FORMAT_MISMATCH";
  case -10:
    return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
  case -11:
    return "CL_BUILD_PROGRAM_FAILURE";
  case -12:
    return "CL_MAP_FAILURE";
  case -13:
    return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
  case -14:
    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
  case -15:
    return "CL_COMPILE_PROGRAM_FAILURE";
  case -16:
    return "CL_LINKER_NOT_AVAILABLE";
  case -17:
    return "CL_LINK_PROGRAM_FAILURE";
  case -18:
    return "CL_DEVICE_PARTITION_FAILED";
  case -19:
    return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

  // compile-time errors
  case -30:
    return "CL_INVALID_VALUE";
  case -31:
    return "CL_INVALID_DEVICE_TYPE";
  case -32:
    return "CL_INVALID_PLATFORM";
  case -33:
    return "CL_INVALID_DEVICE";
  case -34:
    return "CL_INVALID_CONTEXT";
  case -35:
    return "CL_INVALID_QUEUE_PROPERTIES";
  case -36:
    return "CL_INVALID_COMMAND_QUEUE";
  case -37:
    return "CL_INVALID_HOST_PTR";
  case -38:
    return "CL_INVALID_MEM_OBJECT";
  case -39:
    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
  case -40:
    return "CL_INVALID_IMAGE_SIZE";
  case -41:
    return "CL_INVALID_SAMPLER";
  case -42:
    return "CL_INVALID_BINARY";
  case -43:
    return "CL_INVALID_BUILD_OPTIONS";
  case -44:
    return "CL_INVALID_PROGRAM";
  case -45:
    return "CL_INVALID_PROGRAM_EXECUTABLE";
  case -46:
    return "CL_INVALID_KERNEL_NAME";
  case -47:
    return "CL_INVALID_KERNEL_DEFINITION";
  case -48:
    return "CL_INVALID_KERNEL";
  case -49:
    return "CL_INVALID_ARG_INDEX";
  case -50:
    return "CL_INVALID_ARG_VALUE";
  case -51:
    return "CL_INVALID_ARG_SIZE";
  case -52:
    return "CL_INVALID_KERNEL_ARGS";
  case -53:
    return "CL_INVALID_WORK_DIMENSION";
  case -54:
    return "CL_INVALID_WORK_GROUP_SIZE";
  case -55:
    return "CL_INVALID_WORK_ITEM_SIZE";
  case -56:
    return "CL_INVALID_GLOBAL_OFFSET";
  case -57:
    return "CL_INVALID_EVENT_WAIT_LIST";
  case -58:
    return "CL_INVALID_EVENT";
  case -59:
    return "CL_INVALID_OPERATION";
  case -60:
    return "CL_INVALID_GL_OBJECT";
  case -61:
    return "CL_INVALID_BUFFER_SIZE";
  case -62:
    return "CL_INVALID_MIP_LEVEL";
  case -63:
    return "CL_INVALID_GLOBAL_WORK_SIZE";
  case -64:
    return "CL_INVALID_PROPERTY";
  case -65:
    return "CL_INVALID_IMAGE_DESCRIPTOR";
  case -66:
    return "CL_INVALID_COMPILER_OPTIONS";
  case -67:
    return "CL_INVALID_LINKER_OPTIONS";
  case -68:
    return "CL_INVALID_DEVICE_PARTITION_COUNT";

  // extension errors
  case -1000:
    return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
  case -1001:
    return "CL_PLATFORM_NOT_FOUND_KHR";
  case -1002:
    return "CL_INVALID_D3D10_DEVICE_KHR";
  case -1003:
    return "CL_INVALID_D3D10_RESOURCE_KHR";
  case -1004:
    return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
  case -1005:
    return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
  default:
    return "Unknown OpenCL error";
  }
}
} // namespace oclhelpers