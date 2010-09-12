#ifndef matrix_mult_h
#define matrix_mult_h

#include <cstdlib>

#include <OpenCL/OpenCL.h>

struct CLEnv
{
     cl_context        context;
     cl_command_queue  command_queue;
     cl_program        program;
     cl_kernel         kernel;
     cl_device_id*     devices;
     size_t            device_size;
     cl_int            errcode;
};

void opencl_setup( CLEnv& env);
void opencl_mat_mul(CLEnv& env, float* h_A, float* h_B, float* h_C );
void opencl_unsetup( CLEnv& env);

#endif // matrix_mult_h