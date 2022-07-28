kernel void vadd(global float *a, global float *b.global float *c) {
  int i = get_global_id(0);
  c[i] = a[i] + b[i];
}
