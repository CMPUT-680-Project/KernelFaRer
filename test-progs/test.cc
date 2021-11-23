volatile float v;
volatile int o;
void test(const float *A, float *B, float *C, int n, float a, float b, float c)
{
	int j = 0;
	if (a > 10) {
		j = 1;
	}
	for (int i = 0; i <= n; ++i) {
		if (i == 0) {
			B[i] = A[j];
		} else {
			B[i] = A[j - 1];
		}
		// B[i] = -(3 * A[2*i-1] + 2 * A[2*i+1] + 0.25f * (a + b + c + v));
	}
}

