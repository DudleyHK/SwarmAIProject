/*



*/


cbuffer ParticleConstantBuffer
{
	float3 goalPosition;
	float3 bestPosition;
	float  basicForce;
	float  gravityAcceleration;
};



struct ParticleStruct
{
	float3 position;
	float3 velocity;
	float3 forces;
	float3 gravity;
	float  speed;
	float  mass;
};


StructuredBuffer<ParticleStruct> gInput;
RWStructuredBuffer<ParticleStruct> gOutput;








float CalculateDistance(float3 a, float3 b)
{
	float x = (b.x - a.x);
	float y = (b.y - a.y);
	float z = (b.z - a.z);

	return (x * x) + (y * y) + (z * z);
}



float3 CalculateDirection(float3 a, float3 b)
{
	float x = b.x - a.x;
	float y = b.y - a.y;
	float z = b.z - a.z;

	return float3(x, y, z);
}

float3 ComputeForce(float3 dir, float mass)
{
	float x = (dir.x * basicForce) * mass;
	float y = (dir.y * basicForce) * mass;
	float z = (dir.z * basicForce) * mass;

	return float3(x, y, z);
}

float3 GetForce(ParticleStruct p)
{
	// check if this particle has a closer distance
	float3 dir           = CalculateDirection(p.position, bestPosition);
	float3 normalisedDir = normalize(dir);
	float3 force         = ComputeForce(normalisedDir, p.mass);
	
	return p.forces + p.gravity + force;
}


float3 GetVelocity(ParticleStruct p)
{
	float3 acc = p.forces / p.mass;
	float3 deltaVel = acc * 0.001f;
	return deltaVel;
}


float3 GetPosition(ParticleStruct p)
{
	float3 deltaSpeed = p.velocity * 0.001f;
	return p.position + deltaSpeed;
}


[numthreads(16, 16, 1)]
void ParticlePhysicsShader(uint3 DTid : SV_DispatchThreadID)
{
	const unsigned int p_id = DTid.x;
	ParticleStruct p = gInput[p_id];

	// reset forces
	p.forces.x = 0;
	p.forces.y = 0;
	p.forces.z = 0;
	
	p.gravity.xyz = 0;
	p.gravity.y = p.mass * gravityAcceleration;

	p.forces   = GetForce(p);
	p.velocity = GetVelocity(p);
	p.position = GetPosition(p);
	
	p.speed = length(p.velocity);




	// append the normalised vector to output buffer.
	gOutput[p_id].gravity = p.gravity;
	gOutput[p_id].forces = p.forces;
	gOutput[p_id].velocity = p.velocity;
	gOutput[p_id].position = p.position;
	gOutput[p_id].speed = p.speed;
}