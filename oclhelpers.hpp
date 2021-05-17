#pragma once
#include <CL/cl.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

#define OCL_SAFE_CALL(call)                                                    \
  {                                                                            \
    auto status = (call);                                                      \
    if (status) {                                                              \
      std::stringstream ss;                                                    \
      ss << "OpenCL error: " << std::hex << (int)status << " " << std::dec     \
         << "(" << oclhelpers::get_error_string((int)status) << ")\n\tat "     \
         << __FILE__ << ":" << __LINE__ << std::endl;                          \
      throw oclhelpers::OCLHelpersException(ss.str());                         \
    }                                                                          \
  }

namespace oclhelpers {

class OCLHelpersException : public std::runtime_error {
public:
  explicit OCLHelpersException(const std::string &what)
      : std::runtime_error(what) {}
  virtual ~OCLHelpersException() noexcept {}
};

std::vector<cl::Platform> get_platforms();
cl::Platform get_default_platform();
cl::Platform get_platform_matching(std::string_view s);

std::vector<cl::Device> get_gpus(const cl::Platform &p);
std::vector<cl::Device> get_cpus(const cl::Platform &p);
cl::Device get_default_device(const cl::Platform &p);
cl::Device get_default_gpu(const cl::Platform &p);
cl::Device get_default_cpu(const cl::Platform &p);

std::string read_kernel_from_file(const std::string &filename);

cl::Program make_program_from_file(cl::Context &ctx,
                                   const std::string &filename);

void build(cl::Program &program, const cl::Device &device);

/*
The program is compiled with the default device on default platform.
Usage example:
    auto [platform, device, context, program] =
        compile_file_with_defaults("kernel.cl");
*/
std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_defaults(const std::string &filename);

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_default_cpu(const std::string &filename);

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_default_cpu(cl::Platform &platform,
                              const std::string &filename);

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_default_gpu(const std::string &filename);

std::tuple<cl::Platform, cl::Device, cl::Context, cl::Program>
compile_file_with_default_gpu(cl::Platform &platform,
                              const std::string &filename);

namespace {
inline void _set_args(cl::Kernel &k, int pos) {}
template <class T, class... Ts>
inline void _set_args(cl::Kernel &k, int pos, const T &arg,
                      const Ts &... args) {
  k.setArg(pos, arg);
  _set_args(k, pos + 1, args...);
}
} // namespace

template <class... Ts> inline void set_args(cl::Kernel &k, const Ts &... args) {
  _set_args(k, 0, args...);
}
std::string get_error_string(int error);

} // namespace oclhelpers
