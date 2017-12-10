
cbuffer ParticleConstantBuffer
{
	float4 goalPosition;
	float4 bestPosition;
	float  instanceCount;
	float  moveForce;
	float  seperationForce;
	float  gravityAcceleration;
	bool   computeShaderImplementation;
};

struct ParticleData
{
	float4 seperationForce;
	float4 position;
	float4 velocity;
	float  mass;
};


struct CollisionData
{
	float  closestDist;
	float  radius;
	int    closestID;
};

StructuredBuffer<ParticleData>    ParticleDataInput;
StructuredBuffer<CollisionData>   CollisionDataInput;
RWStructuredBuffer<ParticleData>  ParticleDataOutput;



[numthreads(256, 1, 1)]
void ParticlePhysicsShader(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	const unsigned int P_ID = dispatchThreadID.x;

	float4 p_seperationForce = ParticleDataInput[P_ID].seperationForce;
	float4 p_position = ParticleDataInput[P_ID].position;
	float4 p_velocity = ParticleDataInput[P_ID].velocity;
	float  p_mass = ParticleDataInput[P_ID].mass;

	float  c_closestDist = CollisionDataInput[P_ID].closestDist;
	float  c_radius = CollisionDataInput[P_ID].radius;
	int    c_closestID = CollisionDataInput[P_ID].closestID;


	if(computeShaderImplementation)
	{
		/////////////////////////////////// PHYSICS /////////////////////////////
		float4 p_gravity = (0, 0, 0, 0);
		p_gravity.y = p_mass * gravityAcceleration;

		// Get direction between me and best position
		float4 dir = bestPosition - p_position;

		float computeForce = (dir * moveForce) * p_mass;
		float4 totalForces = p_gravity + computeForce + p_seperationForce;

		float4 acc = (totalForces / p_mass);
		float4 deltaAcc = acc * 0.01f;
		p_velocity += deltaAcc;

		float4 deltaVel = p_velocity * 0.01f;
		p_position += deltaVel;
	}

	///////////////////////////////// COLLISIONS /////////////////////////
	// reset the closest data
	c_closestID = -1;
	c_closestDist = 10000.f;

	float4 repel = (0, 0, 0, 0);

	[loop]
	for(uint N_ID = 0; N_ID < instanceCount; N_ID++)
	{
		if(P_ID == N_ID) continue;

		// get the position of the neighbouring particle.
		float4 n_position = ParticleDataInput[N_ID].position;
		float4 dir = n_position - p_position;
		float  dist = pow(dir, 2);

		if(dist < c_radius && dist < c_closestDist)
		{
			c_closestDist = dist;
			c_closestID = N_ID;

			float4 attract = (normalize(dir) * seperationForce) * p_mass;
			repel = attract * -1.f;
		}
	}

	ParticleDataOutput[P_ID].seperationForce = repel;
	ParticleDataOutput[P_ID].position = p_position;
	ParticleDataOutput[P_ID].velocity = p_velocity;
}