void basicSstencil(int imin, int imax, const float *A, float *B)
{
	for (int i = imin; i <= imax; ++i) {
		B[i] = A[i-1] + A[i+1];
	}
}
