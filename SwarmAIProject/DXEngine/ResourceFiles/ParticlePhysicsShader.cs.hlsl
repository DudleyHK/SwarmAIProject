/*



*/


struct Particle
{
	float3 position;
	float3 velocity;
	float3 acceleration;
};


ConsumeStructuredBuffer<Particle> gInput;
AppendStructuredBuffer<Particle> gOutput;


[numthreads(256, 1, 1)]
void ParticlePhysicsShader()
{

}