#version 430 core

in VS_OUT
{
   vec3 N;
   vec3 L;
} fs_in;

void main()
{
   // Normalize the incoming N and L vectors
   vec3 N = normalize(fs_in.N);
   vec3 L = normalize(fs_in.L);

   // Compute the diffuse component for each fragment
   vec3 diffuse = max(dot(N, L), 0.0) * mat_diffuse;

   // Write final color to the framebuffer
   gl_FragColor = vec4(mat_ambient + diffuse, 1.0);
}
