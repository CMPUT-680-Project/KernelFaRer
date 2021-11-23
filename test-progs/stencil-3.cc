void basicSstencil(int imin, int imax, int jmin, int jmax, const float **A, float **B)
{
	for (int i = imin; i <= imax; ++i) {
		for (int j = jmin; j <= jmax; ++j) {
			B[i][j] = 0.25f * (A[i][j-1] + A[i][j+1] + A[i+1][j] + A[i-1][j]);
		}
	}
}
