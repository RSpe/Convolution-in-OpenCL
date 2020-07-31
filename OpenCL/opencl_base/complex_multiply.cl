__kernel void complex_multiply(__global const float2* A,
									__global const float2* B,
									__global float2* C)
{

	// Get the index of the current element to be processed
	int i = get_global_id(0);

	// complex multiply
	C[i].x = (A[i].x * B[i].x) - (A[i].y * B[i].y);
	C[i].y = (A[i].x * B[i].y) + (A[i].y * B[i].x);
}